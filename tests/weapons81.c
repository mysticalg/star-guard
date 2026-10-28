#define main star_guard_main
#include "../src/pc.c"
#undef main
static int checks;
#define V(x) do{checks++;if(!(x)){fprintf(stderr,"FAIL Weapons81 line%d %s ship%d rank%d tick%d\n",__LINE__,#x,g.chassis,power_level(g.weapon),g.tick);exit(1);}}while(0)
static void arena81(int ship,int weapon,int rank,int v){headless=1;pilotConfigured=0;selectedShip=ship;configuredLevel=1;configuredMask=511;configuredArmour=configuredQualityPack=0;memset(starterVariants,0,sizeof(starterVariants));starterVariants[weapon]=v;game_reset();g.mode=1;g.fx=0;g.wave=2;g.weapon=weapon;g.weaponLevel[weapon]=rank;g.clock=-100000;g.shield=100000;ex.phase=ex.oldPhase=mission_phase(2);ex.intro=ex.travel=ex.entryY=0;ex.rearSpawned=1;ex.torpedoCool=100000;ex.options=0;environment_enter();}
static void wall81(void){g.wave=3;ex.section=1;ex.phase=ex.oldPhase=mission_phase(3);environment_enter();terrain.scrollQ=terrain.previousScrollQ=480*Q;g.px=160*Q;player_y=230*Q;for(int y=26;y<=250;y++)for(int x=-8;x<328;x++){int i=terrain_index(x,y);if(i>=0)terrain_write(i,0);}}
static void rock81(int x,int y,int hp){int i=terrain_index(x,y);V(i>=0);terrain_write(i,hp);}
int main(void){int expected[12]={0,-32,32,-64,64,-96,96,-112,112,-128,-16,16},spreadCases=0,geometryCases=0;for(int ship=0;ship<3;ship++)for(int v=0;v<3;v++)for(int rank=1;rank<=4;rank++){
 arena81(ship,1,rank,v);fan_fire(1);int count=rank==4?12:rank*2+3,seen[12]={0},n=0;for(int i=0;i<MAXS;i++){Obj*s=g.shots+i;if(!s->on)continue;n++;V(s->type==1&&s->sourceFamily==2&&!s->echo80);int match=-1,speed=2400*variant_speed_scale(1)/100;for(int a=0;a<count;a++)if(s->vx-g.playerVX==sn(expected[a])*speed/1024&&s->vy==-sn(expected[a]+64)*speed/1024)match=a;V(match>=0);V(!seen[match]++);V(s->x==g.px+sn(expected[match])*(player_hull_rx()+1)*Q/1024);V(s->y==player_y-sn(expected[match]+64)*(player_hull_ry()+1)*Q/1024);}
 V(n==count);for(int i=0;i<count;i++)V(seen[i]==1);V(g.shots[0].vx==g.playerVX&&g.shots[0].vy<0);for(int t=0;t<300;t++)game_step(0,0);for(int i=0;i<MAXS;i++)V(!g.shots[i].on||g.shots[i].type!=1);spreadCases++;
 // One free primary slot always retains forward fire; two slots rotate all other directions.
 int found[12]={0};for(int tick=0;tick<(count-1)*weapon_delay_for(1,6);tick+=weapon_delay_for(1,6)){memset(g.shots,0,sizeof(g.shots));for(int i=2;i<PRIMARY_SHOTS80;i++)g.shots[i].on=1;g.tick=tick;fan_fire(1);V(g.shots[0].vy<0);Obj*s=g.shots+1;for(int a=1;a<count;a++)if(s->vx-g.playerVX==sn(expected[a])*2400*variant_speed_scale(1)/100/1024&&s->vy==-sn(expected[a]+64)*(2400*variant_speed_scale(1)/100)/1024)found[a]=1;}for(int a=1;a<count;a++)V(found[a]);
}
for(int ship=0;ship<3;ship++)for(int side=0;side<2;side++)for(int rank=1;rank<=4;rank++)for(int aim=-64;aim<=64;aim+=64){
 arena81(ship,5,rank,0);wall81();terrain.sideOverride=side;ex.multi=1;g.beam=8;ex.beamAim=aim;int width=beam_half_width(),slope=beam_slope(0),cx=beam_center(0,120),inside=cx+clamp(width/2,1,10);rock81(inside,120,4);V(terrain_beam_tip(0)==120);V(!beam_hits(beam_center(0,70),70,0));V(beam_hits(beam_center(0,180),180,0));rock81(inside,120,0);V(terrain_beam_tip(0)==26);V(beam_hits(beam_center(0,70),70,0));
 // Cache invalidates on writes, scroll and a fresh terrain generation.
 rock81(cx,120,4);V(terrain_beam_tip(0)==120);terrain.scrollQ+=Q;V(terrain_beam_tip(0)==121);terrain.scrollQ-=Q;rock81(cx,120,0);V(terrain_beam_tip(0)==26);
 // Whole aperture cuts actual grains between the former three rays.
 rock81(inside,120,4);terrain_beam_cut81(g.px/Q,223,slope,width,3+clamp(width/3,0,5),3);V(!terrain_solid(inside,120));V(!terrainBeamQuiet81);
 // End row26 is a valid impact even when only an off-centre grain is solid.
 inside=beam_center(0,26)+clamp(width/2,1,10);rock81(inside,26,4);terrain_beam_cut81(g.px/Q,223,slope,width,3+clamp(width/3,0,5),3);V(!terrain_solid(inside,26));
 // Smaller pod aperture uses its own origins and full width.
 ex.options=1;ex.optionsOn=1;ex.optionHeld80=1;for(int i=0;i<64;i++){ex.followX[i]=160*Q;ex.followY[i]=230*Q;}int ox=option_x(0),oy=option_y(0)-7;cx=option_beam_center80(0,0,120);inside=cx+clamp(option_beam_width80()/2,0,4);rock81(inside,120,4);option_beam_refresh80();V(ex.optionBeam80);V(ex.optionBeamTips80[1]==120);V(!option_beam_point80(0,option_beam_center80(0,0,70),70,0));terrain_beam_cut81(ox,oy,beam_slope(0),option_beam_width80(),2,1);V(!terrain_solid(inside,120));geometryCases++;
}
/* Exercise the actual free-obstacle update, including a pod-only target. */
for(int rank=1;rank<=4;rank++)for(int pods=0;pods<2;pods++){
 arena81(0,5,rank,0);ex.options=pods;ex.optionsOn=1;for(int i=0;i<64;i++){ex.followX[i]=230*Q;ex.followY[i]=230*Q;}int x=pods?219:160;Obj*o=ex.obstacles;*o=(Obj){.on=1,.type=0,.x=x*Q,.y=120*Q,.hp=10000,.base=1};int before=o->hp*100;for(int t=0;t<4;t++)game_step(4,0);int delta=before-(o->hp*100-o->wound),expected=beam_target_damage81(100,5);V(o->on);V(delta==(pods?expected/5:expected));V(g.score==0);
 // The regular environment damage wrapper still applies resistance and fractional damage.
 EnvironmentHazard h={.on=1,.type=8,.x=160*Q,.y=120*Q,.hp=10000};int hp=h.hp*100;environment_weapon_hit(&h,beam_target_damage81(120,5),5);V(h.hp*100-h.wound<hp);V(h.hp*100-h.wound>hp-beam_target_damage81(120,5));
}
for(int rank=1;rank<=4;rank++){arena81(0,5,rank,0);int widths[4]={3,8,16,25},damage[4]={110,155,215,290};ex.multi=1;V(beam_half_width()==widths[rank-1]);V(beam_damage81(5)==damage[rank-1]);V(beam_target_damage81(120,5)==120*damage[rank-1]/110);g.weapon=3;ex.multi=0;g.weaponLevel[3]=rank;V(beam_damage81(3)==ice_base83(rank));}
printf("PASS %d Weapons81 checks: %d Spread shape/variant/muzzle/forward-pool/finitelife cases; %d full-aperture wall/pod/scroll/cache/cutting cases; actual Triple width/damage ranks.\n",checks,spreadCases,geometryCases);return 0;}
