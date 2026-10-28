/* Render-only scenery motion. Advance once per simulation tick; never alter
   the game's clocks, actor velocities, colliders or attack timing. */
typedef struct{int ready,mission,tick,boss;double distance,previous;float speed,zoom,mix,damage;} SceneryMotion74;
static SceneryMotion74 scenery74;
static int sceneryProbe74,sceneryTiles74;
static float sceneryAspect74,scenerySourceAspect74;
static float boss_damage74(void){Giant*b=&ex.giant;double hp=0,total=0;for(int n=0;n<9;n++){int born=n==6||(b->archetype!=4&&(n<4||(n<6?b->form>=1:b->form>=2)));if(born&&b->parts[n].maxHp>0){total+=b->parts[n].maxHp;hp+=fmaxf(0,b->parts[n].hp);}}float lost=total>0?1-hp/total:0;return fminf(1,fmaxf(0,b->form/3.0f+lost/3.0f));}
static void scenery_step74(void){
 SceneryMotion74*s=&scenery74;float base=(cavern_phase(ex.phase)?14:250)*environment_rate();
 if(!s->ready||s->mission!=g.wave||g.tick<s->tick){*s=(SceneryMotion74){.ready=1,.mission=g.wave,.tick=g.tick-1,.speed=base,.zoom=journeyZoom>0?journeyZoom:1};}
 int steps=clamp(g.tick-s->tick,0,5);s->tick=g.tick;
 if(g.mode!=1||g.paused||ex.refit){s->previous=s->distance;return;}
 if(ex.giant.active&&!s->boss)s->damage=0;s->boss=ex.giant.active;
 if(s->boss)s->damage=fmaxf(s->damage,boss_damage74());
 for(int n=0;n<steps;n++){
  float targetZoom=s->boss&&ex.giant.zoom>0?ex.giant.zoom:1;
  s->zoom+=(targetZoom-s->zoom)*.027612f;s->mix+=((s->boss?1:0)-s->mix)*.04f;
  if(s->mix<.0001f)s->mix=0;if(s->mix>.9999f)s->mix=1;
  float target=s->boss?260+380*s->damage:base;
  s->speed+=(target-s->speed)*.04f;s->previous=s->distance;
  s->distance+=s->speed*.02/fmaxf(.2f,s->zoom);
 }
}
static float scenery_distance74(void){float t=g.tick-previous.tick;float alpha=t?fminf(1,fmaxf(0,(visualTick-previous.tick)/t)):1;return (float)(scenery74.previous+(scenery74.distance-scenery74.previous)*alpha);}
static void scenery_point74(float*x,float*y,float zoom,float angle){float xx=(*x-480)*zoom,yy=(*y-420)*zoom;*x=480+xx*cosf(angle)-yy*sinf(angle);*y=420+xx*sinf(angle)+yy*cosf(angle);}
static void draw_scene_tiles74(int plate,float opacity,float distance,float z,float forcedHeight){
 if(sceneryProbe74||opacity<=0)return;SDL_Texture*t=journeyPlates;float tw,th,u,v,uw,vh;
 if(plate>=0){SDL_GetTextureSize(t,&tw,&th);u=(plate%3)/3.0f+4/tw;v=(plate/3)/2.0f+4/th;uw=1/3.0f-8/tw;vh=1/2.0f-8/th;}
 else{int p=ex.phase,index=p==2?9:p==15?10:p==4?8:p>=12?0:p>=6?p-4:0;t=scenes[index];SDL_GetTextureSize(t,&tw,&th);u=3/tw;v=3/th;uw=1-6/tw;vh=1-6/th;}
 /* Every source texel gets the same world scale on both axes. Expanded arenas
    reveal additional mirrored tiles; neither side flight nor zoom stretches UVs. */
 float scale=forcedHeight>0?forcedHeight/(th*vh):1.4f;float tileW=tw*uw*scale,tileH=th*vh*scale,a=flight_view_angle();z=fmaxf(.2f,z);
 float scroll=wrap_positive(distance,tileH);int offset=(int)floorf(distance/tileH);
 int nx=(int)ceilf((480*fabsf(cosf(a))+348*fabsf(sinf(a)))/(z*tileW))+2;
 int ny=(int)ceilf((480*fabsf(sinf(a))+348*fabsf(cosf(a)))/(z*tileH))+2;
 scenerySourceAspect74=tw*uw/(th*vh);sceneryAspect74=tileW/tileH;sceneryTiles74=0;nquad=0;
 SDL_SetTextureColorMod(t,255,255,255);SDL_SetTextureAlphaMod(t,255);
 for(int row=-ny;row<=ny;row++)for(int col=-nx;col<=nx;col++){
  float x=480+(col-.5f)*tileW,y=420+(row-.5f)*tileH+scroll;
  float uu=u,vv=v,du=uw,dv=vh;if(col&1){uu+=du;du=-du;}if((row-offset)&1){vv+=dv;dv=-dv;}
  int first=nquad;quad(x,y,tileW,tileH,(SDL_FColor){.7f,.75f,.8f,opacity},uu,vv,du,dv);
  for(int n=first*4;n<nquad*4;n++)scenery_point74(&vertices[n].position.x,&vertices[n].position.y,z,a);
  sceneryTiles74++;
 }
 lastGroundDX=-sinf(a)*distance*z;lastGroundDY=cosf(a)*distance*z;flush(t);
}

static void draw_boss_scenery74(int plate){draw_scene_tiles74(plate,scenery74.mix,scenery_distance74(),journeyZoom,0);}
