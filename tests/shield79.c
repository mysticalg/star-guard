#define main star_guard_main
#include "../src/pc.c"
#undef main
static int checks;
#define V(x) do{checks++;if(!(x)){fprintf(stderr,"FAIL Shield79 line%d %s M%d S%d tick%d inv%d life%d\n",__LINE__,#x,g.wave,ex.section,g.tick,ex.invincibleTicks,g.lives);exit(1);}}while(0)
static void arena79(int ship,int mission,int section){headless=1;pilotConfigured=0;selectedShip=ship;configuredLevel=1;configuredMask=511;configuredArmour=configuredQualityPack=0;memset(starterVariants,0,sizeof(starterVariants));game_reset();g.mode=1;g.fx=0;g.wave=mission;ex.section=section;ex.phase=ex.oldPhase=mission_phase(mission);ex.intro=ex.travel=ex.entryY=0;ex.phaseAge=200;ex.rearSpawned=1;g.clock=-100000;ex.torpedoCool=100000;environment_enter();}
static void pickup79(void){g.drops[0]=(Obj){.on=1,.type=2,.x=g.px,.y=player_y,.vy=0};game_step(0,0);}
static void clear_other79(void){g.devInvincible=g.respawnTicks=g.shield=g.active=ex.phoenixTicks=ex.intro=ex.travel=ex.victory=ex.env.wallGrace=0;}
static void fatal79(void){hurt_amount(g.px/Q,player_y/Q,0,1,10000,1);hurt_amount(g.px/Q,player_y/Q,1,0,10000,2);storm_hurt(10000);wall_ship_lost();worm_ship_lost();}
int main(void){
 for(int ship=0;ship<3;ship++){
  arena79(ship,2,0);g.barrier=55;g.shield=250;pickup79();V(ex.invincibleTicks==300&&g.barrier==75&&g.shield>=249);clear_other79();
  for(int t=0;t<300;t++){V(ex.invincibleTicks==300-t);int life=g.lives,hp=g.barrier,serial=ex.lastDamageSerial;fatal79();V(g.lives==life&&g.barrier==hp&&ex.lastDamageSerial==serial);game_step(0,0);V(ex.invincibleTicks==299-t);}
  V(!ex.invincibleTicks);clear_other79();int life=g.lives;hurt_amount(g.px/Q,player_y/Q,0,1,10000,1);V(g.lives==life-1&&g.respawnTicks==250);
  arena79(ship,2,0);pickup79();for(int i=0;i<47;i++)game_step(0,0);V(ex.invincibleTicks==253);pickup79();V(ex.invincibleTicks==300);game_step(0,'p');for(int i=0;i<1000;i++)game_step(4,0);V(ex.invincibleTicks==300);game_step(0,'p');V(ex.invincibleTicks==299);ex.shieldEligible79[1]=20;ex.shieldUsed79=2;g.mode=3;g.continueTicks=400;game_step(0,13);V(!ex.invincibleTicks&&g.respawnTicks==250&&ex.shieldEligible79[1]==20&&ex.shieldUsed79==2);game_step(0,2008);V(g.devInvincible);ex.invincibleTicks=1;game_step(0,0);V(!ex.invincibleTicks&&g.devInvincible);
 }
 for(int mission=1;mission<=6;mission++)for(int sec=0;sec<journeyCounts[mission-1];sec++){
  arena79(0,mission,sec);pickup79();V(ex.invincibleTicks==300);clear_other79();int hp=g.barrier,life=g.lives,serial=ex.lastDamageSerial;for(int t=0;t<30;t++){fatal79();V(g.lives==life&&g.barrier==hp&&ex.lastDamageSerial==serial);}ex.invincibleTicks=0;clear_other79();wall_ship_lost();V(g.lives==life-1&&g.respawnTicks==250);
 }
 for(int ship=0;ship<3;ship++){
  arena79(ship,1,0);for(int mission=1;mission<=6;mission++){g.wave=mission;int count=0,shields=0,start=g.kills;for(int k=0;k<160;k++){memset(g.drops,0,sizeof(g.drops));g.enemies[0]=(Obj){.on=1,.type=0,.x=160*Q,.y=100*Q,.hp=0};killed(g.enemies);for(int j=0;j<8;j++)if(g.drops[j].on){count++;shields+=g.drops[j].type==2;}if(k==50)ex.section=1;if(k==100){ex.stageKills=g.kills;ex.shieldDrops=0;}}V(shields==1&&count==regular_drop_count(g.kills)-regular_drop_count(start));V(ex.shieldUsed79&(1<<(mission-1)));int eligible=ex.shieldEligible79[mission-1];g.mode=3;g.continueTicks=400;game_step(0,13);V(ex.shieldEligible79[mission-1]==eligible&&(ex.shieldUsed79&(1<<(mission-1))));}
  arena79(ship,1,0);for(int i=0;i<8;i++)g.drops[i]=(Obj){.on=1,.type=7};for(int k=0;k<80;k++){g.enemies[0]=(Obj){.on=1,.type=0,.x=160*Q,.y=100*Q};killed(g.enemies);}V(ex.shieldUsed79&1);memset(g.drops,0,sizeof(g.drops));int shields=0;for(int k=0;k<240;k++){memset(g.drops,0,sizeof(g.drops));g.enemies[0]=(Obj){.on=1,.type=0,.x=160*Q,.y=100*Q};killed(g.enemies);for(int j=0;j<8;j++)shields+=g.drops[j].on&&g.drops[j].type==2;}V(shields==0);
 }
 printf("PASS %d Shield79 checks: actualpickup300ticks/repair/refresh/pause/expiry/allfatalpaths/all38sections/continue/F8distinct, all6missions deterministiconeslot, noextraloot and nofullpoolreroll all3ships\n",checks);return 0;}
