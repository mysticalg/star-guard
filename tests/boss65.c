#define main star_guard_main
#include "../src/pc.c"
#undef main
static int checks;
#define V(x) do{checks++;if(!(x)){fprintf(stderr,"FAIL %d %s M%d tick%d\n",__LINE__,#x,g.wave,g.tick);return 1;}}while(0)
static void scene(int m,int section){configuredLevel=1;selectedShip=0;game_reset();g.mode=1;g.fx=0;g.wave=m;ex.section=section;ex.phase=ex.oldPhase=mission_phase(m);ex.intro=ex.travel=ex.entryY=0;ex.rearSpawned=1;ex.bossSpawned=1;g.clock=-100000;g.shield=100000;environment_enter();}
static void hit(int path,Obj*o){switch(path){case 9:weather_enemy_hit(o,200);break;case 10:{Obj s={.x=o->x,.y=o->y};detonate_torpedo(&s);break;}case 11:{Obj s={.x=o->x,.y=o->y,.type=4,.power=1};detonate_heatseeker(&s);break;}case 12:{Obj s={.x=o->x,.y=o->y};mine_detonate(&s);break;}case 13:{Obj s={.x=o->x,.y=o->y};detonate_charge(&s);break;}case 14:ex.bombSerial++;ex.bombAge=20;ex.bombX=o->x/Q;ex.bombY=o->y/Q;bomb_step();break;case 15:damage_enemy(o,3000);break;default:weapon_damage_enemy(o,200,path);}}
int main(void){headless=1;int seen=0;
for(int m=1;m<=6;m++)for(int s=0;s<journeyCounts[m-1];s++){int v=journeyMinis[m-1][s];if(v<0)continue;seen++;
for(int form=1;form<=3;form++){scene(m,s);boss();Obj*o=g.enemies;while(o<g.enemies+MAXE&&(!o->on||o->type!=4))o++;V(o<g.enemies+MAXE);V(o->openingTicks==(10+5*(m-1))*50);
o->age=240;o->phase=form;o->base=900;o->hp=1200-form*300;o->power=(form-1)*256;o->shieldCharge=0;
for(int p=0;p<16;p++){o->openingTicks=100;o->damageUntil=0;o->wound=0;int hp=o->hp;hit(p,o);V(o->on&&o->hp==hp&&!o->wound&&!o->damageUntil&&o->openingTicks==100);}
o->openingTicks=17;g.paused=1;int clock=g.tick;for(int t=0;t<50;t++)game_step(0,0);V(o->openingTicks==17&&g.tick==clock);g.paused=0;g.mode=3;g.continueTicks=400;game_step(0,13);V(o->openingTicks==17&&g.mode==1);ex.empStop=100;for(int t=0;t<17;t++){g.shield=100000;game_step(0,0);}V(!o->openingTicks);ex.empStop=0;o->attack=0;o->morph=0;o->shieldCharge=0;int hp=o->hp;weapon_damage_enemy(o,100,0);V(o->hp<hp&&o->damageUntil==g.tick+5);
o->openingTicks=4;o->dotTicks=5;o->dotClock=1;o->dotDamage=100;hp=o->hp;game_step(0,0);V(o->hp==hp&&o->openingTicks==3);
if(form<3){o->openingTicks=0;o->hp=o->base*(3-form)/3;o->morph=0;stage_boss_step(o);V(o->phase==form+1&&o->morph>0&&!o->openingTicks);hp=o->hp;damage_enemy(o,10000);V(o->hp==hp);}
}
printf("mini%d mission%d all3forms protectedall16damagepaths/countdown/pause/continue/EMP/DoT/morph PASS\n",v,m);}
V(seen==17);
for(int m=1;m<=6;m++)for(int s=0;s<journeyCounts[m-1];s++){if(journeyMinis[m-1][s]<0)continue;scene(m,s);boss();int attacks=0,initial=boss_entrance_ticks(m);for(int tick=0;tick<initial;tick++){g.shield=100000;game_step(0,0);for(int b=0;b<MAXB;b++)if(g.bullets[b].on){attacks++;g.bullets[b].on=0;}}V(attacks>0);printf("mini mission%d section%d entrance%ds actualattacks%d PASS\n",m,s,initial/50,attacks);}
for(int m=1;m<=6;m++)for(int s=0;s<journeyCounts[m-1];s++){scene(m,s);ex.victory=120;ex.encounterTier=1;V(!mission_report_ready());ex.encounterTier=2;V(mission_report_ready()==section_final());}

for(int seed=0;seed<24;seed++)for(int side=-1;side<=1;side+=2){scene(1,0);int center=160+side*100;for(int y=40;y<=240;y+=5){V(storm_bolt_contact(storm_bolt_x(center,seed,6),storm_bolt_y(6),storm_bolt_x(center,seed,6),storm_bolt_y(6),center,seed,STORM_BOLT_RADIUS));V(!storm_bolt_contact(160,y,160,y,center,seed,STORM_BOLT_RADIUS+8));V(!storm_bolt_contact(114,y,114,y,center,seed,STORM_BOLT_RADIUS+8));V(!storm_bolt_contact(206,y,206,y,center,seed,STORM_BOLT_RADIUS+8));}}
V(STORM_BOLT_RADIUS*2>player_hull_rx()*2);
printf("PASS %d all17minis entrance10..35s/3forms/all damage paths and shared thicklightning core/safechannel checks\n",checks);return 0;}
