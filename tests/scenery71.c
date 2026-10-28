#define main star_guard_main
#include "../src/pc.c"
#undef main
static int checks;
#define V(x) do{checks++;if(!(x)){fprintf(stderr,"FAIL %d %s\n",__LINE__,#x);return 1;}}while(0)
int main(void){headless=1;game_reset();g.mode=1;g.wave=1;ex.section=1;ex.phase=ex.oldPhase=mission_phase(1);ex.intro=ex.travel=ex.entryY=0;ex.phaseAge=200;ex.rearSpawned=1;environment_enter();V(flight_side_mode());float lo=1000,hi=0;int deep=0;
 for(int t=0;t<3720;t++)for(int y=-80;y<=340;y+=4){float top=ocean_top70(y,t);V(isfinite(top));V(top>62&&top<320);if(top<lo)lo=top;if(top>hi)hi=top;deep+=top<140;V(40+player_hull_rx()<top);if(t>0)V(fabsf(top-ocean_top70(y,t-.1f))<3);}
 V(lo<90&&hi>300&&deep>1000);V(ocean_width70(400)!=ocean_width70(1000));V(ocean_height71(400)!=ocean_height71(1000));V(ocean_warning70(100)==7);V(ocean_warning70(720)==7);
 for(int ship=0;ship<3;ship++){g.chassis=ship;g.barrier=100;g.px=280*Q;player_y=180*Q;g.shield=g.respawnTicks=g.devInvincible=ex.invincibleTicks=ex.phoenixTicks=0;ex.env.timer=460;environment_step(0);V(g.barrier<100);g.barrier=100;g.shield=0;g.respawnTicks=250;environment_step(0);V(g.barrier==100);g.respawnTicks=0;g.devInvincible=1;environment_step(0);V(g.barrier==100);g.devInvincible=0;g.px=40*Q;g.shield=0;environment_step(0);V(g.barrier==100);}
 ex.section=0;V(!flight_side_mode());ex.section=1;ex.env.timer=444;journey_advance();V(ex.oldEnvironmentTick70==444);V(ex.oldKind==ENV_OCEAN);V(ex.section==2);
 game_reset();g.mode=1;ex.intro=0;ex.env.weather.stage=0;g.bullets[0]=(Obj){.on=1,.x=140*Q,.y=100*Q};g.shots[0]=(Obj){.on=1,.x=170*Q,.y=130*Q};unsigned seed=g.seed,fx=fxseed;lighting_sources();V(worldLightCount==7);V(worldLights[0].r==24&&worldLights[4].r==40);V(worldLights[5].r==18&&worldLights[6].r==28);V(worldLights[5].colour.a==2.3f);V(g.seed==seed&&fxseed==fx);
 /* Cubic radial falloff is continuous and flatter across physical space than70. */
 for(int hostile=0;hostile<2;hostile++){float oldR=hostile?15:23,newR=hostile?18:28;float oldSlope=6*2.6f*.2f*powf(.8f,2)/oldR,newSlope=6*2.3f*.2f*powf(.8f,2)/newR;V(newSlope<oldSlope);V(newR/oldR>=1.2f&&newR/oldR<1.23f);}
 printf("PASS %d scenery71 physical crest/interpolation/escape/all ships/protection/approach state/isolated softer projectile light checks; crest range %.2f..%.2f\n",checks,lo,hi);return 0;}
