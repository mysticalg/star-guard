#define GOO_CAPACITY 384
typedef struct{float x,y,vx,vy,age,life,size,angle,spin;int cell,blood;} GooDrop;
static GooDrop goo[GOO_CAPACITY];
static float gooSourceDiameter,gooSourceVX,gooSourceVY,gooLastDiameter;
void pc_organic_source(int diameter,int vx,int vy){gooSourceDiameter=diameter;gooSourceVX=vx;gooSourceVY=vy;}
static int gooHead,gooDeathSerial,gooLastX,gooLastY,gooLastBlood;
void pc_organic_reset(void){memset(goo,0,sizeof(goo));gooHead=gooDeathSerial=gooLastBlood=0;gooSourceDiameter=gooSourceVX=gooSourceVY=gooLastDiameter=0;}
static void goo_emit(float x,float y,float vx,float vy,float life,float size,int cell,int blood){GooDrop*p=goo+gooHead;gooHead=(gooHead+1)%GOO_CAPACITY;*p=(GooDrop){x,y,vx,vy,0,life,size,frand()*360,(frand()-.5f)*160,cell,blood};}
void pc_organic_burst(int x,int y,int big,int blood){
 gooDeathSerial++;gooLastX=x;gooLastY=y;gooLastBlood=blood;if(!g.fx)return;
 float diameter=big<0?6:gooSourceDiameter>0?fmaxf(8,fminf(150,gooSourceDiameter)):big?48:20;gooLastDiameter=diameter;
 int count=big<0?4:(quality==2?14:quality==1?10:7)+(big>0?8:0);if(reducedMotion)count=count/2+1;
 /* A compact rupture, tiny wet residue and mist, never an enlarged flat stamp. */
 goo_emit(x,y,0,0,.48f,diameter*.18f,0,blood);
 goo_emit(x,y,0,0,.20f,diameter*.30f,2,blood);
 goo_emit(x,y,0,0,.09f,diameter*.14f,3,blood);
 float direction=hypotf(gooSourceVX,gooSourceVY)>2?atan2f(gooSourceVY,gooSourceVX):-PI*.5f;
 for(int i=0;i<count;i++){float a=i%4?direction+(frand()-.5f)*2.5f:frand()*2*PI,speed=fminf(90,diameter*(.65f+frand()*1.2f))*(reducedMotion?.55f:1);
  goo_emit(x+(frand()-.5f)*diameter*.16f,y+(frand()-.5f)*diameter*.16f,cosf(a)*speed+gooSourceVX*.15f,sinf(a)*speed+gooSourceVY*.15f,.20f+frand()*.34f,diameter*(.022f+frand()*.035f),i%4?1:3,blood);
  GooDrop*p=goo+(gooHead+GOO_CAPACITY-1)%GOO_CAPACITY;p->angle=a*180/PI-90;p->spin=(frand()-.5f)*35;
 }

}
static void goo_step(float dt){for(int i=0;i<GOO_CAPACITY;i++){GooDrop*p=goo+i;if(p->life<=0)continue;float step=fminf(dt,p->life-p->age),gravity=p->cell==1||p->cell==3?22:0;p->x+=p->vx*step;p->y+=p->vy*step+.5f*gravity*step*step;p->vy+=gravity*step;p->angle+=p->spin*step;p->age+=step;if(p->age>=p->life-.00001f)p->life=0;}}
