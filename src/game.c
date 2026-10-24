typedef unsigned char u8;
typedef unsigned int u32;
#include "assets.h"
#include "sector_backgrounds.h"
#include "rock_texture.h"
#define Q 256
#define MAXE 64
#define MAXS 56
#define MAXB 128
#define MAXH 24
#define SECTOR_WAVES 12
#define SECTOR_COUNT 17
static const int missionRoute[SECTOR_COUNT]={10,0,2,9,12,13,15,16,14,4,5,1,3,6,7,8,11};
static int mission_phase(int wave){return missionRoute[(wave-1)%SECTOR_COUNT];}
static int mission_index(int phase){for(int i=0;i<SECTOR_COUNT;i++)if(missionRoute[i]==phase)return i;return 0;}
#define BOSS_SHIELD_MAX 2400
#define BOSS_SHIELD_RECHARGE 300
#define MAXP 160
#define NSTARS 144
typedef struct {int on,x,y,vx,vy,hp,type,age,base,phase,variant,heading,power,wound,morph,bombHit,attack,shieldCharge,shieldDelay;} Obj;
typedef struct {int on,x,y,vx,vy,life,tint;} Particle;
typedef struct {int x,y,z;} Star;
typedef struct {
 int continues,continueTicks,continueReady;
 int tick,missionTicks,mode,quit,paused,wave,clock,groups,score,kills,lives,px,weapon,unlocked,energy,barrier,shield,cool,beam,active,shake,fx,bossPhase,playerVX,lastPlayerX;
 int acquiredWeapons,shipFireCount,salvagePicked,lastLootValue;int weaponLevel[8],weaponCharge[8];u32 seed; Obj enemies[MAXE],shots[MAXS],bullets[MAXB],drops[8],effects[24]; Particle particles[MAXP]; Star stars[NSTARS];
} Game;
Game g;
extern int pc_modern;
int player_y;
#include "expansion_state.h"
extern void pc_sprite(int,float,float);
extern void pc_equipment_sprite(int,float,float,float);
extern void pc_hazard_sprite(int,Obj*,float,float);
extern void pc_enemy_sprite(int,Obj*,float,float);
extern void read_mouse(int*);

extern void sprite_plot(int,int,int), sprite_bank(void*), rowcopy(void*,const void*,int), sound(int,int,int,int);
extern void sound_slide(int,int,int,int),sound_mod(int,int,int);
extern int os_byte(int,int,int), key(int), os_char(void);
extern void putc_native(int), read_screen(int*);
static int rnd(void){g.seed=g.seed*1664525u+1013904223u;return (int)((g.seed>>16)&32767);}
static int absn(int x){return x<0?-x:x;}
static int clamp(int x,int lo,int hi){return x<lo?lo:x>hi?hi:x;}
void game_pointer(int x,int y){g.px=clamp(x,14,306)*Q;player_y=clamp(y,40,240)*Q;}
static int power_level(int weapon){return clamp(g.weaponLevel[clamp(weapon,0,7)],1,4);}
static int power_needed(int weapon){static const int need[4]={2,3,5,0};return need[power_level(weapon)-1];}
static int charge_weapon(int weapon){int need=power_needed(weapon);if(!need)return 0;if(++g.weaponCharge[weapon]>=need){g.weaponCharge[weapon]-=need;g.weaponLevel[weapon]++;return 1;}return 0;}
static int ship_level(void){int upgrades=0;for(int i=0;i<8;i++)upgrades+=g.weaponLevel[i]-1;return clamp(1+upgrades/3,1,8);}
static int enemy_bonus(void){return clamp((g.wave-1)/5,0,3);}
static int weapon_delay(int base){return clamp(base-(power_level(g.weapon)>=2)-(power_level(g.weapon)>=4),2,30);}
static int projectile_damage(const Obj*s){int level=clamp(s->power,1,4),base=s->variant==101?10:s->phase==-2?25:s->type==1?50:s->type==2?75:s->type==4?300:s->type==7?60:100;return base*(100+20*(level-1))/100;}
static void damage_enemy(Obj*o,int damage){if(o->type==4){if(o->age<=120||o->morph>0||(o->attack>60&&o->attack<=85))return;if(o->shieldCharge>0){o->shieldCharge=clamp(o->shieldCharge-damage,0,BOSS_SHIELD_MAX);if(!o->shieldCharge)o->shieldDelay=BOSS_SHIELD_RECHARGE;return;}}o->wound+=damage;o->hp-=o->wound/100;o->wound%=100;if(o->type==4&&o->phase<3){int floor=o->base*(3-o->phase)/3;if(o->hp<floor)o->hp=floor;}}
static int sn(int a){return sine[a&255];}
static int divn(int a,int b){return b?a/b:0;}
static int remn(int a,int b){return b?a%b:0;}
static Obj *slot(Obj *a,int n){int i;for(i=0;i<n;i++)if(!a[i].on){a[i]=(Obj){.on=1};return a+i;}return 0;}
static void particle(int x,int y,int vx,int vy,int life,int tint){int i;if(!g.fx)return;for(i=0;i<MAXP;i++){int n=ex.nextParticle++;Particle*p;if(ex.nextParticle==MAXP)ex.nextParticle=0;p=g.particles+n;if(!p->on){p->on=1;p->x=x;p->y=y;p->vx=vx;p->vy=vy;p->life=life;p->tint=tint;return;}}}
extern void pc_hit_sound(int,int,int);
extern void pc_laser_sound(int,int);
extern void pc_burst(int,int,int),pc_impact(int,int,int),pc_bomb_start(int,int),pc_transition(void),pc_charge_burst(int,int);
static void burst(int x,int y,int big){pc_burst(x,y,big);int i,n=big?48:22;Obj*e=slot(g.effects,12);if(e){e->x=x;e->y=y;e->type=big;e->phase=rnd()&255;}for(i=0;i<n;i++){int a=rnd()&255,s=(big?700:400)+(rnd()&(big?1023:511));particle(x*Q,y*Q,(sn(a)*s)>>10,(sn(a+64)*s)>>10,18+(rnd()&23),i&3?1:0);}g.shake=big?7:3;if(ex.soundTick!=g.tick){ex.soundTick=g.tick;sound(2,2,big?118:84,big?0x2800:0x3400);sound_slide(2,1,0x1100,big?30:14);sound_slide(2,0,0,big?42:22);}}
static void drop(int x,int y,int type){Obj *o=slot(g.drops,8);if(o){o->x=x*Q;o->y=y*Q;o->type=type;o->vy=220;}}
void game_reset(void){int i;u8 *p=(u8*)&g;for(i=0;i<(int)sizeof(g);i++)p[i]=0;g.seed=0x5173;g.continues=3;g.wave=1;g.lives=3;g.px=160*Q;player_y=230*Q;g.energy=100;g.barrier=100;g.shield=100;g.unlocked=255;g.acquiredWeapons=1;for(i=0;i<8;i++)g.weaponLevel[i]=1;g.lastPlayerX=g.px;expansion_reset();g.fx=1;/* Each depth owns a jittered 8 by 6 grid, rather than unbounded random clusters. */
for(i=0;i<NSTARS;i++){int cell=remn(i,48),z=divn(i,48);g.stars[i].x=remn(cell,8)*40+5+remn(rnd(),30);g.stars[i].y=(divn(cell,8)*38+4+remn(rnd(),29))*Q;g.stars[i].z=z;}}
static void shot(int x,int vx,int type){Obj *o=slot(g.shots,MAXS);if(o){o->x=x;o->y=player_y-10*Q;o->vx=vx+g.playerVX;o->vy=type==4?-1300:type==7?-3200:-2400;o->type=type;o->power=power_level(g.weapon);o->hp=type==6?4:type==7?5:0;o->base=0;o->phase=-1;}}
static Obj* bullet(int x,int y,int vx,int vy){Obj *o=slot(g.bullets,MAXB);if(o){o->x=x;o->y=y;o->vx=vx;o->vy=vy;o->type=0;o->variant=absn(vx)>absn(vy)?2:0;}return o;}
static void armored_bullet(int x,int y,int vx,int vy){Obj*o=bullet(x,y,vx,vy);if(o)o->type=1;}
static void armored_aim(int x,int y,int speed){int dx=g.px-x,dy=player_y-y,d=absn(dx)+absn(dy);if(!d)d=1;armored_bullet(x,y,divn(dx*speed,d),divn(dy*speed,d));}
/* Relative swept segments catch fast shots crossing between fixed simulation ticks. */
static int projectile_crosses(const Obj*s,const Obj*b){float dx=(b->x-s->x)/(float)Q,dy=(b->y-s->y)/(float)Q,vx=(b->vx-s->vx)/(float)Q,vy=(b->vy-s->vy)/(float)Q;if(fabsf(dx)>fabsf(vx)+5||fabsf(dy)>fabsf(vy)+5)return 0;float vv=vx*vx+vy*vy,t=vv>0?fmaxf(0,fminf(1,(dx*vx+dy*vy)/vv)):0;dx-=vx*t;dy-=vy*t;return dx*dx+dy*dy<=25;}
static void aim(int x,int y,int speed){int dx=g.px-x,dy=player_y-y,d=absn(dx)+absn(dy);if(!d)d=1;Obj*o=bullet(x,y,divn(dx*speed,d),divn(dy*speed,d));if(o)o->variant=1;}
static void killed(Obj*);
static int enemy_guarded(const Obj*);
#include "bugs.h"
static void specialist_group(int,int);
static void ground_group(int,int);
static void group(int type,int groupno){if(type==26){for(int n=0;n<32;n++){Obj*o=slot(g.enemies,MAXE);if(o)*o=(Obj){.on=1,.type=26,.hp=1,.x=(12+(n%8)*42)*Q,.y=(-20-(n/8)*24)*Q,.phase=n*19,.vy=420};}return;}if(type==25){specialist_group(type,groupno);return;}if(type>=23){ground_group(type,groupno);return;}if(type>=14){specialist_group(type,groupno);return;}if(type>=10){bug_group(type,groupno);return;}int i,n=6+((g.wave-1)>>2);if(n>8)n=8;for(i=0;i<n;i++){Obj *o=slot(g.enemies,MAXE);if(!o)break;o->type=type;o->hp=(type==6?6:type==7?3:type==3?4:1)+enemy_bonus();o->phase=i*27+groupno*41;int pattern=groupno%5;o->base=pattern==1?160+(i&1?1:-1)*(20+(i/2)*27):pattern==2?38+i*14:pattern==3?282-i*14:pattern==4?40+(i%3)*110:28+i*38;o->x=o->base*Q;o->y=(-24-i*(pattern==2||pattern==3?28:14))*Q;o->vx=(i&1)?220:-220;o->vy=type==3?105:180+type*35;}}
static void boss(void){Obj *o=slot(g.enemies,MAXE);if(o){o->type=4;o->variant=ex.phase<0?0:ex.phase;o->hp=300+g.wave*32;o->base=o->hp;o->x=160*Q;o->y=-64*Q;o->phase=1;g.bossPhase=1;}}
static void hit_sparks(int x,int y,int nx,int ny,int tint){pc_impact(x,y,tint);int i;Obj*f=slot(ex.impacts,12);if(f){f->x=x;f->y=y;f->type=tint;}ex.contactX=x;ex.contactY=y;ex.contactTick=g.tick;for(i=0;i<10;i++){int a=rnd()&255;particle(x*Q,y*Q,nx*180+(sn(a)>>1),ny*180+(sn(a+64)>>1),8+(rnd()&7),tint);}}
static void surface_point(int cx,int cy,int rx,int ry,int ox,int oy,int*out){int dx=ox-cx,dy=oy-cy;if(absn(dx)*ry>absn(dy)*rx){out[0]=cx+(dx<0?-rx:rx);out[1]=cy+clamp(dy,-ry,ry);}else{out[0]=cx+clamp(dx,-rx,rx);out[1]=cy+(dy<0?-ry:ry);}}
static void hurt_at(int x,int y,int nx,int ny){if(ex.contactTick!=g.tick||ex.contactX!=x||ex.contactY!=y)hit_sparks(x,y,nx,ny,g.shield||g.active?3:1);if(g.shield||g.active){pc_hit_sound(1,0,x);return;}pc_hit_sound(0,4,x);g.barrier-=35;g.shield=28;g.shake=5;sound(3,2,100,0x3000);if(g.barrier<0){g.lives--;g.barrier=75;g.shield=100;burst(g.px>>8,player_y>>8,1);if(g.lives<=0){g.mode=g.continues?3:2;g.continueTicks=450;g.continueReady=0;}}}
static void carrier_release(const Obj*);
static void option_fire(void);
static void stage_boss_step(Obj*);
static int next_weapon_drop(void){for(int n=0;n<8;n++){int w=ex.weaponDrop++%8;if(g.acquiredWeapons&(1<<w))continue;int pending=0;for(int i=0;i<8;i++)pending|=g.drops[i].on&&g.drops[i].type==10&&g.drops[i].variant==w;if(!pending)return w;}return -1;}
static void salvage_drop(int x,int y){Obj*d=slot(g.drops,8);if(!d)return;int rarity=rnd()%100;*d=(Obj){.on=1,.type=11,.variant=rarity<10?2:rarity<40?1:0,.x=x*Q,.y=y*Q,.vy=220};}
static void unique_weapon_drop(int x,int y){int w=next_weapon_drop();if(w<0){salvage_drop(x,y);return;}Obj*d=slot(g.drops,8);if(d)*d=(Obj){.on=1,.type=10,.variant=w,.x=x*Q,.y=y*Q,.vy=220};}
static void ship_fire(void){int level=ship_level();if(level<2)return;if(level==2&&(g.shipFireCount++&1))return;int pairs=1+(level-2)/2;for(int row=0;row<pairs;row++)for(int side=-1;side<=1;side+=2){Obj*o=slot(g.shots,MAXS);if(o)*o=(Obj){.on=1,.type=g.weapon==3||g.weapon==5?0:g.weapon,.x=g.px+side*(4+row*3)*Q,.y=player_y-(7-row)*Q,.vx=g.playerVX+side*row*70,.vy=g.weapon==4?-1300:-2400,.hp=g.weapon==6?4:g.weapon==7?5:0,.phase=-3,.variant=101,.power=power_level(g.weapon)};}}
static void killed(Obj *o){int x=o->x>>8,y=o->y>>8;Obj destroyed=*o;o->on=0;g.score+=o->type==4?1500:100+o->type*50;g.kills++;burst(x,y,o->type==4);if(o->type==4){g.barrier=100;g.shield=400;ex.victory=1;ex.victoryX=x;ex.victoryY=y;ex.clearBonus=1000+g.lives*250;g.score+=ex.clearBonus;ex.clearScore=g.score-ex.stageScore;ex.clearKills=g.kills-ex.stageKills;ex.clearSalvage=g.salvagePicked-ex.stageSalvage;pc_laser_sound(3,x);drop(x,y,(g.wave&1)?7:4);}else if(remn(g.kills,16)==0){static const int loot[16]={10,6,11,7,10,8,11,6,9,10,11,7,2,11,5,4};int item=loot[remn(divn(g.kills,16)-1,16)];if(item==10)unique_weapon_drop(x,y);else if(item==11)salvage_drop(x,y);else drop(x,y,item);}carrier_release(&destroyed);}
#include "expansion_step.h"
#include "bosses.h"
void game_step(int mask,int ch){int i,j,alive=0,fired=0;
 if(ch==27||ch=='q')g.paused=!g.paused;if(ch=='r'){game_reset();g.mode=1;}if(ch=='f')g.fx=!g.fx;
 if(!g.paused){for(i=0;i<NSTARS;i++){Star*s=g.stars+i;s->y+=((s->z==0?64:s->z==1?160:384)*ex.scrollSpeed)>>8;if(s->y>=232*Q)s->y-=232*Q;}}
 if(g.mode==0){g.tick++;if((mask&4)||(ch>='1'&&ch<='8')){g.mode=1;g.tick=0;if(ch>='1'&&ch<='5')g.wave=ch-'0';if(ch=='6'){g.wave=3;g.clock=300;g.groups=3;}if(ch=='7')g.weapon=6;if(ch=='8')g.weapon=7;}return;}if(g.mode==2){g.tick++;return;}
 if(ch=='p')g.paused=!g.paused;if(g.paused)return;
 if(g.mode==3){g.tick++;if(!(mask&4))g.continueReady=1;if(ch=='n')g.continueTicks=0;if(ch==13||ch=='c'||(g.continueReady&&(mask&4))){g.continues--;g.continueTicks=0;g.mode=1;g.lives=3;g.barrier=g.energy=100;g.shield=150;g.px=160*Q;player_y=220*Q;if(ex.phase==5)g.px=tunnel_lane(220,0)*Q;pc_charge_burst(g.px/Q,220);return;}if(g.continueTicks>0)g.continueTicks--;if(!g.continueTicks)g.mode=2;return;}
 if(ch>='1'&&ch<='8'&&(g.unlocked&(1<<(ch-'1'))))g.weapon=ch-'1';
 if(ch=='d'&&g.energy>=35&&!g.active){g.energy-=35;g.active=45;}
 g.tick++;g.missionTicks++;if(!ex.travel&&!ex.intro&&!ex.victory)g.clock++;if(mask&1)g.px-=1536;if(mask&2)g.px+=1536;g.px=clamp(g.px,14*Q,306*Q);g.playerVX=clamp(g.px-g.lastPlayerX,-6*Q,6*Q)*2/3;g.lastPlayerX=g.px;expansion_step(mask,ch);
 if(g.shield)g.shield--;if(g.active)g.active--;if(g.shake)g.shake--;if(g.cool)g.cool--;if(g.beam)g.beam--;
 if((g.tick&3)==0&&g.energy<100)g.energy++;if((g.tick&15)==0&&g.barrier<100)g.barrier++;
 if((mask&4)&&!g.cool){if(g.weapon==3||g.weapon==5){int cost=g.weapon==5?3:2;if(g.energy>=cost){g.energy-=cost;fired=1;g.beam=8;g.cool=6;ex.multi=g.weapon==5;pc_laser_sound(0,g.px/Q);}else g.cool=3;}else if(g.weapon!=7||g.energy>=6){if(g.weapon==7)g.energy-=6;fired=1;shot(g.px,g.weapon==6?((g.tick&1)?600:-600):0,g.weapon);if(g.weapon==7){shot(g.px-3*Q,-1100,7);shot(g.px+3*Q,1100,7);}if(g.weapon==1){shot(g.px-3*Q,-350,1);shot(g.px+3*Q,350,1);}g.cool=weapon_delay(g.weapon==7?12:g.weapon==6?9:g.weapon==4?14:g.weapon==2?3:g.weapon==1?6:4);if(g.weapon==7)pc_laser_sound(1,g.px/Q);else{sound(1,1,62,g.weapon==2?0x4100+(sn(g.tick*17)>>1):0x3800+(sn(g.tick*7)>>2));sound_slide(1,1,g.weapon==2?0x2c00:0x2000,g.weapon==2?3:7);}particle(g.px,player_y-10*Q,-160,-250,7,3);particle(g.px,player_y-10*Q,160,-250,7,1);}else g.cool=3;}
 if(fired)ship_fire();
 if(g.beam&&(g.tick&3)==0)sound_mod(1,85,0x3800+sn(g.tick*21)*2);
 if((g.tick&1)==0){particle(g.px-2*Q,player_y+8*Q,-50,350,10,0);particle(g.px+2*Q,player_y+8*Q,50,350,10,3);}
 if(!ex.travel&&!ex.intro&&!ex.victory&&!ex.bossSpawned&&g.groups>=SECTOR_WAVES&&g.clock>=encounter_deadline()&&!route_occupied()){
  if(!ex.bossWarning){ex.bossWarning=100;sound(5,1,90,0x2800);sound_slide(5,1,0x4800,90);}
  else if(--ex.bossWarning==0){boss();ex.bossSpawned=1;}
 }
 if(!ex.travel&&!ex.intro&&!ex.victory&&!ex.bossSpawned&&g.groups<SECTOR_WAVES&&enemy_count()<=20&&g.clock+(ex.travel==2?ex.travelTick:0)>=1+g.groups*170){group(ex.phase==5?(g.groups&1?9:8):formation_type(),g.groups);g.groups++;}
 for(i=0;i<MAXS;i++){Obj *o=g.shots+i;if(!o->on)continue;o->age++;if(o->base>0)o->base--;if(o->type==8){if(o->phase){o->vx+=14;}else o->vy+=6;o->x+=o->vx;o->y+=o->vy;if(cave_projectile(o,0)||(ex.phase==11&&o->x>=288*Q)||o->age>=28||o->x<3*Q||o->x>317*Q||o->y>252*Q){detonate_charge(o);}continue;}if(o->type==4)home_shot(o);trail_segment(o->x,o->y,o->x+o->vx,o->y+o->vy,o->type==2||o->type==4?1:3);o->x+=o->vx;o->y+=o->vy;if(cave_projectile(o,0))continue;if(o->type==6||o->type==7){if(o->x<3*Q||o->x>317*Q){o->x=clamp(o->x,3*Q,317*Q);o->vx=-o->vx;if(o->type==7)o->heading++;}if(o->y<26*Q||o->y>252*Q){o->y=clamp(o->y,26*Q,252*Q);o->vy=-o->vy;if(o->type==7)o->heading++;}if(o->age>150)o->on=0;}else if(o->y<20*Q||o->x<0||o->x>320*Q)o->on=0;}
 for(i=0;i<MAXE;i++){Obj *o=g.enemies+i;int x,y;if(!o->on)continue;int oldx=o->x,oldy=o->y;o->age++;
  if(o->type==4){stage_boss_step(o);
  }else{if(o->type<8)o->y+=(o->vy*ex.enemySpeed)>>8;new_enemy_step(o);if(o->type==0)o->x=o->base*Q+sn(o->age*2+o->phase)*5;
   if(o->type==1){o->x=o->base*Q+sn(o->age+o->phase)*9;if(o->age>100)o->y+=180;}
   if(o->type==2)o->x=160*Q+sn(o->age*2+o->phase)*33;
   if(o->type==3)o->x=o->base*Q+sn((o->age>>1)+o->phase)*3;
   if(o->y>24*Q&&((o->age+o->phase)&127)==0)aim(o->x,o->y,450+(g.wave<<3));
  }
  if(o->type==4){int target=clamp((o->x-oldx)*8/Q,-12,12)*16,diff=(target-o->heading+8640)%5760-2880;o->heading=(o->heading+diff/4+5760)%5760;}else{int dx=o->x-oldx,dy=o->y-oldy;if(absn(dx)+absn(dy)>64){int target=(int)(atan2f((float)dx,(float)-dy)*916.732472f);int diff=(target-o->heading+8640)%5760-2880;o->heading=(o->heading+clamp(diff/3,-480,480)+5760)%5760;}}
  x=o->x>>8;y=o->y>>8;if(y>304||(o->age>120&&(x< -80||x>400||y< -96))){o->on=0;continue;}
  if(g.beam&&g.tick%3==0&&beam_hits(x,o->type==4?32+o->power/24:13)&&y<(player_y>>8)-7&&y>15){if(!enemy_guarded(o)||o->type==4)damage_enemy(o,100+20*(power_level(g.weapon)-1));pc_hit_sound(enemy_guarded(o)?1:0,g.weapon,x);}
  for(j=0;j<MAXS;j++){Obj *s=g.shots+j;if(s->on&&!((s->type==6||s->type==7)&&s->base>0)&&absn((s->x>>8)-x)<(o->type==4?33+o->power/24:o->type>=14?15:o->type>=10?10:14)&&absn((s->y>>8)-y)<(o->type==4?26:o->type>=10?10:15)){int pt[2];if(s->type==8){detonate_charge(s);if(!o->on)break;continue;}if(s->type==6||s->type==7)ricochet(s,o,i);else s->on=0;if(!enemy_guarded(o)||o->type==4)damage_enemy(o,projectile_damage(s));surface_point(x,y,o->type==4?30:12,o->type==4?23:12,s->x>>8,s->y>>8,pt);hit_sparks(pt[0],pt[1],0,1,1);pc_hit_sound(enemy_guarded(o)?1:0,s->type,x);}}
  if(!o->on)continue;if(o->hp<=0){killed(o);continue;}if(absn(y-(player_y>>8))<17&&absn(x-(g.px>>8))<17){int pt[2];surface_point(g.px>>8,player_y>>8,6,7,x,y,pt);int vulnerable=!g.shield&&!g.active;int oldLives=g.lives;hurt_at(pt[0],pt[1],(g.px>>8)>x?1:-1,0);if(vulnerable&&g.lives==oldLives){g.energy=clamp(g.energy,0,5);g.barrier=clamp(g.barrier,0,5);}if(o->type!=4){o->on=0;burst(x,y,0);}}
 }
 for(i=0;i<MAXB;i++){Obj *o=g.bullets+i;if(!o->on)continue;o->age++;if(o->type==3&&o->age>45){o->vx-=o->vx/16;o->vy-=o->vy/16;}if(o->type==5&&o->age<90){int dx=g.px-o->x,dy=player_y-o->y,d=absn(dx)+absn(dy);if(d){o->vx+=(divn(dx*750,d)-o->vx)/18;o->vy+=(divn(dy*750,d)-o->vy)/18;}}trail_segment(o->x,o->y,o->x+o->vx,o->y+o->vy,2);o->x+=o->vx;o->y+=o->vy;if(cave_projectile(o,1))continue;
 if(o->type!=1){int intercepted=g.beam&&beam_hits(o->x>>8,6)&&o->y<player_y-12*Q&&(o->type<3||g.tick%3==0),hitDamage=intercepted?100:0;for(j=0;j<MAXS&&!intercepted;j++){Obj*s=g.shots+j;if(s->on&&projectile_crosses(s,o)){intercepted=1;hitDamage=projectile_damage(s);if(s->type==8){hitDamage=300;detonate_charge(s);}else if(s->type==6||s->type==7){if(--s->hp<=0)s->on=0;}else s->on=0;}}if(intercepted){hit_sparks(o->x>>8,o->y>>8,0,0,3);pc_hit_sound(5,g.weapon,o->x>>8);if(o->type>=3){o->wound+=hitDamage;o->hp-=o->wound/100;o->wound%=100;if(o->hp<=0){o->on=0;g.score+=100;pc_charge_burst(o->x/Q,o->y/Q);continue;}}else{o->on=0;continue;}}}
 if(o->type==3&&o->age>=140){int x=o->x,y=o->y;o->on=0;burst(x/Q,y/Q,1);if(absn(g.px-x)+absn(player_y-y)<42*Q)hurt_at(g.px/Q,player_y/Q,0,1);for(int a=0;a<256;a+=16){Obj*f=bullet(x,y,sn(a)*650/1024,sn(a+64)*650/1024);if(f)f->variant=1;}continue;}
 if(o->type==4){if(o->age>=200){o->on=0;pc_charge_burst(o->x/Q,o->y/Q);continue;}if(absn(o->y-player_y)<110*Q)g.px=clamp(g.px+clamp((o->x-g.px)/96,-40,40),14*Q,306*Q);if(o->age%30==0)for(int a=0;a<256;a+=64){Obj*f=bullet(o->x,o->y,sn(a+o->age)*480/1024,sn(a+o->age+64)*480/1024);if(f)f->variant=2;}}
 if(o->type==5&&o->age>=170){o->on=0;pc_charge_burst(o->x/Q,o->y/Q);continue;}

 if(o->type==2&&o->age>=65){int x=o->x,y=o->y;o->on=0;pc_charge_burst(x/Q,y/Q);sound(3,2,65,0x3400);for(int a=0;a<256;a+=32){Obj*fragment=bullet(x,y,sn(a)*520/1024,sn(a+64)*520/1024);if(fragment){fragment->variant=1;fragment->age=1;}}continue;}if(o->x<0||o->x>320*Q||o->y<24*Q||o->y>258*Q)o->on=0;else if(absn(o->x-g.px)<6*Q&&absn(o->y-player_y)<7*Q){int pt[2];o->on=0;surface_point(g.px>>8,player_y>>8,6,7,o->x>>8,o->y>>8,pt);hurt_at(pt[0],pt[1],o->vx>0?1:-1,o->vy>0?1:-1);}}
 for(i=0;i<8;i++){Obj *o=g.drops+i;if(!o->on)continue;o->y+=o->vy;o->age++;if(o->y>267*Q)o->on=0;else if(absn(o->x-g.px)<20*Q&&absn(o->y-player_y)<20*Q){o->on=0;ex.pickupKind=o->type;ex.pickupTicks=100;if(o->type==0||o->type==1||o->type==3||o->type==10){int w=o->type==10?o->variant:o->type==3?3:o->type+1;w=clamp(w,0,7);if(g.acquiredWeapons&(1<<w)){g.lastLootValue=250;g.score+=250;g.salvagePicked++;ex.pickupKind=11;}else{g.acquiredWeapons|=1<<w;g.weapon=w;charge_weapon(w);ex.pickupKind=10;for(int d=0;d<8;d++)if(g.drops[d].on&&g.drops[d].type==10&&g.drops[d].variant==w){g.drops[d].type=11;g.drops[d].variant=0;}}ex.pickupTicks=100;}
 if(o->type==11){static const int value[3]={250,750,1500};g.lastLootValue=value[clamp(o->variant,0,2)];g.score+=g.lastLootValue;g.salvagePicked++;}
 if(o->type==7){int upgraded=charge_weapon(g.weapon);if(power_level(g.weapon)==4&&!upgraded){g.energy=clamp(g.energy+20,0,100);g.score+=100;}ex.pickupKind=upgraded?7:12;ex.pickupTicks=100;}
 if(o->type==5&&ex.bombs<5)ex.bombs++;
 if(o->type==6){ex.options=clamp(ex.options+1,0,5);ex.optionsOn=1;ex.pickupKind=6;ex.pickupTicks=100;}
 if(o->type==8){ex.satellite=1;ex.pickupKind=8;ex.pickupTicks=100;}
 if(o->type==9){ex.dropper=1;ex.pickupKind=9;ex.pickupTicks=100;}
 if(o->type==2){g.shield=220;g.barrier=100;}
 if(o->type==4){if(g.lives<5)g.lives++;g.barrier=100;}
 if(ex.pickupKind!=11)g.score+=50;sound(1,1,80,0x4600);}}
 for(i=0;i<24;i++)if(g.effects[i].on&&++g.effects[i].age>34)g.effects[i].on=0;
 for(i=0;i<MAXP;i++){Particle *p=g.particles+i;if(!p->on)continue;p->x+=p->vx;p->y+=p->vy;if(p->life>14&&p->tint==1){p->vx-=p->vx>>5;p->vy-=p->vy>>5;p->vy+=10;}if(--p->life<=0)p->on=0;}
 for(i=0;i<MAXE;i++)alive+=g.enemies[i].on;
 if(!ex.travel&&ex.bossSpawned&&!alive&&!route_occupied()&&!hostile_rounds_remain()){if((!ex.victory||ex.victory>=350)&&++ex.clearTicks>=75){g.wave++;g.clock=0;g.groups=0;g.energy=100;g.shield=80;ex.clearTicks=0;}}else ex.clearTicks=0;
}
static void pixel(u8 *b,int x,int y,int c){if(x>=0&&x<320&&y>=24&&y<256)b[y*320+x]=c;}
static void glow(u8*b,int x,int y,int tint,int level){if(x>=0&&x<320&&y>=24&&y<256){int a=y*320+x;b[a]=blend[(tint<<10)+(clamp(level,0,3)<<8)+b[a]];}}
static void text(u8*b,int x,int y,const char*s,int c,int scale){int i,j,k,xx,yy;while(*s){int n=-1;for(i=0;fontchars[i];i++)if(fontchars[i]==*s){n=i;break;}if(n>=0)for(j=0;j<7;j++)for(k=0;k<5;k++)if(font[n*7+j]&(1<<(4-k)))for(yy=0;yy<scale;yy++)for(xx=0;xx<scale;xx++){int ax=x+k*scale+xx,ay=y+j*scale+yy;if(ax>=0&&ax<320&&ay>=0&&ay<256)b[ay*320+ax]=c;}x+=6*scale;s++;}}
static void number(u8*b,int x,int y,int n,int c){char s[12];int p=10;s[11]=0;do{int q=divn(n,10);s[p--]=(char)('0'+n-q*10);n=q;}while(n&&p>=0);text(b,x,y,s+p+1,c,1);}
static void ring(u8*b,int x,int y,int r,int tint,int phase){int a,c=tint==1?87:tint==2?143:tint==3?235:139;for(a=0;a<256;a+=8){int xx=x+((sn(a+phase)*r)>>10),yy=y+((sn(a+phase+64)*r)>>10);pixel(b,xx,yy,c);if((a&63)==0)glow(b,xx+1,yy,tint,2);}}
static int star_x(const Star*s){int x=s->x-(((g.px-160*Q)*(s->z+1))>>12);if(x<0)x+=320;else if(x>=320)x-=320;return x;}
#include "expansion_render.h"
void game_render(u8 *b){int y,i,x,dx=0,dy=0,busy=0;int scroll=(ex.backgroundY>>8)&511;
 /* The distant cloud tile drifts as a whole; no scanline wobble or shearing. */
 if(!pc_modern){int off=(((g.px>>8)-160)>>5)*4;if(off<0)off+=320;
 for(y=24;y<256;y++){int sy=(y+scroll)&511;const u8*cloud=phase_background(y);if(tunnel_visible(y)){int world=(ex.backgroundY>>8)+y,center=tunnel_center(y),half=tunnel_half(y),left=tunnel_left(y)+((world>>2)&3)-1,right=tunnel_right(y)-((world>>3)&3)+1,l=left&~3,r=(right+3)&~3,start=l+off;if(start>=320)start-=320;rowcopy(b+y*320+l,cloud+sy*320+start,(r-l)>>2);}else{rowcopy(b+y*320,cloud+sy*320+off,(320-off)>>2);if(off)rowcopy(b+y*320+320-off,cloud+sy*320,off>>2);}}
 for(i=0;i<NSTARS;i++){Star*s=g.stars+i;int z=s->z;int xx=star_x(s);int yy=24+(s->y>>8)-(((player_y-140*Q)*(z+1))>>13);if(yy<24)yy+=232;else if(yy>=256)yy-=232;pixel(b,xx,yy,z==0?2:z==1?3:255);if(z==2&&(i&3)==0){pixel(b,xx,yy+1,3);if(g.fx&&(i&15)==0){pixel(b,xx-1,yy,3);pixel(b,xx+1,yy,3);pixel(b,xx,yy-1,3);}}}
 }else memset(b,0,81920);
 expansion_render(b);if(!pc_modern)draw_trails(b);
 if(g.shake&&g.fx){dx=(sn(g.tick*71)*g.shake)>>11;dy=(sn(g.tick*93)*g.shake)>>12;}
 for(i=0;i<12;i++)busy+=g.effects[i].on;
 if(!pc_modern)for(i=0;i<(busy>4?96:MAXP);i++){Particle*p=g.particles+i;if(p->on){int x=p->x>>8,y=p->y>>8,hot=p->life>20,c=p->tint==1?(hot?87:p->life>10?23:4):p->tint==2?(hot?143:12):(hot?235:p->life>8?139:8);pixel(b,x,y,c);pixel(b,x-(p->vx>>9),y-(p->vy>>9),c);if(hot){pixel(b,x,y+1,p->life>30?255:c);if((i&7)==0)glow(b,x+1,y,p->tint,2);}}}
 if(g.mode==1||g.mode==2){
  for(i=0;i<MAXE;i++){Obj *o=g.enemies+i;if(!o->on)continue;x=(o->x>>8)+dx;y=(o->y>>8)+dy;if(o->type==4){draw_stage_boss(b,o,x,y);}else if(o->type<5)pc_enemy_sprite(o->type==0?1:o->type+5,o,o->x/256.0f+dx,o->y/256.0f+dy);else draw_new_enemy(b,o,x,y);}
  for(i=0;i<8;i++){Obj*o=g.drops+i;if(o->on){if(pc_modern){int art=o->type==11?100+clamp(o->variant,0,2):o->type==2?53:o->type==4?54:o->type==5?55:o->type==6?52:o->type==8?56:o->type==9?57:51;pc_sprite(art,o->x/256.0f,o->y/256.0f);}else if(o->type<5)pc_sprite(9+o->type,o->x/256.0f,o->y/256.0f);else draw_supply(b,o);}}
  for(i=0;i<MAXS;i++)if(g.shots[i].on&&g.shots[i].type==8)pc_equipment_sprite(60,g.shots[i].x/256.0f,g.shots[i].y/256.0f,g.tick*10);
  /* Projectiles are palette-blended energy particles, with a fading wake. */
  if(pc_modern)for(i=0;i<MAXB;i++){Obj*o=g.bullets+i;if(!o->on)continue;if(o->type==2||o->type==3||o->type==4){Obj device=*o;device.base=o->type==2?6:o->type==3?12:15;pc_hazard_sprite(o->type==2?111:o->type==3?113:114,&device,o->x/(float)Q,o->y/(float)Q);}else if(o->type==5)pc_equipment_sprite(115,o->x/(float)Q,o->y/(float)Q,atan2f(o->vx,-o->vy)*57.29578f);}

  if(!pc_modern)for(i=0;i<MAXS+MAXB;i++){Obj*o=i<MAXS?g.shots+i:g.bullets+i-MAXS;if(o->on){int bx=o->x>>8,by=o->y>>8,k,t=i<MAXS?(o->type==2||o->type==4?1:3):2;int vx=i<MAXS?0:-(o->vx>>8),vy=i<MAXS?1:-(o->vy>>8);if(!vy)vy=1;pixel(b,bx-1,by,t==1?87:t==2?143:235);pixel(b,bx+1,by,t==1?87:t==2?143:235);projectile_bloom(b,bx,by,t,i<MAXS?i<12:i<MAXS+12);if(i<MAXS&&o->type==7){int speed=absn(o->vx)+absn(o->vy);if(speed){int tx=bx-divn(o->vx*12,speed),ty=by-divn(o->vy*12,speed);stroke(b,bx-1,by,tx-1,ty,139);stroke(b,bx+1,by,tx+1,ty,235);stroke(b,bx,by,tx,ty,255);}}pixel(b,bx,by,255);pixel(b,bx,by-1,255);pixel(b,bx+1,by,255);}}
  if(!pc_modern){draw_beams(b);draw_impacts(b);}
  if(!pc_modern)for(i=0;i<12;i++)if(g.effects[i].on)draw_explosion(b,g.effects+i,i<(busy>4?1:4));
  if(g.mode==1){x=g.px>>8;if(!ex.travel&&(!g.shield||(g.tick&2)==0))pc_sprite(0,g.px/256.0f+dx,player_y/256.0f+dy);if(g.fx){projectile_bloom(b,x-2,(player_y>>8)+8,3,1);projectile_bloom(b,x+2,(player_y>>8)+8,3,1);}if(!pc_modern&&(g.shield||g.active)){int sz=(g.tick&8)?44:40;pc_sprite((g.tick&8)?27:26,g.px/256.0f,player_y/256.0f);if(g.active&&g.fx)ring(b,x,player_y>>8,25,3,g.tick*2);}}
 }
 if(pc_modern)return;
 rowcopy(b,clear_hud,1920);text(b,4,3,"SCORE",219,1);number(b,40,3,g.score,255);text(b,108,3,"WAVE",219,1);number(b,138,3,g.wave,255);text(b,178,3,"LIVES",219,1);number(b,214,3,g.lives,255);text(b,250,3,"GUN",219,1);number(b,274,3,g.weapon+1,255);
 text(b,4,14,"BARRIER",151,1);for(i=0;i<g.barrier;i++)b[17*320+49+i]=151;text(b,164,14,"CHARGE",223,1);for(i=0;i<g.energy;i++)b[17*320+207+i]=223;
 if(g.mode==0){title_particles(b);text(b,74,108,"WINDOWS C / SDL3 EDITION",151,1);text(b,57,133,"SPACE TO LAUNCH",255,2);text(b,51,162,"MOUSE MOVE  LEFT CLICK/SPACE FIRE",219,1);text(b,8,177,"1-6 SECTOR  1-7 GUN  B BOMB  D SHIELD",219,1);text(b,51,192,"O OPTIONS  P PAUSE  F EFFECTS",219,1);text(b,60,220,"F11 FULLSCREEN  M MUTE  V VSYNC",151,1);}
 if(g.paused)text(b,124,126,"PAUSED",255,2);if(g.mode==2){text(b,70,113,"GAME OVER",255,3);text(b,100,150,"R TO RESTART",219,1);}
 if(g.mode==1&&remn(g.wave,5)==0){for(i=0;i<MAXE;i++)if(g.enemies[i].on&&g.enemies[i].type==4){int hp=divn(g.enemies[i].hp*200,g.enemies[i].base);for(x=0;x<hp;x++)b[27*320+60+x]=223;text(b,114,31,"BOSS PHASE",255,1);number(b,182,31,g.bossPhase,255);break;}}
}
