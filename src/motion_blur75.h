/* Six fixed 50Hz poses per live object. Render frequency never grows history.
 * IDs prevent recycled slots, route changes and respawns from leaving ghosts. */
#define MOTION75_COUNT (1+MAXE+MAXS+MAXB+MAXH+8+96+7)
typedef struct{float x,y,angle;} MotionPose75;
typedef struct{u32 serial;int count,head,side,radius;MotionPose75 pose[6];} MotionTrack75;
static MotionTrack75 motion75[MOTION75_COUNT];
static int motionMap75[1024],motionTick75=-1,motionWave75,motionSection75,motionLives75;
static float motionAlpha75;
static int motionGhosts75,motionProjectileGhosts75,motionWeatherGhosts75,motionDisabled75,motionFollowerGhosts75;
static int motion_enabled75(void){return !motionDisabled75&&g.fx&&!reducedMotion&&quality>0&&g.mode==1&&!ex.travel&&!ex.sectionHandover&&!ex.cavernTurn;}
static void motion_clear75(void){memset(motion75,0,sizeof(motion75));memset(motionMap75,0,sizeof(motionMap75));}
void pc_motion_reset75(void){motion_clear75();motionTick75=-1;}
static void motion_record75(int slot,u32 serial,int on,float x,float y,float angle,int side,int radius){
 MotionTrack75*t=motion75+slot;
 if(!on||!serial){t->serial=0;t->count=0;return;}
 if(t->serial!=serial||(t->count&&hypotf(x-t->pose[t->head].x,y-t->pose[t->head].y)>20)){*t=(MotionTrack75){.serial=serial};}
 t->head=(t->head+1)%6;t->pose[t->head]=(MotionPose75){x,y,angle};t->count=clamp(t->count+1,0,6);t->side=side;t->radius=radius;
 unsigned h=serial&1023;while(motionMap75[h])h=(h+1)&1023;motionMap75[h]=slot+1;
}
static void motion_objects75(int offset,Obj*a,int count,int enemies){for(int i=0;i<count;i++){Obj*o=a+i;motion_record75(offset+i,o->serial,o->on,o->x/(float)Q,o->y/(float)Q,o->heading/16.0f,enemies?o->caveSide:o->x/Q>tunnel_center(o->y/Q),enemies==1?pc_enemy_radius(o):enemies==2?o->base+3:0);}}
static void motion_step75(void){
 if(g.tick==motionTick75&&g.wave==motionWave75&&ex.section==motionSection75&&g.lives==motionLives75)return;
 if(g.tick!=motionTick75+1||g.wave!=motionWave75||ex.section!=motionSection75||g.lives!=motionLives75||!motion_enabled75())motion_clear75();
 motionTick75=g.tick;motionWave75=g.wave;motionSection75=ex.section;motionLives75=g.lives;
 memset(motionMap75,0,sizeof(motionMap75));if(!motion_enabled75())return;
 motion_record75(0,0xffffffffu,1,g.px/(float)Q,player_y/(float)Q,0,g.px/Q>tunnel_center(player_y/Q),player_hull_rx());
 int n=1;motion_objects75(n,g.enemies,MAXE,1);n+=MAXE;motion_objects75(n,g.shots,MAXS,0);n+=MAXS;motion_objects75(n,g.bullets,MAXB,0);n+=MAXB;motion_objects75(n,ex.obstacles,MAXH,2);n+=MAXH;motion_objects75(n,g.drops,8,0);n+=8;motion_objects75(n,ex.mines,96,0);n+=96;
 for(int i=0;i<5;i++){int x=option_x(i),y=option_y(i);motion_record75(n+i,0xffffffe0u+i,ex.optionsOn&&i<ex.options,x,y,0,x>tunnel_center(y),9);}
 int x=satellite_x(),y=satellite_y();motion_record75(n+5,0xffffffe5u,ex.satellite,x,y,g.tick*6,x>tunnel_center(y),8);
 x=g.px/Q+((ex.phase&1)?8:0);y=player_y/Q+((ex.phase&1)?0:9);motion_record75(n+6,0xffffffe6u,ex.dropper,x,y,(ex.phase&1)?-90:0,x>tunnel_center(y),7);
}
static MotionTrack75*motion_find75(u32 serial){if(!serial)return NULL;unsigned h=serial&1023;for(int i=0;i<1024&&motionMap75[h];i++,h=(h+1)&1023){MotionTrack75*t=motion75+motionMap75[h]-1;if(t->serial==serial)return t;}return NULL;}
static int motion_pose75(MotionTrack75*t,int age,float alpha,MotionPose75*out){
 if(!t||t->count<age+2)return 0;MotionPose75 a=t->pose[(t->head-age-1+12)%6],b=t->pose[(t->head-age+12)%6];
 float turn=fmodf(b.angle-a.angle+540,360)-180;*out=(MotionPose75){a.x+(b.x-a.x)*alpha,a.y+(b.y-a.y)*alpha,a.angle+turn*alpha};return 1;
}
static int motion_clearance75(MotionTrack75*t,MotionPose75 p){if(!cavern_guard_active()||!t->radius)return 1;float l,r;return cavern_interval(p.y,t->radius,t->side,&l,&r)&&p.x>=l&&p.x<=r;}
static void draw_painted_blur75(void);
static void draw_projectile_blur75(void);
