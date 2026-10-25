#define main star_guard_main
#include "../src/pc.c"
#undef main
static int checks;
#define V(x) do{if(!(x)){fprintf(stderr,"FAIL %d %s\n",__LINE__,#x);exit(1);}checks++;}while(0)
static void arena(int m){selectedShip=0;game_reset();g.mode=1;g.wave=m;ex.section=journeyGiantAt[m-1];ex.phase=ex.oldPhase=mission_phase(m);g.fx=0;environment_enter();giant_start();ex.giant.timer=150;g.shield=10000;}
static void pulse(int t){ex.giant.timer=t-1;ex.giant.addWaves=2;giant_step(0);}
static int hazards(void){int n=0;for(int i=0;i<32;i++)n+=ex.env.hazards[i].on;return n;}
int main(void){headless=1;
 arena(1);g.shield=0;g.px=68*Q;player_y=230*Q;pulse(200);V(g.barrier==65);ex.giant.parts[0].hp=0;g.barrier=100;g.shield=0;pulse(200);V(g.barrier==100);
 arena(2);pulse(120);V(enemy_count()==2);memset(g.enemies,0,sizeof(g.enemies));ex.giant.parts[0].hp=ex.giant.parts[1].hp=0;pulse(120);V(enemy_count()==0);for(int i=2;i<6;i++)ex.giant.parts[i].hp=0;ex.giant.timer=100;V(!giant_open(6));ex.giant.timer=150;V(giant_open(6));
 arena(3);ex.giant.timer=160;V(!giant_open(0)&&giant_open(1));ex.giant.timer=320;V(giant_open(0)&&!giant_open(1));GiantPart*p=ex.giant.parts;g.shots[0]=(Obj){.on=1,.x=p[1].x*Q,.y=p[1].y*Q};pulse(320);int rounds=0;for(int i=0;i<MAXB;i++)rounds+=g.bullets[i].on;V(rounds==1&&p[1].hp==p[1].maxHp);
 arena(4);ex.giant.timer=40;V(!giant_open(0));ex.giant.timer=120;V(giant_open(0));pulse(80);V(hazards()==6);memset(ex.env.hazards,0,sizeof(ex.env.hazards));ex.giant.parts[0].hp=0;pulse(80);V(hazards()==3);memset(ex.env.hazards,0,sizeof(ex.env.hazards));ex.giant.parts[1].hp=0;pulse(80);V(hazards()==0);
 arena(5);ex.giant.timer=50;worm_positions();V(!giant_open(0)&&!giant_open(6));ex.giant.partsDestroyed=4;ex.giant.timer=220;worm_positions();V(giant_open(6));
 arena(6);g.shield=0;g.px=180*Q;player_y=230*Q;ex.giant.rayX=160;pulse(110);V(g.barrier==65);g.shield=0;g.barrier=100;ex.giant.parts[0].hp=ex.giant.parts[1].hp=0;pulse(110);V(g.barrier==100);
 for(int m=1;m<=6;m++){arena(m);for(int stage=1;stage<=5;stage++){ex.giant.phase=stage;if(stage>=4)ex.giant.parts[6].hp=ex.giant.parts[6].maxHp/2;giant_step(0);for(int i=0;i<9;i++){GiantPart*p=ex.giant.parts+i;if(m==5&&!p->visible)continue;V(p->x-p->w/2>=14&&p->x+p->w/2<=306);V(p->y-p->h/2>=24&&p->y+p->h/2<230);}}}
 sideBlend=0;cameraZoom=.58f;cameraX=480*(1-cameraZoom);cameraY=72+348*(1-cameraZoom);int x=125,y=192;flight_pointer(&x,&y);V(absn(x-100)<=2&&absn(y-230)<=2);
 printf("PASS %d actual tactical damage/spawn/window/target/camera checks\n",checks);return 0;
}
