typedef struct {int x,y,life,tint;} Trail;
typedef struct {
 int empDisable,empGrace,empTicks,empCount,empPulse,empX,empY;Obj empArcs[24];
 int intro,victory,victoryX,victoryY,stageScore,stageKills,stageSalvage,clearScore,clearKills,clearSalvage,clearBonus;
 int phase,oldPhase,transition,scrollSpeed,enemySpeed,backgroundY,bombs,bombFlash,options,optionsOn,followHead,multi,hazardSerial,nextParticle,soundTick;
 int followX[64],followY[64];Obj obstacles[MAXH];
 int travel,travelTick,nextPhase,entryY,entryDistance,phaseAge,optionCool,contactX,contactY,contactTick,bossSpawned;Obj impacts[12];Trail trails[384];int nextTrail;
 int bombAge,bombX,bombY,bombRadius,landScroll,bossWarning,clearTicks,satellite,satelliteCool,dropper,dropperCool,weaponDrop,pickupKind,pickupTicks,rearWarning,rearSpawned,bombSerial;
} Expansion;
Expansion ex;
static void expansion_reset(void){int i;u8*p=(u8*)&ex;for(i=0;i<(int)sizeof(ex);i++)p[i]=0;ex.phase=-1;ex.scrollSpeed=128;ex.enemySpeed=256;ex.bombs=3;ex.weaponDrop=1;ex.options=0;ex.optionsOn=1;ex.optionCool=1;for(i=0;i<64;i++){ex.followX[i]=160*Q;ex.followY[i]=230*Q;}}
