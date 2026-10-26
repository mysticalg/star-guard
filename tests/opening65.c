#define main star_guard_main
#include "../src/pc.c"
#undef main
static int checks;
#define V(x) do{checks++;if(!(x)){fprintf(stderr,"FAIL %d %s clock%d active%d groups%d\n",__LINE__,#x,g.clock,ex.progressTicks,g.groups);return 1;}}while(0)
int main(void){headless=1;configuredLevel=1;selectedShip=0;game_reset();g.mode=1;g.fx=0;int born=0,typeMask=0,attackBirths=0,firstBirth=-1,firstMini=-1,front=0,warning=0,recovery=0,afterFront=0,beforeStormWaves=0,afterStormWaves=0,previousGroups=0,peaks=0;
for(int t=0;t<11000;t++){g.shield=100000;int before=ex.env.timer;game_step(0,0);int now=enemy_count();if(now>peaks)peaks=now;if(g.groups>previousGroups){for(int n=0;n<MAXE;n++){Obj*o=g.enemies+n;if(o->on&&o->age<=1&&o->type!=4){born++;typeMask|=1<<o->type;if(firstBirth<0)firstBirth=ex.progressTicks;}}if(ex.env.timer<1000)beforeStormWaves++;if(ex.env.timer>=1500)afterStormWaves++;previousGroups=g.groups;}
for(int n=0;n<MAXB;n++)if(g.bullets[n].on&&g.bullets[n].age<=1)attackBirths++;
if(ex.env.timer!=before){front+=ex.env.weather.stage==2;warning+=ex.env.weather.stage==1;recovery+=ex.env.weather.stage==3;if(ex.env.timer>=1500){V(!ex.env.weather.stage&&!ex.env.weather.kind&&!weather_exclusive());afterFront++;}}
if(ex.progressTicks<4500)V(!ex.bossSpawned);
if(ex.bossSpawned){firstMini=ex.progressTicks;break;}}
V(firstBirth>=0&&firstBirth<10);V(firstMini>=4500&&firstMini<9000);V(front==300&&warning==100&&recovery==100);V(afterFront>3000&&beforeStormWaves>0&&afterStormWaves>0&&ex.env.weather.strikeSerial==5);V(born>100&&__builtin_popcount((unsigned)typeMask)>6&&attackBirths>100);V(mission_active_target()==9000);V(!mission_report_ready());
printf("{\"fixture\":\"Production game_step normal first route, no firing/clearing/skips; renewed large player shield isolates timing and observed real enemy attacks. Not human survival.\",\"firstEnemyActiveSeconds\":%.2f,\"firstMiniActiveSeconds\":%.2f,\"missionTargetSeconds\":180,\"warningTicks\":%d,\"frontTicks\":%d,\"recoveryTicks\":%d,\"ordinaryBirths\":%d,\"types\":%d,\"hostileBulletBirthSamples\":%d,\"waves\":%d,\"peakNPC\":%d,\"checks\":%d}\n",firstBirth/50.0,firstMini/50.0,warning,front,recovery,born,__builtin_popcount((unsigned)typeMask),attackBirths,g.groups,peaks,checks);return 0;}
