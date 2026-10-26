/* Ordinary mechanical strength is independent of actor identity/variant/path. */
static int enemy_strength_scale(const Obj*o){return 100+clamp(o->strength-1,0,2)*5;}
static int enemy_strength_interval(const Obj*o,int ticks){return clamp(ticks*100/enemy_strength_scale(o),1,ticks);}
static void enemy_strength_assign(Obj*o,int tier){if(o->type==4||enemy_organic(o)){o->strength=-1;return;}o->strength=clamp(tier,1,3);int hp=(o->hp*100-o->wound)*(100+(o->strength-1)*15)/100;o->hp=(hp+99)/100;o->wound=o->hp*100-hp;}
static void enemy_strength_prepare(Obj*o){if(o->strength)return;if(o->type==4||enemy_organic(o)){o->strength=-1;return;}unsigned h=++ex.strengthSerial*2654435761u+(unsigned)g.wave*2246822519u;h^=h>>15;h*=3266489917u;h^=h>>16;int red=clamp(8+(g.wave-1)*2+(g.pilotLevel-1)/5,8,30),yellow=clamp(22+(g.wave-1)*2+(g.pilotLevel-1)/10,22,38),roll=h%100;enemy_strength_assign(o,roll<red?3:roll<red+yellow?2:1);}
