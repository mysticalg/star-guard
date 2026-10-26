#define main star_guard_main
#include "../src/pc.c"
#undef main
static int checks;
#define V(x) do{checks++;if(!(x)){fprintf(stderr,"FAIL %d %s m%d section%d\n",__LINE__,#x,g.wave,ex.section);return 1;}}while(0)
static void scene(int m,int s){configuredLevel=1;selectedShip=0;game_reset();g.mode=1;g.fx=0;g.wave=m;ex.section=s;ex.phase=ex.oldPhase=mission_phase(m);ex.intro=ex.travel=ex.entryY=0;ex.bossSpawned=1;g.clock=0;g.shield=100000;ex.progressTicks=mission_active_target();environment_enter();}
int main(void){headless=1;
for(int m=1;m<=6;m++)for(int s=0;s<journeyCounts[m-1];s++){if(journeyMinis[m-1][s]<0)continue;scene(m,s);ex.encounterTier=1;Obj o={.type=4,.variant=journeyMinis[m-1][s],.x=160*Q,.y=80*Q};killed(&o);int score=g.score;for(int n=0;n<130&&ex.victory;n++){V(!mission_report_ready()&&!ex.refit);g.shield=100000;game_step(0,0);}V(g.score==score);V(!ex.victory&&!ex.refit);V(ex.section==s+1||ex.miniDone);V(!(ex.completedMissionMask&(1<<(m-1))));}
for(int m=1;m<=6;m++){scene(m,journeyGiantAt[m-1]);ex.encounterTier=2;ex.giant.defeated=1;ex.victory=1;V(mission_report_ready()==(m<=4));if(m<=4){for(int i=0;i<120;i++){g.shield=100000;game_step(0,0);}V(mission_report_ready()&&!ex.refit);for(int i=0;i<330&&!ex.refit;i++){g.shield=100000;game_step(0,0);}V(ex.refit&&(ex.completedMissionMask&(1<<(m-1))));}else{for(int i=0;i<130&&ex.victory;i++){g.shield=100000;game_step(0,0);}V(!ex.refit&&!ex.victory&&ex.section==journeyGiantAt[m-1]+1&&!ex.campaignWon);V(!(ex.completedMissionMask&(1<<(m-1))));scene(m,journeyCounts[m-1]-1);ex.bossSpawned=0;ex.env.escaped=1;journey_advance();V(m==5?ex.refit:ex.campaignWon);V(ex.completedMissionMask&(1<<(m-1)));}}
printf("PASS %d actualmini deathcleanup/score/reward checkpoints, no false report/refit, only true wholemission report/refit/campaign completion\n",checks);return 0;}
