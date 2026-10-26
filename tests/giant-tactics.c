#define main star_guard_main
#include "../src/pc.c"
#undef main
static int checks;
#define V(x) do{if(!(x)){fprintf(stderr,"FAIL %d %s m%d f%d\n",__LINE__,#x,g.wave,ex.giant.form);exit(1);}checks++;}while(0)
static void arena(int m,int f){selectedShip=0;game_reset();g.mode=1;g.wave=m;ex.section=journeyGiantAt[m-1];ex.phase=ex.oldPhase=mission_phase(m);g.fx=0;environment_enter();giant_start();ex.giant.timer=150;g.shield=10000;Giant*b=&ex.giant;if(f){b->parts[0].hp=b->parts[1].hp=0;b->partsDestroyed=2;giant_check_form();}if(f==2){for(int i=2;i<6;i++)b->parts[i].hp=0;b->phase=4;b->parts[6].hp=b->parts[6].maxHp/2;giant_check_form();}for(int i=0;i<250;i++)giant_step(0);g.viewZoom=b->zoom;}
static int hazards(void){int n=0;for(int i=0;i<32;i++)n+=ex.env.hazards[i].on;return n;}
int main(void){headless=1;
arena(2,0);environment_spawn(20,160,80,0,0,12,4);bullet(160*Q,180*Q,0,200);giant_start();V(!giant_add_count());for(int n=0;n<MAXB;n++)V(!g.bullets[n].on);ex.giant.timer=0;ex.giant.formAge=0;ex.giant.attackClock=0;ex.giant.escortWaves=0;memset(g.enemies,0,sizeof(g.enemies));for(int n=0;n<119;n++)giant_step(0);V(ex.giant.formAge==0&&ex.giant.attackKind==0);giant_step(0);V(ex.giant.formAge==1&&ex.giant.aimX==g.px/Q);for(int n=0;n<59;n++)giant_step(0);V(ex.giant.formAge==60&&ex.giant.escortWaves==1&&enemy_count()==2);
arena(6,0);int defense=ex.giant.parts[0].hp;ex.baseDamage=8;giant_start();V(ex.giant.parts[0].hp==defense-1200&&ex.giant.parts[1].hp==defense-1200);
arena(1,0);Giant*b=&ex.giant;b->formAge=150;g.shield=0;g.px=b->parts[0].x*Q;player_y=140*Q;giant_attacks();V(g.barrier==65);b->parts[0].hp=0;g.shield=0;g.barrier=100;giant_attacks();V(g.barrier==100);
arena(1,1);b=&ex.giant;b->formAge=100;b->attackSide=0;g.shield=0;g.px=b->parts[4].x*Q;player_y=(play_min_y()+40*(play_max_y()-play_min_y())/120)*Q;giant_attacks();V(g.barrier==65);b->parts[4].hp=0;g.shield=0;g.barrier=100;giant_attacks();V(g.barrier==100);
arena(1,2);b=&ex.giant;b->formAge=90;b->safeX=160;g.shield=0;g.px=160*Q;giant_attacks();V(g.barrier==100);
arena(2,0);b=&ex.giant;b->escortWaves=0;memset(g.enemies,0,sizeof(g.enemies));for(int n=0;n<4;n++){b->formAge=60+n*giant_period();giant_attacks();}V(b->escortWaves==2&&enemy_count()==4);
arena(2,1);V(giant_add_count()==2);for(int n=0;n<800;n++)giant_step(0);V(giant_add_count()==2);for(int n=0;n<32;n++)ex.env.hazards[n].on=0;giant_step(0);V(giant_add_count()==0);int hp=ex.giant.parts[4].hp-100;ex.giant.parts[4].hp=hp;ex.giant.formAge=giant_period()-1;giant_step(0);V(ex.giant.parts[4].hp==hp);
arena(2,2);b=&ex.giant;b->formAge=120;g.shield=0;g.barrier=100;g.px=160*Q;player_y=(play_max_y()-30)*Q;giant_attacks();V(g.barrier==100);/* Lower corridor remains clear during the full-width claw sweep. */
arena(3,0);b=&ex.giant;b->formAge=0;V(giant_open(0)&&!giant_open(1));b->formAge=160;V(!giant_open(0)&&giant_open(1));memset(g.bullets,0,sizeof(g.bullets));g.shots[0]=(Obj){.on=1,.x=b->parts[0].x*Q,.y=b->parts[0].y*Q};giant_step(0);V(b->parts[0].hp==b->parts[0].maxHp);int bullets=0;for(int i=0;i<MAXB;i++)bullets+=g.bullets[i].on;V(!bullets);
arena(4,0);b=&ex.giant;b->attackClock=30;V(!giant_open(0));b->attackClock=120;V(giant_open(0));memset(ex.env.hazards,0,sizeof(ex.env.hazards));b->formAge=80;giant_attacks();V(hazards()==6);memset(ex.env.hazards,0,sizeof(ex.env.hazards));b->parts[0].hp=0;giant_attacks();V(hazards()==3);memset(ex.env.hazards,0,sizeof(ex.env.hazards));b->parts[1].hp=0;giant_attacks();V(hazards()==0);
arena(6,1);b=&ex.giant;b->formAge=100;b->aimX=160;g.shield=0;g.px=160*Q;giant_attacks();V(g.barrier==65);b->parts[4].hp=b->parts[5].hp=0;g.barrier=100;g.shield=0;giant_attacks();V(g.barrier==100);
for(int m=1;m<=6;m++)for(int f=0;f<3;f++){arena(m,f);b=&ex.giant;for(int i=0;i<9;i++){GiantPart*p=b->parts+i;if(!p->hp||(m==5&&!p->visible))continue;float z=b->zoom,x=160+(p->x-160)*z,y=140+(p->y-140)*z;V(x-p->w*z/2>=0&&x+p->w*z/2<=320);V(y-p->h*z/2>=24&&y+p->h*z/2<=256);}b->morph=45;int hp=b->parts[6].hp;giant_raw_hit(6,5000);V(b->parts[6].hp==hp);}
arena(5,0);ex.giant.timer=120;int wx,wy,wv,wa;worm_path(0,6,&wx,&wy,&wv,&wa);V(absn(wa-90)>10);
sideBlend=0;cameraZoom=.32f;cameraX=480*(1-cameraZoom);cameraY=72+348*(1-cameraZoom);int x=125,y=192;flight_pointer(&x,&y);V(absn(x-(160+(125-160)/.32f))<=2);V(absn(y-(140+(192-140)/.32f))<=2);
printf("PASS %d tactical source destruction, finite escorts/no-heal, reflector fairness, vents, power buses,18 layouts/morph gates/camera checks\n",checks);return 0;}
