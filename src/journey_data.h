/* Six continuous journeys. Legacy scenery ids remain internal asset references. */
#define MISSION_COUNT 6
#define JOURNEY_SECTIONS_MAX 12
enum { ENV_SKY,ENV_OCEAN,ENV_WRECK,ENV_ROOT,ENV_HIVE,ENV_WALL,ENV_CRYSTAL,ENV_BLIZZARD,ENV_ICE,ENV_EM,ENV_MOUTH,ENV_BEAST,ENV_TEETH,ENV_REACTOR,ENV_FORTRESS,ENV_ASH,ENV_SPACE,ENV_ESCAPE,ENV_BASE };
static const int journeyCounts[6]={3,3,5,3,4,8};
static const int journeyPhases[6][JOURNEY_SECTIONS_MAX]={{10,7,11},{6,5,2},{4,12,13,12,15},{8,16,5},{1,0,5,1},{3,12,9,11,13,12,14,11}};
static const int journeyKinds[6][JOURNEY_SECTIONS_MAX]={{ENV_SKY,ENV_OCEAN,ENV_WRECK},{ENV_SKY,ENV_ROOT,ENV_HIVE},{ENV_SPACE,ENV_WALL,ENV_WRECK,ENV_WALL,ENV_CRYSTAL},{ENV_SKY,ENV_BLIZZARD,ENV_ICE},{ENV_EM,ENV_MOUTH,ENV_BEAST,ENV_TEETH},{ENV_SPACE,ENV_WALL,ENV_ASH,ENV_BASE,ENV_WRECK,ENV_WALL,ENV_REACTOR,ENV_ESCAPE}};
static const int journeyMinis[6][JOURNEY_SECTIONS_MAX]={{10,7,11},{6,5,2},{4,-1,13,-1,15},{8,12,16},{1,-1,9,-1},{3,-1,14,0,-1,-1,-1,-1}};
static const int journeyGiantAt[6]={2,2,4,2,2,6};
/* Added traversal blocks do not silently increase existing enemy/miniboss HP. */
static const int journeyDepthWeight[6][JOURNEY_SECTIONS_MAX]={{1,1,1},{1,1,1},{1,1,1,0,1},{1,1,1},{1,1,1,1},{1,0,1,1,1,0,1,1}};
static const char*journeyNames[6]={"STORM COAST","HIVE DESCENT","SHATTERED BELT","FROZEN PASSAGE","LIVING NEBULA","REACTOR RUN"};
static const char*journeySections[6][JOURNEY_SECTIONS_MAX]={{"TEMPEST APPROACH","OCEAN / TIDAL FRONT","FLOODED WRECK"},{"CANOPY APPROACH","ROOT CAVERNS","THE INSECT HIVE"},{"SHATTERED APPROACH","ASTEROID WALL / CUT A PATH","DERELICT MINING SHIP","DENSE BELT / CUT A PATH","CRYSTAL MINES"},{"FROST RIDGE","WHITEOUT","ICE CAVERN"},{"ELECTROMAGNETIC FRONT","THE MAW","INSIDE THE LEVIATHAN","ESCAPE THROUGH THE TEETH"},{"HYPERLANE","DEBRIS BULKHEAD / CUT A PATH","ASH AND FIRE","THE WEAPONS BASE","ASSEMBLY HALLS","CORE DEBRIS / CUT A PATH","COLOSSUS REACTOR","FACILITY ESCAPE"}};
typedef struct{int x,y,life;} CritNotice;
typedef struct{int on,type,x,y,vx,vy,r,hp,age,life,dotTicks,dotClock,dotDamage,wound,damageUntil;} EnvironmentHazard;
enum { WEATHER_NONE,WEATHER_STORM,WEATHER_ASTEROID,WEATHER_ACID,WEATHER_ION,WEATHER_FIRE,WEATHER_ICE };
typedef struct{int on,type,x,y,oldX,oldY,vx,vy,r,hp,age;} WeatherActor;
typedef struct{int kind,stage,left,safeX,strikeX,serial,playerHits,enemyHits,bossHits,blasts,recoveryGiven;WeatherActor actors[48];} Weather;
typedef struct{Weather weather;int kind,timer,warning,strikeX,safeX,empActive,escaped,cutter,cellHits,wallGrace;EnvironmentHazard hazards[32];} Environment;
typedef struct{int hp,maxHp,x,y,w,h,role,visible,angle,dotTicks,dotClock,dotDamage,damageUntil;} GiantPart;
#define WORM_SEGMENTS 168
typedef struct{float x,y,angle;int visible;} WormSegment;
typedef struct{int active,defeated,timer,phase,archetype,rayX,partsDestroyed,addWaves,aoeX,aoeY,blastSerial,lastAddTick,form,oldForm,morph,formAge,transformSerial,attackKind,attackClock,attackSerial,aimX,aimY,safeX,attackSide,escortWaves,addMask;float zoom,scale,rootX,rootY;int openingTicks,openingTotal,growthTicks,growthShieldSerial,sweepCycle,sweepForm,sweepSide,sweepPhase,cueTicks,sweepSerial;float rootAngle,previousRootX,previousRootY,previousRootAngle,previousScale,diveX,diveY,morphRootX,morphRootY,morphRootAngle,morphScale;int previousPartX[9],previousPartY[9],previousPartAngle[9],previousPartW[9],previousPartH[9],woundForm[9],woundValid[9];float woundLX[9],woundLY[9];GiantPart parts[9];WormSegment worm[WORM_SEGMENTS];} Giant;
static int mission_phase(int),mission_index(int),journey_kind(void),journey_depth(void),section_final(void),section_has_boss(void),section_wave_target(void),section_duration(void),environment_exclusive(void),custom_environment(void);

static void salvage_flush(void);

typedef struct{int hp,maxHp,x,y,role;} GroundTarget;
typedef struct{int on,x,y,vx,vy,age;} GroundPayload;
typedef struct{int active,complete,cleared,pass,stage,timer,payload,cool,launchRequested,hits,targetsDestroyed,startX,startY,pathX,pathY,offset;float angle;GroundTarget targets[8];GroundPayload bombs[12];} Bombing;
static int bombing_ready(void);
