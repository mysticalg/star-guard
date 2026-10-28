static EnvironmentHazard*environment_spawn(int type,int x,int y,int vx,int vy,int radius,int hp){for(int i=0;i<32;i++)if(!ex.env.hazards[i].on){EnvironmentHazard*h=ex.env.hazards+i;*h=(EnvironmentHazard){1,type,x*Q,y*Q,vx,vy,radius,hp,0,900};cavern_prepare_hazard(h);return h;}return 0;}
#include "journey_bombing.h"
#include "terrain_core.h"
static void environment_enter(void){globules_clear();int kind=journey_kind();ex.env=(Environment){.kind=kind,.safeX=kind==ENV_OCEAN?64+(g.wave*57)%192:160,.strikeX=160};terrain.active=kind==ENV_WALL;if(kind==ENV_BASE)bombing_enter();if(kind==ENV_WALL)terrain_enter();}
static void pod_contact_step(void){for(int n=0;n<5;n++){if(ex.podContact[n])ex.podContact[n]--;if(n>=ex.options||!ex.optionsOn||ex.empDisable||ex.podContact[n])continue;int x=option_x(n),y=option_y(n);for(int i=0;i<MAXE;i++){Obj*o=g.enemies+i;if(o->on&&absn(o->x/Q-x)<13&&absn(o->y/Q-y)<13&&(!enemy_guarded(o)||o->type==4)){damage_enemy(o,o->type==4?25:60);ex.podContact[n]=20;pc_impact(x,y,3);if(o->hp<=0)killed(o);break;}}}}
static void environment_lightning(void){if(journey_kind()==ENV_REACTOR){int em=ex.env.timer%1500;if(em>=350&&em<1000)return;}int cycle=ex.env.timer%600;if(cycle==400)ex.env.strikeX=36+(ex.env.timer/600*83+g.wave*31)%248;if(cycle>=400&&cycle<475)ex.env.warning=1;if(cycle==475){int x=ex.env.strikeX;ex.env.weather.strikeSerial++;ex.env.weather.strikeTick=g.tick;ex.env.weather.strikeLegacy=1;if(storm_bolt_contact(g.px/(float)Q,player_y/(float)Q,ex.damagePrevX/(float)Q,ex.damagePrevY/(float)Q,x,ex.env.timer/600,STORM_BOLT_RADIUS+player_hull_rx()))hurt_at(g.px/Q,player_y/Q,0,-1);for(int i=0;i<MAXE;i++){Obj*o=g.enemies+i;if(o->on&&storm_bolt_contact(o->x/(float)Q,o->y/(float)Q,o->x/(float)Q,o->y/(float)Q,x,ex.env.timer/600,STORM_BOLT_RADIUS+pc_enemy_radius(o))){damage_enemy(o,250);if(o->hp<=0)killed(o);}}}}
static void storm_hurt(int amount){if(g.devInvincible||g.respawnTicks||g.shield||ex.invincibleTicks||ex.phoenixTicks)return;int actual=ship_armour_damage(amount);g.barrier-=actual;ex.lastDamageTick=g.tick;ex.lastDamageAmount=actual;ex.lastDamageKind=4;ex.lastDamageSerial++;ex.lastDamageFatal=0;g.shield=25;pc_hit_sound(0,4,g.px/Q);if(g.barrier<=0){g.shield=0;g.barrier=0;hurt_at(g.px/Q,player_y/Q,0,-1);}}
static void environment_damage(EnvironmentHazard*h,int damage){int old=h->hp*100-h->wound;h->wound+=damage;h->hp-=h->wound/100;h->wound%=100;if(h->hp*100-h->wound<old)h->damageUntil=g.tick+5;}
static void environment_weapon_hit(EnvironmentHazard*h,int damage,int weapon){if(h->type>=6){if(weapon==8)damage=tesla_damage(damage);damage=player_enemy_damage(damage);}int resistance=h->type==6?1<<7:h->type==7?1<<8:h->type==8?(1<<3)|(1<<5):0;damage=family_resistance(damage,resistance,weapon,h->x/Q,h->y/Q);int rolled=player_hit_roll(damage*player_attack_scale(weapon)/100*variant_damage_scale(weapon)/100,h->x/Q,h->y/Q);if(weapon==8){h->dotTicks=150;h->dotDamage=rolled/10;if(!h->dotClock)h->dotClock=50;rolled=rolled*90/100;}environment_damage(h,rolled);}
static void environmental_hazards(void){int kind=journey_kind(),t=ex.env.timer,cycle=t%180;
 if(!ex.giant.active){
 if((kind==ENV_ROOT||kind==ENV_HIVE||kind==ENV_BEAST)&&t%500==200){int count=0;for(int i=0;i<32;i++)count+=ex.env.hazards[i].on&&ex.env.hazards[i].type==15;if(count<2)environment_spawn(15,clamp(g.px/Q+(t/500&1?32:-32),60,260),-18,0,300,6,2);}
 if(!weather_front()){
 if(kind==ENV_HIVE||kind==ENV_ROOT||kind==ENV_BEAST){if(cycle>=100&&cycle<140)ex.env.warning=kind==ENV_BEAST?5:4;if(cycle==140){for(int side=-1;side<=1;side+=2){int y=80+(t/180*41)%100,x=side<0?tunnel_left(y)+10:tunnel_right(y)-10;int dx=g.px/Q-x,dy=player_y/Q-y,d=absn(dx)+absn(dy);if(!d)d=1;environment_spawn(kind==ENV_BEAST?3:1,x,y,dx*300/d,dy*300/d,7,1);}}}
 if((kind==ENV_BEAST||kind==ENV_TEETH||(kind==ENV_MOUTH&&t>350))&&cycle>=80&&cycle<130)ex.env.warning=19;
 if((kind==ENV_BEAST||kind==ENV_TEETH||(kind==ENV_MOUTH&&t>350))&&cycle>=130){ex.env.warning=20;if(t%25==0){int x=30+(t/25*67)%260;if(absn(x-160)>44)environment_spawn(13,x,24,0,700,5,1);}if(absn(g.px/Q-160)>65&&g.tick%50==0)storm_hurt(3);for(int e=0;e<MAXE;e++){Obj*o=g.enemies+e;if(o->on&&absn(o->x/Q-160)>65&&g.tick%50==0&&(!enemy_guarded(o)||o->type==4)){damage_enemy(o,250);if(o->hp<=0)killed(o);}}}
 if(kind==ENV_ASH){int c=t%240;if(c>=30&&c<85){ex.env.warning=21;ex.env.strikeX=50+(t/240*71)%220;}if(c==85)for(int n=-1;n<=1;n++)environment_spawn(14,ex.env.strikeX+n*26,-16-absn(n)*25,n*25,580,11,2);}
 if(kind==ENV_BLIZZARD&&cycle>=70&&cycle<130)ex.env.warning=6;
 if(kind==ENV_BEAST&&ex.waveActive&&ex.waveAge<150&&t%300==40){int side=(t/300)&1,y=65,x=side?tunnel_right(y)-12:tunnel_left(y)+12;environment_spawn(12,x,y,0,90,14,4);}
 if(kind==ENV_BEAST&&ex.waveActive&&ex.waveAge<150&&t%220==1)environment_spawn(9+(t/220)%3,100+(t/220*47)%120,-12,0,260,9,2);
 if(kind==ENV_WRECK||kind==ENV_REACTOR||kind==ENV_ESCAPE){if(t%100==0)environment_spawn(2,48+(t/100*67)%224,-18,((t/100)%3-1)*35,240,11,2);}
 if(kind==ENV_ICE||kind==ENV_BLIZZARD){if(cycle==70)ex.env.strikeX=70+(t/180*71)%180;if(cycle>=70&&cycle<130)ex.env.warning=6;if(cycle==130)for(int n=-1;n<=1;n++)environment_spawn(4,ex.env.strikeX+n*22,-20-absn(n)*24,0,310,9,1);}
 if(kind==ENV_CRYSTAL&&t%110==0)environment_spawn(5,70+(t/110*61)%180,-20,0,260,13,2);

 }}
 for(int i=0;i<32;i++){EnvironmentHazard*h=ex.env.hazards+i;if(!h->on)continue;int oldX=h->x,oldY=h->y;h->age++;if(h->type>=20&&h->type<=25){if(h->age>90){h->vx=(sn(h->age*2+i*61)*120)>>10;h->vy=h->y>170*Q?0:100;}if(!ex.empStop&&h->age%100==40)aim(h->x,h->y,280);}
  if(h->type==12){if(h->age>=120)ex.env.warning=17;if(h->age==180){int freeOrganic=0;for(int z=0;z<32;z++)freeOrganic+=ex.env.hazards[z].on&&ex.env.hazards[z].type>=9&&ex.env.hazards[z].type<=11;if(freeOrganic<10){int x=h->x/Q,y=h->y/Q;h->on=0;environment_spawn(9,x-6,y,0,220,9,2);environment_spawn(10,x+6,y,0,220,9,2);}else h->on=0;continue;}}
  if(h->type==15){int dx=g.px-h->x,dy=player_y-h->y,d=absn(dx)+absn(dy);if(d){h->vx=dx*260/d;h->vy=dy*260/d;}}if(h->type==9||h->type==10){int dx=g.px-h->x,dy=player_y-h->y,d=absn(dx)+absn(dy);if(d){h->vx=dx*380/d;h->vy=dy*380/d;}}if(h->type==11&&h->age%100==0)aim(h->x,h->y,300);h->x+=h->vx*(h->waveMember&&ex.waveAge>=(g.wave==1?600:500)?120:100)/100;h->y+=h->vy*(h->waveMember&&ex.waveAge>=(g.wave==1?600:500)?120:100)/100;cavern_steer_hazard(h,oldX,oldY);int x=h->x/Q,y=h->y/Q;if(y>play_max_y()+45||x<play_min_x()-54||x>play_max_x()+54||h->age>h->life){h->on=0;continue;}if(y<24)continue;
  if(h->type==6&&h->age%80==0)aim(h->x,h->y,400);
  int shielded=0;if(h->type==6)for(int j=0;j<32;j++)if(ex.env.hazards[j].on&&ex.env.hazards[j].type==8&&absn(ex.env.hazards[j].y-h->y)<90*Q)shielded=1;
  for(int j=0;j<MAXS;j++){Obj*s=g.shots+j;if(!s->on||s->base>0||absn(s->x/Q-x)>h->r+3||absn(s->y/Q-y)>h->r+3)continue;if(!shielded)environment_weapon_hit(h,s->type==9?800:projectile_damage(s),s->type);if(h->type==5){s->vy=-s->vy;s->vx=-s->vx+120;s->base=4;pc_reflection_sound(3,0,x);}else s->on=0;pc_impact(x,y,h->type==1?3:1);break;}
  if(!shielded&&g.weapon==8&&g.weaponFireCool[8]==weapon_delay(18))environment_weapon_hit(h,220,8);
  if(!shielded&&g.beam&&g.tick%4==0&&y<player_y/Q&&beam_hits(x,y,h->r))environment_weapon_hit(h,120,g.weapon);
  if(h->dotTicks){h->dotTicks--;if(--h->dotClock<=0){h->dotClock=50;if(!shielded){environment_damage(h,h->dotDamage);}}}
  if(ex.bombAge&&bomb_reached(x,y,h->r))h->hp=0;
  if(h->hp<=0){h->on=0;if(h->type==15){pc_organic_burst(x,y,-1,1);continue;}g.score+=h->type>=6&&h->type<=8?250:30;if(organic_context()&&(h->type==1||h->type==3||h->type>=9))burst_kind(x,y,0,(h->type==9||h->type==10||(h->type>=20&&h->type<=25&&h->type!=24))?2:1);else pc_charge_burst(x,y);if(h->type==7||h->type==8)ex.baseDamage++;if(h->type==12){int count=0;for(int z=0;z<32;z++)count+=ex.env.hazards[z].on&&ex.env.hazards[z].type>=9&&ex.env.hazards[z].type<=11;if(count<12)environment_spawn(9,x,y,0,220,9,1);}continue;}
  if(h->type==13||h->type==14){for(int e=0;e<MAXE;e++){Obj*o=g.enemies+e;if(o->on&&absn(o->x/Q-x)<h->r+10&&absn(o->y/Q-y)<h->r+10&&(!enemy_guarded(o)||o->type==4)){damage_enemy(o,300);if(o->hp<=0)killed(o);h->on=0;break;}}if(absn(g.px/Q-x)<h->r+5&&absn(player_y/Q-y)<h->r+5){storm_hurt(h->type==13?4:6);h->on=0;}continue;}
  if(h->type==15){if(absn(g.px/Q-x)<12&&absn(player_y/Q-y)<14&&globules_latch())h->on=0;continue;}
  if((h->type==9||h->type==10)&&absn(g.px/Q-x)<14&&absn(player_y/Q-y)<14){if(parasites_latch()){h->on=0;}continue;}
  if(absn(g.px/Q-x)<h->r+5&&absn(player_y/Q-y)<h->r+5)hurt_at(g.px/Q,player_y/Q,0,-1);
 }
}
#include "journey_giants.h"
#include "journey_weather.h"
static void environment_step(int mask){
 if(ex.env.kind!=journey_kind())environment_enter();
 if(ex.travel||ex.intro||g.mode!=1)return;
 ex.env.timer++;if(ex.env.wallGrace)ex.env.wallGrace--;ex.env.warning=0;ex.env.empActive=0;
 for(int w=0;w<9;w++){int using=w==g.weapon&&(mask&4)&&!g.weaponHeat[w]&&!ex.empDisable;ex.fatigue[w]=clamp(ex.fatigue[w]+(using?1:-3),0,300);}
 pod_contact_step();int kind=journey_kind();if(kind==ENV_BASE){if(mask&4)bombing_fire74();bombing_step();}
 if(!ex.giant.active){
 if(kind==ENV_WALL)asteroid_wall_step(mask);

 if(kind==ENV_EM||kind==ENV_REACTOR){int c=ex.env.timer%1500;if(c>=350&&c<500)ex.env.warning=2;if(c>=500&&c<1000){ex.env.empActive=1;ex.env.warning=3;if(g.tick%25==0){g.energy=clamp(g.energy-2,0,100);g.weaponEnergy[g.weapon]=clamp(g.weaponEnergy[g.weapon]-2,0,100);}}if(c==500)pc_emp_sound(1,160);}
 if(kind==ENV_OCEAN){ex.env.warning=ocean_warning70(ex.env.timer);if(!ex.travel&&!ex.intro&&ex.phaseAge>100&&g.px/(float)Q+player_hull_rx()>ocean_top70(player_y/(float)Q,ex.env.timer))storm_hurt(12);}

 if(kind==ENV_TEETH||kind==ENV_ESCAPE){ex.env.warning=kind==ENV_TEETH?8:9;int y=journey_exit_gate(ex.env.timer);if(absn(y-player_y/Q)<18&&absn(g.px/Q-160)>45){hurt_at(g.px/Q,player_y/Q,0,-1);g.px=160*Q;}if(y>player_y/Q+18&&absn(g.px/Q-160)<45)ex.env.escaped=1;}
 if(kind==ENV_REACTOR){if(g.tick%100==0){g.energy=clamp(g.energy-1,0,100);g.weaponEnergy[g.weapon]=clamp(g.weaponEnergy[g.weapon]-1,0,100);}if(!ex.env.warning&&g.tick%100<30)ex.env.warning=18;}
 }
 environmental_hazards();globules_step();parasites_step();giant_step(mask);weather_step();
 if(!ex.travel&&!ex.intro&&!ex.victory&&!ex.bossSpawned&&!section_has_boss()&&((kind!=ENV_TEETH&&kind!=ENV_ESCAPE)||g.clock>=section_duration())&&!ex.waveActive&&!ex.waveRest&&terrain_ready()&&mission_progress_ready()&&g.groups>=section_wave_target()&&!route_occupied()&&!hostile_rounds_remain()&&((kind!=ENV_TEETH&&kind!=ENV_ESCAPE)||ex.env.escaped))journey_advance();
}
