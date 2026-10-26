#define GOO_CAPACITY 384
typedef struct{float x,y,vx,vy,age,life,size,angle,spin;int cell,blood;} GooDrop;
static GooDrop goo[GOO_CAPACITY];
static int gooHead,gooDeathSerial,gooLastX,gooLastY,gooLastBlood;
void pc_organic_reset(void){memset(goo,0,sizeof(goo));gooHead=gooDeathSerial=gooLastBlood=0;}
static void goo_emit(float x,float y,float vx,float vy,float life,float size,int cell,int blood){GooDrop*p=goo+gooHead;gooHead=(gooHead+1)%GOO_CAPACITY;*p=(GooDrop){x,y,vx,vy,0,life,size,frand()*360,(frand()-.5f)*160,cell,blood};}
void pc_organic_burst(int x,int y,int big,int blood){
 gooDeathSerial++;gooLastX=x;gooLastY=y;gooLastBlood=blood;if(!g.fx)return;
 float scale=big<0?.35f:big?1.3f:1;int count=big<0?4:(quality==2?18:quality==1?11:7)*(big?2:1);if(reducedMotion)count=count/2+1;
 goo_emit(x,y,0,0,big>0?1.5f:1.1f,16*scale,0,blood);
 goo_emit(x,y,0,0,.38f,21*scale,2,blood);
 for(int i=0;i<count;i++){float a=frand()*2*PI,speed=(18+frand()*65)*scale*(reducedMotion?.5f:1);goo_emit(x,y,cosf(a)*speed,sinf(a)*speed,.45f+frand()*.7f,(1.3f+frand()*2.5f)*scale,i%5?1:3,blood);}
}
static void goo_step(float dt){for(int i=0;i<GOO_CAPACITY;i++){GooDrop*p=goo+i;if(p->life<=0)continue;float step=fminf(dt,p->life-p->age),gravity=p->cell==1||p->cell==3?22:0;p->x+=p->vx*step;p->y+=p->vy*step+.5f*gravity*step*step;p->vy+=gravity*step;p->angle+=p->spin*step;p->age+=step;if(p->age>=p->life-.00001f)p->life=0;}}
