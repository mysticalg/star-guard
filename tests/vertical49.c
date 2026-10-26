#define main star_guard_main
#include "../src/pc.c"
#undef main
static int checks;
#define V(x) do{if(!(x)){fprintf(stderr,"FAIL %d %s\n",__LINE__,#x);exit(1);}checks++;}while(0)
static void arena(int m,int s){selectedShip=0;game_reset();g.mode=1;g.wave=m;ex.section=s;ex.phase=ex.oldPhase=mission_phase(m);ex.intro=ex.travel=ex.entryY=0;ex.phaseAge=100;ex.rearSpawned=1;g.shield=100000;g.clock=-100000;environment_enter();}
static void clear_scene(void){memset(g.enemies,0,sizeof(g.enemies));memset(g.shots,0,sizeof(g.shots));memset(g.bullets,0,sizeof(g.bullets));memset(ex.obstacles,0,sizeof(ex.obstacles));if(journey_kind()==ENV_WALL)terrain_clear_box(130,wall_origin_y(),60,TERRAIN_H);}
static int endurance(int ship,int w,int rank){arena(1,0);g.chassis=ship;g.weapon=w;g.weaponLevel[w]=rank;g.groups=6;int ticks=0;while(!g.weaponHeat[w]&&ticks<4000){clear_scene();game_step(4,0);ticks++;}V(g.weaponHeat[w]>0);return ticks;}
static void finish_giant(void){Giant*b=&ex.giant;int ticks=0;while(b->active&&ticks++<5000){for(int h=0;h<32;h++)ex.env.hazards[h].on=0;b->addMask=3;int target=-1;for(int i=0;i<9;i++)if(i!=6&&giant_open(i)){target=i;break;}if(target<0&&giant_open(6))target=6;if(target>=0)giant_hit(target,100000);g.tick++;g.viewZoom=b->zoom;giant_step(0);}V(!b->active&&b->defeated&&ex.victory&&b->transformSerial==2);}


int main(void){headless=1;
 for(int s=0;s<3;s++)for(int p=-1;p<17;p++){arena(1,s);ex.phase=p;ex.cavernSide=1;ex.cavernTurn=75;V(!flight_side_mode());V(!side_mode());}
 for(int m=2;m<=6;m++)for(int s=0;s<journeyCounts[m-1];s++){arena(m,s);ex.cavernSide=0;int p=mission_phase(m),expected=journey_kind()==ENV_BASE?0:p==2||p==15?0:p&1;V(flight_side_mode()==expected);if(p==2||p==15){ex.cavernSide=1;V(flight_side_mode());}}
 arena(1,1);ex.env.timer=499;game_step(0,0);V(ex.env.warning==7&&ex.env.safeX==121);ex.env.timer=929;g.px=121*Q;player_y=230*Q;g.shield=0;int armour=g.barrier,lives=g.lives;environment_step(0);V(g.barrier==armour&&g.lives==lives);g.px=250*Q;g.shield=0;ex.env.timer=929;environment_step(0);V(g.barrier<armour||g.lives<lives);
 arena(1,2);ex.env.timer=99;environment_step(0);V(ex.env.hazards[0].on&&ex.env.hazards[0].type==2&&ex.env.hazards[0].vy>0&&ex.env.hazards[0].y<0);V(!flight_side_mode());
 selectedShip=0;game_reset();g.mode=1;int sections=0,minis=0,giants=0,transitions=0,old=-1,ticks=0;
 while(!ex.refit&&ticks++<20000){V(g.wave==1&&!flight_side_mode());sections|=1<<ex.section;if(ex.travel&&ex.phase!=old){transitions++;old=ex.phase;}g.shield=100000;g.px=160*Q;player_y=230*Q;
 for(int n=0;n<MAXE;n++)if(g.enemies[n].on){if(g.enemies[n].type==4){minis++;V(g.enemies[n].variant==journeyMinis[0][ex.section]);killed(g.enemies+n);}else g.enemies[n].on=0;}memset(g.bullets,0,sizeof(g.bullets));memset(ex.obstacles,0,sizeof(ex.obstacles));if(ex.giant.active){giants++;finish_giant();V(!flight_side_mode());}game_step(0,0);
 }V(ex.refit&&sections==7&&minis==3&&giants==1&&transitions>=2);V(!flight_side_mode());journey_commit_refit();V(g.wave==2&&ex.section==0);arena(2,1);V(flight_side_mode());printf("PASS Storm Coast %d ticks all three sections, 3 minis, finale/forms, transition/refit/exit; later routes preserved; %d assertions\n",ticks,checks);return 0;}
