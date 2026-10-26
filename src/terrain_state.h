/* One world-unit density lattice. Kept outside Expansion: frame interpolation
 * copies Expansion, but must neither copy nor mutate authoritative terrain. */
#define TERRAIN_W 336
#define TERRAIN_H 672
#define TERRAIN_ORIGIN (-(TERRAIN_H+14))
#define TERRAIN_COLS (TERRAIN_W/28)
#define TERRAIN_ROWS (TERRAIN_H/28)
#define TERRAIN_CELLS (TERRAIN_COLS*TERRAIN_ROWS)
#define TERRAIN_N (TERRAIN_W*TERRAIN_H)
#define TERRAIN_DEBRIS 192
typedef struct{int x,y,vx,vy,life,material;} TerrainChip;
typedef struct{
 u8 density[TERRAIN_N],material[TERRAIN_N],scored[TERRAIN_CELLS];unsigned short opened[TERRAIN_N],solidCount[TERRAIN_CELLS];
 unsigned int serial,revision;int active,dirtyLeft,dirtyTop,dirtyRight,dirtyBottom;
 int impacts,removed,restored,solid,chipHead,novaSerial,lastShipX,lastShipY,sideOverride,turnTest,scrollQ,previousScrollQ,advanceQ;
 TerrainChip chips[TERRAIN_DEBRIS];
} Terrain;
static Terrain terrain;
static void terrain_enter(void);
static int terrain_projectile(Obj*,int,int,int);
static void terrain_respawn_clear(void);
