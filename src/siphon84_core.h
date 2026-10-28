/* Isolated prototype: actual accepted hull damage is the only transfer source. */
extern void pc_siphon_event84(int,int,int,int),pc_siphon_reset84(void),pc_siphon_shift84(int);
static void siphon_resume84(void),siphon_step84(void);
static int siphon_family84(int family){return family==3&&g.variantEquip[3]==3&&!ex.phoenixTicks;}
static int siphon_active84(int family){return g.weapon==3&&siphon_family84(family);}
static int siphon_hazard84(const EnvironmentHazard*h){return (h->type>=6&&h->type<=12)||(h->type>=20&&h->type<=25);}
static int siphon_contact84(Siphon84*s,int damage,int frozen,int boss,int canDisarm,int x,int y){if(!siphon_active84(3)||!g.beam||damage<=0||frozen||ex.siphonEcho84)return 0;int began=0;
 if(canDisarm&&(!boss||!s->cooldown)){began=!s->disarm;s->disarm=boss?15:25;s->bornTick=g.tick;if(boss)s->cooldown=boss==2?150:100;if(began){ex.siphonDisarms84++;pc_siphon_event84(0,x,y,0);}}
 if(g.tick-ex.siphonStart84>=50){ex.siphonStart84=g.tick;ex.siphonWindowGiven84=0;}s->remainder+=damage;int units=s->remainder/100;s->remainder%=100;int reserve=boss==2?24:boss?12:8,credit=clamp(units,0,clamp(reserve-s->paid,0,reserve));credit=clamp(credit,0,clamp(8-ex.siphonWindowGiven84,0,8));credit=clamp(credit,0,clamp(100-g.weaponEnergy[3],0,100));if(credit){g.weaponEnergy[3]+=credit;g.energy=g.weaponEnergy[g.weapon];s->paid+=credit;ex.siphonWindowGiven84+=credit;ex.siphonTotal84+=credit;pc_siphon_event84(1,x,y,credit);}return began;
}
static void siphon_tick84(Siphon84*s,int giant){if(s->bornTick==g.tick)return;if(s->cooldown)s->cooldown--;if(s->disarm&&!--s->disarm&&giant){s->rearm=12;s->bornTick=g.tick;}else if(s->rearm)s->rearm--;}
