/* Existing atlas opaque-foot/muzzle metrics; native artwork is preserved. */
static int floor_enemy(const Obj*o){return o->type==9||o->type==23||o->type==24;}
static int floor_art(const Obj*o){return organic_context()?(o->type==9?139:140):o->type==23?63:64;}
static float floor_depth(const Obj*o){return organic_context()?14:o->type==23?5.2109375f:10.08203125f;}
static int floor_side(const Obj*o){return o->type==9&&(o->phase&1)?-1:1;}
static float floor_surface_side(float y,int side){if(!tunnel_visible((int)y))return side>0?288:32;int lo=(int)floorf(y/2)*2;float f=(y-lo)/2;int a=side>0?tunnel_right(lo):tunnel_left(lo),b=side>0?tunnel_right(lo+2):tunnel_left(lo+2);return a+(b-a)*f;}
static void floor_mount_step(Obj*o){if(!o->floorMounted){o->floorY=o->y;o->floorMounted=1;}float y=o->floorY/(float)Q;int side=floor_side(o),cave=tunnel_visible((int)y);o->floorY+=cave?ex.scrollSpeed>>2:o->type==23?400:o->type==24?300:340;y=o->floorY/(float)Q;float slope=cave?(floor_surface_side(y+4,side)-floor_surface_side(y-4,side))/8:0,angle=-side*1.570796327f-atanf(slope),depth=floor_depth(o);o->heading=(int)(angle*916.732472f);o->x=(int)((floor_surface_side(y,side)+depth*sinf(angle))*Q);o->y=(int)((y-depth*cosf(angle))*Q);}
static void floor_muzzle(const Obj*o,int*x,int*y){float a=o->heading/916.732472f,c=cosf(a),s=sinf(a),lx=organic_context()?0:o->type==23?-14.171371f:-9.53125f,ly=organic_context()?-14:o->type==23?-5.747102f:-10.062612f;*x=o->x+(int)((lx*c-ly*s)*Q);*y=o->y+(int)((lx*s+ly*c)*Q);}
static int floor_shot_hit(const Obj*o,int x,int y){float a=o->heading/916.732472f,c=cosf(a),s=sinf(a),dx=(x-o->x)/(float)Q,dy=(y-o->y)/(float)Q,lx=dx*c+dy*s,ly=-dx*s+dy*c;if(organic_context())return lx*lx/(14*14)+ly*ly/(16*16)<=1;return o->type==23?lx>=-17.91f&&lx<=18.75f&&ly>=-8.93f&&ly<=8.21f:lx>=-14.38f&&lx<=15.15f&&ly>=-13.31f&&ly<=13.09f;}

static void floor_fire(Obj*o,int speed){int x,y;floor_muzzle(o,&x,&y);ex.floorShotSerial++;ex.floorShotX=x;ex.floorShotY=y;aim(x,y,speed);}
