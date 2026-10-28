/* Attached organisms belong to the hull, not the projectile/hazard arrays.
   Consequently ordinary shots and nova sweeps cannot accidentally remove them. */
static int parasite_x(int n){return g.px/Q+(n==0?-7:n==1?7:0);}
static int parasite_y(int n){return player_y/Q+(n==2?7:-2);}
static void parasites_new_hull(void){ex.optionBeam80=ex.optionHeld80=ex.optionPaid80=0;globules_clear();ex.leeches=ex.leechDrain=0;ex.leechGrace=100;ex.repairCool=0;ex.env.wallGrace=100;for(int b=0;b<5;b++)ex.buffTicks[b]=0;}
static void parasites_electric_purge(void){globules_electric_purge();if(!ex.leeches)return;for(int n=0;n<ex.leeches;n++)pc_organic_burst(parasite_x(n),parasite_y(n),-1,1);ex.leeches=ex.leechDrain=0;ex.leechGrace=150;ex.purgeSerial++;pc_charge_burst(g.px/Q,player_y/Q);pc_emp_sound(1,g.px/Q);}
static int parasites_latch(void){if(journey_kind()!=ENV_BEAST||ex.leechGrace||ex.leeches>=3||ex.phoenixTicks)return 0;ex.leeches++;ex.leechSerial++;ex.leechGrace=100;pc_hit_sound(3,8,g.px/Q);return 1;}
static void parasites_step(void){int k=journey_kind();if(k!=ENV_BEAST&&k!=ENV_TEETH){ex.leeches=ex.leechDrain=0;ex.leechGrace=100;return;}if(ex.leechGrace)ex.leechGrace--;
 if(ex.leeches&&++ex.leechDrain>=100){ex.leechDrain=0;int w=g.weapon;g.weaponEnergy[w]=clamp(g.weaponEnergy[w]-ex.leeches,0,100);}
 /* A free predictable purge every ten seconds prevents equipment softlocks. */
 if(k==ENV_BEAST){int c=ex.env.timer%500;if(c>=400&&c<475)ex.env.warning=14;if(c>=475){ex.env.warning=15;if(absn(g.px/Q-160)<35)parasites_electric_purge();}}
}

static void repair_ship(void){if(g.mode!=1||ex.repairCool)return;g.barrier=clamp(g.barrier+25,0,100);g.energy=clamp(g.energy+25,0,100);g.weaponEnergy[g.weapon]=clamp(g.weaponEnergy[g.weapon]+25,0,100);g.weaponHeat[g.weapon]=0;ex.repairCool=1500;ex.repairFlash=40;ex.repairSerial++;pc_charge_burst(g.px/Q,player_y/Q);pc_hit_sound(1,0,g.px/Q);}
