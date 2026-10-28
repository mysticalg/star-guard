static int weather_safe_half(void){return cavern_guard_active()?32:46;}
static int weather_center_at(int y){return journey_kind()==ENV_OCEAN?ex.env.safeX:tunnel_visible(y)?tunnel_lane(y,0):160;}
static void weather_cavern(WeatherActor*a){if(!cavern_guard_active())return;float l,r;int side=a->oldX/Q>tunnel_center(a->oldY/Q),radius=(int)ceilf(a->r*1.42f);if(!cavern_interval(a->y/(float)Q,radius,side,&l,&r)){a->on=0;return;}a->x=clamp(a->x,(int)ceilf(l*Q),(int)floorf(r*Q));}
/* Authored fronts use fixed 50Hz simulation ticks and a bounded actor pool.
   All positions below are world coordinates shared by drawing and contact. */
static int weather_sweep(float ax,float ay,float bx,float by,float radius){float dx=bx-ax,dy=by-ay,t=dx*dx+dy*dy;t=t>0?clamp((int)((-ax*dx-ay*dy)*65536/t),0,65536)/65536.0f:0;ax+=dx*t;ay+=dy*t;return ax*ax+ay*ay<=radius*radius;}
static int weather_part_contact(int n,float x,float y,float xx,float yy,float radius){Giant*b=&ex.giant;GiantPart*p=b->parts+n;if(!weather_sweep(x-b->previousPartX[n],y-b->previousPartY[n],xx-p->x,yy-p->y,radius+hypotf(p->w,p->h)*.5f))return 0;int steps=clamp((int)(hypotf(xx-x-(p->x-b->previousPartX[n]),yy-y-(p->y-b->previousPartY[n]))/3)+1,1,160);for(int step=0;step<=steps;step++){float t=step/(float)steps,cx=b->previousPartX[n]+(p->x-b->previousPartX[n])*t,cy=b->previousPartY[n]+(p->y-b->previousPartY[n])*t,a=(b->previousPartAngle[n]+(p->angle-b->previousPartAngle[n])*t)*.01745329252f,c=cosf(a),s=sinf(a),dx=x+(xx-x)*t-cx,dy=y+(yy-y)*t-cy,lx=dx*c+dy*s,ly=-dx*s+dy*c;int l=clamp((int)((lx-radius)/p->w*64+32),0,63),r=clamp((int)((lx+radius)/p->w*64+32),0,63),top=clamp((int)((ly-radius)/p->h*64+32),0,63),bottom=clamp((int)((ly+radius)/p->h*64+32),0,63);for(int py=top;py<=bottom;py+=2)for(int px=l;px<=r;px+=2)if(giant_module_alpha(n,px,py)){float qx=((px+.5f)/64-.5f)*p->w-lx,qy=((py+.5f)/64-.5f)*p->h-ly;if(qx*qx+qy*qy<=(radius+2)*(radius+2))return 1;}}return 0;}
static int weather_enemy_hit(Obj*o,int damage){if(!o->on||(!enemy_guarded(o)||o->type==4)==0)return 0;if(o->type!=4&&ex.env.weather.kind!=WEATHER_ASTEROID){if(o->age<100||o->weatherHitUntil>g.tick)return 0;o->weatherHitUntil=g.tick+100;if(damage>80)damage=80;}int hp=o->hp*100-o->wound;damage_enemy(o,damage);int lost=hp-(o->hp*100-o->wound);if(lost>0){ex.env.weather.enemyHits++;if(o->type==4)ex.env.weather.bossHits++;}if(o->hp<=0)killed(o);return lost>0;}
static int weather_targets(int x,int y,int xx,int yy,int radius,int damage){int hit=0;for(int i=0;i<MAXE;i++){Obj*o=g.enemies+i;if(!o->on)continue;float r=pc_enemy_radius(o);if(weather_sweep(x-(o->previousX||o->previousY?o->previousX:o->x)/(float)Q,y-(o->previousX||o->previousY?o->previousY:o->y)/(float)Q,xx-o->x/(float)Q,yy-o->y/(float)Q,radius+r)){weather_enemy_hit(o,damage);hit=1;}}
 if(ex.giant.active)for(int n=0;n<9;n++)if(giant_open(n)&&weather_part_contact(n,x,y,xx,yy,radius)){int before=ex.giant.parts[n].hp;giant_raw_hit(n,damage);if(ex.giant.parts[n].hp<before){ex.env.weather.bossHits++;hit=1;}break;}return hit;}
static void weather_lightning_targets(int x,int seed,int damage){
 for(int i=0;i<MAXE;i++){Obj*o=g.enemies+i;if(o->on&&storm_bolt_contact(o->x/(float)Q,o->y/(float)Q,(o->previousX||o->previousY?o->previousX:o->x)/(float)Q,(o->previousX||o->previousY?o->previousY:o->y)/(float)Q,x,seed,STORM_BOLT_RADIUS+pc_enemy_radius(o)))weather_enemy_hit(o,damage);}
 if(ex.giant.active)for(int n=0;n<9;n++)if(giant_open(n)){for(int k=0;k<STORM_BOLT_SEGMENTS;k++)if(weather_part_contact(n,storm_bolt_x(x,seed,k),storm_bolt_y(k),storm_bolt_x(x,seed,k+1),storm_bolt_y(k+1),STORM_BOLT_RADIUS)){int hp=ex.giant.parts[n].hp;giant_raw_hit(n,damage);ex.env.weather.bossHits+=ex.giant.parts[n].hp<hp;break;}}
}
static void weather_blast_sound(int x,int y,int r,int audible){Weather*w=&ex.env.weather;w->blasts++;if(audible){pc_charge_burst(x,y);pc_torpedo_sound(1,x);}if(terrain.active)terrain_chip(x,y,r,18);for(int i=0;i<MAXH;i++){Obj*o=ex.obstacles+i;if(o->on&&absn(o->x/Q-x)<r+o->base&&absn(o->y/Q-y)<r+o->base){o->hp-=4;if(o->hp<=0){o->on=0;pc_impact(o->x/Q,o->y/Q,2);}}}}
static void weather_blast(int x,int y,int r){weather_blast_sound(x,y,r,1);}
static WeatherActor*weather_spawn(int kind,int x,int y,int vx,int vy,int r){Weather*w=&ex.env.weather;for(int i=0;i<48;i++){WeatherActor*a=w->actors+i;if(a->on)continue;*a=(WeatherActor){.on=1,.type=kind,.x=x*Q,.y=y*Q,.oldX=x*Q,.oldY=y*Q,.vx=vx,.vy=vy,.r=r,.hp=kind==WEATHER_ASTEROID?500:kind==WEATHER_ICE?180:100};if(kind==WEATHER_ASTEROID)a->age=-25;w->serial++;return a;}return 0;}
static void weather_lane_damage(int kind){Weather*w=&ex.env.weather;int safe=w->safeX,width=weather_safe_half();/* Cloud alpha is cosmetic. This separately marked lane is the damaging region. */
 if(absn(g.px/Q-safe)>width&&!player_is_protected()){int serial=ex.lastDamageSerial;storm_hurt(kind==WEATHER_ACID?4:5);w->playerHits+=ex.lastDamageSerial!=serial;if(kind==WEATHER_ION)g.weaponEnergy[g.weapon]=clamp(g.weaponEnergy[g.weapon]-2,0,100);}
 for(int i=0;i<MAXE;i++){Obj*o=g.enemies+i;if(o->on&&absn(o->x/Q-weather_center_at(o->y/Q))>width)weather_enemy_hit(o,kind==WEATHER_ACID?180:220);}
 if(ex.giant.active)for(int n=0;n<9;n++){GiantPart*p=ex.giant.parts+n;if(giant_open(n)&&absn(p->x-weather_center_at(p->y))>width){int before=p->hp;giant_raw_hit(n,120);w->bossHits+=p->hp<before;}}
}
static void weather_step(void){Weather*w=&ex.env.weather;int kind=weather_profile(),t=ex.env.timer,c=t%2500,start,end;weather_window(&start,&end);int prior=w->stage;w->kind=kind;w->stage=kind?weather_stage():0;w->left=w->stage==1?start-t:w->stage==2?end-t:w->stage==3?end+100-t:0;
 int center=weather_center_at(player_y/Q);w->safeX=center;
 if(w->stage==1&&prior!=1){w->recoveryGiven=0;if(kind&&kind!=WEATHER_STORM)pc_hit_sound(5,0,160);}if(w->stage==2&&prior!=2&&kind==WEATHER_ION)pc_emp_sound(1,160);if(w->stage==1)ex.env.warning=22+kind;if(w->stage==2)ex.env.warning=29+kind;if(w->stage==3)ex.env.warning=36;
 if(w->stage==2){
  if((kind==WEATHER_STORM||kind==WEATHER_ION)&&lightning_clock()>=0&&lightning_clock()%lightning_period()==35){int seed=lightning_seed(),side=seed&1;w->strikeSerial++;w->strikeTick=g.tick;w->strikeLegacy=0;w->strikeX=side?center+100:center-100;if(kind==WEATHER_ION)pc_emp_sound(1,w->strikeX);if(storm_bolt_contact(g.px/(float)Q,player_y/(float)Q,ex.damagePrevX/(float)Q,ex.damagePrevY/(float)Q,w->strikeX,seed,STORM_BOLT_RADIUS+player_hull_rx())){int serial=ex.lastDamageSerial;storm_hurt(kind==WEATHER_STORM?18:12);w->playerHits+=ex.lastDamageSerial!=serial;}weather_lightning_targets(w->strikeX,seed,450);weather_blast_sound(w->strikeX,80+(seed*37)%140,18,0);}
  if(kind==WEATHER_ACID||kind==WEATHER_ION){if(c%50==0)weather_lane_damage(kind);}
  int interval=kind==WEATHER_ASTEROID?17:kind==WEATHER_FIRE?28:kind==WEATHER_ICE?20:kind==WEATHER_ACID?34:0;
  if(interval&&c%interval==0){int serial=w->serial,side=serial&1,span=clamp((play_max_x()-play_min_x())/2-68,16,500),x=center+(side?1:-1)*(84+(serial*37)%span),r=kind==WEATHER_ASTEROID?12+(serial*7)%13:kind==WEATHER_ICE?9:7;
   if(kind==WEATHER_ASTEROID){/* Full-width staggered paths, no permanent centre sanctuary. */int width=play_max_x()-play_min_x(),lane=(serial*5+serial/7)%11;x=play_min_x()+r+4+(width-2*r-8)*lane/10;r=8+(serial*7)%13;}
   if(tunnel_visible(-30))x=clamp(x,tunnel_left(-30)+r+3,tunnel_right(-30)-r-3);
   weather_spawn(kind,x,play_min_y()-r-12,kind==WEATHER_ASTEROID?((serial*43)%181-90):(side?18:-18),kind==WEATHER_ASTEROID?360+(serial*53)%250:kind==WEATHER_FIRE?580:320,r);
  }
 }
 if(w->stage==3&&!w->recoveryGiven){w->recoveryGiven=1;drop(center,90,2);/* Real shield/20 armour pickup. H remains free even at zero energy. */}
 for(int i=0;i<48;i++){WeatherActor*a=w->actors+i;if(!a->on)continue;if(a->age<0){a->age++;continue;}a->oldX=a->x;a->oldY=a->y;a->x+=a->vx;a->y+=a->vy;a->age++;weather_cavern(a);if(!a->on)continue;int x=a->x/Q,y=a->y/Q;if(y>play_max_y()+a->r+25||x<play_min_x()-80||x>play_max_x()+80||a->age>800){a->on=0;continue;}
  for(int s=0;s<MAXS;s++){Obj*p=g.shots+s;if(p->on&&p->base<=0&&weather_sweep(a->oldX/(float)Q-p->previousX/(float)Q,a->oldY/(float)Q-p->previousY/(float)Q,x-p->x/(float)Q,y-p->y/(float)Q,a->r+3)){a->hp-=p->type==9?800:projectile_damage(p);p->on=0;break;}}
  if(g.beam&&g.tick%4==0&&y<player_y/Q&&beam_hits(x,y,a->r))a->hp-=ex.phoenixTicks?120:beam_target_damage81(120,ex.multi?5:3);if(ex.bombAge&&bomb_reached(x,y,a->r))a->hp=0;
  if(a->hp<=0){w->interceptions++;a->on=0;weather_blast(x,y,a->r+4);continue;}
  if(terrain.active&&a->age>=150&&terrain_solid(x,y)){a->on=0;weather_blast(x,y,a->r+4);continue;}
  int hit=weather_targets(a->oldX/Q,a->oldY/Q,x,y,a->r,a->type==WEATHER_ASTEROID?500:250);
  if(weather_sweep(a->oldX/(float)Q-ex.damagePrevX/(float)Q,a->oldY/(float)Q-ex.damagePrevY/(float)Q,x-g.px/(float)Q,y-player_y/(float)Q,a->r+player_hull_rx())){int serial=ex.lastDamageSerial;storm_hurt(a->type==WEATHER_ASTEROID?22:a->type==WEATHER_FIRE?10:6);w->playerHits+=ex.lastDamageSerial!=serial;hit=1;}
  if(hit){a->on=0;weather_blast(x,y,a->r+4);}
 }
 /* Delayed side impacts open actual fine-mask grains, never an automatic player tunnel. */
 if(terrain.active&&t>=650&&t%250==0){int x=(t/250&1)?34:286,y=clamp(wall_origin_y()+TERRAIN_H-70,35,160);weather_blast(x,y,22);}
 /* Legacy flying debris now shares both-side damage, using its actual world position. */
 for(int i=0;i<32;i++){EnvironmentHazard*h=ex.env.hazards+i;if(!h->on||!(h->type==1||h->type==2||h->type==3||h->type==4||h->type==13||h->type==14))continue;if(weather_targets(h->x/Q,h->y/Q,h->x/Q,h->y/Q,h->r,250)){h->on=0;pc_impact(h->x/Q,h->y/Q,2);}}
}
