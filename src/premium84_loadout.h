/* Keep the original 27 inventory bits. Four reserved ownership flags use
 * bits27..30; never shift nine four-bit inventories into a 32-bit argument. */
static int premium84_index(int w){return w==0?0:w==3?1:w==4?2:w==5?3:-1;}
static int premium84_enabled(int w){return w==3||w==4||w==5;}
static int premium84_variant(int w,int v){return v>=0&&v<3?v:v==3&&premium84_enabled(w)?3:0;}
static int premium84_inventory(int w,unsigned int packed){int old=(packed>>(w*3))&7,index=premium84_index(w);return old|(index>=0&&premium84_enabled(w)&&((packed>>(27+index))&1)?8:0);}
static int premium84_owned_variant(int w,int v){v=premium84_variant(w,v);if(v==3&&!(g.inventoryBits[w]&8)){g.equippedMask&=~(1<<w);return 0;}return v;}

/* Mega stays in slot6 and uses one coherent aperture, rather than stacking three lanes. */
static int mega85_fitted(void){return g.variantEquip[5]==3&&!ex.phoenixTicks;}
static int beam_split85(void){return ex.multi&&!mega85_fitted();}
static int mega85_damage(int rank){static const int d[4]={350,500,700,950};return d[clamp(rank,1,4)-1];}

/* Source identity is copied into each missile, including paid pod echoes. */
static int super86_fitted(void){return g.variantEquip[4]==3&&!ex.phoenixTicks;}
static int terrain_recovery_wide86(void){return !ex.phoenixTicks&&((g.weapon==5&&mega85_fitted())||(g.weapon==3&&g.variantEquip[3]==3)||(g.weapon==4&&super86_fitted()));}

/* Narrow Siphon and guided missiles cannot reliably open hull clearance at point blank range.
 * Their held-fire terrain cutter remains local and never damages actors or returns energy. */
static int terrain_recovery_assist86(void){return !ex.phoenixTicks&&((g.weapon==3&&g.variantEquip[3]==3)||(g.weapon==4&&super86_fitted()));}
