#define main star_guard_main
#include "../src/pc.c"
#undef main
static int checks;
static Game snapshotGame74;static Expansion snapshotExpansion74;
#define V(x) do{checks++;if(!(x)){fprintf(stderr,"FAIL line%d M%d tick%d %s\n",__LINE__,g.wave,g.tick,#x);return 1;}}while(0)
static void scene(int mission){headless=1;game_reset();g.mode=1;g.wave=mission;g.devInvincible=1;ex.section=journeyGiantAt[mission-1];ex.phase=ex.oldPhase=mission_phase(mission);ex.intro=ex.entryY=ex.travel=0;environment_enter();giant_start();ex.giant.openingTicks=ex.giant.growthTicks=ex.giant.morph=0;ex.giant.timer=150;journeyZoom=ex.giant.zoom;scenery74=(SceneryMotion74){0};}
int main(void){
 for(int m=1;m<=6;m++){scene(m);V(fabsf(boss_damage74())<.0001f);unsigned seed=g.seed;snapshotGame74=g;snapshotExpansion74=ex;scenery_step74();V(!memcmp(&snapshotGame74,&g,sizeof(g))&&!memcmp(&snapshotExpansion74,&ex,sizeof(ex))&&g.seed==seed);
  for(int t=0;t<250;t++){game_step(0,0);scenery_step74();V(isfinite(scenery74.speed)&&scenery74.speed<=640&&scenery74.speed>=0);}
  /* Environmental attacks can already damage an exposed ice boss. */
  V(scenery74.speed>250&&scenery74.speed<=640);double d=scenery74.distance;float speed=scenery74.speed;
  /* Actual gated damage to a currently open part drives the visual ramp. */
  int damaged=0;for(int t=0;t<1000&&!damaged;t++){ex.giant.openingTicks=ex.giant.growthTicks=ex.giant.morph=0;for(int i=0;i<9;i++)if(giant_open(i)){int hp=ex.giant.parts[i].hp;giant_raw_hit(i,300);damaged=ex.giant.parts[i].hp<hp;break;}if(!damaged){game_step(0,0);scenery_step74();}}
  V(damaged);float damage=boss_damage74();V(damage>0);for(int t=0;t<200;t++){game_step(0,0);scenery_step74();}V(scenery74.speed>speed&&scenery74.distance>d);V(scenery74.damage>=damage);
  g.paused=1;d=scenery74.distance;speed=scenery74.speed;for(int t=0;t<200;t++){game_step(0,0);scenery_step74();}V(scenery74.distance==d&&scenery74.speed==speed);g.paused=0;
  g.mode=3;g.continueTicks=400;for(int t=0;t<20;t++){game_step(0,0);scenery_step74();}V(scenery74.distance==d);game_step(0,13);scenery_step74();V(g.mode==1&&scenery74.distance-d<30);
 }
 for(int a=0;a<=90;a++)for(int zi=0;zi<4;zi++){float zs[4]={1,.55f,.44f,.32f},z=zs[zi],x=200,y=240,xx=x+100,yy=y,xxx=x,yyy=y+100;scenery_point74(&x,&y,z,a*PI/180);scenery_point74(&xx,&yy,z,a*PI/180);scenery_point74(&xxx,&yyy,z,a*PI/180);float u=xx-x,v=yy-y,uu=xxx-x,vv=yyy-y;V(fabsf(hypotf(u,v)-100*z)<.001f);V(fabsf(hypotf(uu,vv)-100*z)<.001f);V(fabsf(u*uu+v*vv)<.05f);}
 printf("PASS %d scenery74 checks: all6 actual damage ramps/pause/continue/no game mutation and uniform camera geometry\n",checks);return 0;
}
