/* Cosmetic weather is sampled from fixed simulation time, never frame count. */
typedef struct{float x,y,dx,dy,length,width,alpha,speed;} RainDrop;
static int renderedRain;
static float rainClockSample,rainFirstX,rainFirstY,rainDirectionX,rainDirectionY;
static void rain_drop_at(int i,float seconds,float angle,int reduced,RainDrop*d){
 unsigned h=(unsigned)(i+1)*2246822519u;int layer=(h>>6)%3;
 float depth=layer/2.0f,variation=((h>>16)&255)/255.0f;
 float speed=(360+layer*330+variation*120)*(reduced?.35f:1);
 float wind=(-65+18*sinf(seconds*.43f))*(.65f+depth*.35f)*(reduced?.35f:1);
 float windShift=(-65*seconds+18/.43f*(1-cosf(seconds*.43f)))*(.65f+depth*.35f)*(reduced?.35f:1);
 float wx=wrap_positive((h%10007)/10007.0f*1400+windShift,1400)-700;
 float wy=wrap_positive(((h>>10)%10007)/10007.0f*1400+seconds*speed,1400)-700;
 float c=cosf(angle),s=sinf(angle),dx=wind*c-speed*s,dy=wind*s+speed*c,len=sqrtf(dx*dx+dy*dy);
 *d=(RainDrop){.x=480+wx*c-wy*s,.y=420+wx*s+wy*c,.dx=dx/len,.dy=dy/len,
  .length=(5+layer*7+variation*7)*(reduced?.65f:1),.width=.65f+depth*.8f+variation*.3f,
  .alpha=(.14f+depth*.16f+variation*.10f)*(reduced?.6f:1),.speed=speed};
}
static void draw_storm_rain(void){
 renderedRain=0;int kind=journey_kind();float strength=kind==ENV_OCEAN?.2f:kind==ENV_SKY&&ex.phase==10?.1f+.1f*earth_cloud_cover():0;
 if(ex.env.weather.kind==WEATHER_STORM&&ex.env.weather.stage==2)strength=1.35f;
 if(!strength||!g.fx)return;
 int count=(int)((quality==2?720:quality==1?420:220)*fminf(1,.25f+strength*.6f));if(reducedMotion)count/=3;
 float clock=visualTick*.02f,angle=flight_view_angle();rainClockSample=clock;
 SDL_Rect clip={0,72,960,696};SDL_SetRenderClipRect(renderer,&clip);
 for(int pass=0;pass<2;pass++){
  SDL_SetTextureBlendMode(halo,pass?SDL_BLENDMODE_BLEND:SDL_BLENDMODE_ADD);nquad=0;
  for(int i=0;i<count;i++){RainDrop d;rain_drop_at(i,clock,angle,reducedMotion,&d);
   if(d.x<-30||d.x>990||d.y<42||d.y>798)continue;
   if(!pass){if(!renderedRain){rainFirstX=d.x;rainFirstY=d.y;rainDirectionX=d.dx;rainDirectionY=d.dy;}renderedRain++;}
   float width=d.width*(pass?1:2.8f),alpha=d.alpha*strength*(pass?1:.16f);
   warp_ribbon(d.x,d.y,d.dx,d.dy,d.length,width,(SDL_FColor){.66f,.81f,.94f,alpha});
  }flush(halo);
 }
 SDL_SetTextureBlendMode(halo,SDL_BLENDMODE_ADD);SDL_SetRenderClipRect(renderer,0);
}
