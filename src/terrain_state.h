/* One world-unit density lattice. Kept outside Expansion: frame interpolation
 * copies Expansion, but must neither copy nor mutate authoritative terrain. */
#define TERRAIN_W 336
#define TERRAIN_H 448
#define TERRAIN_N (TERRAIN_W*TERRAIN_H)
#define TERRAIN_DEBRIS 192
typedef struct{int x,y,vx,vy,life,material;} TerrainChip;
typedef struct{
 u8 density[TERRAIN_N],material[TERRAIN_N],scored[192];unsigned short opened[TERRAIN_N],solidCount[192];
 unsigned int serial,revision;int active,dirtyLeft,dirtyTop,dirtyRight,dirtyBottom;
 int impacts,removed,restored,solid,chipHead,novaSerial,lastShipX,lastShipY,sideOverride,turnTest,scrollQ,previousScrollQ;
 TerrainChip chips[TERRAIN_DEBRIS];
} Terrain;
static Terrain terrain;
static void terrain_enter(void);
static int terrain_projectile(Obj*,int,int,int);
static void terrain_respawn_clear(void);
