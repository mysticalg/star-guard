static int giant_period(void);
static float giant_hermite(float p0,float p1,float m0,float m1,float t,float span){float a=t*t,b=a*t;return (2*b-3*a+1)*p0+(b-2*a+t)*m0*span+(-2*b+3*a)*p1+(b-a)*m1*span;}
/* A held, readable aim lock precedes a real hull dive, side exit, hidden return
   and useful re-entry window. The seven Hermite nodes share time derivatives. */
static void giant_motion_point(float clock,int period,int archetype,float goalX,float goalY,int side,float*x,float*y){
 float time[7]={70,110,155,215,240,270,period},px[7]={0,-side*100,goalX,side*320,0,-side*320,0},py[7]={-12,-32,goalY,-210,-340,-240,-12};
 if(archetype==1){px[1]=side*125;py[1]=goalY*.45f-20;}
 if(archetype==2){px[1]=-side*175;py[1]=-70;}
 if(archetype==3){px[1]=goalX;py[1]=-68;}
 if(archetype==5){px[1]=-side*135;py[1]=goalY*.35f-55;}
 if(clock<=70){*x=0;*y=-12;return;}if(clock>=period){*x=0;*y=-12;return;}
 int i=0;while(i<5&&clock>=time[i+1])i++;float span=time[i+1]-time[i],t=(clock-time[i])/span;
 float mx0=i?(px[i+1]-px[i-1])/(time[i+1]-time[i-1]):0,my0=i?(py[i+1]-py[i-1])/(time[i+1]-time[i-1]):0;
 float mx1=i+1<6?(px[i+2]-px[i])/(time[i+2]-time[i]):0,my1=i+1<6?(py[i+2]-py[i])/(time[i+2]-time[i]):0;
 *x=giant_hermite(px[i],px[i+1],mx0,mx1,t,span);*y=giant_hermite(py[i],py[i+1],my0,my1,t,span);
}
static void giant_move_root(void){Giant*b=&ex.giant;if(b->archetype==4){b->rootX=b->rootY=b->rootAngle=0;return;}
 if(b->morph){float t=giant_morph_fraction();b->rootX=b->morphRootX*(1-t);b->rootY=b->morphRootY+(-12-b->morphRootY)*t;b->rootAngle=b->morphRootAngle*(1-t);b->sweepPhase=b->cueTicks=0;return;}
 if(b->timer<120){b->rootX=0;b->rootY=-12*b->timer/120.0f;b->rootAngle=0;b->sweepPhase=b->cueTicks=0;return;}
 int period=giant_period(),cycle=b->formAge/period,c=b->formAge%period;
 if(cycle!=b->sweepCycle||b->sweepForm!=b->form){b->sweepCycle=cycle;b->sweepForm=b->form;b->sweepSide=((cycle+b->archetype+b->form)&1)?1:-1;float scale=giant_world_scale();b->diveX=(clamp(g.px/Q,play_min_x()+28,play_max_x()-28)-160)/scale;b->diveY=(clamp(player_y/Q,play_min_y()+50,play_max_y()-20)-100)/scale-40;}
 b->sweepPhase=c<70?1:c<155?2:c<215?3:c<270?4:5;b->cueTicks=c<70?70-c:0;if(c==70)b->sweepSerial++;
 giant_motion_point(c,period,b->archetype,b->diveX,b->diveY,b->sweepSide,&b->rootX,&b->rootY);
 float xx,yy;giant_motion_point(c+1,period,b->archetype,b->diveX,b->diveY,b->sweepSide,&xx,&yy);float dx=xx-b->rootX,dy=yy-b->rootY,target=0;
 if(c>=70&&c<215&&fabsf(dx)+fabsf(dy)>.01f)target=clamp((int)(-atan2f(dx,dy)*57.2957795f*.32f),b->archetype==3?-8:-18,b->archetype==3?8:18);
 b->rootAngle+=(target-b->rootAngle)*.16f;
}
