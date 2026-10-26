/* Fixed topology shared by strike collision, forecast and rendering. The32-unit bright
   trunk is wider than the16-unit collision hull. Corona/blue branches are cosmetic. */
#define STORM_BOLT_RADIUS 16
#define STORM_BOLT_SEGMENTS 12
static float storm_bolt_x(int x,int seed,int n){static const int bend[13]={0,-7,9,-10,5,-3,10,-8,4,-10,8,-4,0};return x+bend[(n+seed%13)%13];}
static float storm_bolt_y(int n){return play_min_y()-32+(play_max_y()-play_min_y()+64)*n/(float)STORM_BOLT_SEGMENTS;}
static int storm_bolt_contact(float x,float y,float oldx,float oldy,int center,int seed,float radius){
 for(int n=0;n<STORM_BOLT_SEGMENTS;n++){float ax=storm_bolt_x(center,seed,n),ay=storm_bolt_y(n),dx=storm_bolt_x(center,seed,n+1)-ax,dy=storm_bolt_y(n+1)-ay,d=dx*dx+dy*dy;
  for(int step=0;step<=4;step++){float px=oldx+(x-oldx)*step/4,py=oldy+(y-oldy)*step/4,t=d>0?fmaxf(0,fminf(1,((px-ax)*dx+(py-ay)*dy)/d)):0,xx=px-ax-dx*t,yy=py-ay-dy*t;if(xx*xx+yy*yy<=radius*radius)return 1;}}
 return 0;
}
