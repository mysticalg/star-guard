#define main star_guard_main
#include "../src/pc.c"
#undef main
static int checks;
#define V(x) do{if(!(x)){fprintf(stderr,"FAIL %d: %s\n",__LINE__,#x);exit(1);}checks++;}while(0)
static void setup(void){headless=1;pilotConfigured=0;game_reset();g.mode=1;g.fx=0;g.shield=100000;ex.phase=ex.oldPhase=10;ex.intro=ex.travel=0;g.clock=-100000;ex.rearSpawned=1;}
static Obj* target(int x,int y,int ready){Obj*o=g.enemies;*o=(Obj){.on=1,.type=0,.hp=1000,.base=x,.x=x*Q,.y=y*Q,.entryTicks=ready};return o;}
int main(void){
 setup();for(int w=0;w<9;w++){Obj*o=target(160,-10,12);weapon_damage_enemy(o,1000000,w);V(o->hp==1000&&o->wound==0);o=target(-24,90,12);damage_enemy(o,1000000);V(o->hp==1000);o=target(160,90,0);weapon_damage_enemy(o,1000000,w);V(o->hp==1000);o->entryTicks=12;weapon_damage_enemy(o,100,w);V(o->hp<1000||o->wound>0);}
 Obj*o=target(160,90,0);for(int n=0;n<11;n++){game_step(0,0);weapon_damage_enemy(o,1000000,5);V(o->hp==1000);}game_step(0,0);V(o->entryTicks==12);weapon_damage_enemy(o,1000000,5);V(o->hp<=0);
 setup();o=target(160,-10,12);ex.bombX=160;ex.bombY=30;ex.bombAge=20;bomb_step();V(o->on&&o->hp==1000);o=target(160,90,0);ex.bombY=90;ex.bombAge=20;bomb_step();V(o->on);o->entryTicks=12;bomb_step();V(!o->on);
 setup();group(0,0);V(g.enemies[0].hp==3);setup();group(10,0);V(g.enemies[0].hp==3);setup();group(19,0);V(g.enemies[0].hp==17);setup();group(26,0);V(g.enemies[0].hp==2);
 setup();ex.section=journeyGiantAt[0];ex.encounterTier=1;ex.bossSpawned=1;g.clock=2000;g.groups=18;journey_after_encounter();V(ex.miniDone&&g.clock==0&&g.groups==0&&section_wave_target()>=6&&section_duration()==700);
 for(int mode=0;mode<2;mode++){headless=1;pilotConfigured=0;game_reset();g.mode=1;g.fx=0;int firstMini=-1,lastMiniEnd=-1,postWaves=0,formations=0,spawned=0,combat=0,oldMini=0;int types[27]={0},intro=0,transit=0,clear=0,bossTime=0;for(int t=0;t<20000&&!ex.giant.active;t++){
   g.shield=100000;g.px=160*Q;player_y=220*Q;if(mode){for(int w=0;w<9;w++){g.weaponLevel[w]=4;g.gearQuality[w]=3;}g.weapon=5;g.pilotLevel=10;}
   if(ex.travel)transit++;else if(ex.intro)intro++;else if(ex.victory)clear++;else if(ex.bossSpawned)bossTime++;else combat++;
   int groups=g.groups;game_step(4,0);if(g.groups>groups){formations++;if(ex.miniDone)postWaves++;}
   for(int n=0;n<MAXE;n++){Obj*a=g.enemies+n;if(!a->on)continue;if(!a->age){spawned++;types[clamp(a->type,0,26)]=1;}if(a->type==4){if(firstMini<0)firstMini=g.tick;weapon_damage_enemy(a,1000000,boss_weakness(a->variant));if(a->hp<=0)killed(a);}else{weapon_damage_enemy(a,1000000,mode?5:0);if(a->hp<=0)killed(a);}}
   if(ex.section==2&&ex.miniDone&&!oldMini)lastMiniEnd=g.tick;oldMini=ex.section==2&&ex.miniDone;
  }
  int varied=0;for(int n=0;n<27;n++)varied+=types[n];V(ex.giant.active);V(combat>=4000&&combat<=5000);V(postWaves>=6);V(g.tick-lastMiniEnd>=700);V(formations>=30);printf("MODE %d: final %.2fs, ordinary combat %.2fs, first mini %.2fs, post-mini %.2fs, formations %d, post waves %d\n",mode,g.tick/50.,combat/50.,firstMini/50.,(g.tick-lastMiniEnd)/50.,formations,postWaves);printf("BREAKDOWN opening %.2fs / transit %.2fs / clear %.2fs / miniboss %.2fs\n",intro/50.,transit/50.,clear/50.,bossTime/50.);
 }
 setup();ex.section=0;ex.intro=400;previousEx=ex;V(opening_closeup(1)==0);ex.intro=350;previousEx=ex;V(opening_closeup(1)==1);ex.intro=60;previousEx=ex;V(fabsf(opening_closeup(1)-.5f)<.001f);ex.intro=10;previousEx=ex;V(opening_closeup(1)==0);ex.intro=0;V(opening_closeup(1)==0);ex.intro=350;ex.section=1;V(opening_closeup(1)==0);ex.section=0;reducedMotion=1;V(opening_closeup(1)==0);
 printf("PASS %d entry/HP/fast-clear/post-mini/close-up checks\n",checks);return 0;
}
