#define main star_guard_main
#include "../src/pc.c"
#undef main
static int checks;
#define V(x) do{if(!(x)){fprintf(stderr,"FAIL %d %s\n",__LINE__,#x);exit(1);}checks++;}while(0)
int main(void){headless=1;g.seed=12345;for(int reduced=0;reduced<2;reduced++)for(int side=0;side<2;side++)for(int i=0;i<720;i++){RainDrop a,b;rain_drop_at(i,7.2f,side*PI/2,reduced,&a);rain_drop_at(i,7.201f,side*PI/2,reduced,&b);V(isfinite(a.x)&&isfinite(a.y));V(a.alpha>0&&a.alpha<=.41f&&a.width<1.8f&&a.length<=27);V(side?a.dx<-.98f:a.dy>.98f);float dx=b.x-a.x,dy=b.y-a.y,d=sqrtf(dx*dx+dy*dy);if(d<4&&d>.02f)V((dx*a.dx+dy*a.dy)/d>.998f);RainDrop same;rain_drop_at(i,7.2f,side*PI/2,reduced,&same);V(memcmp(&a,&same,sizeof a)==0);}V(g.seed==12345);printf("PASS %d rain checks: finite bounded soft particles, velocity follows wind/flight direction, repeatable fixed-time sampling, reduced motion, no gameplay RNG consumption\n",checks);return 0;}
