/* Painted hull exposure, separate from main sprites, lighting and shadow casters. */
static void draw_painted_blur75(void){
 motionGhosts75=motionFollowerGhosts75=0;if(!motion_enabled75())return;SDL_Texture*batch=NULL;nquad=0;
 int budget=quality==2?160:64;
 for(int pass=quality==2?1:0;pass>=0;pass--)for(int i=0;i<ndraw&&motionGhosts75<budget;i++){
  Draw*d=draws+i;if(!d->motionSerial||(zoomed_pilot_pass()&&d->n==136))continue;
  MotionTrack75*t=motion_find75(d->motionSerial);MotionPose75 p;int age=pass?4:2;
  if(!motion_pose75(t,age,motionAlpha75,&p)||!motion_clearance75(t,p))continue;
  float distance=hypotf(d->x-p.x,d->y-p.y),speed=distance/age;if(distance<.55f||distance>25||speed<.22f)continue;
  Draw ghost=*d;ghost.x=p.x;ghost.y=p.y;ghost.angle=d->angle-d->motionHeading+p.angle;ghost.damageUntil=0;
  ghost.opacity=(d->opacity>0?d->opacity:1)*fminf(.38f,.14f+speed*.055f)*(pass?.48f:.8f);
  SDL_Texture*texture=actor_texture(&ghost);if(texture!=batch){if(batch)flush(batch);batch=texture;}draw_shape(&ghost,0);motionGhosts75++;if(d->motionSerial>=0xffffffe0u&&d->motionSerial!=0xffffffffu)motionFollowerGhosts75++;
 }
 if(batch)flush(batch);
}
static void draw_projectile_blur75(void){
 motionProjectileGhosts75=0;if(!motion_enabled75())return;
 for(int i=0;i<MAXS+MAXB&&motionProjectileGhosts75<(quality==2?160:64);i++){
  Obj*o=i<MAXS?g.shots+i:g.bullets+i-MAXS;if(!o->on)continue;MotionTrack75*t=motion_find75(o->serial);MotionPose75 p;
  if(!motion_pose75(t,2,motionAlpha75,&p))continue;float x=o->x/(float)Q,y=o->y/(float)Q,distance=hypotf(x-p.x,y-p.y);
  if(distance<.7f||distance>32)continue;SDL_FColor c=projectile_colour(o,i>=MAXS);c.a=.17f;
  beam_ribbon(p.x,p.y,x,y,.55f,c);light(p.x,p.y,1.1f,c.r,c.g,c.b,.24f);motionProjectileGhosts75++;
 }
}
