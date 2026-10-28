/* Shoot-removable hull globules are separate from electricity-only hull leeches.
   They never enter enemy/drop/charge accounting. Coordinates share the ship's
   world facing, so the existing flight camera rotates them with hull and fire. */
static int beam_hits(int,int,int);
static int globules_count(void){int count=0;for(int n=0;n<3;n++)count+=ex.globuleHP[n]>0;return count;}
static int globules_handling(void){return 100-4*globules_count();}
static float globule_x(int n){return g.px/(float)Q+(n==0?-3:n==1?3:0);}
static float globule_y(int n){return player_y/(float)Q-(n==2?5:2);}
static void globules_clear(void){for(int n=0;n<3;n++)ex.globuleHP[n]=ex.globuleDamageUntil[n]=0;ex.globuleDrain=0;ex.globuleGrace=100;}
static void globules_remove(int n){pc_organic_burst(globule_x(n),globule_y(n),-1,1);ex.globuleHP[n]=0;ex.globuleClearSerial++;if(!globules_count())ex.globuleDrain=0;}
static void globules_electric_purge(void){for(int n=0;n<3;n++)if(ex.globuleHP[n])globules_remove(n);ex.globuleGrace=150;}
static int globules_latch(void){int kind=journey_kind();if((kind!=ENV_ROOT&&kind!=ENV_HIVE&&kind!=ENV_BEAST)||ex.globuleGrace||g.shield||g.active||ex.phoenixTicks)return 0;for(int n=0;n<3;n++)if(!ex.globuleHP[n]){ex.globuleHP[n]=240;ex.globuleDamageUntil[n]=0;ex.globuleGrace=100;ex.globuleSerial++;pc_hit_sound(3,0,g.px/Q);return 1;}return 0;}
static void globules_hit(int n,int damage){if(!ex.globuleHP[n])return;ex.globuleHP[n]=clamp(ex.globuleHP[n]-damage,0,240);ex.globuleDamageUntil[n]=g.tick+5;if(!ex.globuleHP[n])globules_remove(n);else pc_hit_sound(3,0,globule_x(n));}
static int globules_shot(Obj*s,int oldX,int oldY){if(!s->on||!globules_count())return 0;float ax=oldX/(float)Q,ay=oldY/(float)Q,dx=(s->x-oldX)/(float)Q,dy=(s->y-oldY)/(float)Q,length=dx*dx+dy*dy;for(int n=0;n<3;n++){if(!ex.globuleHP[n])continue;float x=globule_x(n)-ax,y=globule_y(n)-ay,t=length>0?(x*dx+y*dy)/length:0;t=fmaxf(0,fminf(1,t));x-=dx*t;y-=dy*t;if(x*x+y*y<=36){int weapon=s->sourceFamily?s->sourceFamily-1:s->type==9?g.weapon:clamp(s->type,0,8);globules_hit(n,clamp(projectile_damage(s)*player_attack_scale(weapon)/100*variant_damage_scale(weapon)/100,25,200));s->on=0;return 1;}}return 0;}
static void globules_step(void){if(ex.globuleGrace)ex.globuleGrace--;int count=globules_count();if(!count)return;int kind=journey_kind();if(kind!=ENV_ROOT&&kind!=ENV_HIVE&&kind!=ENV_BEAST){globules_clear();return;}if(++ex.globuleDrain>=100){ex.globuleDrain=0;int w=g.weapon;g.weaponEnergy[w]=clamp(g.weaponEnergy[w]-count,0,100);}if(g.tick%6==0)for(int n=0;n<3;n++)if(ex.globuleHP[n]){int pods=option_beam_count80(globule_x(n),globule_y(n),6);if(pods){globules_hit(n,80*pods/5*player_attack_scale(g.weapon)/100);break;}}if(g.beam&&g.tick%6==0)for(int n=0;n<3;n++)if(ex.globuleHP[n]&&beam_hits(globule_x(n),globule_y(n),6)){globules_hit(n,80*player_attack_scale(g.weapon)/100);break;}}
