#include "ocean_profile70.h"
/* Three readable but unequal crest intervals, heights and widths. */
static float ocean_clock71(float ticks,int*cycle){static const float length[3]={600,720,540};float t=fmodf(fmaxf(0,ticks),1860);int n=0;while(n<2&&t>=length[n]){t-=length[n];n++;}if(cycle)*cycle=n;return t*600/length[n];}
static float ocean_height71(float ticks){static const float heights[3]={190,165,205};int n;ocean_clock71(ticks,&n);return heights[n];}
/* Logical x becomes screen height in side flight; logical y runs right-to-left. */
static float ocean_phase70(float ticks){return fminf(350,fmaxf(0,ocean_clock71(ticks,0)-200))*3.0f/350;}
static float ocean_crest_y70(float ticks){return -100+(ocean_clock71(ticks,0)-200)*460.0f/350;}
static float ocean_strength70(float ticks){float c=ocean_clock71(ticks,0),up=fminf(60,fmaxf(0,c-170))/60,down=fminf(70,fmaxf(0,570-c))/70;return up*down;}
static float ocean_base70(float y,float ticks){return 280+18*sinf(y*.012f+ticks*.012f)+8*sinf(y*.026f-ticks*.006f);}
static float ocean_width70(float ticks){static const int widths[3]={340,480,620};int n;ocean_clock71(ticks,&n);return widths[n];}
static float ocean_edge71(float u){float a=fminf(1,fmaxf(0,u*10)),b=fminf(1,fmaxf(0,(1-u)*10));return a*a*(3-2*a)*b*b*(3-2*b);}
static float ocean_top70(float worldY,float ticks){float width=ocean_width70(ticks),u=(ocean_crest_y70(ticks)+width*.5f-worldY)/width,base=ocean_base70(worldY,ticks);if(u<0||u>1)return base;float frame=ocean_phase70(ticks),f=frame-(int)frame,x=u*63;int a=clamp((int)frame,0,3),b=clamp(a+1,0,3),j=clamp((int)x,0,62);float t=x-j,one=oceanProfile70[a][j]*(1-t)+oceanProfile70[a][j+1]*t,two=oceanProfile70[b][j]*(1-t)+oceanProfile70[b][j+1]*t;return base-ocean_height71(ticks)*ocean_strength70(ticks)*ocean_edge71(u)*(one+(two-one)*f)/100;}
static int ocean_warning70(int ticks){int c=(int)ocean_clock71(ticks,0);return c>=100&&c<200?7:c>=200&&c<570?22:0;}
