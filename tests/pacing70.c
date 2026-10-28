#define main star_guard_main
#include "../src/pc.c"
#undef main
static int checks;
#define V(x) do{checks++;if(!(x)){fprintf(stderr,"FAIL %d %s groups%d age%d count%d\n",__LINE__,#x,g.groups,ex.waveAge,enemy_count());return 1;}}while(0)
static void arena(int m,int section){headless=1;pilotConfigured=0;selectedShip=0;configuredLevel=1;game_reset();g.mode=1;g.fx=0;g.wave=m;ex.section=section;ex.phase=ex.oldPhase=mission_phase(m);ex.intro=ex.travel=ex.entryY=0;ex.rearSpawned=1;ex.phaseAge=200;g.devInvincible=1;environment_enter();}
int main(void){arena(1,0);game_step(0,0);V(g.groups==1&&enemy_count()<=6);int type=-1;for(int n=0;n<MAXE;n++)if(g.enemies[n].on){if(type<0)type=g.enemies[n].type;V(g.enemies[n].type==type);}Obj*anchor=g.enemies;*anchor=(Obj){.on=1,.type=0,.hp=1000,.strength=1,.difficultyReady=1,.x=160*Q,.y=100*Q,.base=160,.waveBorn=g.tick};int secondAt=0,mixedMask=0,peak=0;
for(int t=1;t<=1800;t++){anchor->x=160*Q;anchor->y=100*Q;anchor->vy=0;game_step(0,0);if(g.groups>=2&&!secondAt)secondAt=t;if(enemy_count()>peak)peak=enemy_count();V(anchor->on&&g.mode==1&&enemy_count()<=MAXE);if(g.groups>=4)for(int n=0;n<MAXE;n++)if(g.enemies[n].on)mixedMask|=1<<g.enemies[n].type;}
V(secondAt>0&&secondAt<=400);V(g.groups>=4&&__builtin_popcount((unsigned)mixedMask)>=2);V(regular_enemy_enraged(anchor));V(peak<=40);int age=ex.waveAge;game_step(0,'p');for(int t=0;t<60;t++)game_step(0,0);V(ex.waveAge==age);
for(int m=1;m<=6;m++){arena(m,0);int max=0,old=0,last=0,gap=0;for(int t=0;t<7000&&!ex.bossSpawned;t++){game_step(0,0);int count=enemy_count();if(count>max)max=count;V(count<=MAXE);if(g.groups>old){int d=t-last;if(old&&d>gap)gap=d;last=t;old=g.groups;}}printf("M%d idle natural departures maxNPC%d maxAdmissionGap%.2fs groups%d boss%d\n",m,max,gap/50.,g.groups,ex.bossSpawned);V(g.groups>1&&max<=MAXE);}
printf("PASS %d steady bounded/mixed/learning/enrage/pause tests; first admission while original survivor alive at%.2fs, peak%d\n",checks,secondAt/50.,peak);return 0;}
