typedef struct{int tick,wave,section,amount;float x,y,bend;} SiphonReturn84;
static SiphonReturn84 siphonReturns84[64];static unsigned siphonReturnHead84;
static void pc_siphon_sound84(int,int,int,int);
void pc_siphon_reset84(void){memset(siphonReturns84,0,sizeof(siphonReturns84));siphonReturnHead84=0;}
void pc_siphon_shift84(int gap){for(int i=0;i<64;i++)if(siphonReturns84[i].amount)siphonReturns84[i].tick+=gap;}
void pc_siphon_event84(int kind,int x,int y,int amount){pc_siphon_sound84(kind,x,y,amount);if(kind!=1||amount<=0||!g.fx)return;int count=reducedMotion?1:clamp(amount*3,3,12);for(int n=0;n<count;n++){unsigned serial=siphonReturnHead84++;SiphonReturn84*p=siphonReturns84+serial%64;*p=(SiphonReturn84){g.tick-(n%3),g.wave,ex.section,amount,x,y,((int)(serial%7)-3)*3.f};}}
static int siphon_return_point84(const SiphonReturn84*p,float t,float*x,float*y){if(!p->amount||p->wave!=g.wave||p->section!=ex.section||t<0||t>=1)return 0;float smooth=t*t*(3-2*t),px=g.px/(float)Q,py=player_y/(float)Q,dx=px-p->x,dy=py-p->y,len=fmaxf(1,hypotf(dx,dy)),bend=sinf(t*PI)*p->bend;*x=p->x+dx*smooth-dy/len*bend;*y=p->y+dy*smooth+dx/len*bend;return 1;}
