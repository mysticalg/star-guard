/* Six continuous journeys. Legacy scenery ids remain internal asset references. */
#define MISSION_COUNT 6
enum { ENV_SKY,ENV_OCEAN,ENV_WRECK,ENV_ROOT,ENV_HIVE,ENV_WALL,ENV_CRYSTAL,ENV_BLIZZARD,ENV_ICE,ENV_EM,ENV_MOUTH,ENV_BEAST,ENV_TEETH,ENV_REACTOR,ENV_FORTRESS,ENV_ASH,ENV_SPACE,ENV_ESCAPE,ENV_BASE };
static const int journeyCounts[6]={3,3,4,3,4,6};
static const int journeyPhases[6][6]={{10,7,11},{6,5,2},{4,12,13,15},{8,16,5},{1,0,5,1},{3,9,11,13,14,11}};
static const int journeyKinds[6][6]={{ENV_SKY,ENV_OCEAN,ENV_WRECK},{ENV_SKY,ENV_ROOT,ENV_HIVE},{ENV_SPACE,ENV_WALL,ENV_WRECK,ENV_CRYSTAL},{ENV_SKY,ENV_BLIZZARD,ENV_ICE},{ENV_EM,ENV_MOUTH,ENV_BEAST,ENV_TEETH},{ENV_SPACE,ENV_ASH,ENV_BASE,ENV_WRECK,ENV_REACTOR,ENV_ESCAPE}};
static const int journeyMinis[6][6]={{10,7,11},{6,5,2},{4,-1,13,15},{8,12,16},{1,-1,9,-1},{3,14,0,-1,-1,-1}};
static const int journeyGiantAt[6]={2,2,3,2,2,4};
static const char*journeyNames[6]={"STORM COAST","HIVE DESCENT","SHATTERED BELT","FROZEN PASSAGE","LIVING NEBULA","REACTOR RUN"};
static const char*journeySections[6][6]={{"TEMPEST APPROACH","OCEAN / TIDAL FRONT","FLOODED WRECK"},{"CANOPY APPROACH","ROOT CAVERNS","THE INSECT HIVE"},{"SHATTERED APPROACH","ASTEROID WALL / CUT A PATH","DERELICT MINING SHIP","CRYSTAL MINES"},{"FROST RIDGE","WHITEOUT","ICE CAVERN"},{"ELECTROMAGNETIC FRONT","THE MAW","INSIDE THE LEVIATHAN","ESCAPE THROUGH THE TEETH"},{"HYPERLANE","ASH AND FIRE","THE WEAPONS BASE","ASSEMBLY HALLS","COLOSSUS REACTOR","FACILITY ESCAPE"}};
typedef struct{int x,y,life;} CritNotice;
typedef struct{int on,type,x,y,vx,vy,r,hp,age,life,dotTicks,dotClock,dotDamage,wound;} EnvironmentHazard;
typedef struct{int kind,timer,warning,strikeX,safeX,empActive,escaped,cutter,cellHits,wallGrace;EnvironmentHazard hazards[32];} Environment;
typedef struct{int hp,maxHp,x,y,w,h,role,visible,angle,dotTicks,dotClock,dotDamage,damageUntil;} GiantPart;
typedef struct{int active,defeated,timer,phase,archetype,rayX,partsDestroyed,addWaves,aoeX,aoeY,blastSerial,lastAddTick,form,oldForm,morph,formAge,transformSerial,attackKind,attackClock,attackSerial,aimX,aimY,safeX,attackSide,escortWaves,addMask;float zoom,scale;GiantPart parts[9];} Giant;
static int mission_phase(int),mission_index(int),journey_kind(void),journey_depth(void),section_final(void),section_has_boss(void),section_wave_target(void),section_duration(void),environment_exclusive(void),custom_environment(void);

static void salvage_flush(void);

typedef struct{int hp,maxHp,x,y,role;} GroundTarget;
typedef struct{int on,x,y,vx,vy,age;} GroundPayload;
typedef struct{int active,complete,cleared,pass,stage,timer,payload,cool,launchRequested,hits,targetsDestroyed,startX,startY,pathX,pathY,offset;float angle;GroundTarget targets[8];GroundPayload bombs[12];} Bombing;
static int bombing_ready(void);
