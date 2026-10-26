/* Independent collision targets; hull art is not a collision rectangle. HP uses hundredths. */
static int giant_live(int n){return ex.giant.parts[n].hp>0;}
static int giant_open(int n){Giant*b=&ex.giant;int c=b->timer%240;if(!b->active||b->timer<120||b->morph||!giant_live(n)||giant_add_count())return 0;
 if(b->archetype==4)return b->parts[n].visible&&(n!=6||b->partsDestroyed>=4);
 if(n==6)return !giant_live(0)&&!giant_live(1)&&!giant_live(2)&&!giant_live(3)&&!giant_live(4)&&!giant_live(5)&&!giant_live(7)&&!giant_live(8)&&(b->archetype!=1||b->attackClock>=180||b->attackClock<60);
 if(n>=7)return 1;
 if(n>=2&&n<=5&&(giant_live(0)||giant_live(1)))return 0;
 if(b->archetype==2&&n<2)return n==((b->formAge/160)&1);
 if(b->archetype==3&&n<2)return b->attackClock>=80&&b->attackClock<260;
 return 1;}
#include "giant_forms.h"
static void giant_start(void){Giant*b=&ex.giant;*b=(Giant){.active=1,.phase=1,.archetype=g.wave-1,.zoom=.55f,.scale=1,.rayX=160};ex.encounterTier=2;ex.bossSpawned=1;for(int h=0;h<32;h++)ex.env.hazards[h].on=0;for(int n=0;n<MAXB;n++)g.bullets[n].on=0;
 static const int xy[9][2]={{64,53},{256,53},{64,145},{256,145},{110,44},{210,44},{160,114},{110,156},{210,156}};
 for(int i=0;i<9;i++){int hp=i==6?16000+g.wave*1800:i<2?2600:1800;b->parts[i]=(GiantPart){.hp=i>=4&&i!=6?0:hp,.maxHp=hp,.x=xy[i][0],.y=xy[i][1],.w=i==6?38:28,.h=i==6?38:30,.role=i};}
 if(b->archetype==4){for(int i=0;i<9;i++){b->parts[i].maxHp=i==6?24000:2200;b->parts[i].hp=i>=7?0:b->parts[i].maxHp;b->parts[i].w=i==6?40:32;b->parts[i].h=i==6?40:32;}}
 for(int i=0;i<9;i++){b->parts[i].hp=b->parts[i].hp*(100+3*(g.pilotLevel-1))/100;b->parts[i].maxHp=b->parts[i].maxHp*(100+3*(g.pilotLevel-1))/100;}
 if(b->archetype!=4)giant_layout();else{b->scale=1;b->zoom=.68f;}
 if(b->archetype==5){int reduction=clamp(ex.baseDamage,0,8)*150;b->parts[0].hp-=reduction;b->parts[1].hp-=reduction;}
}
static void giant_raw_hit(int n,int damage){Giant*b=&ex.giant;GiantPart*p=b->parts+n;if(!giant_open(n))return;int old=p->hp;p->hp=clamp(p->hp-damage,0,p->maxHp);if(n==6&&b->form<2&&p->hp*3<=p->maxHp*2)p->hp=p->maxHp*2/3;if(p->hp<old)p->damageUntil=g.tick+5;phoenix_charge((old-p->hp)/50);if(old&&p->hp==0){b->partsDestroyed++;g.score+=n==6?5000:500;burst(p->x,p->y,1);}
 if(n==6&&p->hp>0&&b->phase<4&&p->hp*3<=p->maxHp*2)b->phase=4;giant_check_form();
 if(n==6&&p->hp*3<p->maxHp)b->phase=5;
 if(n==6&&!p->hp){ex.giantRewardMask|=1<<(g.wave-1);b->active=0;b->defeated=1;ex.victory=1;ex.victoryX=b->parts[6].x;ex.victoryY=b->parts[6].y;ex.clearScore=g.score-ex.stageScore;ex.clearKills=g.kills-ex.stageKills;ex.clearSalvage=g.salvagePicked-ex.stageSalvage;ex.clearBonus=3000;g.score+=ex.clearBonus;for(int i=0;i<MAXB;i++)g.bullets[i].on=0;}
}
static int giant_resist_mask(int n){return n<2?1<<7:n==4||n==5?1<<8:n==6?(1<<4)|(1<<9):0;}
static void giant_hit_family(int n,int damage,int weapon){if(!giant_open(n))return;GiantPart*p=ex.giant.parts+n;damage=family_resistance(damage,giant_resist_mask(n),weapon,p->x,p->y);giant_raw_hit(n,player_hit_roll(damage*(weapon>=0&&weapon<9?variant_damage_scale(weapon):100)/100,p->x,p->y));}
static void giant_hit(int n,int damage){giant_hit_family(n,damage,g.weapon);}
static void giant_electric_hit(int n,int damage){if(!giant_open(n))return;GiantPart*p=ex.giant.parts+n;damage=family_resistance(damage,giant_resist_mask(n),8,p->x,p->y);int rolled=player_hit_roll(damage*variant_damage_scale(8)/100,p->x,p->y);p->dotTicks=150;p->dotDamage=rolled/10;if(!p->dotClock)p->dotClock=50;giant_raw_hit(n,rolled*70/100);}
static void giant_fan(int x,int y,int speed,int spread){for(int n=-1;n<=1;n++)bullet(x*Q,y*Q,n*spread,speed);}
static void worm_path(int form,int i,int*x,int*y,int*visible,int*angle){Giant*b=&ex.giant;static const int order[9]={1,2,3,4,5,6,0,7,8};int period=form==2?700:form==1?620:600,c=b->timer%period,reverse=(b->timer/period)&1,travel=form==2?340:form==1?280:260;float u=(c-90-order[i]*(form==2?22:26))/(float)travel;int sy=form==0?100:form==1?145:80,ey=form==0?100:form==1?70:155;if(reverse){int swap=sy;sy=ey;ey=swap;}float yy=sy+(ey-sy)*u+sinf(u*6.2831853f)*(form==2?26:form==1?34:24);int start=tunnel_left((int)yy)+6,end=tunnel_right((int)yy)-6;*x=(int)(reverse?end-(end-start)*u:start+(end-start)*u);*y=(int)yy;*visible=u>=0&&u<=1&&*x>tunnel_left(*y)+8&&*x<tunnel_right(*y)-8;float ddy=(ey-sy)*.02f+(sinf((u+.02f)*6.2831853f)-sinf(u*6.2831853f))*(form==2?26:form==1?34:24);*angle=(int)(atan2f(ddy,(reverse?-1:1)*(end-start)*.02f)*57.29578f)+90;}
static void worm_positions(void){Giant*b=&ex.giant;float t=b->morph?giant_morph_fraction():1;for(int i=0;i<9;i++){GiantPart*p=b->parts+i;int x,y,v,angle,ox,oy,ov,oa;worm_path(b->form,i,&x,&y,&v,&angle);worm_path(b->oldForm,i,&ox,&oy,&ov,&oa);p->x=ox+(int)((x-ox)*t);p->y=oy+(int)((y-oy)*t);p->visible=v||(b->morph&&ov);p->angle=angle;float w=i==6?(b->form==2?68:b->form==1?52:40):(b->form==2?56:b->form==1?44:32),h=i==6?w:(b->form==2?44:b->form==1?40:32);p->w=(int)w;p->h=(int)h;}}
#include "giant_attacks.h"
static void giant_step(int mask){Giant*b=&ex.giant;if(!b->active)return;b->timer++;giant_check_form();giant_move_root();int a=b->archetype;
 b->zoom=a==4?(b->form==2?.44f:b->form==1?.56f:.68f):(b->form==2?.32f:b->form==1?.44f:.55f);if(a!=4){if(b->scale<=0)b->scale=1;float target=b->form==2?2.7f:b->form==1?2:1.6f;b->scale+=(target-b->scale)*.025f;giant_layout();}else worm_positions();
 if(b->timer<120){b->attackClock=b->attackKind=0;return;}
 if(b->morph){for(int i=0;i<9;i++){GiantPart*p=b->parts+i;if(p->dotTicks){p->dotTicks--;if(--p->dotClock<=0)p->dotClock=50;}}b->morph--;b->attackKind=0;return;}b->oldForm=b->form;b->formAge++;
 if(a==1&&b->form==1&&!(b->addMask&1)){b->addMask|=1;for(int n=0;n<2;n++)environment_spawn(24,120+n*80,150,0,0,13,4);}
 if(a==4&&b->form==1&&!(b->addMask&2)){b->addMask|=2;environment_spawn(12,120,80,0,0,13,3);environment_spawn(12,200,80,0,0,13,3);}
 if(b->timer>=120)giant_attacks();
 for(int i=0;i<9;i++){GiantPart*p=b->parts+i;if(p->dotTicks){p->dotTicks--;if(--p->dotClock<=0){p->dotClock=50;giant_raw_hit(i,p->dotDamage);}}if(!p->hp||(a==4&&!p->visible))continue;for(int n=0;n<MAXS;n++){Obj*s=g.shots+n;if(!s->on||s->base>0||absn(s->x/Q-p->x)>p->w/2+3||absn(s->y/Q-p->y)>p->h/2+3)continue;int open=giant_open(i);if(open)giant_hit_family(i,(s->type==9?800:projectile_damage(s))*player_attack_scale(s->sourceFamily?s->sourceFamily-1:s->type)/100,s->sourceFamily?s->sourceFamily-1:s->type);s->on=0;pc_impact(p->x,p->y,open?1:3);break;}
  if(g.weapon==8&&(mask&4)&&g.weaponFireCool[8]==weapon_delay(18))giant_electric_hit(i,220*player_attack_scale(8)/100);
  if(!b->active)return;
  if(g.beam&&(g.tick&3)==0&&p->y<player_y/Q&&beam_hits(p->x,p->y,p->w/2))giant_hit_family(i,(ex.phoenixTicks?600:120)*player_attack_scale(ex.multi?5:3)/100,ex.multi?5:3);
  if(ex.bombAge==25)giant_hit_family(i,1200,-1);
  for(int n=0;n<ex.options&&n<5;n++)if(!ex.podContact[n]&&absn(option_x(n)-p->x)<p->w/2+6&&absn(option_y(n)-p->y)<p->h/2+6){giant_hit_family(i,25,0);ex.podContact[n]=20;}
  if(absn(g.px/Q-p->x)<p->w/2+5&&absn(player_y/Q-p->y)<p->h/2+5)hurt_at(g.px/Q,player_y/Q,0,-1);
 }
}
