#include "giant_hull_masks.h"
static int giant_mask_at(int boss,int form,int x,int y){return x>=0&&x<GIANT_MASK_W&&y>=0&&y<GIANT_MASK_H&&(giantHullMask[boss][form][y][x>>5]&(1u<<(x&31)))!=0;}
static int giant_hull_at(float px,float py,float rootX,float rootY,float angle,float scale){
 Giant*b=&ex.giant;if(b->archetype==4||scale<=0)return 0;
 float a=angle*.01745329252f,c=cosf(a),s=sinf(a),dx=(px-160)/scale-rootX,dy=(py-100)/scale-rootY;
 float lx=dx*c+dy*s,ly=-dx*s+dy*c;lx-=giant_flex_at(ly,b->timer);float u=(lx+176)*GIANT_MASK_W/352,v=(ly+96)*GIANT_MASK_H/192;
 float rx=player_hull_rx(),ry=player_hull_ry(),bx=(rx*fabsf(c)+ry*fabsf(s))/scale*GIANT_MASK_W/352+1,by=(rx*fabsf(s)+ry*fabsf(c))/scale*GIANT_MASK_H/192+1;
 int left=clamp((int)floorf(u-bx),0,GIANT_MASK_W-1),right=clamp((int)ceilf(u+bx),0,GIANT_MASK_W-1),top=clamp((int)floorf(v-by),0,GIANT_MASK_H-1),bottom=clamp((int)ceilf(v+by),0,GIANT_MASK_H-1);
 float cellX=352.0f/GIANT_MASK_W*scale*.5f,cellY=192.0f/GIANT_MASK_H*scale*.5f,exx=fabsf(c)*cellX+fabsf(s)*cellY,eyy=fabsf(s)*cellX+fabsf(c)*cellY;
 for(int y=top;y<=bottom;y++)for(int x=left;x<=right;x++)if(giant_mask_at(b->archetype,b->form,x,y)){
  float xx=(x+.5f)*352/GIANT_MASK_W-176,yy=(y+.5f)*192/GIANT_MASK_H-96;
  xx+=giant_flex_at(yy,b->timer);float wx=160+(rootX+xx*c-yy*s)*scale,wy=100+(rootY+xx*s+yy*c)*scale;
  float qx=fmaxf(0,fabsf(wx-px)-exx)/rx,qy=fmaxf(0,fabsf(wy-py)-eyy)/ry;
  if(qx*qx+qy*qy<=1)return 1;
 }return 0;
}
static int giant_hull_contact(void){
 Giant*b=&ex.giant;if(b->archetype==4)return 0;float scale=giant_world_scale(),oldScale=b->previousScale>0?b->previousScale:scale;
 float distance=hypotf((b->rootX-b->previousRootX)*scale-(g.px-ex.damagePrevX)/(float)Q,(b->rootY-b->previousRootY)*scale-(player_y-ex.damagePrevY)/(float)Q)+fabsf(b->rootAngle-b->previousRootAngle)*.01745329252f*176*scale+fabsf(scale-oldScale)*176;
 int steps=clamp((int)ceilf(distance/4)+1,1,256);
 for(int i=0;i<=steps;i++){float t=i/(float)steps,px=(ex.damagePrevX+(g.px-ex.damagePrevX)*t)/Q,py=(ex.damagePrevY+(player_y-ex.damagePrevY)*t)/Q;
  if(giant_hull_at(px,py,b->previousRootX+(b->rootX-b->previousRootX)*t,b->previousRootY+(b->rootY-b->previousRootY)*t,b->previousRootAngle+(b->rootAngle-b->previousRootAngle)*t,oldScale+(scale-oldScale)*t))return 1;
 }return 0;
}
static int giant_part_in_view(int n){GiantPart*p=ex.giant.parts+n;return p->x+p->w/2>=play_min_x()&&p->x-p->w/2<=play_max_x()&&p->y+p->h/2>=play_min_y()&&p->y-p->h/2<=play_max_y();}
static int giant_box_sweep(float x,float y,float xx,float yy,float rx,float ry){float dx=xx-x,dy=yy-y,lo=0,hi=1;for(int axis=0;axis<2;axis++){float a=axis?y:x,d=axis?dy:dx,r=axis?ry:rx;if(fabsf(d)<.000001f){if(a< -r||a>r)return 0;}else{float u=(-r-a)/d,v=(r-a)/d;if(u>v){float swap=u;u=v;v=swap;}lo=fmaxf(lo,u);hi=fminf(hi,v);if(lo>hi)return 0;}}return 1;}
static int giant_projectile_contact(const Obj*s,int n){Giant*b=&ex.giant;GiantPart*p=b->parts+n;float x=s->age?s->previousX/(float)Q:s->x/(float)Q,y=s->age?s->previousY/(float)Q:s->y/(float)Q;return giant_box_sweep(x-b->previousPartX[n],y-b->previousPartY[n],s->x/(float)Q-p->x,s->y/(float)Q-p->y,p->w*.5f+3,p->h*.5f+3);}

#include "giant_module_contact.h"
