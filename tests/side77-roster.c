#define main star_guard_main
#include "../src/pc.c"
#undef main
int main(void){headless=1;game_reset();g.mode=1;for(int m=1;m<=6;m++)for(int s=0;s<journeyCounts[m-1];s++){g.wave=m;ex.section=s;ex.phase=ex.oldPhase=mission_phase(m);ex.giant=(Giant){0};terrain.active=0;ex.intro=ex.travel=0;environment_enter();if(!flight_side_mode())continue;printf("M%d S%d phase%d kind%d quota%d mini%d roster:",m,s+1,ex.phase,journey_kind(),section_wave_target(),journeyMinis[m-1][s]);for(g.groups=0;g.groups<section_wave_target();g.groups++)printf(" %d",formation_type());puts("");}return 0;}
