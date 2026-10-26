static int option_x(int n){int offset=n==4?0:(n&1?1:-1)*(11+(n/2)*11),x=ex.followX[(ex.followHead-1-(n+1)*8)&63]>>8,y=clamp((ex.followY[(ex.followHead-1-(n+1)*8)&63]>>8)+10+(n/2)*5,34,247);return cavern_option_x(x,y,offset); }
static int option_y(int n){return clamp((ex.followY[(ex.followHead-1-(n+1)*8)&63]>>8)+10+(n/2)*5,34,247);}
static int satellite_x(void){return (g.px>>8)+(sn(g.tick*3)*19>>10);}
static int satellite_y(void){return (player_y>>8)+(sn(g.tick*3+64)*16>>10);}
static void option_fire(void){if(!ex.optionsOn||ex.empDisable)return;for(int n=0;n<ex.options;n++){Obj*o=slot(g.shots,MAXS);if(o){o->x=option_x(n)*Q;o->y=(option_y(n)-7)*Q;o->vx=(n&1?65:-65)+g.playerVX;o->vy=g.weapon==4?-1300:-2200;o->type=g.weapon==4?4:0;o->phase=-2;o->power=power_level(g.weapon);}}}
static int beam_half_width(void){if(ex.phoenixTicks)return flight_side_mode()?58:80;static const int single[4]={3,6,11,18},triple[4]={3,5,9,14};return (ex.multi?triple:single)[power_level(ex.multi?5:3)-1];}
static int beam_slope(int lane){int fan=ex.multi&&!ex.phoenixTicks?lane*(8+4*(power_level(5)-1))*256/100:0;int angle=clamp(ex.beamAim+fan,-256,256);return flight_side_mode()?angle*960*960/(696*696):angle;}
static int beam_center(int lane,int y){return g.px/Q+lane*6+(player_y/Q-7-y)*beam_slope(lane)/256;}
static int beam_hits(int x,int y,int radius){for(int lane=ex.multi?-1:0;lane<=(ex.multi?1:0);lane++){float slope=beam_slope(lane)/256.0f;if(absn(x-beam_center(lane,y))<(radius+beam_half_width())*sqrtf(1+slope*slope))return 1;}return 0;}
static void home_shot(Obj*s){int i,best=999999,target=-1;for(i=0;i<MAXE;i++){Obj*o=g.enemies+i;if(o->on&&enemy_damageable(o)){int d=absn(o->x-s->x)+absn(o->y-s->y);if(d<best){best=d;target=i;}}}if(target>=0){Obj*o=g.enemies+target;int dx=o->x-s->x,dy=o->y-s->y,dist=absn(dx)+absn(dy);if(dist){s->vx+=(divn(dx*1800,dist)-s->vx)>>3;s->vy+=(divn(dy*1800,dist)-s->vy)>>3;}}}
static void hazard(int,int,int);
static int tunnel_center(int);
static int tunnel_lane(int,int);
static int tunnel_left(int),tunnel_right(int),tunnel_visible(int);
#include "floor_mount.h"
#include "enemies.h"
static int formation_type(void){if(opening_front()&&g.groups<2)return g.groups?12:10;static const int table[17][3]={{10,1,14},{12,5,15},{7,13,16},{11,0,17},{6,19,2},{8,9,8},{18,10,20},{21,7,13},{3,12,14},{19,15,16},{14,20,12},{23,24,14},{12,14,18},{15,19,25},{20,21,15},{13,18,17},{19,16,25}};if(!organic_context()){int event=g.groups%12;if(event==3)return 16;if(event==5)return 27;if(event==9)return 28;if(event==11)return 25;}if(g.groups==7)return 26;if(g.groups%9==8)return 25;if(g.groups%6==2)return 10+(ex.phase+g.groups/3)%4;if(g.groups%6==4)return 14+(ex.phase+g.groups)%8;return table[clamp(ex.phase,0,16)][g.groups%3];}
static void hazard(int type,int x,int y){Obj*o=slot(ex.obstacles,MAXH);if(o){o->type=type;o->x=x*Q;o->y=y*Q;o->vx=0;o->vy=ex.phase==5?(ex.scrollSpeed>>2):type==2?230:type==3?100:160+(ex.scrollSpeed>>1);o->base=type==1?21:type==3?10:13;o->hp=type==3?2:8;o->phase=rnd()&255;ex.hazardSerial++;cavern_prepare_obstacle(o);}}
static int tunnel_center(int),tunnel_half(int),tunnel_left(int),tunnel_right(int);
static void new_enemy_step(Obj*o){if(o->type>=14){specialist_step(o);return;}if(o->type>=10){bug_step(o);return;}if(o->type>=8){int y;if(o->type==9)floor_mount_step(o);else{o->y+=340;o->x=tunnel_lane(o->y/Q,o->phase&1)*Q;}y=o->y/Q;if(remn(o->age,96)==64&&y>35&&y<220){if(o->type==9)floor_fire(o,430);else aim(o->x,o->y,430);}return;}if(o->type==5){o->x=o->base*Q+sn(o->age*3+o->phase)*14;o->y+=90;if(remn(o->age,96)==64&&o->y>30*Q){aim(o->x-8*Q,o->y,450);aim(o->x+8*Q,o->y,450);}}if(o->type==6){o->x=o->base*Q+sn((o->age>>1)+o->phase)*7;o->y-=65;if((o->age&63)==0&&o->y>30*Q){int k;for(k=-1;k<=1;k++)bullet(o->x,o->y,k*180,330);}}if(o->type==7){o->x=o->base*Q+sn(o->age+o->phase)*6;if((o->age&127)==64&&o->y>35*Q&&o->y<175*Q)hazard(3,o->x>>8,o->y>>8);}}
static int tunnel_visible(int y);
/* Long smooth chambers and bottlenecks, continuous across the 512-unit world wrap. */
#include "cavern_profile.h"
static int tunnel_center(int y){if(ex.giant.active){if(ex.giant.archetype!=4)return 160;int w=(ex.backgroundY>>8)+y;return 160+(sn(w>>1)>>6)+(sn(w)>>7);}return (int)cavern_phrase(y,1);}
static int tunnel_half(int y){if(ex.giant.active)return ex.giant.archetype==4?86:1500;float half=cavern_phrase(y,0),island=cavern_island_radius(y);if(island>0)half=fmaxf(half,86+island);float arena=ex.bossWarning?(100-ex.bossWarning)/100.0f:ex.bossSpawned?1:0;return (int)(half+(155-half)*arena);}
static int tunnel_left(int y){int w=ex.giant.active?(ex.backgroundY>>8)+y:(int)cavern_world(y);return tunnel_center(y)-tunnel_half(y)+(sn(w*2+13)+1024)/512;}
static int tunnel_right(int y){int w=ex.giant.active?(ex.backgroundY>>8)+y:(int)cavern_world(y);return tunnel_center(y)+tunnel_half(y)-(sn(w*2+97)+1024)/512;}
static int tunnel_island(int y,int*l,int*r){int radius=(int)cavern_island_radius(y);if(radius<2)return 0;int center=tunnel_center(y);*l=center-radius;*r=center+radius;return 1;}
static int tunnel_lane(int y,int side){int l,r;if(tunnel_island(y,&l,&r))return side?(r+tunnel_right(y))/2:(tunnel_left(y)+l)/2;return tunnel_center(y);}
#include "cavern_guard.h"
static int cavern_speed(void){int w=((ex.backgroundY>>8)+(player_y>>8))&511;return w>=350&&w<448?1536:w>=112&&w<288?3072:w<96?3840:2304;}
static void cavern_crash(int x,int y){hurt_amount(x,y,0,-1,75,3);int yy=player_y/Q,l=tunnel_left(yy)+12,r=tunnel_right(yy)-12,il,ir;if(tunnel_island(yy,&il,&ir)&&g.px/Q+8>il&&g.px/Q-8<ir)g.px=tunnel_lane(yy,g.px/Q>tunnel_center(yy))*Q;g.px=clamp(g.px,l*Q,r*Q);}

static void trail_point(int x,int y,int tint){Trail*t=ex.trails+ex.nextTrail;ex.nextTrail++;if(ex.nextTrail==384)ex.nextTrail=0;t->x=x;t->y=y;t->life=g.fx?15:8;t->tint=tint;}
static void trail_segment(int x,int y,int xx,int yy,int tint){trail_point(x,y,tint);if(g.fx){trail_point((x+xx)>>1,(y+yy)>>1,tint);if(absn(xx-x)+absn(yy-y)>8*Q)trail_point((x*3+xx)>>2,(y*3+yy)>>2,tint);}}
static int cave_projectile(Obj*o,int hostile){int x=o->x>>8,y=o->y>>8,l,r;if(y<24||y>=256||!tunnel_visible(y))return 0;l=tunnel_left(y);r=tunnel_right(y);int il,ir,island=tunnel_island(y,&il,&ir)&&x>il&&x<ir;if(x<l||x>r||island){o->on=0;hit_sparks(clamp(x,l,r),y,x<l?1:-1,0,hostile?2:3);if(o->type!=8)pc_hit_sound(ex.phase==15?3:2,o->type,x);if(o->type==7)pc_reflection_sound(ex.phase==15?3:2,0,x);return 1;}return 0;}
static int cruise_speed(int phase){return cavern_phase(phase)?3072:phase==10?384:phase>=12?320:phase>=6?192:phase==5?3072:phase==4?1536:phase==0?128:phase==1?80:phase==2?224:phase==3?(g.clock<240?384:640):96;}
/* Eight-second pace phrases, blended over two seconds without position jumps. */
static int section_speed(int phase){int base=cruise_speed(phase);if(ex.bossSpawned||ex.bossWarning)return cavern_phase(phase)?640:clamp(base/2,64,192);if(cavern_phase(phase))return cavern_speed();if(ex.intro)return base*176/256;if(ex.victory)return base/2;static const int pace[6]={180,256,384,224,448,208};int beat=g.clock/400,slot=(beat+phase*2)%6,t=clamp(g.clock%400,0,100);t=t*t*(300-2*t)/10000;int from=pace[(slot+5)%6],to=pace[slot];return base*(from+(to-from)*t/100)/256;}
static int tunnel_visible(int y){if(ex.giant.active&&ex.giant.archetype!=4)return 0;return (cavern_phase(ex.phase)&&(!ex.entryY||y<ex.entryY))||(cavern_phase(ex.oldPhase)&&ex.entryY>0&&y>=ex.entryY);}
static int encounter_deadline(void){return section_duration();}
static int storm_kind(void){if(custom_environment()||environment_exclusive())return 0;return !ex.travel&&!ex.intro&&!ex.victory&&!ex.bossSpawned&&g.clock>=120&&g.clock<600?(ex.phase==2?1:ex.phase==8?2:ex.phase==9?3:ex.phase>=12?ex.phase-8:0):0;}
static int storm_hp(int kind,int radius){if(kind==2||kind==4)return radius>=22?2:1;if(kind==5)return radius>=22?5:3;if(kind==6||kind==8)return radius>=22?3:2;if(kind==7)return radius>=22?4:2;return radius>=24?3:2;}
static void storm_step(void){int kind=storm_kind();if(!kind||cavern_turn_pending()||ex.cavernTurn)return;int dense=g.clock>=240&&g.clock<540;if(ex.phase==2||ex.phase==15){if(g.clock%(dense?40:75))return;for(int n=0;n<2;n++){Obj*o=slot(ex.obstacles,MAXH);if(!o)break;int y=-32-n*35,x=tunnel_lane(y,n),radius=9+(ex.hazardSerial%7);*o=(Obj){.on=1,.type=kind+3,.x=x*Q,.y=y*Q,.vy=650,.base=radius,.hp=storm_hp(kind,radius),.phase=ex.hazardSerial++};}return;}if(dense){int spacing=kind==5?60:40;if(g.clock%spacing)return;int free=0;for(int n=0;n<MAXH;n++)free+=!ex.obstacles[n].on;if(free<8)return;static const int widths[8]={32,44,36,52,40,48,36,32};int edge=0;for(int n=0;n<8;n++){Obj*o=slot(ex.obstacles,MAXH);int width=widths[(n+(g.clock/40)%8)%8],radius=width/2+2;*o=(Obj){.on=1,.type=kind+3,.x=(edge+width/2)*Q,.y=-32*Q,.vy=700,.base=radius,.hp=storm_hp(kind,radius),.phase=ex.hazardSerial++*23};edge+=width;}return;}if(g.clock%14)return;int gap=48+remn((g.clock/90)*67,224);for(int n=0;n<2;n++){int x=18+remn(ex.hazardSerial*73+n*113,284);if(absn(x-gap)<29)continue;Obj*o=slot(ex.obstacles,MAXH);if(!o)break;int radius=6+remn(ex.hazardSerial*7,21);*o=(Obj){.on=1,.type=kind+3,.x=x*Q,.y=(-28-n*27)*Q,.vx=(remn(ex.hazardSerial,3)-1)*40,.vy=650+remn(ex.hazardSerial*37,400),.base=radius,.hp=storm_hp(kind,radius),.phase=ex.hazardSerial++*23};}}
static int enemy_count(void){int count=0;for(int i=0;i<MAXE;i++)count+=g.enemies[i].on;return count;}
static int route_occupied(void){if(ex.rearWarning)return 1;for(int i=0;i<MAXE;i++)if(g.enemies[i].on)return 1;for(int i=0;i<MAXH;i++)if(ex.obstacles[i].on)return 1;return 0;}
static int hostile_rounds_remain(void){for(int i=0;i<MAXB;i++)if(g.bullets[i].on)return 1;return 0;}
static void rear_group(void){for(int n=0;n<3;n++){Obj*o=slot(g.enemies,MAXE);if(o){int x=ex.phase==5?tunnel_lane(288,n&1)+(n-1)*2:clamp((g.px>>8)+(n-1)*62,28,292);*o=(Obj){.on=1,.type=22,.hp=3+enemy_bonus(),.x=x*Q,.y=(288+n*22)*Q,.base=x,.phase=n*35};cavern_prepare_enemy(o,1);}}}
static void detonate_charge(Obj*charge){int before=g.kills;int x=charge->x>>8,y=charge->y>>8;charge->on=0;pc_charge_burst(x,y);for(int i=0;i<MAXE;i++){Obj*o=g.enemies+i;if(o->on&&(!enemy_guarded(o)||o->type==4)&&absn((o->x>>8)-x)<25&&absn((o->y>>8)-y)<25){damage_enemy(o,160);if(o->hp<=0)killed(o);}}for(int i=0;i<MAXB;i++){Obj*o=g.bullets+i;if(o->on&&!o->type&&absn((o->x>>8)-x)<22&&absn((o->y>>8)-y)<22)o->on=0;}aoe_combo(g.kills-before,0);}
static void detonate_torpedo(Obj*shot){int before=g.kills;int x=shot->x/Q,y=shot->y/Q,radius=48;shot->on=0;ex.torpedoBlasts++;g.shake=4;pc_torpedo_blast(x,y);pc_torpedo_sound(1,x);ex.soundTick=g.tick;
 for(int i=0;i<MAXE;i++){Obj*o=g.enemies+i;int dx=o->x/Q-x,dy=o->y/Q-y,d=dx*dx+dy*dy;if(o->on&&d<radius*radius&&(!enemy_guarded(o)||o->type==4)){weapon_damage_enemy(o,800-500*d/(radius*radius),9);if(o->hp<=0)killed(o);}}
 for(int i=0;i<MAXH;i++){Obj*o=ex.obstacles+i;int dx=o->x/Q-x,dy=o->y/Q-y;if(o->on&&o->type!=2&&dx*dx+dy*dy<radius*radius){o->hp-=8;if(o->hp<=0){o->on=0;g.score+=25;pc_burst(o->x/Q,o->y/Q,0);}}}
 for(int i=0;i<MAXB;i++){Obj*o=g.bullets+i;int dx=o->x/Q-x,dy=o->y/Q-y;if(o->on&&!o->type&&dx*dx+dy*dy<radius*radius)o->on=0;}aoe_combo(g.kills-before,0);}
/* One missile detonation applies one radial damage event per unit, no direct-hit double dip. */
static void detonate_heatseeker(Obj*shot){int x=shot->x/Q,y=shot->y/Q,rank=clamp(shot->power,1,4),radius=40+(rank-1)*6,damage=projectile_damage(shot),before=g.kills;shot->on=0;ex.missileBlasts++;pc_heatseeker_blast(x,y,radius);pc_torpedo_sound(1,x);ex.soundTick=g.tick;if(g.shake<3)g.shake=3;
 for(int i=0;i<MAXE;i++){Obj*o=g.enemies+i;int dx=o->x/Q-x,dy=o->y/Q-y,d=dx*dx+dy*dy;if(o->on&&d<radius*radius&&(!enemy_guarded(o)||o->type==4)){weapon_damage_enemy(o,damage*(100-65*d/(radius*radius))/100,4);if(o->hp<=0)killed(o);}}
 for(int i=0;i<MAXH;i++){Obj*o=ex.obstacles+i;int dx=o->x/Q-x,dy=o->y/Q-y,d=dx*dx+dy*dy;if(o->on&&o->type!=2&&d<radius*radius){int hit=damage*(100-65*d/(radius*radius))/100;o->wound+=hit;o->hp-=o->wound/100;o->wound%=100;if(o->hp<=0){o->on=0;g.score+=25;pc_crush_sound(hazard_material(o),o->base,o->x/Q);pc_burst(o->x/Q,o->y/Q,0);}}}
 aoe_combo(g.kills-before,0);
}
static int torpedo_fire(void){Obj*o=slot(g.shots,MAXS);if(!o){int oldest=-1;for(int i=0;i<MAXS;i++)if(g.shots[i].type!=9&&(oldest<0||g.shots[i].age>g.shots[oldest].age))oldest=i;if(oldest>=0)o=g.shots+oldest;}if(!o)return 0;*o=(Obj){.on=1,.type=9,.x=g.px,.y=player_y-12*Q,.vx=g.playerVX/3,.vy=-900,.power=1};ex.torpedoSerial++;pc_torpedo_sound(0,g.px/Q);return 1;}
static void dropper_fire(void){int side=flight_side_mode();for(int n=-1;n<=1;n++){Obj*o=slot(g.shots,MAXS);if(o)*o=(Obj){.on=1,.type=8,.x=g.px+(side?8:n*3)*Q,.y=player_y+(side?n*3:8)*Q,.vx=side?180:n*140,.vy=side?n*140:70,.phase=side,.power=1};}}
static void chaff_deploy(void){if(!ex.chaff)return;ex.chaff--;ex.chaffDeploy=50;pc_chaff_deploy();for(int n=0;n<48;n++){Obj*m=slot(ex.mines,96);if(!m)break;int tx=20+(n%8)*40+(rnd()%15-7),ty=44+(n/8)*33+(rnd()%13-6);if(cavern_phase(ex.phase))tx=tunnel_lane(ty,n&1);if(ex.phase==11&&!custom_environment())tx=clamp(tx,16,270);*m=(Obj){.on=1,.x=g.px+(n&1?5:-5)*Q,.y=player_y-6*Q,.vx=(tx*Q-g.px)/35,.vy=(ty*Q-player_y)/35,.type=0,.base=tx,.phase=ty};}}
static void mine_detonate(Obj*m){int before=g.kills;int x=m->x/Q,y=m->y/Q;m->on=0;pc_mine_blast(x,y);if(g.shake<2)g.shake=2;for(int n=0;n<MAXE;n++){Obj*o=g.enemies+n;int dx=o->x/Q-x,dy=o->y/Q-y,r=o->type==4?36+o->power/24:17;if(o->on&&dx*dx+dy*dy<r*r&&(!enemy_guarded(o)||o->type==4)){damage_enemy(o,o->type==4?200:800);if(o->hp<=0)killed(o);}}aoe_combo(g.kills-before,0);}
static void chaff_step(void){if(ex.chaffDeploy)ex.chaffDeploy--;for(int i=0;i<96;i++){Obj*m=ex.mines+i;if(!m->on)continue;m->age++;if(m->age<36){m->x+=m->vx;m->y+=m->vy;}else{m->vx=m->vx*7/8;m->vy=32;m->x+=m->vx;m->y+=m->vy;}if(m->age>=350||m->y>268*Q){m->on=0;continue;}if(m->age<12)continue;for(int n=0;n<MAXE;n++){Obj*o=g.enemies+n;int dx=o->x/Q-m->x/Q,dy=o->y/Q-m->y/Q,r=o->type==4?32+o->power/24:11;if(o->on&&dx*dx+dy*dy<r*r){mine_detonate(m);break;}}}}
static void satellite_step(void){if(!ex.satellite)return;int x=satellite_x(),y=satellite_y();if(ex.satelliteCool)ex.satelliteCool--;for(int i=0;i<MAXB;i++){Obj*o=g.bullets+i;if(o->on&&!o->type&&absn((o->x>>8)-x)<7&&absn((o->y>>8)-y)<7){o->on=0;pc_impact(x,y,3);}}if(!ex.satelliteCool)for(int i=0;i<MAXE;i++){Obj*o=g.enemies+i;if(o->on&&(!enemy_guarded(o)||o->type==4)&&absn((o->x>>8)-x)<13&&absn((o->y>>8)-y)<13){damage_enemy(o,150);ex.satelliteCool=15;pc_impact(x,y,3);if(o->hp<=0)killed(o);break;}}}
static void expansion_step(int mask,int ch){if(ex.phoenixTicks){ex.phoenixTicks--;if(!ex.phoenixTicks)g.beam=0;}if(ch=='x'&&ex.phoenixMeter>=PHOENIX_CHARGE_MAX&&!ex.phoenixTicks&&!ex.travel&&!ex.victory){ex.phoenixMeter=0;ex.phoenixTicks=300;ex.phoenixSerial++;ex.empDisable=0;g.beam=0;pc_bomb_start(g.px/Q,player_y/Q);pc_torpedo_sound(1,g.px/Q);}
 int i,j,phase=mission_phase(g.wave),targetScroll,targetEnemy;
 if(ex.repairCool)ex.repairCool=clamp(ex.repairCool-(ex.buffTicks[2]&&g.tick%4==0?2:1),0,1500);if(ex.repairFlash)ex.repairFlash--;if(ch=='h')repair_ship();
 if(ex.phase<0){ex.intro=400;ex.stageScore=g.score;ex.stageKills=g.kills;ex.stageSalvage=g.salvagePicked;ex.phase=phase;ex.cavernSide=phase==15;ex.cavernLeg=0;ex.cavernTurn=0;ex.oldPhase=phase==5?0:phase;ex.entryY=phase==5?24:0;ex.phaseAge=0;ex.transition=0;if(phase==5){g.shield=100;}}
 else if((phase!=ex.phase||ex.sectionHandover)&&!ex.travel){ex.sectionHandover=0;ex.nextPhase=phase;ex.travel=1;ex.travelTick=0;pc_transition();/* Keep every live enemy, shot, pickup and hazard moving through the handover. */g.shield=120;}
 if(ex.travel){ex.travelTick++;if(ex.travel==1&&ex.travelTick>=90){ex.oldPhase=ex.phase;ex.phase=ex.nextPhase;ex.cavernSide=ex.phase==15;ex.cavernLeg=0;ex.cavernTurn=0;ex.shieldDrops=0;ex.empBombs=1;ex.empStop=0;player_y=(ex.missionHandover?140:230)*Q;g.px=160*Q;ex.intro=ex.missionHandover?250:80;ex.victory=0;ex.stageScore=g.score;ex.stageKills=g.kills;ex.stageSalvage=g.salvagePicked;ex.entryY=24;ex.entryDistance=0;ex.travel=2;ex.travelTick=0;ex.bossSpawned=0;ex.bossWarning=0;ex.clearTicks=0;ex.rearSpawned=0;ex.rearWarning=0;ex.phaseAge=0;}else if(ex.travel==2&&ex.travelTick>=90&&!ex.entryY&&ex.scrollSpeed<=cruise_speed(ex.phase)+48){ex.travel=0;ex.travelTick=0;ex.phaseAge=0;g.shield=50;}}
 if(ex.intro&&!ex.travel){ex.intro--;}
 if(ex.victory){if(ex.victory<351)ex.victory++;g.shield=100;if(ex.victory<85){g.shake=clamp((85-ex.victory)/5,0,12);int radius=ex.victory*7;for(int n=0;n<MAXB;n++){Obj*o=g.bullets+n;if(o->on&&absn(o->x/Q-ex.victoryX)+absn(o->y/Q-ex.victoryY)<radius)o->on=0;}for(int n=0;n<MAXH;n++){Obj*o=ex.obstacles+n;if(o->on&&absn(o->x/Q-ex.victoryX)+absn(o->y/Q-ex.victoryY)<radius){o->on=0;pc_burst(o->x/Q,o->y/Q,0);}}for(int n=0;n<MAXE;n++){Obj*o=g.enemies+n;if(o->on&&absn(o->x/Q-ex.victoryX)+absn(o->y/Q-ex.victoryY)<radius)killed(o);}}if(ex.victory>275&&!ex.travel&&mission_report_ready())player_y-=4*Q;}
 phase=ex.phase;
 targetScroll=ex.travel?((cruise_speed(ex.oldPhase)+cruise_speed(ex.nextPhase))/2):section_speed(phase);
 targetEnemy=phase==5?192:phase==1?192:phase==2?288:phase==3?352:256;
 if(ex.cavernTurn)ex.cavernTurn--;if(cavern_turn_pending()&&!route_occupied()&&!hostile_rounds_remain()){ex.cavernLeg++;ex.cavernSide=!ex.cavernSide;ex.cavernTurn=75;g.shield=75;pc_transition();}
 if(ex.scrollSpeed<targetScroll)ex.scrollSpeed=clamp(ex.scrollSpeed+(ex.travel||cavern_phase(phase)?64:16),0,targetScroll);else if(ex.scrollSpeed>targetScroll)ex.scrollSpeed=clamp(ex.scrollSpeed-(ex.travel||cavern_phase(phase)?64:16),targetScroll,8192);
 if(ex.enemySpeed<targetEnemy)ex.enemySpeed+=4;else if(ex.enemySpeed>targetEnemy)ex.enemySpeed-=4;
 /* Subtracting world position moves scenery down toward the ship: forward flight. */
 if(!ex.cavernWorldReady){ex.cavernWorldQ=ex.backgroundY;ex.cavernWorldReady=1;}ex.cavernWorldQ-=ex.scrollSpeed>>2;ex.backgroundY=(ex.backgroundY-(ex.scrollSpeed>>2))&262143;ex.landScroll=(ex.landScroll+ex.scrollSpeed/2)&16777215;
 if(ex.entryY){ex.entryDistance+=clamp(ex.scrollSpeed>>2,512,2048);ex.entryY=24+(ex.entryDistance>>8);if(ex.entryY>=256){ex.entryY=0;ex.oldPhase=ex.phase;}}
 if(!ex.travel&&!ex.intro)ex.phaseAge++;
 if(ex.pickupTicks)ex.pickupTicks--;
 if(!environment_exclusive()&&!ex.travel&&!ex.bossSpawned&&!ex.rearSpawned&&g.clock>=260){ex.rearSpawned=1;ex.rearWarning=60;}
 if(ex.rearWarning&&--ex.rearWarning==0)rear_group();
 if(ex.torpedoCool)ex.torpedoCool=clamp(ex.torpedoCool-(ex.buffTicks[2]&&g.tick%4==0?2:1),0,150);if((mask&4)&&!ex.torpedoCool&&!ex.empDisable&&!ex.travel&&!ex.intro&&!ex.victory&&torpedo_fire())ex.torpedoCool=150;
 if(ex.empStop)ex.empStop--;if(ch=='e'&&ex.empBombs&&!ex.travel&&!ex.victory){ex.empBombs--;parasites_electric_purge();ex.empStop=200;pc_emp_sound(1,g.px/Q);pc_charge_burst(g.px/Q,player_y/Q);}
 if(ch=='c'&&ex.chaff&&!ex.travel&&!ex.victory)chaff_deploy();chaff_step();
 satellite_step();if(ex.dropperCool)ex.dropperCool--;if(ex.dropper&&!ex.empDisable&&(mask&4)&&!ex.dropperCool){dropper_fire();ex.dropperCool=60;}
 if(ex.bombFlash)ex.bombFlash--;
 for(i=0;i<384;i++)if(ex.trails[i].life)ex.trails[i].life--;
 for(i=0;i<12;i++)if(ex.impacts[i].on&&++ex.impacts[i].age>9)ex.impacts[i].on=0;
 ex.followX[ex.followHead]=g.px;ex.followY[ex.followHead]=player_y;ex.followHead=(ex.followHead+1)&63;
 if(tunnel_visible(player_y>>8)&&!ex.travel&&ex.phaseAge>50){int yy,px=g.px>>8;for(yy=(player_y>>8)-player_hull_ry()+2;yy<=(player_y>>8)+player_hull_ry()-2;yy+=player_hull_ry()-2){int half=yy==(player_y>>8)?player_hull_rx():player_hull_rx()*3/5;int l=tunnel_left(yy),r=tunnel_right(yy),il,ir;if(tunnel_island(yy,&il,&ir)&&px+half>il&&px-half<ir){cavern_crash(px,yy);break;}if(px-half<l){cavern_crash(l,yy);break;}if(px+half>r){cavern_crash(r,yy);break;}}}
 if(phase>=6&&phase<10&&player_y>222*Q)player_y-=Q;
 if(phase==11&&!custom_environment()&&!ex.travel)g.px=clamp(g.px,14*Q,278*Q);
 if(ch=='o')ex.optionsOn=!ex.optionsOn;
 if(ex.optionCool)ex.optionCool--;if(!ex.optionCool&&ex.optionsOn){option_fire();ex.optionCool=14;}
 if(ch=='b'&&ex.bombs&&!ex.bombAge){ex.bombs--;ex.bombSerial++;ex.novaKills=0;ex.bombAge=1;ex.bombX=g.px>>8;ex.bombY=player_y>>8;ex.bombRadius=0;g.shield=80;g.shake=5;pc_bomb_start(ex.bombX,ex.bombY);sound(3,2,127,0x2200);sound_slide(3,1,0x1100,65);sound_slide(3,0,0,110);}
 bomb_step();
 /* Cavern encounters are placed inside the live corridor, not over solid rock. */
 if(!custom_environment()&&!environment_exclusive()&&phase==5&&!ex.travel&&!ex.bossSpawned&&g.clock<encounter_deadline()-40&&ex.phaseAge>75&&remn(ex.phaseAge,110)==0){Obj*o=slot(g.enemies,MAXE);if(o){o->type=(ex.phaseAge&128)?9:8;o->hp=o->type==9?5:3;o->phase=ex.hazardSerial++&1;o->x=tunnel_center(-32)*Q;o->y=-32*Q;o->vy=o->type==9?(ex.scrollSpeed>>2):340;cavern_prepare_enemy(o,0);}}
 if(!custom_environment()&&!environment_exclusive()&&phase==5&&!ex.travel&&!ex.bossSpawned&&g.clock<encounter_deadline()-40&&ex.phaseAge>140&&remn(ex.phaseAge,170)==0&&tunnel_right(-12)-tunnel_left(-12)>128)hazard(0,tunnel_center(-12),-12);
 if(!custom_environment()&&!environment_exclusive()&&phase>=6&&phase<10&&!ex.travel&&!ex.bossSpawned&&g.clock<encounter_deadline()&&g.clock>100&&remn(g.clock,110)==0)hazard(phase==9?3:0,30+remn(ex.hazardSerial*89,260),-18);
 if(!custom_environment()&&!environment_exclusive()&&phase>=12&&!ex.travel&&!ex.bossSpawned&&g.clock>100&&g.clock<encounter_deadline()&&g.clock%65==0){hazard(phase-5,24+remn(ex.hazardSerial*71,272),-26);Obj*ore=0;for(int n=0;n<MAXH;n++)if(ex.obstacles[n].on&&ex.obstacles[n].age==0)ore=ex.obstacles+n;if(ore){ore->base=8+remn(ex.hazardSerial*7,19);ore->hp=storm_hp(phase-8,ore->base);}}
 storm_step();
 if(ex.phoenixTicks){for(int n=0;n<MAXB;n++){Obj*b=g.bullets+n;int dx=b->x/Q-g.px/Q,dy=b->y/Q-player_y/Q;if(b->on&&dx*dx+dy*dy<26*26)b->on=0;}for(int n=0;n<MAXH;n++){Obj*o=ex.obstacles+n;int dx=o->x/Q-g.px/Q,dy=o->y/Q-player_y/Q,r=20+o->base;if(o->on&&o->type!=2&&dx*dx+dy*dy<r*r){o->on=0;g.score+=25;pc_burst(o->x/Q,o->y/Q,0);}}}
 /* Telegraph each hazard at the top edge while it enters. Gate gaps stay at least 104 pixels wide. */
 if(!custom_environment()&&!environment_exclusive()&&!ex.travel&&!ex.bossSpawned&&phase==2&&g.clock<encounter_deadline()&&g.clock>60&&remn(g.clock,64)==0)hazard((ex.hazardSerial&3)==0?1:0,26+remn(ex.hazardSerial*79,268),-26);
 if(!ex.travel&&!ex.bossSpawned&&phase==3&&g.clock<encounter_deadline()&&g.clock>80&&remn(g.clock,150)==0)hazard(2,80+remn(ex.hazardSerial*67,160),-16);
 for(i=0;i<MAXH;i++){Obj*o=ex.obstacles+i;int x,y;if(!o->on)continue;int caveOldX=o->x,caveOldY=o->y;o->age++;o->y+=o->vy;o->x+=o->vx;cavern_steer_obstacle(o,caveOldX,caveOldY);x=o->x>>8;y=o->y>>8;
  if(y>288){o->on=0;continue;}if((phase==2||phase==15)&&o->type!=2){int l=tunnel_left(y),r=tunnel_right(y),il,ir;if(tunnel_island(y,&il,&ir)){if(o->phase&1)l=ir;else r=il;}o->base=clamp(o->base,6,(r-l)/2-2);o->x=clamp(o->x,(l+o->base)*Q,(r-o->base)*Q);x=o->x/Q;}
  if(o->type==2){if(absn(y-(player_y>>8))<12&&absn(x-(g.px>>8))>40)hurt_at(g.px>>8,clamp(y,(player_y>>8)-7,(player_y>>8)+7),0,y<(player_y>>8)?1:-1);continue;}
  for(j=0;j<MAXS;j++){Obj*s=g.shots+j;if(s->on&&absn((s->x>>8)-x)<o->base&&absn((s->y>>8)-y)<o->base){if(s->type==4){detonate_heatseeker(s);break;}if(s->type==9){detonate_torpedo(s);break;}s->on=0;o->hp--;if(s->type==7)pc_reflection_sound(hazard_material(o),o->type,x);hit_sparks(s->x>>8,s->y>>8,0,1,1);if(s->type!=8)pc_hit_sound(hazard_material(o),s->type,x);}}
  if(!o->on)continue;
  if(g.beam&&beam_hits(x,y,o->base)&&(g.tick&3)==0&&y<(player_y>>8)){o->hp--;pc_hit_sound(hazard_material(o),g.weapon,x);}
  if(o->hp<=0){o->on=0;g.score+=25;if(o->type!=2&&o->type!=3){pc_crush_sound(hazard_material(o),o->base,x);ex.soundTick=g.tick;}burst(x,y,0);if(o->type==9||o->type==11)for(int a=0;a<256;a+=64){Obj*shard=bullet(o->x,o->y,sn(a)*400/1024,sn(a+64)*400/1024);if(shard)shard->variant=2;}continue;}
  if((o->type==9||o->type==11)&&g.tick%35==0&&absn(x-(g.px>>8))<o->base+22&&absn(y-(player_y>>8))<o->base+22)hurt_at(g.px/Q,player_y/Q,0,1);
  if(absn(x-(g.px>>8))<o->base+4&&absn(y-(player_y>>8))<o->base+5){int pt[2];surface_point(g.px>>8,player_y>>8,6,7,x,y,pt);hurt_at(pt[0],pt[1],(g.px>>8)>x?1:-1,0);o->on=0;burst(x,y,0);}
 }
}
