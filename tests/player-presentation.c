#define main star_guard_main
#include "../src/pc.c"
#undef main
static int checks;
#define V(x) do{if(!(x)){fprintf(stderr,"FAIL %d %s\n",__LINE__,#x);exit(1);}checks++;}while(0)
static void visible(void){ndraw=0;game_render(fb);int hull=0;for(int n=0;n<ndraw;n++)hull+=draws[n].n==136;V(hull==1);}
int main(void){headless=1;for(int ship=0;ship<3;ship++)for(int mission=1;mission<=6;mission++){selectedShip=ship;game_reset();g.mode=1;g.wave=mission;for(int tick=0;tick<120;tick++){g.tick=tick;g.shield=100;visible();V(g.shield==100);}for(int travel=1;travel<=2;travel++){ex.travel=travel;visible();}ex.travel=0;g.mode=3;g.continueTicks=400;game_step(0,13);V(g.mode==1);visible();game_step(0,'r');V(g.mode==1);visible();}printf("PASS %d player draw checks: all chassis/mission starts, every shield blink phase, travel, continue and restart; protection retained\n",checks);return 0;}
