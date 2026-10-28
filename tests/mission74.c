#define main star_guard_main
#include "../src/pc.c"
#undef main
static int checks;
#define V(x) do{checks++;if(!(x)){fprintf(stderr,"FAIL line%d M%d section%d tick%d %s\n",__LINE__,g.wave,ex.section+1,g.tick,#x);return 1;}}while(0)
static int route74(int viaSkip){headless=1;selectedShip=0;game_reset();g.mode=1;g.fx=0;g.wave=viaSkip?1:6;ex.phase=-1;game_step(0,0);game_step(0,2008);if(viaSkip)for(int m=1;m<6;m++)game_step(0,2010);int seen=0,last=-1,sectionTicks[12]={0},airWaves[12]={0},targets=0;
 for(int tick=0;tick<120000&&g.mode==1&&!ex.campaignWon;tick++){
  int section=ex.section;V(section>=0&&section<12);seen|=1<<section;sectionTicks[section]++;if(g.groups>airWaves[section])airWaves[section]=g.groups;
  if(last!=section){printf("enter section%d tick%d kind%d phase%d\n",section+1,g.tick,journey_kind(),ex.phase);last=section;}
  if(tick%500==0)game_step(0,2009);
  if(!ex.travel&&!ex.intro&&!ex.victory&&!ex.refit){
   int x=160,y=220;
   if(journey_kind()==ENV_BASE&&!ex.bombing.complete){for(int n=0;n<8;n++){GroundTarget*t=ex.bombing.targets+n;if(t->hp>0&&!bombing_shielded(n)&&t->y>24&&t->y<215){x=t->x;y=clamp(t->y+75,140,245);break;}}targets=ex.bombing.targetsDestroyed;}
   else if(ex.giant.active){for(int n=0;n<9;n++)if(giant_open(n)){x=ex.giant.parts[n].x;y=clamp(ex.giant.parts[n].y+70,play_min_y(),play_max_y());break;}}
   else if(journey_kind()!=ENV_WALL&&journey_kind()!=ENV_ESCAPE){for(int n=0;n<MAXE;n++){Obj*o=g.enemies+n;if(o->on&&enemy_damageable(o)&&o->y/Q>24&&o->y/Q<210){x=o->x/Q;y=clamp(o->y/Q+65,40,245);break;}}}
   game_pointer(x,y);
  }
  game_step(4,0);if(ex.bombing.targetsDestroyed>targets)targets=ex.bombing.targetsDestroyed;V(g.devInvincible&&g.lives>=3&&g.lives<=5);V(journey_kind()!=ENV_BASE||ex.bombing.angle==0);
 }
 for(int n=0;n<12;n++)printf("section%d ticks%d waves%d\n",n+1,sectionTicks[n],airWaves[n]);printf("end mode%d campaign%d section%d targets%d mask%x tick%d\n",g.mode,ex.campaignWon,ex.section+1,targets,seen,g.tick);
 V(seen==4095&&ex.campaignWon&&g.mode==2&&targets==8);printf("PASS %d normal-init Mission6 all12sections actual Pulse/ground payloads/part gates/escape checks; F8 once,F9 supplies, no section skips or forced actor clearing\n",checks);return 0;
}

int main(void){int result=route74(0);return result?result:route74(1);}
