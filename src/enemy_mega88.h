/* One visible warning, 150 damaging simulation ticks, then a finite retreat.
 * Geometry is sampled after cavern steering and shared by damage and draw. */
enum { ENEMY_MEGA_WARNING88=60, ENEMY_MEGA_FIRE88=150, ENEMY_MEGA_TAIL88=15 };
static int terrain_solid(int,int),tunnel_visible(int),tunnel_island(int,int*,int*);
extern void pc_enemy_mega_sound88(int,int);
static int enemy_mega_blocked88(const Obj*o){return o->ice83.frozen||o->siphon84.disarm||o->empFreeze||ex.empStop||ex.env.empActive||!enemy_damageable(o)||ex.travel||ex.intro||ex.victory;}
static int enemy_mega_solid88(float x,float y){
 int xx=(int)floorf(x),yy=(int)floorf(y),l,r;
 if(journey_kind()==ENV_WALL&&terrain.active&&terrain_solid(xx,yy))return 1;
 return yy>=24&&yy<256&&tunnel_visible(yy)&&(xx<tunnel_left(yy)||xx>tunnel_right(yy)||(tunnel_island(yy,&l,&r)&&xx>=l&&xx<=r));
}
static void enemy_mega_geometry88(Obj*o){
 EnemyMega88*m=&o->mega88;int ax=o->x,ay=o->y;pc_enemy_side_muzzle77(o,&ax,&ay,0);
 m->ax=ax/(float)Q;m->ay=ay/(float)Q;m->half=22;
 float dx=o->aimX/(float)Q-m->ax,dy=o->aimY/(float)Q-m->ay,d=hypotf(dx,dy);
 if(d<1){dx=0;dy=1;d=1;}dx/=d;dy/=d;float length=0;
 for(int k=0;k<=700;k++){
  int blocked=0;
  for(int w=-22;w<=22;w++)if(enemy_mega_solid88(m->ax+dx*k-dy*w,m->ay+dy*k+dx*w)){blocked=1;break;}
  if(blocked)break;length=(float)k;
 }
 m->bx=m->ax+dx*length;m->by=m->ay+dy*length;m->stamp=g.tick;
}
static int enemy_mega_contact88(const EnemyMega88*m,float x,float y){
 float dx=m->bx-m->ax,dy=m->by-m->ay,d=hypotf(dx,dy);if(d<1)return 0;
 float support=hypotf(dy*player_hull_rx()/d,dx*player_hull_ry()/d);
 return aircraft_beam_distance(x,y,m->ax,m->ay,m->bx,m->by)<=m->half+support;
}
static int enemy_mega_clear_hull88(int x,int y){
 for(int yy=-1;yy<=1;yy++)for(int xx=-1;xx<=1;xx++)if(enemy_mega_solid88(x+xx*player_hull_rx(),y+yy*player_hull_ry()))return 0;
 return 1;
}
static int enemy_mega_safe_lane88(Obj*o){
 EnemyMega88*m=&o->mega88;int y=player_y/Q,px=g.px/Q,best=-1;
 EnemyMega88 margin=*m;margin.half+=8;
 for(int d=0;d<=play_max_x()-play_min_x()&&best<0;d+=2)for(int side=0;side<(d?2:1);side++){
  int x=px+(side?-d:d);if(x<play_min_x()+player_hull_rx()+4||x>play_max_x()-player_hull_rx()-4)continue;
  if(enemy_mega_contact88(&margin,x,y)||!enemy_mega_clear_hull88(x,y))continue;
  int clear=1,steps=absn(x-px);for(int n=0;n<=steps;n++)if(!enemy_mega_clear_hull88(px+(x>=px?n:-n),y)){clear=0;break;}
  if(clear){best=x;break;}
 }
 m->safeX=best;return best>=0;
}
static int enemy_mega_available88(const Obj*o){
 for(int i=0;i<MAXE;i++){const Obj*p=g.enemies+i;if(p!=o&&p->on&&p->type==27&&!p->mega88.done&&p->weaponClock>0&&p->weaponClock<=ENEMY_MEGA_WARNING88+ENEMY_MEGA_FIRE88+ENEMY_MEGA_TAIL88&&!enemy_mega_blocked88(p))return 0;}
 return 1;
}
static void enemy_mega_step88(Obj*o){
 EnemyMega88*m=&o->mega88;m->phase=0;
 if(m->done)return;
 if(enemy_mega_blocked88(o)){o->weaponClock=0;m->fired=m->hit=m->absorbed=0;return;}
 if(!o->weaponClock){
  if(o->age>340||o->y>(player_y-35*Q)||o->y<(play_min_y()+10)*Q||!enemy_mega_available88(o))return;
  o->aimX=g.px;o->aimY=player_y;enemy_mega_geometry88(o);
  if(!enemy_mega_safe_lane88(o))return;
  m->fired=m->hit=m->absorbed=0;pc_laser_charge76(o->x/Q,o->y/Q,ENEMY_MEGA_WARNING88);
 }
 int c=++o->weaponClock;
 if(c>ENEMY_MEGA_WARNING88+ENEMY_MEGA_FIRE88+ENEMY_MEGA_TAIL88){m->done=1;return;}
 enemy_mega_geometry88(o);
 if(!enemy_mega_safe_lane88(o)){o->weaponClock=0;m->fired=m->hit=m->absorbed=0;return;}
 if(c<=ENEMY_MEGA_WARNING88){m->phase=1;return;}
 if(c>ENEMY_MEGA_WARNING88+ENEMY_MEGA_FIRE88){m->phase=3;return;}
 m->phase=2;m->fired++;if(c==ENEMY_MEGA_WARNING88+1)o->weaponShots++;
 if(hypotf(m->bx-m->ax,m->by-m->ay)>1)pc_enemy_mega_sound88((int)m->ax,(int)m->ay);
 if(m->hit)return;
 float x0=ex.damagePrevX/(float)Q,y0=ex.damagePrevY/(float)Q,dx=g.px/(float)Q-x0,dy=player_y/(float)Q-y0;
 int steps=clamp((int)ceilf(hypotf(dx,dy)),1,1024);
 for(int n=0;n<=steps;n++)if(enemy_mega_contact88(m,x0+dx*n/steps,y0+dy*n/steps)){
  /* Protection absorbs the contact; only actual fatal damage latches a life. */
  if(player_is_protected()){ex.blockedTick=g.tick;if(!m->absorbed){hurt_amount(g.px/Q,player_y/Q,0,1,1000,1);m->absorbed=1;}}else if(hurt_amount(g.px/Q,player_y/Q,0,1,1000,1)>0)m->hit=1;
  break;
 }
}
