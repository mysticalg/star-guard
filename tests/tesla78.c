#define main star_guard_main
#include "../src/pc.c"
#undef main
static int checks;
#define V(x) do{checks++;if(!(x)){fprintf(stderr,"FAIL Tesla78 line%d: %s\n",__LINE__,#x);return 1;}}while(0)
static void arena78(int ship,int variant){headless=1;pilotConfigured=0;selectedShip=ship;configuredLevel=1;configuredMask=511;configuredArmour=configuredQualityPack=0;memset(starterVariants,0,sizeof(starterVariants));starterVariants[8]=variant;game_reset();g.mode=1;g.fx=0;g.wave=2;g.clock=-100000;ex.phase=ex.oldPhase=mission_phase(2);ex.intro=ex.travel=ex.entryY=0;ex.rearSpawned=1;ex.torpedoCool=100000;ex.options=0;g.shield=100000;environment_enter();}
int main(void){
 for(int ship=0;ship<3;ship++)for(int v=0;v<3;v++)for(int buff=0;buff<2;buff++){
  arena78(ship,v);ex.buffTicks[1]=buff?600:0;int energy[64],remainder[64],result[64],heat[64];
  for(int i=0;i<64;i++){result[i]=weapon_drain(8,i%7==0?12:3);energy[i]=g.weaponEnergy[8];remainder[i]=g.weaponDrainRemainder[8];heat[i]=g.weaponHeat[8];}
  V(energy[63]==0&&heat[63]==300&&!result[63]);
  arena78(ship,v);ex.buffTicks[1]=buff?600:0;
  for(int i=0;i<64;i++){g.weapon=i%8;int r=weapon_drain(8,i%7==0?12:3);V(r==result[i]&&g.weaponEnergy[8]==energy[i]&&g.weaponDrainRemainder[8]==remainder[i]&&g.weaponHeat[8]==heat[i]);int before=g.weaponDrainRemainder[8];weapon_drain(i%8,1);V(g.weaponDrainRemainder[8]==before);}
 }
 arena78(0,0);for(int i=0;i<6;i++)g.enemies[i]=(Obj){.on=1,.type=0,.hp=10000,.x=(130+i%3*22)*Q,.y=(130+i/3*20)*Q,.base=130+i%3*22,.entryTicks=12,.age=150,.strength=1,.difficultyReady=1};g.weapon=8;game_step(4,0);V(ex.empCritical>0&&ex.empCriticalCount==6);int tick=g.tick,bank=g.weaponEnergy[8],remain=g.weaponDrainRemainder[8],network=ex.empCritical,hp=g.enemies[0].hp*100-g.enemies[0].wound,freeze=g.enemies[0].empFreeze;game_step(0,'p');for(int i=0;i<200;i++)game_step(4,0);V(g.tick==tick&&g.weaponEnergy[8]==bank&&g.weaponDrainRemainder[8]==remain&&ex.empCritical==network&&g.enemies[0].hp*100-g.enemies[0].wound==hp&&g.enemies[0].empFreeze==freeze);game_step(0,'p');for(int i=0;i<24;i++)game_step(4,0);V(g.weaponEnergy[8]<bank&&g.enemies[0].hp*100-g.enemies[0].wound<hp&&g.enemies[0].empFreeze>0&&g.enemies[0].empFreeze<=12);g.weaponEnergy[8]=0;g.weaponHeat[8]=300;for(int i=0;i<5;i++)game_step(4,0);V(!ex.empCritical&&!ex.empTicks);for(int i=0;i<15;i++)game_step(0,0);V(!g.enemies[0].empFreeze);
 printf("PASS %d Tesla78 accounting/bank-switch/exhaustion/pause/livefeedback/freeze-release checks\n",checks);return 0;}
