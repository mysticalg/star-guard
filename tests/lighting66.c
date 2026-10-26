#define main star_guard_main
#include "../src/pc.c"
#undef main
static int checks;
#define V(x) do{checks++;if(!(x)){fprintf(stderr,"FAIL %d %s\n",__LINE__,#x);return 1;}}while(0)
int main(void){headless=1;game_reset();g.mode=1;ex.intro=0;ex.phase=mission_phase(1);environment_enter();
 float low=1,high=0;for(int m=1;m<=6;m++)for(int s=0;s<journeyCounts[m-1];s++){g.wave=m;ex.section=s;ex.env.weather.stage=0;ex.bossSpawned=ex.giant.active=0;for(int part=0;part<5;part++){ex.progressTicks=part*1500;float a=lighting_target();V(a>=.25f&&a<=1);if(a<low)low=a;if(a>high)high=a;}}
 V(high>.9f&&low<.31f);g.wave=2;ex.section=1;ex.progressTicks=0;ex.lightAmbient=.98f;ex.lightReady=1;float before=ex.lightAmbient;lighting_step();V(ex.lightAmbient<before&&ex.lightAmbient>before-.02f);for(int i=0;i<200;i++)lighting_step();V(ex.lightAmbient<.4f);
 unsigned seed=g.seed,fx=fxseed;for(int i=0;i<MAXB;i++)g.bullets[i]=(Obj){.on=1,.x=160*Q,.y=150*Q};for(int i=0;i<MAXS;i++)g.shots[i]=(Obj){.on=1,.x=160*Q,.y=150*Q};g.beam=8;ex.multi=1;for(int i=0;i<64;i++)waves[i]=(Wave){160,120,.1f,1,60,1};lighting_sources();V(worldLightCount==WORLD_LIGHT_MAX);V(!worldLightDropped);V(g.seed==seed&&fxseed==fx);
 for(int i=0;i<worldLightCount;i++)V(isfinite(worldLights[i].x)&&isfinite(worldLights[i].y)&&worldLights[i].r>0);
 for(int side=0;side<=2;side++)for(int z=0;z<2;z++){sideBlend=side*.5f;journeyZoom=z?.32f:1;float x=160,y=140;world_light_point(&x,&y);V(fabsf(x-480)<.001f&&fabsf(y-420)<.001f);x=160;y=90;world_light_point(&x,&y);V((side==0&&y<420)||(side==1&&x>480&&y<420)||(side==2&&x>480));}
 /* The tapered core must end at one exact point; reject the former unsigned side overflow. */
 for(int q=0;q<3;q++)for(int strike=0;strike<32;strike++){quality=q;nquad=0;draw_storm_forks(60,strike,1);for(int i=0;i<nquad*4;i++){V(isfinite(vertices[i].position.x)&&fabsf(vertices[i].position.x)<2000);V(isfinite(vertices[i].position.y)&&fabsf(vertices[i].position.y)<2000);}}
 float x[4]={0,10,20,30},y[4]={0,4,2,5},r[4]={4,2.8f,1.6f,0};nquad=0;storm_fork_mesh(x,y,r,4,(SDL_FColor){1,1,1,1});V(nquad==3);V(vertices[9].position.x==vertices[10].position.x&&vertices[9].position.y==vertices[10].position.y);V(g.seed==seed&&fxseed==fx);
 printf("PASS %d ambient all38sections/smoothing/240-source budget/no RNG/projection/pointed bounded forks checks\n",checks);return 0;}
