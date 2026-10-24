static int option_x(int n){int offset=n==4?0:(n&1?1:-1)*(11+(n/2)*11);return clamp((ex.followX[(ex.followHead-1-(n+1)*8)&63]>>8)+offset,9,311);}
static int option_y(int n){return clamp((ex.followY[(ex.followHead-1-(n+1)*8)&63]>>8)+10+(n/2)*5,34,247);}
static int satellite_x(void){return (g.px>>8)+(sn(g.tick*3)*19>>10);}
static int satellite_y(void){return (player_y>>8)+(sn(g.tick*3+64)*16>>10);}
static void option_fire(void){if(!ex.optionsOn)return;for(int n=0;n<ex.options;n++){Obj*o=slot(g.shots,MAXS);if(o){o->x=option_x(n)*Q;o->y=(option_y(n)-7)*Q;o->vx=(n&1?65:-65)+g.playerVX;o->vy=g.weapon==4?-1300:-2200;o->type=g.weapon==4?4:0;o->phase=-2;o->power=power_level(g.weapon);}}}
static int beam_hits(int x,int radius){int d=absn(x-(g.px>>8));return d<radius||(ex.multi&&(absn(d-28)<radius));}
static void home_shot(Obj*s){int i,best=999999,target=-1;for(i=0;i<MAXE;i++){Obj*o=g.enemies+i;if(o->on&&o->y>12*Q){int d=absn(o->x-s->x)+absn(o->y-s->y);if(d<best){best=d;target=i;}}}if(target>=0){Obj*o=g.enemies+target;int dx=o->x-s->x,dy=o->y-s->y,dist=absn(dx)+absn(dy);if(dist){s->vx+=(divn(dx*1800,dist)-s->vx)>>3;s->vy+=(divn(dy*1800,dist)-s->vy)>>3;}}}
static void hazard(int,int,int);
static int tunnel_center(int);
#include "enemies.h"
static int formation_type(void){static const int table[12][3]={{10,1,14},{12,5,15},{7,13,16},{11,0,17},{6,19,2},{8,9,8},{18,10,20},{21,7,13},{3,12,14},{19,15,16},{14,20,12},{23,24,14}};return table[clamp(ex.phase,0,11)][clamp(g.groups,0,2)];}
static void hazard(int type,int x,int y){Obj*o=slot(ex.obstacles,12);if(o){o->type=type;o->x=x*Q;o->y=y*Q;o->vx=0;o->vy=ex.phase==5?(ex.scrollSpeed>>2):type==2?230:type==3?100:160+(ex.scrollSpeed>>1);o->base=type==1?21:type==3?10:13;o->hp=type==3?2:8;o->phase=rnd()&255;ex.hazardSerial++;}}
static int tunnel_center(int),tunnel_half(int),tunnel_left(int),tunnel_right(int);
static void new_enemy_step(Obj*o){if(o->type>=14){specialist_step(o);return;}if(o->type>=10){bug_step(o);return;}if(o->type>=8){int y;o->y+=o->type==9?(ex.scrollSpeed>>2):340;y=o->y>>8;if(o->type==9)o->x=((o->phase&1)?tunnel_right(y)-9:tunnel_left(y)+9)*Q;else o->x=clamp(tunnel_center(y)+(sn(o->age*3)*(tunnel_half(y)-22)>>10),tunnel_left(y)+12,tunnel_right(y)-12)*Q;if(remn(o->age,96)==64&&y>35&&y<220)aim(o->x,o->y,430);return;}if(o->type==5){o->x=o->base*Q+sn(o->age*3+o->phase)*14;o->y+=90;if(remn(o->age,96)==64&&o->y>30*Q){aim(o->x-8*Q,o->y,450);aim(o->x+8*Q,o->y,450);}}if(o->type==6){o->x=o->base*Q+sn((o->age>>1)+o->phase)*7;o->y-=65;if((o->age&63)==0&&o->y>30*Q){int k;for(k=-1;k<=1;k++)bullet(o->x,o->y,k*180,330);}}if(o->type==7){o->x=o->base*Q+sn(o->age+o->phase)*6;if((o->age&127)==64&&o->y>35*Q&&o->y<175*Q)hazard(3,o->x>>8,o->y>>8);}}
static int tunnel_visible(int y);
static int tunnel_center(int y){int w=(ex.backgroundY>>8)+y;return 160+(sn(w)>>5)+(sn((w>>1)+43)>>6);}
static int tunnel_half(int y){int w=(ex.backgroundY>>8)+y,ch=absn((w&255)-128),pinch=absn(((w+91)&255)-128);return 46+(sn(w*2)>>8)+(ch<62?(62-ch)>>1:0)-(pinch<24?(24-pinch)>>2:0);}
static int tunnel_left(int y){int w=(ex.backgroundY>>8)+y,d=absn(((w+39)&127)-64);return tunnel_center(y)-tunnel_half(y)+(d<20?(20-d)>>1:0);}
static int tunnel_right(int y){int w=(ex.backgroundY>>8)+y,d=absn(((w+87)&127)-64);return tunnel_center(y)+tunnel_half(y)-(d<20?(20-d)>>1:0);}
static void trail_point(int x,int y,int tint){Trail*t=ex.trails+ex.nextTrail;ex.nextTrail++;if(ex.nextTrail==384)ex.nextTrail=0;t->x=x;t->y=y;t->life=g.fx?15:8;t->tint=tint;}
static void trail_segment(int x,int y,int xx,int yy,int tint){trail_point(x,y,tint);if(g.fx){trail_point((x+xx)>>1,(y+yy)>>1,tint);if(absn(xx-x)+absn(yy-y)>8*Q)trail_point((x*3+xx)>>2,(y*3+yy)>>2,tint);}}
static int cave_projectile(Obj*o,int hostile){int x=o->x>>8,y=o->y>>8,l,r;if(y<24||y>=256||!tunnel_visible(y))return 0;l=tunnel_left(y);r=tunnel_right(y);if(x<l||x>r){o->on=0;hit_sparks(clamp(x,l,r),y,x<l?1:-1,0,hostile?2:3);return 1;}return 0;}
static int cruise_speed(int phase){return phase==10?384:phase>=6?192:phase==5?768:phase==0?128:phase==1?80:phase==2?224:phase==3?(g.clock<240?384:640):96;}
static int tunnel_visible(int y){return (ex.phase==5&&(!ex.entryY||y<ex.entryY))||(ex.oldPhase==5&&ex.entryY>0&&y>=ex.entryY);}
static int encounter_deadline(void){return ex.phase==5?900:520;}
static int route_occupied(void){if(ex.rearWarning)return 1;for(int i=0;i<MAXE;i++)if(g.enemies[i].on)return 1;for(int i=0;i<12;i++)if(ex.obstacles[i].on)return 1;return 0;}
static int hostile_rounds_remain(void){for(int i=0;i<MAXB;i++)if(g.bullets[i].on)return 1;return 0;}
static void rear_group(void){for(int n=0;n<3;n++){Obj*o=slot(g.enemies,MAXE);if(o){int x=ex.phase==5?tunnel_center(288)+(n-1)*12:clamp((g.px>>8)+(n-1)*62,28,292);*o=(Obj){.on=1,.type=22,.hp=3+enemy_bonus(),.x=x*Q,.y=(288+n*22)*Q,.base=x,.phase=n*35};}}}
static void detonate_charge(Obj*charge){int x=charge->x>>8,y=charge->y>>8;charge->on=0;pc_charge_burst(x,y);for(int i=0;i<MAXE;i++){Obj*o=g.enemies+i;if(o->on&&!enemy_guarded(o)&&absn((o->x>>8)-x)<25&&absn((o->y>>8)-y)<25){damage_enemy(o,160);if(o->hp<=0)killed(o);}}for(int i=0;i<MAXB;i++){Obj*o=g.bullets+i;if(o->on&&!o->type&&absn((o->x>>8)-x)<22&&absn((o->y>>8)-y)<22)o->on=0;}}
static void dropper_fire(void){int side=ex.phase&1;for(int n=-1;n<=1;n++){Obj*o=slot(g.shots,MAXS);if(o)*o=(Obj){.on=1,.type=8,.x=g.px+(side?8:n*3)*Q,.y=player_y+(side?n*3:8)*Q,.vx=side?180:n*140,.vy=side?n*140:70,.phase=side,.power=1};}}
static void satellite_step(void){if(!ex.satellite)return;int x=satellite_x(),y=satellite_y();if(ex.satelliteCool)ex.satelliteCool--;for(int i=0;i<MAXB;i++){Obj*o=g.bullets+i;if(o->on&&!o->type&&absn((o->x>>8)-x)<7&&absn((o->y>>8)-y)<7){o->on=0;pc_impact(x,y,3);}}if(!ex.satelliteCool)for(int i=0;i<MAXE;i++){Obj*o=g.enemies+i;if(o->on&&!enemy_guarded(o)&&absn((o->x>>8)-x)<13&&absn((o->y>>8)-y)<13){damage_enemy(o,150);ex.satelliteCool=15;pc_impact(x,y,3);if(o->hp<=0)killed(o);break;}}}
static void expansion_step(int mask,int ch){int i,j,phase=remn(g.wave-1,12),targetScroll,targetEnemy;
 if(ex.phase<0){ex.phase=phase;ex.oldPhase=phase==5?0:phase;ex.entryY=phase==5?24:0;ex.phaseAge=0;ex.transition=0;if(phase==5){g.groups=3;g.shield=100;}}
 else if(phase!=ex.phase&&!ex.travel){ex.nextPhase=phase;ex.travel=1;ex.travelTick=0;pc_transition();/* Keep every live enemy, shot, pickup and hazard moving through the handover. */g.shield=120;}
 if(ex.travel){ex.travelTick++;if(ex.travel==1&&ex.travelTick>=90){ex.oldPhase=ex.phase;ex.phase=ex.nextPhase;ex.entryY=24;ex.entryDistance=0;ex.travel=2;ex.travelTick=0;ex.bossSpawned=0;ex.bossWarning=0;ex.clearTicks=0;ex.rearSpawned=0;ex.rearWarning=0;ex.phaseAge=0;if(ex.phase==5)g.groups=3;}else if(ex.travel==2&&ex.travelTick>=90&&!ex.entryY&&ex.scrollSpeed<=cruise_speed(ex.phase)+48){ex.travel=0;ex.travelTick=0;ex.phaseAge=0;g.shield=50;}}
 phase=ex.phase;
 targetScroll=ex.travel?((cruise_speed(ex.oldPhase)+cruise_speed(ex.nextPhase))/2):cruise_speed(phase);
 targetEnemy=phase==5?192:phase==1?192:phase==2?288:phase==3?352:256;
 if(ex.scrollSpeed<targetScroll)ex.scrollSpeed=clamp(ex.scrollSpeed+(ex.travel?64:16),0,targetScroll);else if(ex.scrollSpeed>targetScroll)ex.scrollSpeed=clamp(ex.scrollSpeed-(ex.travel?48:16),targetScroll,8192);
 if(ex.enemySpeed<targetEnemy)ex.enemySpeed+=4;else if(ex.enemySpeed>targetEnemy)ex.enemySpeed-=4;
 /* Subtracting world position moves scenery down toward the ship: forward flight. */
 ex.backgroundY=(ex.backgroundY-(ex.scrollSpeed>>2))&131071;ex.landScroll=(ex.landScroll+ex.scrollSpeed/2)&16777215;
 if(ex.entryY){ex.entryDistance+=clamp(ex.scrollSpeed>>2,512,2048);ex.entryY=24+(ex.entryDistance>>8);if(ex.entryY>=256){ex.entryY=0;ex.oldPhase=ex.phase;}}
 if(!ex.travel)ex.phaseAge++;
 if(ex.pickupTicks)ex.pickupTicks--;
 if(!ex.travel&&!ex.bossSpawned&&!ex.rearSpawned&&g.clock>=260){ex.rearSpawned=1;ex.rearWarning=60;}
 if(ex.rearWarning&&--ex.rearWarning==0)rear_group();
 satellite_step();if(ex.dropperCool)ex.dropperCool--;if(ex.dropper&&(mask&4)&&!ex.dropperCool){dropper_fire();ex.dropperCool=60;}
 if(ex.bombFlash)ex.bombFlash--;
 for(i=0;i<384;i++)if(ex.trails[i].life)ex.trails[i].life--;
 for(i=0;i<12;i++)if(ex.impacts[i].on&&++ex.impacts[i].age>9)ex.impacts[i].on=0;
 ex.followX[ex.followHead]=g.px;ex.followY[ex.followHead]=player_y;ex.followHead=(ex.followHead+1)&63;
 if(tunnel_visible(player_y>>8)&&!ex.travel&&ex.phaseAge>50){int yy,px=g.px>>8;for(yy=(player_y>>8)-5;yy<=(player_y>>8)+5;yy+=5){int l=tunnel_left(yy),r=tunnel_right(yy);if(px-6<l){if((g.tick&3)==0||!g.shield)hurt_at(clamp(l,px-6,px+6),yy,1,0);break;}if(px+6>r){if((g.tick&3)==0||!g.shield)hurt_at(clamp(r,px-6,px+6),yy,-1,0);break;}}}
 if(phase>=6&&phase<10&&player_y>222*Q)player_y-=Q;
 if(phase==11&&!ex.travel)g.px=clamp(g.px,14*Q,278*Q);
 if(ch=='o')ex.optionsOn=!ex.optionsOn;
 if(ex.optionCool)ex.optionCool--;if(!ex.optionCool&&ex.optionsOn){option_fire();ex.optionCool=14;}
 if(ch=='b'&&ex.bombs&&!ex.bombAge){ex.bombs--;ex.bombSerial++;ex.bombAge=1;ex.bombX=g.px>>8;ex.bombY=player_y>>8;ex.bombRadius=0;g.shield=80;g.shake=5;pc_bomb_start(ex.bombX,ex.bombY);sound(3,2,127,0x2200);sound_slide(3,1,0x1100,65);sound_slide(3,0,0,110);}
 bomb_step();
 /* Cavern encounters are placed inside the live corridor, not over solid rock. */
 if(phase==5&&!ex.travel&&!ex.bossSpawned&&g.clock<860&&ex.phaseAge>75&&remn(ex.phaseAge,110)==0){Obj*o=slot(g.enemies,MAXE);if(o){o->type=(ex.phaseAge&128)?9:8;o->hp=o->type==9?5:3;o->phase=ex.hazardSerial++&1;o->x=tunnel_center(-32)*Q;o->y=-32*Q;o->vy=o->type==9?(ex.scrollSpeed>>2):340;}}
 if(phase==5&&!ex.travel&&!ex.bossSpawned&&g.clock<860&&ex.phaseAge>140&&remn(ex.phaseAge,170)==0&&tunnel_right(-12)-tunnel_left(-12)>128)hazard(0,tunnel_center(-12),-12);
 if(phase>=6&&phase<10&&!ex.travel&&!ex.bossSpawned&&g.clock<encounter_deadline()&&g.clock>100&&remn(g.clock,110)==0)hazard(phase==9?3:0,30+remn(ex.hazardSerial*89,260),-18);
 /* Telegraph each hazard at the top edge while it enters. Gate gaps stay at least 104 pixels wide. */
 if(!ex.travel&&!ex.bossSpawned&&phase==2&&g.clock<encounter_deadline()&&g.clock>60&&remn(g.clock,64)==0)hazard((ex.hazardSerial&3)==0?1:0,26+remn(ex.hazardSerial*79,268),-26);
 if(!ex.travel&&!ex.bossSpawned&&phase==3&&g.clock<encounter_deadline()&&g.clock>80&&remn(g.clock,150)==0)hazard(2,80+remn(ex.hazardSerial*67,160),-16);
 for(i=0;i<12;i++){Obj*o=ex.obstacles+i;int x,y;if(!o->on)continue;o->age++;o->y+=o->vy;x=o->x>>8;y=o->y>>8;
  if(y>288){o->on=0;continue;}
  if(o->type==2){if(absn(y-(player_y>>8))<12&&absn(x-(g.px>>8))>40)hurt_at(g.px>>8,clamp(y,(player_y>>8)-7,(player_y>>8)+7),0,y<(player_y>>8)?1:-1);continue;}
  for(j=0;j<MAXS;j++){Obj*s=g.shots+j;if(s->on&&absn((s->x>>8)-x)<o->base&&absn((s->y>>8)-y)<o->base){s->on=0;o->hp--;hit_sparks(s->x>>8,s->y>>8,0,1,1);}}
  if(g.beam&&beam_hits(x,o->base)&&(g.tick&3)==0&&y<(player_y>>8))o->hp--;
  if(o->hp<=0){o->on=0;g.score+=25;burst(x,y,0);continue;}
  if(absn(x-(g.px>>8))<o->base+4&&absn(y-(player_y>>8))<o->base+5){int pt[2];surface_point(g.px>>8,player_y>>8,6,7,x,y,pt);hurt_at(pt[0],pt[1],(g.px>>8)>x?1:-1,0);o->on=0;burst(x,y,0);}
 }
}
