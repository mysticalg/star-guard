#define main star_guard_main
#include "../src/pc.c"
#undef main
static int checks;
#define V(x) do{checks++;if(!(x)){fprintf(stderr,"FAIL enemy88 line%d %s tick%d clock%d phase%d fired%d lives%d safe%d\n",__LINE__,#x,g.tick,g.enemies[0].weaponClock,g.enemies[0].mega88.phase,g.enemies[0].mega88.fired,g.lives,g.enemies[0].mega88.safeX);exit(1);}}while(0)
static Obj*arena88(int ship){headless=1;pilotConfigured=0;selectedShip=ship;configuredLevel=10;configuredMask=511;configuredArmour=3;game_reset();g.mode=1;g.wave=1;g.fx=0;g.clock=-100000;g.shield=g.respawnTicks=0;g.barrier=100;ex.phase=ex.oldPhase=mission_phase(1);ex.intro=ex.travel=ex.entryY=0;ex.rearSpawned=1;ex.torpedoCool=100000;environment_enter();sideBlend=0;Obj*o=g.enemies;*o=(Obj){.on=1,.serial=++g.actorSerial,.type=27,.hp=10000,.x=160*Q,.y=70*Q,.base=160,.age=150,.entryTicks=12,.difficultyReady=1,.strength=1};return o;}
static void tick88(Obj*o){g.tick++;enemy_mega_step88(o);}
int main(void){
 for(int ship=0;ship<3;ship++)for(int strength=1;strength<=3;strength++){
  Obj*o=arena88(ship);o->strength=strength;g.devInvincible=1;int warning=0,fire=0,tail=0;
  for(int t=1;t<=226;t++){tick88(o);EnemyMega88*m=&o->mega88;warning+=m->phase==1;fire+=m->phase==2;tail+=m->phase==3;V(g.lives==3);if(m->phase==1||m->phase==2){V(m->safeX>=0);V(!enemy_mega_contact88(m,m->safeX,player_y/Q));V(m->half==22);V(m->stamp==g.tick);}if(t==60)V(o->weaponShots==0);if(t==61)V(o->weaponShots==1);}
  V(warning==60&&fire==150&&tail==15&&o->mega88.fired==150&&o->mega88.done);int clock=o->weaponClock;for(int n=0;n<1000;n++)tick88(o);V(o->weaponClock==clock&&o->weaponShots==1);
 }
 // Each accepted normal contact is fatal through maximum armour, once per beam.
 for(int ship=0;ship<3;ship++){
  Obj*o=arena88(ship);for(int n=0;n<61;n++)tick88(o);V(g.lives==2&&g.barrier==75&&g.respawnTicks==250&&o->mega88.hit);V(ex.lastDamageFatal&&ex.lastDamageAmount>=100);
  for(int n=0;n<149;n++){g.shield=g.respawnTicks=0;tick88(o);V(g.lives==2);}V(o->mega88.fired==150);
 }
 // Every legitimate protection prevents loss; expiry during the live beam can hit.
 for(int protection=0;protection<6;protection++){
  Obj*o=arena88(0);if(protection==0)g.devInvincible=1;if(protection==1)g.shield=300;if(protection==2)g.respawnTicks=250;if(protection==3)g.active=100;if(protection==4)ex.invincibleTicks=100;if(protection==5)ex.phoenixTicks=100;
  for(int n=0;n<90;n++)tick88(o);V(g.lives==3&&!o->mega88.hit);g.devInvincible=g.shield=g.respawnTicks=g.active=ex.invincibleTicks=ex.phoenixTicks=0;tick88(o);V(g.lives==2&&o->mega88.hit);
 }
 // Warning aim is locked; a safe dodge remains safe for all 150 damaging ticks.
 Obj*o=arena88(0);tick88(o);int aim=o->aimX,safe=o->mega88.safeX;g.px=safe*Q;ex.damagePrevX=g.px;for(int n=0;n<209;n++){tick88(o);V(o->aimX==aim&&g.lives==3);}V(o->mega88.fired==150);
 // A fast pointer crossing the physical lane is a hit even if endpoints are safe.
 o=arena88(0);g.devInvincible=1;for(int n=0;n<61;n++)tick88(o);g.devInvincible=0;ex.damagePrevX=70*Q;ex.damagePrevY=230*Q;g.px=250*Q;tick88(o);V(g.lives==2&&o->mega88.hit);
 // One warning/fire lease prevents multiple aircraft from closing every route.
 o=arena88(0);g.devInvincible=1;g.enemies[1]=*o;g.enemies[1].serial=++g.actorSerial;g.enemies[1].x=200*Q;for(int n=0;n<100;n++){tick88(o);enemy_mega_step88(g.enemies+1);V(o->weaponClock>0&&g.enemies[1].weaponClock==0);}o->on=0;tick88(g.enemies+1);V(g.enemies[1].weaponClock==1&&g.enemies[1].mega88.phase==1);
 // Ice, Siphon, individual/global EMP all cancel immediately and re-warn.
 for(int interrupt=0;interrupt<5;interrupt++){
  o=arena88(0);g.devInvincible=1;for(int n=0;n<90;n++)tick88(o);if(interrupt==0)o->ice83.frozen=10;if(interrupt==1)o->siphon84.disarm=10;if(interrupt==2)o->empFreeze=10;if(interrupt==3)ex.empStop=10;if(interrupt==4)ex.env.empActive=1;tick88(o);V(!o->weaponClock&&!o->mega88.phase&&!o->mega88.fired);o->ice83.frozen=o->siphon84.disarm=o->empFreeze=ex.empStop=ex.env.empActive=0;tick88(o);V(o->weaponClock==1&&o->mega88.phase==1);for(int n=0;n<59;n++)tick88(o);V(o->mega88.phase==1);tick88(o);V(o->mega88.phase==2);
 }
 // Full-aperture fine density: an edge strand clips the whole coherent beam.
 o=arena88(0);ex.section=1;g.wave=3;ex.phase=ex.oldPhase=mission_phase(3);environment_enter();memset(terrain.density,0,sizeof(terrain.density));terrain.active=1;terrain.scrollQ=-TERRAIN_ORIGIN*Q;o->aimX=160*Q;o->aimY=230*Q;int k=terrain_index(181,140);V(k>=0);terrain.density[k]=1;enemy_mega_geometry88(o);V(o->mega88.by<140&&o->mega88.by>=138);V(!enemy_mega_contact88(&o->mega88,160,230));terrain.density[k]=0;enemy_mega_geometry88(o);V(o->mega88.by>230);V(terrain.removed==0&&g.score==0);
 // Cavern/island surfaces clip before solid material; a full blocked row cannot fire.
 for(int wave=1;wave<=6;wave++)for(int section=0;section<journeyCounts[wave-1];section++){
  o=arena88(0);g.wave=wave;ex.section=section;ex.phase=ex.oldPhase=mission_phase(wave);environment_enter();if(!cavern_phase(ex.phase)||journey_kind()==ENV_WALL)continue;o->x=tunnel_lane(70,0)*Q;o->aimX=tunnel_lane(230,0)*Q;o->aimY=230*Q;enemy_mega_geometry88(o);EnemyMega88*m=&o->mega88;float dx=m->bx-m->ax,dy=m->by-m->ay,d=hypotf(dx,dy);for(int n=0;n<(int)d;n++)for(int w=-22;w<=22;w++)V(!enemy_mega_solid88(m->ax+dx*n/d-dy*w/d,m->ay+dy*n/d+dx*w/d));
 }
 // Real game step: pause freezes warning/fire; Continue and all skips clear live state.
 o=arena88(0);g.devInvincible=1;for(int n=0;n<100;n++)game_step(0,0);V(o->mega88.phase==2);int clock=o->weaponClock,fire=o->mega88.fired;g.paused=1;for(int n=0;n<100;n++)game_step(0,0);V(o->weaponClock==clock&&o->mega88.fired==fire);g.paused=0;game_step(0,0);V(o->mega88.fired==fire+1);developer_command(2010);for(int n=0;n<MAXE;n++)V(!g.enemies[n].on);
 // Other aircraft retain their old mine/EMP/torpedo clock and non-instant damage.
 for(int type=16;type<=28;type++)if(type==16||type==25||type==28){o=arena88(0);o->type=type;for(int n=0;n<39;n++)aircraft_weapon_step(o);V(o->weaponShots==0);aircraft_weapon_step(o);V(o->weaponShots==1&&g.lives==3);int found=0;for(int n=0;n<MAXB;n++)if(g.bullets[n].on)found++;V(found==1);}
 printf("PASS %d enemy88 checks: fixed60warning/150fire/15tail, one fatal contact, all protection/expiry, swept dodge, global lease, five interruptions/re-warning, full-aperture fine/cavern/island clipping, pause/skip and unchanged ordinary aircraft\n",checks);return 0;
}
