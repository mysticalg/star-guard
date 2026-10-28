typedef struct {int x,y,life,tint;} Trail;
typedef struct {
 Bombing bombing;
 int waveActive,waveAge,waveRest,waveCleared,waveEnrageSerial;
 float lightAmbient,lightTarget;int lightReady;
 unsigned strengthSerial;
 int sectionHandover,oldEnvironmentTick70;
 int cavernWorldQ,cavernWorldReady;
 int floorShotSerial,floorShotX,floorShotY;int lastDamageTick,lastDamageAmount,lastDamageKind,lastDamageSerial,lastDamageFatal,blockedTick,damagePrevX,damagePrevY;int progressTicks,escortClock,escortUsed,escortWarning,escortOwner;int resistTicks,resistFamily,resistX,resistY;
 int miniRewardMask,giantRewardMask,completedMissionMask,pendingParts,partsCollected,partPickupTicks;Obj partBundle;
 int refit,refitNext,collectedFamily,collectedVariant,collectedTicks;u32 combatSeed;int buffTicks[5],critSerial,critHead;CritNotice critNotices[8];
 int globuleHP[3],globuleDamageUntil[3],globuleGrace,globuleDrain,globuleSerial,globuleClearSerial;
 int repairCool,repairSerial,repairFlash,leeches,leechGrace,leechDrain,leechSerial,purgeSerial,baseDamage,section,oldKind,missionHandover,missionTicks,campaignWon,miniDone,encounterTier,fatigue[9],podContact[5];Environment env;Giant giant;
 int empDisable,empGrace,empTicks,empCount,empPulse,empX,empY,empCritical,empNetwork,empFeedbackAge,empCriticalCount;Obj empArcs[96];
 int cavernSide,cavernLeg,cavernTurn;
 int shieldDrops,invincibleTicks,empBombs,empStop;
 int shieldEligible79[6],shieldUsed79;
 int chaff,chaffDeploy;Obj mines[96];
 int torpedoCool,torpedoSerial,torpedoBlasts;
 int phoenixMeter,phoenixTicks,phoenixSerial,beamAim;
 int comboKills,comboBonus,comboTicks,novaKills,missileBlasts;
 int intro,victory,victoryX,victoryY,stageScore,stageKills,stageSalvage,clearScore,clearKills,clearSalvage,clearBonus;
 int phase,oldPhase,transition,scrollSpeed,enemySpeed,backgroundY,bombs,bombFlash,options,optionsOn,followHead,multi,hazardSerial,nextParticle,soundTick;
 int followX[64],followY[64];Obj obstacles[MAXH];
 int travel,travelTick,nextPhase,entryY,entryDistance,phaseAge,optionCool,contactX,contactY,contactTick,bossSpawned;Obj impacts[12];Trail trails[384];int nextTrail;
 int bombAge,bombX,bombY,bombRadius,landScroll,bossWarning,clearTicks,satellite,satelliteCool,dropper,dropperCool,weaponDrop,pickupKind,pickupTicks,rearWarning,rearSpawned,bombSerial;
} Expansion;
Expansion ex;
static void expansion_reset(void){int i;u8*p=(u8*)&ex;for(i=0;i<(int)sizeof(ex);i++)p[i]=0;ex.phase=-1;ex.env.kind=-1;ex.scrollSpeed=128;ex.enemySpeed=256;ex.bombs=3;ex.chaff=2;ex.empBombs=1;ex.weaponDrop=1;ex.options=0;ex.optionsOn=1;ex.optionCool=1;for(i=0;i<64;i++){ex.followX[i]=160*Q;ex.followY[i]=230*Q;}}
