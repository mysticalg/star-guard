/* Finite slow plasma; the ledger is keyed by projectile AND target identity. */
#define ORB87_LIFE 150
#define ORB87_FADE 10
#define ORB87_TARGETS 16
typedef struct{u32 owner;int count,key[ORB87_TARGETS];u32 identity[ORB87_TARGETS];} OrbLedger87;
static OrbLedger87 orbLedger87[MAXS];
static void orb87_reset(void){memset(orbLedger87,0,sizeof(orbLedger87));}
static int orb87_live(const Obj*s){return s->on&&s->type==10&&s->orbCharge87>0&&!s->orbFade87;}
static int orb87_ready(void){int free=0;for(int i=0;i<PRIMARY_SHOTS80;i++){Obj*s=g.shots+i;if(s->on&&s->type==10)return 0;free|=!s->on;}return free;}
static int orb87_radius(const Obj*s){static const int size[4]={18,22,26,30};int full=clamp(s->orbFull87,1,9),charge=clamp(s->orbCharge87,1,full);float r=size[clamp(s->power,1,4)-1]*sqrtf(charge/(float)full)*(s->echo80?.55f:1);if(s->orbFade87)r*=s->orbFade87/(float)ORB87_FADE;return clamp((int)ceilf(r),1,30);}
static void orb87_fizzle(Obj*s){if(s->orbFade87)return;s->orbCharge87=0;s->orbFade87=ORB87_FADE;if(!s->echo80)pc_orb_sound87(2,s->x/Q,0);}
static void orb87_spend(Obj*s){if(s->orbCharge87>0&&--s->orbCharge87==0)orb87_fizzle(s);}
static int orb87_seen(Obj*s,int key,u32 identity,int record){int slot=(int)(s-g.shots);if(slot<0||slot>=MAXS)return 1;OrbLedger87*l=orbLedger87+slot;if(l->owner!=s->serial){memset(l,0,sizeof(*l));l->owner=s->serial;}for(int i=0;i<l->count;i++)if(l->key[i]==key&&l->identity[i]==identity)return 1;if(l->count>=ORB87_TARGETS)return 1;if(record){l->key[l->count]=key;l->identity[l->count++]=identity;}return 0;}
static int orb87_circle(const Obj*s,float x,float y,float rx,float ry){float dx=fmaxf(0,fabsf(s->x/(float)Q-x)-rx),dy=fmaxf(0,fabsf(s->y/(float)Q-y)-ry),r=orb87_radius(s);return dx*dx+dy*dy<=r*r;}
static int orb87_los(const Obj*s,int x,int y){return missile_line82(s->x/Q,s->y/Q,x,y);}
static int orb87_enemy_contact(const Obj*s,const Obj*o){int r=orb87_radius(s);if(!orb87_circle(s,o->x/(float)Q,o->y/(float)Q,o->type==4?35+o->power/24:24,o->type==4?30+o->power/32:24))return 0;int side=pc_enemy_side_hit77(o,s->x,s->y,r);if(side<0)return orb87_circle(s,o->x/(float)Q,o->y/(float)Q,o->type==4?33+o->power/24:o->type>=14?15:o->type>=10?10:14,o->type==4?26:o->type>=10?10:15);if(side)return 1;for(int y=-r;y<=r;y+=3)for(int x=-r;x<=r;x+=3)if(x*x+y*y<=r*r&&pc_enemy_side_hit77(o,s->x+x*Q,s->y+y*Q,2)>0)return 1;return 0;}
static int orb87_damage(const Obj*s){return (600*(100+20*(clamp(s->power,1,4)-1))/100)*(s->echo80?20:100)/100;}
static void orb87_fire(void){int rank=power_level(0),charge=1+rank*2;Obj*s=slot(g.shots,MAXS);if(!s)return;u32 serial=s->serial;*s=(Obj){.serial=serial,.on=1,.type=10,.sourceFamily=1,.power=rank,.orbCharge87=charge,.orbFull87=charge,.x=g.px,.y=player_y-(player_hull_ry()+4+18+(rank-1)*4)*Q,.vx=g.playerVX/4,.vy=-320,.phase=-1};s->previousX=s->x;s->previousY=s->y;pc_orb_sound87(0,s->x/Q,charge);}
static void orb87_move(Obj*s){s->previousX=s->x;s->previousY=s->y;s->age++;if(s->orbFade87){if(--s->orbFade87==0)s->on=0;return;}if(s->age>=ORB87_LIFE){orb87_fizzle(s);return;}int r=orb87_radius(s),oldx=s->x,oldy=s->y;s->x+=s->vx;s->y+=s->vy;s->vx=s->vx*95/100;if(!s->echo80&&s->age%5==0)pc_orb_sound87(1,s->x/Q,s->orbCharge87);
 if(journey_kind()==ENV_WALL&&terrain.active){int hit[2],blocked=0;for(int dx=-r;dx<=r&&!blocked;dx++){int dy=(int)sqrtf(r*r-dx*dx);blocked=terrain_trace(oldx/Q+dx,oldy/Q-dy,s->x/Q+dx,s->y/Q-dy,hit);}if(blocked){s->x=oldx;s->y=oldy;terrain_chip(hit[0],hit[1],clamp(r/2,4,12),s->echo80?2:6);orb87_spend(s);return;}}
 for(int dy=-r;dy<=r;dy+=3){int y=s->y/Q+dy;if(y<24||y>=256||!tunnel_visible(y))continue;int dx=(int)sqrtf(r*r-dy*dy),il,ir;if(s->x/Q-dx<tunnel_left(y)||s->x/Q+dx>tunnel_right(y)||(tunnel_island(y,&il,&ir)&&s->x/Q+dx>il&&s->x/Q-dx<ir)){orb87_fizzle(s);return;}}
 if(s->x/Q+r<play_min_x()||s->x/Q-r>play_max_x()||s->y/Q+r<play_min_y()||s->y/Q-r>play_max_y())orb87_fizzle(s);
}
static void orb87_contacts(void){for(int n=0;n<MAXS;n++){Obj*s=g.shots+n;if(!orb87_live(s))continue;int damage=orb87_damage(s);
 /* Giant gates are checked before this contact pass can retire required adds. */
 Giant*b=&ex.giant;if(b->active)for(int i=0;i<9&&orb87_live(s);i++){GiantPart*p=b->parts+i;if(p->hp<=0||!giant_open(i)||!orb87_circle(s,p->x,p->y,p->w*.5f,p->h*.5f)||!orb87_los(s,p->x,p->y)||orb87_seen(s,128+i,(u32)g.wave,0))continue;int hp=p->hp;giant_hit_nonprimary84(i,damage*player_attack_scale(0)/100,0);if(p->hp<hp){orb87_seen(s,128+i,(u32)g.wave,1);orb87_spend(s);pc_impact(p->x,p->y,1);}}
 for(int i=0;i<MAXE&&orb87_live(s);i++){Obj*o=g.enemies+i;if(!o->on||o->hp<=0||!enemy_damageable(o)||!orb87_enemy_contact(s,o)||!orb87_los(s,o->x/Q,o->y/Q)||orb87_seen(s,i,o->serial,0))continue;int hp=o->hp*100-o->wound,shield=o->shieldCharge;if(o->type!=4&&!enemy_guarded(o)){orb87_seen(s,i,o->serial,1);killed(o);orb87_spend(s);}else{weapon_damage_nonprimary84(o,damage,0);if(o->hp*100-o->wound<hp||o->shieldCharge<shield||enemy_guarded(o)){orb87_seen(s,i,o->serial,1);orb87_spend(s);if(o->hp<=0)killed(o);}}pc_impact(o->x/Q,o->y/Q,1);}
 for(int i=0;i<32&&orb87_live(s);i++){EnvironmentHazard*h=ex.env.hazards+i;u32 born=(u32)(g.tick-h->age);if(!h->on||h->hp<=0||!orb87_circle(s,h->x/(float)Q,h->y/(float)Q,h->r,h->r)||!orb87_los(s,h->x/Q,h->y/Q)||orb87_seen(s,256+i,born,0))continue;int hp=h->hp*100-h->wound;if(!environment_shielded82(h))environment_weapon_hit(h,damage,0);if(h->hp*100-h->wound<hp){orb87_seen(s,256+i,born,1);orb87_spend(s);}}
 for(int i=0;i<MAXH&&orb87_live(s);i++){Obj*o=ex.obstacles+i;if(!o->on||o->hp<=0||!orb87_circle(s,o->x/(float)Q,o->y/(float)Q,o->base,o->base)||!orb87_los(s,o->x/Q,o->y/Q)||orb87_seen(s,384+i,o->serial,0))continue;o->hp-=clamp(damage/100,1,20);orb87_seen(s,384+i,o->serial,1);orb87_spend(s);pc_impact(o->x/Q,o->y/Q,1);}
 for(int i=0;i<48&&orb87_live(s);i++){WeatherActor*a=ex.env.weather.actors+i;u32 born=(u32)(g.tick-a->age);if(!a->on||a->hp<=0||!orb87_circle(s,a->x/(float)Q,a->y/(float)Q,a->r,a->r)||!orb87_los(s,a->x/Q,a->y/Q)||orb87_seen(s,512+i,born,0))continue;a->hp-=damage;orb87_seen(s,512+i,born,1);orb87_spend(s);pc_impact(a->x/Q,a->y/Q,1);}
 }}
