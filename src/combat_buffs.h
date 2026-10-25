static int variant_damage_scale(int w){static const int pct[3]={100,94,108};static const int quality[4]={100,108,116,125};return w<0||w>8?100:pct[clamp(g.variantEquip[w],0,2)]*quality[g.gearQuality[w]]/100;}
static int variant_energy_scale(int w){static const int pct[3]={100,92,106};static const int quality[4]={100,103,106,109},armour[4]={100,103,106,100};w=clamp(w,0,8);return pct[clamp(g.variantEquip[w],0,2)]*quality[g.gearQuality[w]]/100*armour[g.armourGear]/100;}
static int variant_speed_scale(int w){static const int pct[3]={100,108,94};return pct[clamp(g.variantEquip[clamp(w,0,8)],0,2)];}
static void collect_variant(int w,int v){if(w<0||w>8||v<0||v>2)return;g.inventoryBits[w]|=1<<v;ex.collectedFamily=w;ex.collectedVariant=v;ex.collectedTicks=200;}
static int equip_variant(int w,int v){if(!ex.refit||w<0||w>8||v<0||v>2||!(g.inventoryBits[w]&(1<<v)))return 0;g.variantEquip[w]=v;return 1;}
extern void pc_crit_sound(int);
static void grant_buff(int kind){kind=clamp(kind,0,4);ex.buffTicks[kind]=600;}
static void combat_buffs_step(void){if(ex.collectedTicks)ex.collectedTicks--;for(int i=0;i<5;i++)if(ex.buffTicks[i])ex.buffTicks[i]--;for(int i=0;i<8;i++)if(ex.critNotices[i].life)ex.critNotices[i].life--;}
/* A separate combat RNG makes crits independent of effects and rendering. */
static int player_hit_roll(int damage,int x,int y){if(damage<=0)return 0;if(ex.buffTicks[3])damage=damage*125/100;ex.combatSeed=(ex.combatSeed?ex.combatSeed:0x51c01u)*1664525u+1013904223u;int roll=(ex.combatSeed>>16)%100;if(roll<5+(ex.buffTicks[4]?10:0)){damage*=2;ex.critSerial++;ex.critNotices[ex.critHead++&7]=(CritNotice){x,y,40};pc_crit_sound(x);}return damage;}
