typedef struct{int disarm,cooldown,rearm,paid,remainder,bornTick;} Siphon84;
static int siphon_damage84(int rank){static const int d[4]={80,100,125,155};return d[rank<1?0:rank>4?3:rank-1];}
