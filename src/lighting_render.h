/* Bounded GPU light field. MOD reveals original world texture under actual sources.
   No readback, render-time RNG/emission or per-frame allocation. HUD stays outside. */
#define WORLD_LIGHT_MAX 240
static SDL_Texture *worldLightMasks[3];
static int worldLightCount,worldLightDropped;
static float worldLampStrength;
typedef struct {float x,y,r;SDL_FColor colour;} WorldLight;
static WorldLight worldLights[WORLD_LIGHT_MAX];
static void lighting_init(void){for(int i=0;i<3;i++){worldLightMasks[i]=SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_TARGET,120+60*i,100+50*i);if(worldLightMasks[i]){SDL_SetTextureScaleMode(worldLightMasks[i],SDL_SCALEMODE_LINEAR);SDL_SetTextureBlendMode(worldLightMasks[i],SDL_BLENDMODE_MOD);}}}
static void world_light(float x,float y,float r,SDL_FColor c){if(worldLightCount==WORLD_LIGHT_MAX){worldLightDropped++;return;}worldLights[worldLightCount++]=(WorldLight){x,y,r,c};}
static void world_light_point(float *x,float *y){*x=480+(*x*3-480)*journeyZoom;*y=420+(*y*3-420)*journeyZoom;flight_screen_point(x,y);}
static void lighting_sources(void){
 worldLightCount=worldLightDropped=0;float ambient=ex.lightReady?ex.lightAmbient:lighting_target();
 worldLampStrength=fmaxf(0,fminf(1,(.75f-ambient)/.45f));
 float x=g.px/(float)Q,y=player_y/(float)Q;
 static const float ahead[5]={0,18,50,90,140},radius[5]={24,18,25,32,40},strength[5]={1.8f,1.7f,1.6f,1.4f,1.1f};
 for(int i=0;i<5;i++)world_light(x,y-ahead[i],radius[i],(SDL_FColor){.88f,.95f,1,strength[i]*worldLampStrength});
 /* Critical hostile/player visibility has priority over optional explosions. */
 for(int i=0;i<MAXB+MAXS;i++){Obj *o=i<MAXB?g.bullets+i:g.shots+i-MAXB;if(!o->on)continue;SDL_FColor c=projectile_colour(o,i<MAXB);c.r=.45f+.55f*c.r;c.g=.45f+.55f*c.g;c.b=.45f+.55f*c.b;c.a=2.6f;world_light(o->x/(float)Q,o->y/(float)Q,i<MAXB?15:23,c);}
 if(g.beam){SDL_FColor c=player_beam_colour();c.r=.5f+.5f*c.r;c.g=.5f+.5f*c.g;c.b=.5f+.5f*c.b;c.a=1.8f;for(int lane=ex.multi?-1:0;lane<=(ex.multi?1:0);lane++){float tip=terrain_beam_tip(lane),end=y-8;for(int i=0;i<6;i++){float yy=tip+(end-tip)*(i+.5f)/6;world_light(beam_center(lane,(int)yy),yy,fmaxf(26,(end-tip)/6),c);}}}

 /* Actual Nova/EMP expansion also reveals surfaces at its moving blast front. */
 if(ex.bombAge){float r=ex.bombRadius,fade=fminf(1,(61-ex.bombAge)/10.0f);world_light(ex.bombX,ex.bombY,25+r*.4f,(SDL_FColor){.45f,.75f,1,fade});for(int i=0;i<8;i++){float a=i*PI/4;world_light(ex.bombX+cosf(a)*r,ex.bombY+sinf(a)*r*.8f,27,(SDL_FColor){.4f,.7f,1,fade*(reducedMotion?.8f:1.6f)});}}
 if(ex.empPulse)world_light(ex.empX/(float)Q,ex.empY/(float)Q,25+(22-ex.empPulse)*6,(SDL_FColor){.45f,.7f,1,ex.empPulse/22.0f});
 Weather *weather=&ex.env.weather;
 if(weather->stage==2&&(weather->kind==WEATHER_STORM||weather->kind==WEATHER_ION)){int clock=lightning_clock()%lightning_period();if(clock>=35&&clock<53){int seed=lightning_seed(),center=weather->safeX+((seed&1)?100:-100);for(int i=0;i<=STORM_BOLT_SEGMENTS;i+=2)world_light(storm_bolt_x(center,seed,i),storm_bolt_y(i),65,(SDL_FColor){.7f,.85f,1,reducedMotion?.8f:1.8f});}}
 for(int i=0;i<64&&worldLightCount<WORLD_LIGHT_MAX;i++){Wave *w=waves+i;if(w->life<=0||w->age>=w->life)continue;float fade=1-w->age/w->life;world_light(w->x,w->y,18+w->radius*.48f,(SDL_FColor){1,.72f,.36f,fade*(reducedMotion?1:2)});}
}
static void lighting_apply(int cameraActive){
 lighting_sources();SDL_Texture *mask=worldLightMasks[clamp(quality,0,2)];if(!mask)return;
 SDL_Texture *target=SDL_GetRenderTarget(renderer);SDL_SetRenderTarget(renderer,mask);SDL_SetRenderLogicalPresentation(renderer,960,800,SDL_LOGICAL_PRESENTATION_STRETCH);
 float ambient=ex.lightReady?ex.lightAmbient:lighting_target();int a=(int)(255*ambient);SDL_SetRenderDrawColor(renderer,a,a,a,255);SDL_RenderClear(renderer);
 SDL_SetTextureBlendMode(halo,SDL_BLENDMODE_ADD);nquad=0;
 for(int i=0;i<worldLightCount;i++){WorldLight *l=worldLights+i;if(l->colour.a<=0)continue;int first=nquad;quad(0,0,1,1,l->colour,0,0,1,1);float xx[4]={l->x-l->r,l->x+l->r,l->x+l->r,l->x-l->r},yy[4]={l->y-l->r,l->y-l->r,l->y+l->r,l->y+l->r};for(int j=0;j<4;j++){world_light_point(xx+j,yy+j);vertices[first*4+j].position=(SDL_FPoint){xx[j],yy[j]};}}
 flush(halo);SDL_SetTextureBlendMode(halo,SDL_BLENDMODE_ADD);SDL_SetRenderTarget(renderer,target);SDL_SetRenderLogicalPresentation(renderer,960,800,cameraActive?SDL_LOGICAL_PRESENTATION_STRETCH:SDL_LOGICAL_PRESENTATION_LETTERBOX);
 SDL_Rect clip={0,72,960,696};SDL_SetRenderClipRect(renderer,&clip);SDL_RenderTexture(renderer,mask,0,0);SDL_SetRenderClipRect(renderer,0);
}
