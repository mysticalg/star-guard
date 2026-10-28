/* ICE occupies existing family3/key4; all bank accounting stays in its owner. */
static int enemy_guarded(const Obj*);
extern void pc_ice_event83(int,int,int,int);
static int ice_living_hazard83(const EnvironmentHazard*h){return (h->type>=9&&h->type<=12)||(h->type>=20&&h->type<=25);}
static void ice_step83(void);
static void ice_resume83(void);
static int ice_mini_phase_pending83(const Obj*o){if(o->type!=4)return 0;int phase=o->hp*3>o->base*2?1:o->hp*3>o->base?2:3;return o->morph>0||o->openingTicks>0||phase!=o->phase;}
static void ice_event83(int event,int x,int y,int radius){if(event==1)ex.iceFreezes83++;else if(event==2)ex.iceShatters83++;else if(event==3)ex.iceThaws83++;pc_ice_event83(event,x,y,radius);}
/* Return this contact's hull damage. Ordinary chill first encases the target;
   a charged shell consumes further contacts until a single shatter burst. */
static int ice_contact83(Ice83*s,int rank,int amount,int boss,int*event){*event=0;if(amount<=0)return 0;int base=ice_base83(rank);static const int chill[4]={100,140,180,220};int points=clamp(chill[clamp(rank,1,4)-1]*amount/base,1,1000);s->hitTick=g.tick;
 if(boss){if(!s->cooldown){s->charge=clamp(s->charge+points,0,boss==2?900:600);if(s->charge>=(boss==2?900:600)){s->charge=0;s->frozen=20;s->cooldown=boss==2?150:100;s->freezeTick=g.tick;*event=1;}}return amount;}
 if(s->frozen){s->fracture+=amount;if(s->fracture>=s->shell){s->frozen=s->charge=s->fracture=0;s->cooldown=25;s->freezeTick=g.tick;*event=2;return base*6;}return 0;}
 if(s->cooldown)return amount;s->charge=clamp(s->charge+points,0,300);if(s->charge>=300){s->charge=0;s->frozen=65;s->fracture=0;s->shell=base*4;s->freezeTick=g.tick;*event=1;}return 0;
}
static void ice_tick83(Ice83*s,int x,int y,int radius){if(s->freezeTick!=g.tick){if(s->cooldown)s->cooldown--;if(s->frozen&&!--s->frozen){s->charge=s->fracture=0;if(s->cooldown<25)s->cooldown=25;s->freezeTick=g.tick;ice_event83(3,x,y,radius);}}if(s->charge&&g.tick-s->hitTick>10)s->charge=clamp(s->charge-4,0,s->charge);}
