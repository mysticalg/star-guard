#define main star_guard_main
#include "../src/pc.c"
#undef main
static int checks;
#define V(x) do{checks++;if(!(x)){fprintf(stderr,"FAIL %d %s M%d S%d wave%d age%d remaining%d\n",__LINE__,#x,g.wave,ex.section,g.groups,ex.waveAge,regular_wave_remaining());return 1;}}while(0)
static void arena(int m,int section){headless=1;pilotConfigured=0;selectedShip=0;configuredLevel=1;configuredMask=511;game_reset();g.mode=1;g.fx=0;g.wave=m;ex.section=section;ex.phase=ex.oldPhase=mission_phase(m);ex.intro=ex.travel=ex.entryY=0;ex.rearSpawned=1;ex.phaseAge=100;g.shield=100000;environment_enter();}
int main(void){
 arena(1,0);game_step(0,0);V(g.groups==1&&ex.waveActive&&enemy_count()>0);int original=enemy_count();
 for(int t=0;t<800;t++){g.shield=100000;game_step(0,0);V(g.groups==1&&ex.waveActive);}
 V(ex.waveAge>=600&&ex.waveEnrageSerial==1);V(enemy_count()==original);V(!ex.bossSpawned);
 Obj *o=g.enemies;V(regular_enemy_enraged(o));int hot=enemy_strength_interval(o,128);ex.waveAge=0;int cold=enemy_strength_interval(o,128);V(hot<cold);ex.waveAge=800;
 int age=ex.waveAge;game_step(0,'p');for(int i=0;i<100;i++)game_step(0,0);V(ex.waveAge==age);game_step(0,'p');
 memset(g.enemies,0,sizeof(g.enemies));ex.rearWarning=2;regular_wave_step();V(ex.waveActive);ex.rearWarning=0;
 g.enemies[0]=(Obj){.on=1,.type=19,.hp=1,.x=160*Q,.y=100*Q,.entryTicks=12};killed(g.enemies);V(enemy_count()==2);regular_wave_step();V(ex.waveActive);
 memset(g.enemies,0,sizeof(g.enemies));game_step(0,0);V(!ex.waveActive&&ex.waveRest==40&&g.groups==1);
 for(int t=0;t<39;t++){game_step(0,0);V(g.groups==1);}game_step(0,0);V(g.groups==2&&ex.waveActive&&ex.waveAge==0);
 /* Every offscreen entrant blocks; survivors recycle without healing or rewards. */
 o=g.enemies;o->type=0;o->strength=1;o->difficultyReady=1;o->age=150;o->vy=0;o->y=(play_max_y()+80)*Q;o->wound=7;int hp=o->hp,score=g.score;game_step(0,0);V(o->on&&o->age==0&&o->hp==hp&&o->wound==7&&g.score==score&&g.groups==2);
 /* Clock can be arbitrarily overdue; remaining regular enemies still block the boss. */
 g.groups=section_wave_target();g.clock=100000;for(int t=0;t<150;t++){g.shield=100000;game_step(0,0);V(!ex.bossSpawned);}
 memset(g.enemies,0,sizeof(g.enemies));for(int t=0;t<142;t++)game_step(0,0);V(ex.bossSpawned&&ex.encounterTier==1);
 /* First-level easing happens once and excludes bosses; M2 unchanged. */
 arena(1,0);Obj a={.on=1,.type=0,.hp=10,.strength=1};enemy_strength_prepare(&a);V(a.hp*100-a.wound==920);enemy_strength_prepare(&a);V(a.hp*100-a.wound==920);Obj bossObj={.on=1,.type=4,.hp=10};enemy_strength_prepare(&bossObj);V(bossObj.hp==10&&!bossObj.wound);
 arena(2,0);a=(Obj){.on=1,.hp=10,.strength=1};enemy_strength_prepare(&a);V(a.hp==10&&!a.wound);
 /* Real secret commands, every direct damage/death path, reset and skip cleanup. */
 arena(3,1);game_step(0,2008);V(g.devInvincible&&g.devUsed);g.shield=g.active=ex.invincibleTicks=ex.phoenixTicks=0;int lives=g.lives,armour=g.barrier;hurt_amount(160,230,0,1,1000,1);storm_hurt(1000);wall_ship_lost();worm_ship_lost();V(g.lives==lives&&g.barrier==armour);game_step(0,2008);V(!g.devInvincible&&g.devUsed);hurt_amount(160,230,0,1,20,1);V(g.barrier<armour);
 game_step(0,2009);V(g.lives==5&&g.barrier==100&&g.energy==100&&g.equippedMask==511);for(int w=0;w<9;w++)V(g.weaponLevel[w]==4&&g.weaponEnergy[w]==100&&g.weaponHeat[w]==0&&g.inventoryBits[w]==7);V(ex.options==5&&ex.optionsOn&&ex.satellite&&ex.dropper&&ex.bombs==5&&ex.chaff==2&&ex.empBombs==1);
 for(int m=1;m<=6;m++){arena(m,0);g.enemies[0].on=g.bullets[0].on=g.shots[0].on=1;ex.waveActive=1;ex.waveAge=900;ex.giant.active=1;ex.bossSpawned=1;game_step(0,2010);V(!enemy_count()&&!g.bullets[0].on&&!g.shots[0].on&&!ex.waveActive&&!ex.giant.active&&!ex.bossSpawned);if(m<6){V(g.wave==m+1&&ex.section==0&&g.mode==1);for(int t=0;t<10;t++)game_step(0,0);V(ex.travel||ex.intro);}else V(ex.campaignWon&&g.mode==2);}
 game_step(0,'r');V(!g.devUsed&&!g.devInvincible&&g.wave==1);
 printf("PASS %d sequential pending/clear/recycle/enrage/pause/boss/easing/dev-key checks\n",checks);return 0;
}
