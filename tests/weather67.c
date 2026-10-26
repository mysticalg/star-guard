#define main star_guard_main
#include "../src/pc.c"
#undef main
static int checks;
#define V(x) do{checks++;if(!(x)){fprintf(stderr,"FAIL %d %s M%dS%d t%d\n",__LINE__,#x,g.wave,ex.section,ex.env.timer);return 1;}}while(0)
static void setup(int m,int s){game_reset();g.mode=1;g.wave=m;g.fx=0;ex.section=s;ex.phase=ex.oldPhase=mission_phase(m);ex.intro=ex.travel=0;environment_enter();}
int main(void){headless=1;selectedShip=0;configuredLevel=1;int profiles=0;
for(int m=1;m<=6;m++)for(int s=0;s<journeyCounts[m-1];s++){setup(m,s);int front=0,forecast=0,recovery=0,exclusive=0,peak=0,bands[3]={0},start=0,end=0;for(int t=1;t<=2500;t++){g.tick++;ex.env.timer=t;g.shield=100000;weather_step();Weather*w=&ex.env.weather;front+=w->stage==2;forecast+=w->stage==1;recovery+=w->stage==3;exclusive+=weather_exclusive();int count=0;for(int n=0;n<48;n++){WeatherActor*a=w->actors+n;count+=a->on;if(a->on&&a->age==1)bands[a->x/Q<110?0:a->x/Q>210?2:1]++;}if(count>peak)peak=count;V(count<=48);if(w->kind&&w->stage==2)profiles|=1<<w->kind;if(weather_profile()!=WEATHER_ASTEROID)V(!weather_exclusive());if(journey_kind()!=ENV_WALL&&weather_profile()!=WEATHER_ASTEROID)V(!environment_exclusive());}
 V(front==(m==1&&s<=1?400:500));V(forecast==100&&recovery==100);V(peak<=48);if(journey_kind()==ENV_SPACE){V(bands[0]>0&&bands[1]>0&&bands[2]>0);V(exclusive>=500&&exclusive<780);}else if(weather_profile()!=WEATHER_ASTEROID)V(exclusive==0);
}
V(profiles==126);setup(1,0);int front=0,bursts=0,last=0;for(int t=1;t<=4500;t++){ex.env.timer=t;int stage=weather_stage();front+=stage==2;if(stage==2&&last!=2)bursts++;last=stage;}V(front==800&&bursts==2);setup(1,1);front=0;for(int t=1;t<=2250;t++){ex.env.timer=t;front+=weather_stage()==2;}V(front==400);
setup(2,1);Weather*w=&ex.env.weather;w->kind=WEATHER_ACID;Obj*o=g.enemies;*o=(Obj){.on=1,.type=0,.hp=10,.entryTicks=12,.age=20,.x=40*Q,.y=100*Q};V(!weather_enemy_hit(o,450)&&o->hp==10);o->age=110;V(weather_enemy_hit(o,450));V(o->hp*100-o->wound==920);V(!weather_enemy_hit(o,450));g.tick+=100;V(weather_enemy_hit(o,450)&&o->hp*100-o->wound==840);
*o=(Obj){.on=1,.type=4,.hp=100,.base=100,.phase=3,.age=300,.openingTicks=100};V(!weather_enemy_hit(o,450)&&o->hp==100);
setup(3,0);ex.env.timer=610;WeatherActor*a=weather_spawn(WEATHER_ASTEROID,160,100,0,100,12);V(a&&a->age==-25);int initialY=a->y;for(int t=0;t<25;t++)weather_step();V(a->y==initialY&&a->age==0);weather_step();V(a->y>initialY);V(mission_active_target()==18000);g.weapon=3;g.unlocked|=8;for(int i=0;i<65;i++){g.shield=100000;game_step(4,0);}V(ex.env.weather.interceptions>0);
setup(6,10);giant_start();ex.giant.timer=150;int events=0,echo=0;for(int i=0;i<450;i++){g.shield=100000;game_step(0,0);events=ex.giant.flybySerial;for(int n=0;n<12;n++)echo+=ex.giant.echoes[n].tick>0&&g.tick-ex.giant.echoes[n].tick<24;}
V(events>=3&&events<8&&echo>0);g.paused=1;int tick=g.tick,serial=ex.giant.flybySerial;for(int i=0;i<100;i++)game_step(0,0);V(g.tick==tick&&ex.giant.flybySerial==serial);
printf("PASS %d all38 short biome windows/3earlyM1bursts/asteroidfullwidth+cue+shortexclusive/NPCgrace+actualdamage/bossimmunity/realmotionboundedtrails+events/pause checks\n",checks);return 0;}
