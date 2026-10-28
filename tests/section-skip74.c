#define main star_guard_main
#include "../src/pc.c"
#undef main
static int checks;
#define V(x) do{checks++;if(!(x)){fprintf(stderr,"FAIL line%d M%d section%d %s\n",__LINE__,g.wave,ex.section+1,#x);return 1;}}while(0)
int main(void){headless=1;for(int ship=0;ship<3;ship++)for(int god=0;god<2;god++)for(int state=0;state<3;state++)for(int m=1;m<=6;m++)for(int section=0;section<journeyCounts[m-1];section++){
 selectedShip=ship;game_reset();g.mode=state==2?3:1;g.paused=state==1;g.continueTicks=400;g.wave=m;ex.section=section;ex.phase=ex.oldPhase=mission_phase(m);g.weapon=5;g.score=12345;g.devInvincible=god;g.devUsed=0;g.lives=state==2?0:4;g.salvagePicked=37;ex.partsCollected=9;ex.pendingParts=3;ex.completedMissionMask=2;ex.giantRewardMask=1;
 g.enemies[0]=(Obj){.on=1,.type=4,.hp=100,.shieldCharge=100};g.shots[0].on=g.bullets[0].on=g.drops[0].on=1;ex.obstacles[0].on=ex.mines[0].on=1;ex.env.kind=journey_kind();ex.env.hazards[0].on=1;ex.env.weather.actors[0].on=1;ex.giant.active=1;ex.giant.openingTicks=500;ex.bombing.active=1;ex.bombing.stage=2;ex.bombing.angle=180;ex.bombing.bombs[0].on=1;terrain.active=1;ex.cavernTurn=30;ex.victory=100;ex.refit=1;ex.partBundle.on=1;
 game_step(0,2007);
 V(g.devUsed&&g.devNotice==7&&g.devInvincible==god);V(g.chassis==ship&&g.weapon==5);V(g.score==12345&&g.salvagePicked==37&&ex.partsCollected==9&&ex.pendingParts==3&&ex.completedMissionMask==2&&ex.giantRewardMask==1);V(!g.paused&&!ex.refit&&g.lives>=3&&g.lives<=5);
 V(!enemy_count()&&!g.shots[0].on&&!g.bullets[0].on&&!g.drops[0].on&&!ex.obstacles[0].on&&!ex.mines[0].on&&!ex.giant.active&&!ex.bombing.active&&!terrain.active&&!ex.cavernTurn&&!ex.victory&&!ex.partBundle.on);V(ex.env.kind==-1&&!ex.env.hazards[0].on&&!ex.env.weather.actors[0].on);
 if(section+1<journeyCounts[m-1]){int progress=0;for(int n=0;n<ex.section;n++)progress+=journeySeconds[m-1][n]*50;V(g.wave==m&&ex.section==section+1&&ex.sectionHandover&&g.mode==1&&ex.progressTicks==progress);}
 else if(m<6){V(g.wave==m+1&&ex.section==0&&g.mode==1);}
 else{V(g.mode==2&&ex.campaignWon);}
 if(g.mode==1){for(int t=0;t<20;t++)game_step(0,0);V(g.devInvincible==god&&g.lives>=3&&g.score==12345);}
 }
 printf("PASS %d F7 checks: all38nodes/3ships/God on-off/pause/Continue, safe hazard cleanup, last-section route, no awards, preserved God/loadout\n",checks);return 0;}
