#define main star_guard_main
#include "../src/pc.c"
#undef main
static int checks;static Game beforeGame76;static Expansion beforeExpansion76;
#define V(x) do{checks++;if(!(x)){fprintf(stderr,"FAIL audio76 line%d %s\n",__LINE__,#x);return 1;}}while(0)
static void arena76(int organic){headless=1;game_reset();g.mode=1;g.devInvincible=1;g.wave=organic?5:6;ex.section=organic?2:0;ex.phase=ex.oldPhase=mission_phase(g.wave);ex.intro=ex.travel=ex.entryY=0;g.clock=-100000;g.groups=section_wave_target();environment_enter();sideBlend=0;journeyZoom=1;}
int main(void){
 for(int organic=0;organic<2;organic++){
  arena76(organic);regularSpawnBudget=10;group(0,0);regularSpawnBudget=-1;
  for(int t=0;t<360;t++){game_step(0,0);beforeGame76=g;beforeExpansion76=ex;audio_sources_step76();V(!memcmp(&g,&beforeGame76,sizeof(g))&&!memcmp(&ex,&beforeExpansion76,sizeof(ex)));V(audioSourceCount76<=6);if(organic)V(audioSourceCount76==0);for(int i=0;i<audioSourceCount76;i++){AudioSource76*s=audioSources76+i;V(s->drive>0&&s->drive<=1.4f&&fabsf(s->pan)<=.85f&&s->near>=0&&s->near<=1);V(s->id!=0);for(int j=i+1;j<audioSourceCount76;j++)V(s->id!=audioSources76[j].id);}}
 }
 arena76(0);for(int i=0;i<12;i++){Obj*o=slot(g.enemies,MAXE);o->x=(30+i*22)*Q;o->y=100*Q;o->previousX=o->x-Q;o->previousY=o->y;o->type=i==0?18:i==1?10:0;o->age=100;}
 g.tick++;audio_sources_step76();V(audioSourceCount76==6);for(int i=0;i<audioSourceCount76;i++)V(audioSources76[i].type!=10);
 memset(g.enemies,0,sizeof(g.enemies));Obj*o=slot(g.enemies,MAXE);o->type=18;o->x=90*Q;o->y=100*Q;o->previousX=o->x-Q;o->previousY=o->y;g.tick++;audio_sources_step76();V(audioSourceCount76==1&&audioSources76[0].kind==2&&audioSources76[0].pan<0);float left=audioSources76[0].pan;
 sideBlend=1;g.tick++;audio_sources_step76();V(fabsf(audioSources76[0].pan-left)>.1f);o->previousX=o->x;o->previousY=o->y;g.tick++;audio_sources_step76();V(audioSourceCount76==0);
 o->previousY=o->y-Q;g.tick++;audio_sources_step76();V(audioSourceCount76==1);ex.travel=1;audio_sources_step76();V(audioSourceCount76==0);game_reset();V(audioSourceCount76==0&&audioSourceTick76==-1);
 printf("PASS %d passive audio source checks: actual movement,6-source bound, stable identities/no organic jets, camera pan, energy hum, stationary/death/scene reset and no game mutation\n",checks);return 0;
}
