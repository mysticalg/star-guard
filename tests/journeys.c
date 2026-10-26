#define main star_guard_main
#include "../src/pc.c"
#undef main
static int checks;
#define V(x) do{if(!(x)){fprintf(stderr,"FAIL %d %s\n",__LINE__,#x);exit(1);}checks++;}while(0)
static void arena(int m,int s){selectedShip=0;game_reset();g.mode=1;g.wave=m;ex.section=s;ex.phase=ex.oldPhase=mission_phase(m);ex.intro=ex.travel=ex.entryY=0;ex.phaseAge=100;ex.rearSpawned=1;g.shield=100000;g.clock=-100000;environment_enter();}
static void clear_scene(void){memset(g.enemies,0,sizeof(g.enemies));memset(g.shots,0,sizeof(g.shots));memset(g.bullets,0,sizeof(g.bullets));memset(ex.obstacles,0,sizeof(ex.obstacles));if(journey_kind()==ENV_WALL)terrain_clear_box(130,wall_origin_y(),60,TERRAIN_H);}
static int endurance(int ship,int w,int rank){arena(1,0);g.chassis=ship;g.weapon=w;g.weaponLevel[w]=rank;g.groups=6;int ticks=0;while(!g.weaponHeat[w]&&ticks<4000){clear_scene();game_step(4,0);ticks++;}V(g.weaponHeat[w]>0);return ticks;}
static void finish_giant(void){Giant*b=&ex.giant;int ticks=0;while(b->active&&ticks++<5000){for(int h=0;h<32;h++)ex.env.hazards[h].on=0;b->addMask=3;int target=-1;for(int i=0;i<9;i++)if(i!=6&&giant_open(i)){target=i;break;}if(target<0&&giant_open(6))target=6;if(target>=0)giant_hit(target,100000);g.tick++;g.viewZoom=b->zoom;giant_step(0);}V(!b->active&&b->defeated&&ex.victory&&b->transformSerial==2);}

int main(void){headless=1;int seen[17]={0},sections=0;for(int m=0;m<6;m++){V(journeyCounts[m]>=3);for(int s=0;s<journeyCounts[m];s++){sections++;int n=journeyMinis[m][s];if(n>=0)seen[n]++;}}V(sections==38);for(int i=0;i<17;i++)V(seen[i]==1);
 for(int ship=0;ship<3;ship++)for(int w=0;w<3;w++)for(int r=1;r<=4;r++){int ticks=endurance(ship,w,r),base=w==0?1250:w==1?600:500,target=base*(ship==1?120:ship==2?85:100)/100;V(absn(ticks-target)<50);printf("ship %d weapon %d rank %d endurance %.2fs\n",ship,w,r,ticks/50.0);}
 arena(3,1);V(environment_exclusive()&&section_wave_target()==0&&wall_remaining()==192);ex.env.timer=750;terrain.scrollQ=terrain.previousScrollQ=450*Q;g.shots[0]=(Obj){.on=1,.x=wall_x(180)*Q,.y=wall_y(180)*Q,.type=0};terrain_projectile(&g.shots[0],g.shots[0].x,g.shots[0].y,0);asteroid_wall_step(4);V(wall_remaining()<192);int before=terrain.removed;g.weaponHeat[0]=100;g.tick=20;g.px=160*Q;player_y=230*Q;asteroid_wall_step(4);V(ex.env.cutter&&terrain.removed>before);for(int t=0;t<500;t++){clear_scene();game_step(0,0);V(enemy_count()==0);}
 for(int m=1;m<=6;m++){arena(m,journeyGiantAt[m-1]);ex.miniDone=1;giant_start();ex.giant.timer=150;V(ex.giant.active&&ex.encounterTier==2);int hp=ex.giant.parts[6].hp;giant_hit(6,1000);V(ex.giant.parts[6].hp==hp);finish_giant();ex.progressTicks=mission_active_target();journey_after_encounter();if(ex.refit)journey_commit_refit();if(m<5)V(g.wave==m+1&&ex.section==0);else V(g.wave==m&&journey_kind()==(m==5?ENV_TEETH:ENV_ESCAPE));}
 arena(6,journeyCounts[5]-1);ex.progressTicks=mission_active_target();journey_advance();V(ex.campaignWon&&g.mode==2);
 arena(5,0);ex.env.timer=499;environment_step(0);V(ex.env.empActive&&player_attack_scale(0)==80);ex.fatigue[0]=300;V(player_attack_scale(0)==72);ex.env.empActive=0;V(player_attack_scale(0)==90);for(int t=0;t<60;t++)environment_step(0);V(ex.fatigue[0]==120);
 for(int chassis=0;chassis<3;chassis++){selectedShip=chassis;game_reset();V(g.chassis==chassis);g.mode=3;g.continueTicks=100;g.continueReady=0;ex.section=1;game_step(0,13);V(g.chassis==chassis&&ex.section==1&&ex.bombs==5);}
 arena(2,2);player_y=180*Q;for(int n=0;n<64;n++){ex.followX[n]=160*Q;ex.followY[n]=180*Q;}ex.options=1;ex.optionsOn=1;int x=option_x(0),y=option_y(0);g.enemies[0]=(Obj){.on=1,.entryTicks=12,.x=x*Q,.y=y*Q,.hp=10};pod_contact_step();V(g.enemies[0].wound==60&&ex.podContact[0]==20);pod_contact_step();V(g.enemies[0].wound==60);
 /* Exercise the real section scheduler, warnings, victories and handovers end to end. */
 selectedShip=0;game_reset();g.mode=1;g.fx=0;int minis=0,giants=0,visited[6][JOURNEY_SECTIONS_MAX]={{0}},ticks=0;
 while(!ex.campaignWon&&ticks++<240000){if(ex.refit)journey_commit_refit();g.shield=100000;g.px=160*Q;player_y=230*Q;visited[g.wave-1][ex.section]=1;
  for(int i=0;i<MAXE;i++)if(g.enemies[i].on){if(g.enemies[i].type==4){V(g.enemies[i].variant==journeyMinis[g.wave-1][ex.section]);minis++;killed(g.enemies+i);}else g.enemies[i].on=0;}
  memset(g.bullets,0,sizeof(g.bullets));memset(ex.obstacles,0,sizeof(ex.obstacles));if(journey_kind()==ENV_WALL)terrain_clear_box(130,wall_origin_y(),60,TERRAIN_H);
  if(ex.giant.active){giants++;finish_giant();}if(journey_kind()==ENV_BASE&&!ex.bombing.complete){for(int i=0;i<8;i++)ex.bombing.targets[i].hp=0;ex.bombing.targetsDestroyed=8;ex.bombing.cleared=1;ex.bombing.stage=3;ex.bombing.timer=74;}game_step(0,0);
 }
 V(ex.campaignWon&&minis==17&&giants==6);for(int m=0;m<6;m++)for(int s=0;s<journeyCounts[m];s++)V(visited[m][s]);printf("Full campaign: %d ticks, %d minibosses, %d giants\n",ticks,minis,giants);
 printf("PASS %d journey/giant/ship/environment checks\n",checks);return 0;
}
