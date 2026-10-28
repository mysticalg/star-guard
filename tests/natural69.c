#define main star_guard_main
#include "../src/pc.c"
#undef main
int main(void){headless=1;selectedShip=2;configuredLevel=1;pilotConfigured=0;game_reset();g.mode=1;g.fx=0;int lastGroup=0,start=0,mini=0,clears=0;printf("{\"fixture\":\"Normal stock Bastion, actual pointer targeting and held fire, no HP/energy/God mutations. Automatic torpedo remains. Normal continues.\",\"waves\":[");
for(int t=0;t<90000;t++){int command=0;if(g.mode==3)command=13;if(g.mode==2)break;if(ex.bossSpawned){mini=1;break;}
 int best=-1,dist=999999;for(int i=0;i<MAXE;i++)if(g.enemies[i].on&&enemy_damageable(g.enemies+i)){int d=absn(g.enemies[i].x-g.px)+absn(g.enemies[i].y-player_y)/4;if(d<dist){dist=d;best=i;}}
 if(best>=0)game_pointer(clamp(g.enemies[best].x/Q,24,296),230);else game_pointer(160,230);
 if(g.groups!=lastGroup){if(lastGroup){if(clears++)printf(",");printf("{\"wave\":%d,\"ticks\":%d}",lastGroup,t-start);}lastGroup=g.groups;start=t;}
 if(g.weaponHeat[g.weapon]&&g.equippedMask&2)command='2';else if(!g.weaponHeat[0]&&g.weapon!=0)command='1';
 game_step(4,command);}
printf("],\"firstMiniReached\":%d,\"activeSeconds\":%.2f,\"kills\":%d,\"lives\":%d,\"continuesUsed\":%d,\"mode\":%d,\"groups\":%d}\n",mini,ex.progressTicks/50.,g.kills,g.lives,3-g.continues,g.mode,g.groups);return !mini;}
