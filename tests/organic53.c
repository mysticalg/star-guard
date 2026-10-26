#define main star_guard_main
#include "../src/pc.c"
#undef main
static int checks;
#define V(x) do{if(!(x)){fprintf(stderr,"FAIL %d %s\n",__LINE__,#x);exit(1);}checks++;}while(0)
static void arena(int m,int section){selectedShip=0;game_reset();g.mode=1;g.wave=m;ex.section=section;ex.phase=ex.oldPhase=mission_phase(m);environment_enter();g.tick=100;}
int main(void){headless=1;int contexts=0;
for(int m=1;m<=6;m++)for(int section=0;section<journeyCounts[m-1];section++){arena(m,section);if(!organic_context())continue;contexts++;for(int type=0;type<=28;type++){Obj o={.on=1,.type=type,.hp=100,.base=100,.age=200,.phase=3,.damageUntil=105};ndraw=0;pc_enemy_sprite(7,&o,160,100);V(ndraw==1&&draws[0].enemy&&draws[0].n>=137&&draws[0].n<=144&&draws[0].damageUntil==105);if(type==4)V(draws[0].n==143);if(floor_enemy(&o))V(draws[0].n==floor_art(&o)&&floor_depth(&o)==14);}
 for(int type=0;type<=28;type++){Obj o={.on=1,.type=type,.hp=100,.base=100,.age=200,.phase=3,.entryTicks=20,.x=160*Q,.y=100*Q};damage_enemy(&o,0);V(!o.damageUntil);damage_enemy(&o,1);V(o.damageUntil==105&&o.wound==1);}
 EnvironmentHazard h={.on=1,.hp=10};environment_damage(&h,0);V(!h.damageUntil);environment_damage(&h,1);V(h.damageUntil==105&&h.wound==1);g.paused=1;for(int t=0;t<20;t++)game_step(0,0);V(g.tick==100);g.paused=0;
 for(int type=9;type<=24;type++){Obj o={.on=1,.type=type,.hp=10,.y=150*Q};if(!floor_enemy(&o))continue;floor_mount_step(&o);float a=o.heading/916.732472f,bx=o.x/(float)Q-floor_depth(&o)*sinf(a),by=o.y/(float)Q+floor_depth(&o)*cosf(a);V(fabsf(bx-floor_surface_side(o.floorY/(float)Q,floor_side(&o)))<.06f&&fabsf(by-o.floorY/(float)Q)<.06f);int x,y;floor_muzzle(&o,&x,&y);V(!tunnel_visible(y/Q)||(x/Q>tunnel_left(y/Q)&&x/Q<tunnel_right(y/Q)));}
}
V(contexts==5);arena(1,0);for(int type=0;type<=28;type++){Obj o={.on=1,.type=type,.hp=100,.base=100,.age=200,.phase=3,.damageUntil=105};ndraw=0;pc_enemy_sprite(7,&o,160,100);V(ndraw>0);for(int i=0;i<ndraw;i++)V(draws[i].n<137&&draws[i].damageUntil==105);}
Obj o={.on=1,.type=4,.hp=100,.base=100,.age=200,.phase=3,.shieldCharge=100};damage_enemy(&o,1);V(!o.damageUntil&&o.shieldCharge==99);o.shieldCharge=0;o.type=0;o.entryTicks=5;damage_enemy(&o,100);V(!o.damageUntil);printf("PASS %d organic roster/theme,all29roles,5interiorcontexts,accepted HP flash,protected gates,pause,floor/muzzle checks\n",checks);return 0;}
