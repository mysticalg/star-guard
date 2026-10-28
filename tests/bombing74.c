#define main star_guard_main
#include "../src/pc.c"
#undef main
static int checks;
#define V(x) do{checks++;if(!(x)){fprintf(stderr,"FAIL line%d ship%d tick%d %s\n",__LINE__,selectedShip,g.tick,#x);return 1;}}while(0)
static void arena(int ship){headless=1;selectedShip=ship;game_reset();g.mode=1;g.fx=0;g.wave=6;ex.section=3;ex.phase=ex.oldPhase=mission_phase(6);ex.intro=ex.travel=ex.entryY=0;ex.rearSpawned=1;environment_enter();game_step(0,2008);}
int main(void){
 for(int ship=0;ship<3;ship++)for(int manual=0;manual<2;manual++){
  arena(ship);Bombing*b=&ex.bombing;V(b->active&&!bombing_ready()&&!b->manual);int ticks=0;
  for(;ticks<24000&&!b->complete;ticks++){
   if(manual&&!b->stage){int target=-1;for(int i=0;i<8;i++)if(b->targets[i].hp>0&&!bombing_shielded(i)&&b->targets[i].y>24&&b->targets[i].y<215){target=i;break;}if(target>=0)game_pointer(b->targets[target].x,clamp(b->targets[target].y+75,140,245));}
   game_step(4,0);V(b->angle==0);V(b->payload>=0&&b->payload<=6);V(b->targetsDestroyed>=0&&b->targetsDestroyed<=8);
  }
  printf("ship%d manual%d targets%d ticks%d passes%d hits%d\n",ship,manual,b->targetsDestroyed,ticks,b->pass,b->hits);
  V(b->complete&&b->targetsDestroyed==8&&bombing_ready());V(!manual||b->manual);
 }
 arena(0);Bombing*b=&ex.bombing;game_pointer(75,90);V(b->manual);for(int stage=0;stage<4;stage++){b->stage=stage;b->timer=1;b->startX=75;b->startY=90;game_pointer(75,90);bombing_step();V(g.px==75*Q&&player_y==90*Q&&b->angle==0);}
 arena(0);b=&ex.bombing;int hp=b->targets[2].hp;b->bombs[0]=(GroundPayload){1,b->targets[2].x*Q,b->targets[2].y*Q,0,0,5};bombing_payload_step();V(b->targets[2].hp==hp&&b->hits==1);b->targets[0].hp=b->targets[1].hp=0;b->bombs[0]=(GroundPayload){1,b->targets[2].x*Q,b->targets[2].y*Q,0,0,5};bombing_payload_step();V(b->targets[2].hp==hp-3);
 int timer=b->timer,payload=b->payload;g.paused=1;for(int n=0;n<100;n++)game_step(4,'g');V(b->timer==timer&&b->payload==payload);g.paused=0;
 printf("PASS %d bombing74 checks: actual held-fire payload completion, all3ships/guided+manual, upright/full-control rearm, generator gates and pause\n",checks);return 0;
}
