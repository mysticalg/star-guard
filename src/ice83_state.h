#ifndef STAR_GUARD_ICE83
#define STAR_GUARD_ICE83 1
/* Transient combat state only: existing pilot inventory and bank IDs stay intact. */
typedef struct {int charge,frozen,cooldown,fracture,shell,hitTick,freezeTick;} Ice83;
static int ice_base83(int rank){static const int power[4]={70,90,115,145};return power[rank<1?0:rank>4?3:rank-1];}
#endif
