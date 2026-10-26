#define main star_guard_main
#include "../src/pc.c"
#undef main
static int checks;
#define V(x) do{if(!(x)){fprintf(stderr,"FAIL %d %s\n",__LINE__,#x);exit(1);}checks++;}while(0)
int main(void){headless=1;for(int ship=0;ship<3;ship++){selectedShip=ship;for(int w=0;w<9;w++)starterVariants[w]=w%3;game_reset();g.mode=1;V(g.chassis==ship&&g.unlocked==511);for(int w=0;w<9;w++){V(g.variantEquip[w]==w%3&&g.inventoryBits[w]==(1|(1<<(w%3))));V(!equip_variant(w,0));for(int v=0;v<3;v++){collect_variant(w,v);ex.refit=1;V(equip_variant(w,v));V(g.variantEquip[w]==v);ex.refit=0;}}V(!equip_variant(-1,0)&&!equip_variant(9,0));ex.refit=1;V(!equip_variant(0,3));ex.refit=0;g.mode=3;g.continueTicks=400;int before=g.variantEquip[0],owned=g.inventoryBits[0];game_step(0,13);V(g.variantEquip[0]==before&&g.inventoryBits[0]==owned);}
 selectedShip=0;memset(starterVariants,0,sizeof(starterVariants));game_reset();g.mode=1;g.wave=1;ex.section=2;ex.phase=ex.oldPhase=mission_phase(1);ex.intro=0;int tick=g.tick;ex.progressTicks=mission_active_target();journey_advance();V(ex.refit&&ex.refitNext==2&&g.wave==1);int energy=g.weaponEnergy[0];game_step(4,0);V(g.tick==tick&&g.weaponEnergy[0]==energy);V(!equip_variant(0,1));collect_variant(0,1);game_step(0,1001);V(g.variantEquip[0]==1);game_step(0,13);V(!ex.refit&&g.wave==2&&ex.section==0&&g.variantEquip[0]==1&&ex.missionHandover);
 game_reset();g.mode=1;g.shield=100000;ex.phase=ex.oldPhase=mission_phase(1);ex.intro=0;g.clock=-100000;g.groups=6;int weapon=g.weapon;g.drops[0]=(Obj){.on=1,.type=10,.variant=3,.heading=2,.x=g.px,.y=player_y};game_step(0,0);V(g.weapon==weapon&&(g.inventoryBits[3]&(1<<2))&&ex.collectedTicks>0);
 printf("PASS %d legal slot/81-combination inventory/refit/continue/no-forced-swap checks\n",checks);return 0;
}
