#define main star_guard_main
#include "../src/pc.c"
#undef main
static int checks;
#define VERIFY(x) do{if(!(x)){fprintf(stderr,"FAIL %d: %s\n",__LINE__,#x);exit(1);}checks++;}while(0)
static void arena(int mission){game_reset();headless=1;g.mode=1;g.wave=mission;ex.phase=ex.oldPhase=mission_phase(mission);ex.intro=ex.entryY=ex.travel=0;ex.rearSpawned=1;g.clock=-100000;g.groups=SECTOR_WAVES;g.shield=1000000;}
static int duration(int weapon,int rank,int hz){arena(1);g.weapon=weapon;g.weaponLevel[weapon]=rank;double acc=0;int ticks=0;while(!g.weaponHeat[weapon]&&ticks<5000){acc+=1.0/hz;while(acc+.000000001>=.02&&!g.weaponHeat[weapon]){for(int i=0;i<MAXS;i++)g.shots[i].on=0;game_step(4,0);acc-=.02;ticks++;VERIFY(g.weaponEnergy[weapon]>=0&&g.weaponEnergy[weapon]<=100);VERIFY(g.kills==0&&g.salvagePicked==0&&g.wave==1);}}VERIFY(g.weaponHeat[weapon]==300);return ticks;}
static int rounds(int mission,int family,int variant){arena(mission);if(family==4)ex.bossSpawned=1;g.enemies[0]=(Obj){.on=1,.type=family,.x=120*Q,.y=80*Q,.base=family==4?10000:120,.hp=family==4?10000:1000,.age=family==4?121:0,.phase=family==4?1:0,.variant=variant};int total=0;for(int tick=0;tick<600;tick++){for(int i=0;i<MAXB;i++)g.bullets[i].on=0;for(int i=0;i<MAXH;i++)ex.obstacles[i].on=0;if(family!=4){g.enemies[0].on=1;g.enemies[0].x=120*Q;g.enemies[0].y=80*Q;}game_step(0,0);int count=0;for(int i=0;i<MAXB;i++)count+=g.bullets[i].on;VERIFY(count<=MAXB);total+=count;}return total;}
static int formations(int mission){arena(mission);g.clock=0;g.groups=0;for(int t=0;t<1000;t++){for(int i=0;i<MAXE;i++)g.enemies[i].on=0;for(int i=0;i<MAXB;i++)g.bullets[i].on=0;game_step(0,0);}return g.groups;}
int main(void){
 headless=1;
 for(int w=0;w<3;w++)for(int rank=1;rank<=4;rank++){int ticks=duration(w,rank,60),target=w==0?1250:w==1?600:500;VERIFY(absn(ticks-target)<=50);for(int h=1;h<4;h++){static const int rates[4]={60,120,240,1000};VERIFY(duration(w,rank,rates[h])==ticks);}printf("weapon=%d rank=%d seconds=%.2f\n",w+1,rank,ticks/50.0);}
 int lastSpawn=1000,lastFire=1000,lastBoss=1000;
 for(int mission=1;mission<=17;mission++){arena(mission);int spawn=formation_interval(),fire=enemy_fire_interval(),boss=attack_interval(40);VERIFY(spawn<=lastSpawn&&fire<=lastFire&&boss<=lastBoss);VERIFY(spawn>=128&&fire>=86&&boss>=28);lastSpawn=spawn;lastFire=fire;lastBoss=boss;}
 for(int family=0;family<=26;family++){if(family==4||family<4||family>=5){int early=rounds(1,family,0),middle=rounds(9,family,0),late=rounds(17,family,0);VERIFY(late>=early);printf("family=%d rounds/12s early=%d middle=%d late=%d\n",family,early,middle,late);}}
 for(int variant=0;variant<17;variant++){int early=rounds(1,4,variant),middle=rounds(9,4,variant),late=rounds(17,4,variant);printf("boss=%d rounds/12s early=%d middle=%d late=%d\n",variant,early,middle,late);VERIFY(late>=early);}
 int a=formations(1),b=formations(9),c=formations(17);VERIFY(a<=b&&b<c);printf("formations/20s: %d %d %d\n",a,b,c);
 arena(17);for(int i=0;i<MAXB-1;i++)g.bullets[i].on=1;Obj shooter={.x=160*Q,.y=60*Q};enemy_volley(&shooter,500);int count=0;for(int i=0;i<MAXB;i++)count+=g.bullets[i].on;VERIFY(count==MAXB);
 for(int family=0;family<=26;family++){if(family==4)continue;arena(17);for(int i=0;i<MAXE-1;i++)g.enemies[i].on=1;group(family,0);count=0;for(int i=0;i<MAXE;i++)count+=g.enemies[i].on;VERIFY(count<=MAXE);}
 printf("PASS %d game-level balance/progression checks\n",checks);return 0;
}
