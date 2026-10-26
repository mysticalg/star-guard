#define main star_guard_main
#include "../src/pc.c"
#undef main
static int checks;
#define V(x) do{checks++;if(!(x)){fprintf(stderr,"FAIL %d %s\n",__LINE__,#x);return 1;}}while(0)
int main(void){headless=1;game_reset();g.mode=1;unsigned seed=g.seed,fx=fxseed;for(int q=0;q<3;q++)for(int strike=0;strike<32;strike++){quality=q;nquad=0;renderedStormForks=0;draw_storm_forks(60,strike,1);int count=nquad,forks=renderedStormForks;V(count>0&&count<NQ&&forks>=8&&forks<=20);SDL_Vertex *copy=malloc(count*4*sizeof(SDL_Vertex));memcpy(copy,vertices,count*4*sizeof(SDL_Vertex));for(int i=0;i<count*4;i++){V(isfinite(vertices[i].position.x)&&isfinite(vertices[i].position.y));V(vertices[i].color.a>=0&&vertices[i].color.a<=1);}g.tick+=137;nquad=0;renderedStormForks=0;draw_storm_forks(60,strike,1);V(nquad==count&&renderedStormForks==forks&&!memcmp(copy,vertices,count*4*sizeof(SDL_Vertex)));free(copy);V(g.seed==seed&&fxseed==fx);}printf("PASS %d bounded deterministic forks/tertiary tips/allquality/finite vertices/alpha/no game-or-FX-RNG checks\n",checks);return 0;}
