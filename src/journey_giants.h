#include "giant_pose.h"
#include "giant_contact.h"
#include "giant_anchor.h"
static int giant_opening_ticks(int mission){return boss_entrance_ticks(mission);}
static int giant_opening_selected(int archetype){return archetype>=0&&archetype<6;}
/* Independent collision targets; hull art is not a collision rectangle. HP uses hundredths. */
static int giant_live(int n){return ex.giant.parts[n].hp>0;}
static int giant_open(int n){Giant*b=&ex.giant;int c=b->timer%240;if(!b->active||b->timer<120||b->morph||b->openingTicks||b->growthTicks||!giant_live(n)||giant_add_count())return 0;
 if(b->archetype==4)return n==6&&b->parts[6].visible;if(!giant_part_in_view(n))return 0;
 if(n==6)return !giant_live(0)&&!giant_live(1)&&!giant_live(2)&&!giant_live(3)&&!giant_live(4)&&!giant_live(5)&&!giant_live(7)&&!giant_live(8)&&(b->archetype!=1||b->attackClock>=180||b->attackClock<60);
 if(n>=7)return 1;
 if(n>=2&&n<=5&&(giant_live(0)||giant_live(1)))return 0;
 if(b->archetype==2&&n<2)return n==((b->formAge/160)&1);
 if(b->archetype==3&&n<2)return b->attackClock>=80&&b->attackClock<260;
 return 1;}
#include "giant_forms.h"
static void giant_start(void){Giant*b=&ex.giant;*b=(Giant){.active=1,.phase=1,.archetype=g.wave-1,.zoom=.55f,.scale=1,.rayX=160,.sweepCycle=-1};b->openingTicks=b->openingTotal=giant_opening_selected(b->archetype)?giant_opening_ticks(g.wave):0;ex.encounterTier=2;ex.bossSpawned=1;for(int h=0;h<32;h++)ex.env.hazards[h].on=0;for(int n=0;n<MAXB;n++)g.bullets[n].on=0;
 static const int xy[9][2]={{64,53},{256,53},{64,145},{256,145},{110,44},{210,44},{160,114},{110,156},{210,156}};
 for(int i=0;i<9;i++){int hp=i==6?16000+g.wave*1800:i<2?2600:1800;b->parts[i]=(GiantPart){.hp=i>=4&&i!=6?0:hp,.maxHp=hp,.x=xy[i][0],.y=xy[i][1],.w=i==6?38:28,.h=i==6?38:30,.role=i};}
 if(b->archetype==4){for(int i=0;i<9;i++){b->parts[i].maxHp=i==6?24000:2200;b->parts[i].hp=i==6?b->parts[i].maxHp:0;b->parts[i].w=i==6?40:32;b->parts[i].h=i==6?40:32;}}
 for(int i=0;i<9;i++){b->parts[i].hp=b->parts[i].hp*(100+3*(g.pilotLevel-1))/100;b->parts[i].maxHp=b->parts[i].maxHp*(100+3*(g.pilotLevel-1))/100;}
 if(b->archetype!=4)giant_layout();else{b->scale=1;b->zoom=.78f;}
 for(int i=0;i<9;i++){b->previousPartX[i]=b->parts[i].x;b->previousPartY[i]=b->parts[i].y;b->previousPartAngle[i]=b->parts[i].angle;b->previousPartW[i]=b->parts[i].w;b->previousPartH[i]=b->parts[i].h;}b->previousScale=giant_world_scale();
 if(b->archetype==5){int reduction=clamp(ex.baseDamage,0,8)*150;b->parts[0].hp-=reduction;b->parts[1].hp-=reduction;}
}
static void giant_raw_hit(int n,int damage){Giant*b=&ex.giant;GiantPart*p=b->parts+n;if(!giant_open(n))return;int old=p->hp;p->hp=clamp(p->hp-damage,0,p->maxHp);if(n==6&&b->form<2){int floor=b->archetype==4?p->maxHp*(2-b->form)/3:p->maxHp*2/3;if(p->hp<floor)p->hp=floor;}if(p->hp<old){pc_giant_hit76(organic_context(),b->archetype==5,p->x,p->y);p->damageUntil=g.tick+5;b->woundForm[n]=b->form;if(!p->hp)giant_capture_wound(n);}phoenix_charge((old-p->hp)/50);if(old&&p->hp==0){b->partsDestroyed++;g.score+=n==6?5000:500;burst_kind(p->x,p->y,1,organic_context()?(b->archetype==1&&n==6?2:1):0);}
 if(n==6&&p->hp>0&&b->phase<4&&p->hp*3<=p->maxHp*2)b->phase=4;giant_check_form();
 if(n==6&&p->hp*3<p->maxHp)b->phase=5;
 if(n==6&&!p->hp){ex.giantRewardMask|=1<<(g.wave-1);b->active=0;b->defeated=1;ex.victory=1;ex.victoryX=b->parts[6].x;ex.victoryY=b->parts[6].y;ex.clearScore=g.score-ex.stageScore;ex.clearKills=g.kills-ex.stageKills;ex.clearSalvage=g.salvagePicked-ex.stageSalvage;ex.clearBonus=3000;g.score+=ex.clearBonus;for(int i=0;i<MAXB;i++)g.bullets[i].on=0;}
}
static int giant_resist_mask(int n){return n<2?1<<7:n==4||n==5?1<<8:n==6?(1<<4)|(1<<9):0;}
static void giant_hit_family(int n,int damage,int weapon){if(!giant_open(n))return;int raw=damage,before=ex.giant.parts[n].hp;damage=player_enemy_damage(damage);GiantPart*p=ex.giant.parts+n;damage=family_resistance(damage,giant_resist_mask(n),weapon,p->x,p->y);giant_raw_hit(n,player_hit_roll(damage*(weapon>=0&&weapon<9?variant_damage_scale(weapon):100)/100,p->x,p->y));if(weapon==3&&!ex.phoenixTicks&&!siphon_family84(weapon)&&p->hp>0&&p->hp<before&&!ex.giant.morph&&!ex.giant.openingTicks&&!ex.giant.growthTicks){int event;ice_contact83(&ex.giant.ice83,power_level(3),raw,2,&event);p->ice83.hitTick=g.tick;p->ice83.charge=ex.giant.ice83.charge;if(event)ice_event83(event,p->x,p->y,p->w/2);}if(siphon_active84(weapon)&&before>p->hp)siphon_contact84(&ex.giant.siphon84,(before-clamp(p->hp,0,before))*100,ex.giant.ice83.frozen,2,p->hp>0&&!ex.giant.morph&&!ex.giant.openingTicks&&!ex.giant.growthTicks,p->x,p->y);}
static void giant_hit_nonprimary84(int part,int damage,int weapon){int prior=ex.siphonEcho84;ex.siphonEcho84=1;giant_hit_family(part,damage,weapon);ex.siphonEcho84=prior;}
static void giant_heatseeker_blast82(int x,int y,int radius,int damage){Giant*b=&ex.giant;if(!b->active)return;int eligible[9];for(int i=0;i<9;i++)eligible[i]=giant_open(i);for(int i=0;i<9;i++){GiantPart*p=b->parts+i;int dx=p->x-x,dy=p->y-y,d=dx*dx+dy*dy;if(eligible[i]&&p->hp>0&&d<radius*radius&&missile_line82(x,y,p->x,p->y))giant_hit_family(i,heatseeker_falloff82(damage,d,radius)*player_attack_scale(4)/100,4);}}
static void giant_hit(int n,int damage){giant_hit_family(n,damage,g.weapon);}
static void giant_electric_hit(int n,int damage){if(!giant_open(n))return;GiantPart*p=ex.giant.parts+n;damage=option_emp_damage80(player_enemy_damage(tesla_damage(damage)),p->x,p->y);damage=family_resistance(damage,giant_resist_mask(n),8,p->x,p->y);int rolled=player_hit_roll(damage*variant_damage_scale(8)/100,p->x,p->y);p->dotTicks=150;p->dotDamage=rolled/10;if(!p->dotClock)p->dotClock=50;giant_raw_hit(n,rolled*90/100);}
static Obj*giant_audio_bullet76(int x,int y,int vx,int vy){Obj*o=bullet(x,y,vx,vy);if(o)o->audioClass76=2|(organic_context()?1:ex.giant.archetype==1||ex.giant.archetype==5?4:0);return o;}
static void giant_fan(int x,int y,int speed,int spread){for(int n=-1;n<=1;n++)giant_audio_bullet76(x*Q,y*Q,n*spread,speed);}
#include "worm_long.h"
#include "giant_attacks.h"
static void giant_step(int mask){Giant*b=&ex.giant;if(!b->active)return;int cold=b->ice83.frozen>0&&!b->morph&&!b->openingTicks&&!b->growthTicks;b->previousRootX=b->rootX;b->previousRootY=b->rootY;b->previousRootAngle=b->rootAngle;b->previousScale=giant_world_scale();for(int i=0;i<9;i++){b->previousPartX[i]=b->parts[i].x;b->previousPartY[i]=b->parts[i].y;b->previousPartAngle[i]=b->parts[i].angle;b->previousPartW[i]=b->parts[i].w;b->previousPartH[i]=b->parts[i].h;}if(!cold)b->timer++;if(b->openingTicks)b->openingTicks--;else if(!b->morph&&b->growthTicks)b->growthTicks--;giant_check_form();if(!cold)giant_move_root();int a=b->archetype;
 b->zoom=a==4?.78f:(b->form==2?.32f:b->form==1?.44f:.55f);if(a!=4){float target=b->form==2?2.7f:b->form==1?2:1.6f;if(b->morph)b->scale=b->morphScale+(target-b->morphScale)*giant_morph_fraction();else if(b->timer<120){float t=b->timer/120.0f;t=t*t*(3-2*t);b->scale=1+(target-1)*t;}else b->scale=target;giant_layout();}else worm_positions();
 if(b->timer<120){b->attackClock=b->attackKind=0;return;}
 if(a==4&&!b->morph)worm_body_contact();
 pc_giant_damage();if(b->morph){for(int i=0;i<9;i++){GiantPart*p=b->parts+i;if(p->dotTicks){p->dotTicks--;if(--p->dotClock<=0)p->dotClock=50;}}b->morph--;if(!b->morph){if(b->archetype!=4){b->scale=b->form==2?2.7f:2;giant_layout();}b->growthTicks=300;b->growthShieldSerial++;}b->attackKind=0;return;}b->oldForm=b->form;if(!cold&&b->timer>=120)b->formAge++;
 if(a==1&&b->form==1&&!(b->addMask&1)){b->addMask|=1;for(int n=0;n<2;n++)environment_spawn(24,120+n*80,150,0,0,13,4);}
 if(a==4&&b->form==1&&!(b->addMask&2)){b->addMask|=2;environment_spawn(12,120,80,0,0,13,3);environment_spawn(12,200,80,0,0,13,3);}
 if(!cold&&!b->siphon84.disarm&&!b->siphon84.rearm&&b->timer>=120)giant_attacks();
 for(int i=0;i<9;i++){GiantPart*p=b->parts+i;if(p->dotTicks){p->dotTicks--;if(--p->dotClock<=0){p->dotClock=50;giant_raw_hit(i,p->dotDamage);}}if(!p->hp||(a==4&&!p->visible))continue;for(int n=0;n<MAXS;n++){Obj*s=g.shots+n;if(!s->on||s->type==10||s->base>0||(a==4?!worm_shot_contact(s,p):!giant_projectile_contact(s,i)))continue;int open=giant_open(i);if(s->type==4)detonate_heatseeker(s);else if(open)giant_hit_nonprimary84(i,(s->type==9?800:projectile_damage(s))*player_attack_scale(s->sourceFamily?s->sourceFamily-1:s->type)/100,s->sourceFamily?s->sourceFamily-1:s->type);s->on=0;pc_impact(p->x,p->y,open?1:3);break;}
  if(g.weapon==8&&(mask&4)&&g.weaponFireCool[8]==weapon_delay(18))giant_electric_hit(i,220*player_attack_scale(8)/100);
  if(!b->active)return;
  if(g.beam&&(g.tick&3)==0&&p->y<player_y/Q&&beam_hits(p->x,p->y,p->w/2))giant_hit_family(i,(ex.phoenixTicks?600:beam_target_damage81(120,ex.multi?5:3))*player_attack_scale(ex.multi?5:3)/100,ex.multi?5:3);
  if(ex.optionBeam80&&(g.tick&3)==0){int pods=option_beam_count80(p->x,p->y,p->w/2);if(pods){giant_hit_nonprimary84(i,beam_target_damage81(120,ex.multi?5:3)*pods/5*player_attack_scale(ex.multi?5:3)/100,ex.multi?5:3);}}
  if(ex.bombAge==25)giant_hit_family(i,1200,-1);
  for(int n=0;n<ex.options&&n<5;n++)if(!ex.podContact[n]&&absn(option_x(n)-p->x)<p->w/2+6&&absn(option_y(n)-p->y)<p->h/2+6){giant_hit_family(i,25,0);ex.podContact[n]=20;}

 }
 if(a!=4){int contact=giant_hull_contact();for(int i=0;i<9&&!contact;i++){GiantPart*p=b->parts+i;if(p->hp)contact=giant_module_contact(i);}if(contact)hurt_amount(g.px/Q,player_y/Q,0,-1,100,2);}
}
