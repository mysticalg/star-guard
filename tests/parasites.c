#define main star_guard_main
#include "../src/pc.c"
#undef main
static int checks;
#define V(x) do{if(!(x)){fprintf(stderr,"FAIL %d %s\n",__LINE__,#x);exit(1);}checks++;}while(0)
static void arena(void){selectedShip=0;game_reset();g.fx=0;g.mode=1;g.wave=5;ex.section=2;ex.phase=ex.oldPhase=mission_phase(5);ex.intro=ex.travel=0;ex.rearSpawned=1;g.groups=6;g.clock=-100000;g.shield=100000;environment_enter();ex.leechGrace=0;}
int main(void){headless=1;arena();environment_spawn(9,g.px/Q,player_y/Q,0,0,9,2);environmental_hazards();V(ex.leeches==1&&!ex.env.hazards[0].on&&ex.leechSerial==1);V(!parasites_latch());ex.leechGrace=0;V(parasites_latch());ex.leechGrace=0;V(parasites_latch());ex.leechGrace=0;V(!parasites_latch()&&ex.leeches==3);
 int x=parasite_x(0),y=parasite_y(0);g.px+=30*Q;player_y-=20*Q;V(parasite_x(0)==x+30&&parasite_y(0)==y-20);
 for(int i=0;i<99;i++)parasites_step();V(g.weaponEnergy[0]==100);parasites_step();V(g.weaponEnergy[0]==97&&g.barrier==100);
 g.shots[0]=(Obj){.on=1,.x=g.px,.y=player_y};ex.bombAge=25;ex.bombRadius=1000;environmental_hazards();V(ex.leeches==3);ex.bombAge=0;game_step(0,'b');V(ex.leeches==3);ex.bombAge=0;g.weapon=0;game_step(4,0);V(ex.leeches==3);
 emp_storm();V(ex.leeches==0&&ex.purgeSerial==1&&ex.leechGrace==150);ex.leechGrace=0;V(parasites_latch());game_step(0,'e');V(!ex.leeches&&ex.empBombs==0);ex.leechGrace=0;V(parasites_latch());ex.env.timer=475;g.px=160*Q;parasites_step();V(!ex.leeches&&ex.purgeSerial==3);
 ex.env.timer=470;ex.leechGrace=0;V(parasites_latch());g.px=210*Q;ex.env.timer=480;parasites_step();V(ex.leeches==1);g.weaponEnergy[8]=0;g.weaponHeat[8]=300;g.weapon=8;ex.empBombs=0;ex.env.timer=475;g.px=160*Q;parasites_step();V(!ex.leeches);V(g.weaponEnergy[8]==0);for(int i=0;i<330;i++){g.tick++;weapon_batteries_step();}V(g.weaponEnergy[8]>0&&!g.weaponHeat[8]);
 arena();ex.leechGrace=0;V(parasites_latch());int energy=g.weaponEnergy[0];g.paused=1;for(int i=0;i<200;i++)game_step(4,0);V(g.weaponEnergy[0]==energy&&ex.leechDrain==0);g.paused=0;g.mode=3;g.continueTicks=400;game_step(0,13);V(!ex.leeches&&g.mode==1);ex.leechGrace=0;V(parasites_latch());game_reset();V(!ex.leeches);
 arena();ex.leechGrace=0;parasites_latch();g.wave=1;ex.section=0;parasites_step();V(!ex.leeches&&!parasites_latch());
 printf("PASS %d parasite attachment/anchor/electric-only purge/free recovery/pause/hull checks\n",checks);return 0;
}
