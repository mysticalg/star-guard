/* Independent, silent workshop simulation. Production game_step owns every shot.
 * No main loop, SDL window, audio device, filesystem, storage, or score endpoint. */
#define main star_guard_main
#include "pc.c"
#undef main
static int rangeShip,rangeWeapon,rangeVariant,rangeQuality,rangeArmour,rangeAge;
EMSCRIPTEN_KEEPALIVE void range_init(int ship,int weapon,int variant,int gear,int armour){
 headless=1;muted=1;rangeShip=selectedShip=clamp(ship,0,2);
 rangeWeapon=clamp(weapon,0,8);rangeVariant=clamp(variant,0,2);rangeQuality=clamp(gear,0,3);rangeArmour=clamp(armour,0,3);rangeAge=0;
 pilotConfigured=0;memset(starterVariants,0,sizeof(starterVariants));starterVariants[rangeWeapon]=rangeVariant;
 game_reset();g.mode=1;g.fx=1;memset(sparks,0,sizeof(sparks));memset(waves,0,sizeof(waves));memset(smoke,0,sizeof(smoke));sparkHead=waveHead=smokeHead=0;g.weapon=rangeWeapon;g.gearQuality[rangeWeapon]=rangeQuality;g.armourGear=rangeArmour;
 g.px=g.lastPlayerX=160*Q;player_y=220*Q;g.shield=100000;g.clock=-100000;
 ex.phase=ex.oldPhase=0;ex.intro=ex.travel=ex.entryY=0;ex.torpedoCool=10000;ex.optionsOn=0;
}
EMSCRIPTEN_KEEPALIVE void range_step(void){
 if(rangeAge++>=300)range_init(rangeShip,rangeWeapon,rangeVariant,rangeQuality,rangeArmour);
 /* Durable range targets: production guidance, hit and chain routines apply. */
 for(int i=0;i<3;i++){Obj *o=g.enemies+i;int linked=o->empLinked;*o=(Obj){.on=1,.type=0,.hp=100000,.entryTicks=12,.base=130+i*30,.x=(130+i*30)*Q,.y=(120-i*18)*Q,.empLinked=linked};}
 memset(g.bullets,0,sizeof(g.bullets));g.clock=-100000;ex.torpedoCool=10000;
 game_step(4,0);update_fx(.02f);
}
EMSCRIPTEN_KEEPALIVE int range_read(int n,int field){
 if(n<MAXS){Obj *o=g.shots+n;switch(field){case 0:return o->on;case 1:return o->x;case 2:return o->y;case 3:return o->vx;case 4:return o->vy;case 5:return o->type;case 6:{SDL_FColor c=projectile_colour(o,0);return ((int)(c.r*255)<<16)|((int)(c.g*255)<<8)|(int)(c.b*255);}}}
 if(n>=100&&n<196){Obj *o=ex.empArcs+n-100;switch(field){case 0:return ex.empTicks&&n-100<ex.empCount;case 1:return o->x;case 2:return o->y;case 3:return o->vx;case 4:return o->vy;}}
 if(n>=300&&n<303){Obj *o=g.enemies+n-300;return field==1?o->x:field==2?o->y:o->on;}
 if(n>=400&&n<464){Wave *o=waves+n-400;switch(field){case 0:return o->life>0;case 1:return o->x*256;case 2:return o->y*256;case 3:return o->radius*100;case 4:return o->age*1000;case 5:return o->life*1000;}}
 if(n==213){SDL_FColor c=player_beam_colour();return ((int)(c.r*255)<<16)|((int)(c.g*255)<<8)|(int)(c.b*255);}
 switch(n){case 200:return g.beam;case 201:return ex.multi;case 202:return beam_half_width();case 203:return beam_center(field,20);case 204:return g.tick;case 205:return g.weaponEnergy[g.weapon];case 206:return g.weaponHeat[g.weapon];case 207:return g.variantEquip[g.weapon];case 208:return g.gearQuality[g.weapon];case 209:return g.armourGear;case 210:return g.chassis;case 211:return power_level(g.weapon);}
 return 0;
}
int main(void){return 0;}
