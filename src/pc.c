#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <emscripten.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "game.c"
#define PI 3.14159265358979323846f
#define NP 12288
#define NQ (NP+4096)
static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Texture *base,*atlas,*halo,*fxTarget;
static SDL_AudioStream *astream;
static unsigned char fb[81920];
static uint32_t rgba[81920],palette[256];
typedef struct{float x,y,w,h,u,v,uw,vh;} Sprite;
static Sprite sprites[40];
typedef struct{int n;float x,y,angle;} Draw;
static Draw draws[256];static int ndraw;
static SDL_Vertex vertices[NQ*4];static int indices[NQ*6],nquad;
static int muted,quality=2,headless,mouseControl,fullscreen,vsync=0,cap=120,showStats=1;
static unsigned int fxseed=1234567;
static float frand(void){fxseed=fxseed*1664525u+1013904223u;return (fxseed>>8)*(1.0f/16777216.0f);}
typedef struct{float x,y,px,py,vx,vy,age,life,size,r,g,b,spin;} Spark;
static Spark sparks[NP];static int sparkHead,aliveParticles,peakParticles;
typedef struct{float x,y,age,life,radius;int big;} Wave;
static Wave waves[64];static int waveHead;
static float lastExplosionX=160;
static float intensity(void){return fminf(6.0f,1.0f+(g.wave-1)*.2f+(ex.phase==3?.5f:0)+(g.bossPhase>1?.5f:0));}
static void emit(float x,float y,float vx,float vy,float life,float size,int tint){Spark*p=sparks+sparkHead;sparkHead=(sparkHead+1)%NP;static const float c[5][3]={{.12f,.45f,1},{1,.3f,.04f},{.7f,.12f,1},{.1f,.85f,1},{.35f,1,.08f}};*p=(Spark){x,y,x,y,vx,vy,0,life,size,c[tint<0?0:tint>4?4:tint][0],c[tint<0?0:tint>4?4:tint][1],c[tint<0?0:tint>4?4:tint][2],(frand()-.5f)*5};}
void pc_burst(int x,int y,int big){lastExplosionX=(float)x;if(!g.fx)return;int n=(int)((big?330:100)*intensity()*(quality==2?1:quality==1?.5f:.15f));for(int i=0;i<n;i++){float a=frand()*2*PI,s=8+frand()*(big?130:65);emit(x,y,cosf(a)*s,sinf(a)*s,.25f+frand()*(big?1.8f:.9f),.35f+frand()*2.5f,(i&7)?1:3);}Wave*w=waves+waveHead;waveHead=(waveHead+1)%64;*w=(Wave){x,y,0,big?1.4f:.65f,big?130:60,big};}
void pc_bomb_start(int x,int y){for(int i=0;i<480;i++){float a=frand()*2*PI,s=25+frand()*180;emit(x,y,cosf(a)*s,sinf(a)*s*.8f,.3f+frand()*.8f,.6f+frand()*2,3);}}
void pc_impact(int x,int y,int tint){if(!g.fx)return;int n=(int)((quality==2?18:7)*intensity());for(int i=0;i<n;i++){float a=frand()*2*PI,s=10+frand()*55;emit(x,y,cosf(a)*s,sinf(a)*s,.12f+frand()*.35f,.4f+frand(),tint);}}
void pc_sprite(int n,float x,float y){if(ndraw<256&&n>=0&&n<40)draws[ndraw++]=(Draw){n,x,y};}
void pc_enemy_sprite(int n,Obj*o,float x,float y){if(ndraw<256&&n>=0&&n<40)draws[ndraw++]=(Draw){n,x,y,o->heading/16.0f-(o->type>=10&&o->type<14?0:180)};}
void sprite_plot(int n,int x,int y){pc_sprite(n,(float)x,(float)y);}
void rowcopy(void*d,const void*s,int n){memcpy(d,s,(size_t)n*4);}
void sprite_bank(void*p){(void)p;}
/* Single-producer/single-consumer audio commands. Only the audio callback touches voices. */
typedef struct{int op,ch,kind,vol,pitch,type,target,duration;float pan;} Command;
static Command commands[256];static SDL_AtomicInt commandRead,commandWrite,audioFrames,audioErrors,audioDrops;
static float audioPeak;static int audioNonzero;
typedef struct{int on,ch,kind;float age,life,phase,phase2,freq,target,slide,amp,volTarget,volSlide,pan,noise;} Voice;
static Voice voices[48];static int voiceHead;static unsigned int noiseSeed=0x8b931;
static float pitch_hz(int p){return 720.0f*powf(2.0f,(p-0x3800)/4096.0f);}
static void enqueue(Command c){if(headless)return;int w=SDL_GetAtomicInt(&commandWrite),next=(w+1)&255;if(next==SDL_GetAtomicInt(&commandRead)){SDL_AddAtomicInt(&audioDrops,1);return;}commands[w]=c;SDL_SetAtomicInt(&commandWrite,next);}
void sound(int ch,int sample,int volume,int pitch){int kind=sample==2?2:(g.weapon==3||g.weapon==5)?3:1;enqueue((Command){.op=0,.ch=ch,.kind=kind,.vol=volume,.pitch=pitch,.pan=((sample==2?lastExplosionX:g.px/256.0f)-160)/220});}
void pc_transition(void){enqueue((Command){.op=0,.ch=9,.kind=4,.vol=85,.pitch=0x2200,.pan=0});}
void sound_slide(int ch,int type,int target,int duration){enqueue((Command){.op=1,.ch=ch,.type=type,.target=target,.duration=duration});}
void sound_mod(int ch,int volume,int pitch){enqueue((Command){.op=2,.ch=ch,.vol=volume,.pitch=pitch});}
static void audio_commands(void){int r=SDL_GetAtomicInt(&commandRead),w=SDL_GetAtomicInt(&commandWrite);while(r!=w){Command c=commands[r];r=(r+1)&255;
 if(c.op==0){if(!c.vol){for(int i=0;i<48;i++)if(voices[i].ch==c.ch)voices[i].on=0;continue;}int chosen=voiceHead++%48;for(int skip=0;skip<48&&voices[chosen].on&&voices[chosen].kind==4;skip++)chosen=voiceHead++%48;Voice*v=voices+chosen;float hz=pitch_hz(c.pitch);*v=(Voice){.on=1,.ch=c.ch,.kind=c.kind,.life=c.kind==4?4.3f:c.kind==2?1.3f:c.kind==3?.45f:.23f,.freq=hz,.target=hz,.amp=c.vol/127.0f*.17f,.pan=fmaxf(-.85f,fminf(.85f,c.pan))};}
 else{for(int j=0;j<48;j++){Voice*v=voices+(voiceHead-1-j+96)%48;if(!v->on||v->ch!=c.ch)continue;if(c.op==2){v->target=pitch_hz(c.pitch);v->slide=.025f;v->life=fmaxf(v->life,v->age+.08f);}else if(c.type){v->target=pitch_hz(c.target);v->slide=fmaxf(.01f,c.duration*.008f);}else{v->volTarget=c.target/127.0f*.17f;v->volSlide=fmaxf(.01f,c.duration*.008f);}break;}}
 }SDL_SetAtomicInt(&commandRead,r);}
static void mix_audio(float*out,int frames){audio_commands();for(int n=0;n<frames;n++){float l=0,r=0;for(int i=0;i<48;i++){Voice*v=voices+i;if(!v->on)continue;v->age+=1/48000.0f;if(v->age>=v->life){v->on=0;continue;}if(v->slide>0){v->freq+=(v->target-v->freq)/fmaxf(1,v->slide*48000);v->slide-=1/48000.0f;}if(v->volSlide>0){v->amp+=(v->volTarget-v->amp)/fmaxf(1,v->volSlide*48000);v->volSlide-=1/48000.0f;}
 float env=fminf(v->age*900,1)*fminf((v->life-v->age)*80,1),sample;
 noiseSeed=noiseSeed*1664525u+1013904223u;float noise=(noiseSeed>>8)*(2/16777216.0f)-1;
 v->phase+=v->freq/48000;if(v->phase>=1)v->phase-=floorf(v->phase);v->phase2+=1/48000.0f;
 if(v->kind==4){float t=v->age/v->life,sweep=sinf(PI*t);v->noise+=(.015f+.35f*sweep)*(noise-v->noise);sample=(noise*.25f+v->noise*1.5f+sinf(2*PI*v->phase)*.08f)*powf(fmaxf(0,sweep),.8f);}
 else if(v->kind==2){v->noise+=.07f*(noise-v->noise);sample=(v->noise*2.4f+sinf(2*PI*v->phase)*expf(-v->age*12)*.65f)*expf(-v->age*3.4f);}
 else if(v->kind==3){sample=(sinf(2*PI*v->phase+1.8f*sinf(2*PI*v->phase*2))+sinf(2*PI*(v->phase+v->phase2*9))*.45f)*.65f;}
 else sample=(sinf(2*PI*v->phase+1.2f*sinf(2*PI*v->phase*2))*.8f+noise*.12f)*expf(-v->age*17);
 sample*=env*v->amp;l+=sample*(1-v->pan)*.7071f;r+=sample*(1+v->pan)*.7071f;
 }l=l/(1+fabsf(l));r=r/(1+fabsf(r));if(!isfinite(l)||!isfinite(r)){l=r=0;SDL_AddAtomicInt(&audioErrors,1);}audioPeak=fmaxf(audioPeak,fmaxf(fabsf(l),fabsf(r)));if(fabsf(l)+fabsf(r)>.00001f)audioNonzero++;out[n*2]=l;out[n*2+1]=r;}SDL_AddAtomicInt(&audioFrames,frames);}
static void SDLCALL audio_callback(void*unused,SDL_AudioStream*s,int additional,int total){(void)unused;(void)total;float block[512*2];int frames=additional/8;while(frames>0){int n=frames>512?512:frames;mix_audio(block,n);SDL_PutAudioStreamData(s,block,n*8);frames-=n;}}
static void quad(float x,float y,float w,float h,SDL_FColor c,float u,float v,float uw,float vh){if(nquad>=NQ)return;int k=nquad*4;vertices[k]=(SDL_Vertex){{x,y},c,{u,v}};vertices[k+1]=(SDL_Vertex){{x+w,y},c,{u+uw,v}};vertices[k+2]=(SDL_Vertex){{x+w,y+h},c,{u+uw,v+vh}};vertices[k+3]=(SDL_Vertex){{x,y+h},c,{u,v+vh}};nquad++;}
static void flush(SDL_Texture*t){if(nquad)SDL_RenderGeometry(renderer,t,vertices,nquad*4,indices,nquad*6);nquad=0;}
static void light(float x,float y,float size,float r,float gg,float b,float alpha){quad((x-size)*3,(y-size)*3,size*6,size*6,(SDL_FColor){r,gg,b,alpha},0,0,1,1);}
static void init_graphics(void){for(int i=0;i<NQ;i++){int k=i*6,a=i*4;indices[k]=a;indices[k+1]=a+1;indices[k+2]=a+2;indices[k+3]=a;indices[k+4]=a+2;indices[k+5]=a+3;}
 for(int c=0;c<256;c++){int r=17*(((c&16)>>1)+(c&4)+(c&3)),gg=17*(((c&64)>>3)+((c&32)>>3)+(c&3)),b=17*(((c&128)>>4)+((c&8)>>1)+(c&3));palette[c]=0xff000000u|(b<<16)|(gg<<8)|r;}
 base=SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA32,SDL_TEXTUREACCESS_STREAMING,320,256);SDL_SetTextureScaleMode(base,SDL_SCALEMODE_NEAREST);
 uint32_t *pixels=calloc(512*512,4);FILE*f=fopen("Assets/sprites.bin","rb");if(!f){SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"Missing assets","Keep the Assets folder beside StarGuard.exe.",window);exit(1);}uint32_t count;fread(&count,4,1,f);int x=0,y=0,rh=0;
 for(uint32_t i=0;i<count&&i<40;i++){uint32_t w,h;fread(&w,4,1,f);fread(&h,4,1,f);if(x+(int)w>512){x=0;y+=rh;rh=0;}if(y+(int)h>512)exit(2);sprites[i]=(Sprite){0,0,w,h,x/512.0f,y/512.0f,w/512.0f,h/512.0f};for(uint32_t row=0;row<h;row++)fread(pixels+(y+row)*512+x,4,w,f);x+=w;if((int)h>rh)rh=h;}fclose(f);
 atlas=SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA32,SDL_TEXTUREACCESS_STATIC,512,512);SDL_UpdateTexture(atlas,0,pixels,512*4);SDL_SetTextureBlendMode(atlas,SDL_BLENDMODE_BLEND);SDL_SetTextureScaleMode(atlas,SDL_SCALEMODE_NEAREST);free(pixels);
 uint32_t glowPixels[64*64];for(int j=0;j<64;j++)for(int i=0;i<64;i++){float dx=(i-31.5f)/31.5f,dy=(j-31.5f)/31.5f,d=dx*dx+dy*dy;float a=fmaxf(0,1-d);a=a*a*a;glowPixels[j*64+i]=((uint32_t)(a*255)<<24)|0xffffff;}
 fxTarget=SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA32,SDL_TEXTUREACCESS_TARGET,1920,1600);if(!fxTarget)exit(3);SDL_SetTextureBlendMode(fxTarget,SDL_BLENDMODE_ADD);SDL_SetTextureScaleMode(fxTarget,SDL_SCALEMODE_LINEAR);
 halo=SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA32,SDL_TEXTUREACCESS_STATIC,64,64);SDL_UpdateTexture(halo,0,glowPixels,64*4);SDL_SetTextureBlendMode(halo,SDL_BLENDMODE_ADD);SDL_SetTextureScaleMode(halo,SDL_SCALEMODE_LINEAR);
}
static float trailTime,bombRenderRadius;
static void update_fx(float dt){aliveParticles=0;for(int i=0;i<NP;i++){Spark*p=sparks+i;if(p->life<=0)continue;p->age+=dt;if(p->age>=p->life){p->life=0;continue;}p->px=p->x;p->py=p->y;float drag=1/(1+dt*1.3f),vx=p->vx;p->vx=(p->vx-p->vy*p->spin*dt*.1f)*drag;p->vy=(p->vy+vx*p->spin*dt*.1f)*drag;p->x+=p->vx*dt;p->y+=p->vy*dt;aliveParticles++;}if(aliveParticles>peakParticles)peakParticles=aliveParticles;for(int i=0;i<64;i++)if(waves[i].life>0){waves[i].age+=dt;if(waves[i].age>waves[i].life)waves[i].life=0;}}
static void draw_fx(float dt){float power=intensity();int level=quality==2?2:1;trailTime+=dt;int emissions=(int)(trailTime*120);if(emissions>4)emissions=4;if(emissions)trailTime-=emissions/120.0f;
 if(g.fx&&g.mode==1){for(int e=0;e<emissions;e++){for(int i=0;i<MAXS+MAXB;i++){Obj*o=i<MAXS?g.shots+i:g.bullets+i-MAXS;if(!o->on)continue;int tint=i<MAXS?((o->type==6||o->type==7)?4:o->type==2||o->type==4?1:3):2;for(int k=0;k<level;k++)emit(o->x/256.0f+(frand()-.5f)*2,o->y/256.0f+(frand()-.5f)*2,-o->vx/256.0f*4+(frand()-.5f)*12,-o->vy/256.0f*3,.12f+power*.07f,.5f+frand(),tint);}
 for(int j=0;j<level*3;j++){emit(g.px/256.0f+(j&1?4:-4),player_y/256.0f+14,(frand()-.5f)*12,30+frand()*45,.18f+frand()*.18f,.4f+frand()*1.2f,3);}
 if(ex.travel&&quality==2)for(int k=0;k<10;k++)emit(frand()*320,frand()*232+24,0,160+frand()*500,.3f+frand()*.4f,.3f+frand(),(k&1)?0:2);
 }}
 SDL_Rect clip={0,72,960,696};SDL_SetRenderClipRect(renderer,&clip);nquad=0;
 if(g.fx){for(int i=0;i<NP;i++){Spark*p=sparks+i;if(p->life<=0)continue;float fade=1-p->age/p->life;light(p->x,p->y,p->size*(1+fade*1.4f),p->r,p->g,p->b,fade*.65f);if(quality==2&&p->age<.08f)light(p->x,p->y,p->size*.38f,1,1,1,fade);}
 for(int i=0;i<64;i++){Wave*w=waves+i;if(!w->life)continue;float t=w->age/w->life,r=w->radius*t,fade=(1-t)*(1-t);light(w->x,w->y,10*(1-t)+2,1,.55f,.12f,fade*.8f);int n=quality==2?96:40;for(int j=0;j<n;j++){float a=j*2*PI/n;light(w->x+cosf(a)*r,w->y+sinf(a)*r,.6f+2*(1-t),.25f,.65f,1,fade*.65f);}}
 for(int i=0;i<MAXS+MAXB;i++){Obj*o=i<MAXS?g.shots+i:g.bullets+i-MAXS;if(o->on){int t=i<MAXS?o->type:7;if(t==6|| (i<MAXS&&t==7)){light(o->x/256.0f,o->y/256.0f,4,.35f,1,.12f,1);continue;}light(o->x/256.0f,o->y/256.0f,3.5f,t==2||t==4?1:.2f,t==7?.15f:.7f,t==2?.1f:1,.8f);}}
 if(g.beam)for(int lane=ex.multi?-1:0;lane<=(ex.multi?1:0);lane++)for(int y=26;y<(player_y>>8)-12;y+=6)light((g.px>>8)+lane*28,y,7,.12f,.5f,1,.26f);
 if(ex.bombAge){flush(halo);float r=bombRenderRadius,fade=fminf(1,(61-ex.bombAge)/10.0f),cx=(float)ex.bombX,cy=(float)ex.bombY;quad((cx-r)*3,(cy-r*.8f)*3,r*6,r*4.8f,(SDL_FColor){.08f,.35f,1,.11f*fade},0,0,1,1);for(int j=0;j<256;j++){float a=j*2*PI/256;light(cx+cosf(a)*r,cy+sinf(a)*r*.8f,5.5f,.1f,.5f,1,.8f*fade);light(cx+cosf(a)*r*.96f,cy+sinf(a)*r*.768f,2,.55f,.9f,1,.4f*fade);}}
 }flush(halo);
 if(ex.bombAge){SDL_FPoint points[257];SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_ADD);float fade=fminf(1,(61-ex.bombAge)/10.0f);for(int line=-1;line<=1;line++){float r=bombRenderRadius+line*2;for(int j=0;j<=256;j++){float a=j*2*PI/256;points[j]=(SDL_FPoint){(ex.bombX+cosf(a)*r)*3,(ex.bombY+sinf(a)*r*.8f)*3};}SDL_SetRenderDrawColor(renderer,line?60:235,line?160:255,255,(Uint8)((line?140:255)*fade));SDL_RenderLines(renderer,points,257);}SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);}
 SDL_SetRenderClipRect(renderer,0);
}
static Game previous,saved;static Expansion previousEx,savedEx;static int previousY;
static int lerpi(int a,int b,float t){return (int)(a+(b-a)*t);}
static void interp_objects(Obj*a,Obj*b,int n,float t){for(int i=0;i<n;i++)if(a[i].on&&b[i].on&&a[i].type==b[i].type){a[i].x=lerpi(b[i].x,a[i].x,t);a[i].y=lerpi(b[i].y,a[i].y,t);int turn=(a[i].heading-b[i].heading+8640)%5760-2880;a[i].heading=(b[i].heading+(int)(turn*t)+5760)%5760;}}
static void render_frame(float alpha,float dt){saved=g;savedEx=ex;int sy=player_y;g.px=lerpi(previous.px,g.px,alpha);player_y=lerpi(previousY,player_y,alpha);interp_objects(g.enemies,previous.enemies,MAXE,alpha);interp_objects(g.shots,previous.shots,MAXS,alpha);interp_objects(g.bullets,previous.bullets,MAXB,alpha);interp_objects(g.drops,previous.drops,8,alpha);
 int delta=(ex.backgroundY-previousEx.backgroundY)&131071;if(delta>65535)delta-=131072;ex.backgroundY=(previousEx.backgroundY+(int)(delta*alpha))&131071;for(int i=0;i<NSTARS;i++){int d=g.stars[i].y-previous.stars[i].y;if(d<0)d+=232*Q;g.stars[i].y=(previous.stars[i].y+(int)(d*alpha))%(232*Q);}
 bombRenderRadius=ex.bombAge?((previousEx.bombAge?previousEx.bombRadius:0)+(ex.bombRadius-(previousEx.bombAge?previousEx.bombRadius:0))*alpha):0;ndraw=0;game_render(fb);for(int i=0;i<81920;i++)rgba[i]=palette[fb[i]];SDL_UpdateTexture(base,0,rgba,320*4);SDL_SetRenderDrawColor(renderer,3,5,14,255);SDL_RenderClear(renderer);SDL_FRect area={0,0,960,768};SDL_RenderTexture(renderer,base,0,&area);
 SDL_Rect clip={0,72,960,696};SDL_SetRenderClipRect(renderer,&clip);nquad=0;for(int i=0;i<ndraw;i++){Draw*d=draws+i;Sprite*s=sprites+d->n;float scale=d->n>=32?.65f:1;quad((d->x-s->w*.5f*scale)*3,(d->y-s->h*.5f*scale)*3,(s->w*3*scale)*(d->n>=28&&d->n<32?(1+.09f*sinf(g.tick*.7f+d->x)):1),s->h*3*scale,(SDL_FColor){1,1,1,1},s->u,s->v,s->uw,s->vh);if(d->angle!=0){float angle=d->angle*PI/180,c=cosf(angle),ss=sinf(angle),cx=d->x*3,cy=d->y*3;for(int v=(nquad-1)*4;v<nquad*4;v++){float xx=vertices[v].position.x-cx,yy=vertices[v].position.y-cy;vertices[v].position.x=cx+xx*c-yy*ss;vertices[v].position.y=cy+xx*ss+yy*c;}}}flush(atlas);SDL_SetRenderClipRect(renderer,0);SDL_SetRenderTarget(renderer,fxTarget);SDL_SetRenderLogicalPresentation(renderer,960,800,SDL_LOGICAL_PRESENTATION_STRETCH);SDL_SetRenderDrawColor(renderer,0,0,0,255);SDL_RenderClear(renderer);draw_fx(dt);SDL_SetRenderTarget(renderer,0);SDL_FRect fxArea={0,0,960,800};SDL_RenderTexture(renderer,fxTarget,0,&fxArea);
 g=saved;ex=savedEx;player_y=sy;
}
static void ui_text(int x,int y,const char*s,SDL_Color c){SDL_SetRenderDrawColor(renderer,c.r,c.g,c.b,c.a);while(*s){int glyph=-1;for(int i=0;fontchars[i];i++)if(fontchars[i]==*s){glyph=i;break;}if(glyph>=0)for(int j=0;j<7;j++)for(int k=0;k<5;k++)if(font[glyph*7+j]&(1<<(4-k))){SDL_FRect r={x+k*2.0f,y+j*2.0f,2,2};SDL_RenderFillRect(renderer,&r);}x+=12;s++;}}
static void fatal(const char*s){SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"Star Guard PC",s,window);exit(1);}
static void screenshot(const char*name){SDL_Surface*s=SDL_RenderReadPixels(renderer,0);if(s){SDL_SaveBMP(s,name);SDL_DestroySurface(s);}}
static double ms_now(void){return SDL_GetPerformanceCounter()*1000.0/SDL_GetPerformanceFrequency();}
static int compare_double(const void*a,const void*b){double x=*(const double*)a,y=*(const double*)b;return (x>y)-(x<y);}
static void selftest(void){
 headless=1;int seen[22]={0},weapons=0,checks=0,phaseSeen[10]={0};double begin=ms_now();
 #define CHECK(x) do{if(!(x)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);exit(4);}checks++;}while(0)
 game_reset();CHECK(g.unlocked==255&&ex.bombs==3&&ex.options==2);game_step(4,0);CHECK(g.mode==1);
 for(int w=0;w<8;w++){game_reset();g.mode=1;g.shield=100000;ex.optionsOn=0;g.weapon=w;g.cool=0;g.energy=100;game_step(4,0);if(w==3||w==5)CHECK(g.beam>0&&g.energy<100);else{int n=0;for(int i=0;i<MAXS;i++)n+=g.shots[i].on;CHECK(n==(w==1||w==7?3:1));}weapons|=1<<w;}
 game_reset();g.mode=1;g.shield=0;bullet(g.px,player_y,0,0);game_step(0,0);CHECK(g.barrier==65&&g.shield>0);g.shield=0;g.barrier=0;bullet(g.px,player_y,0,0);game_step(0,0);CHECK(g.lives==2);
 for(int i=0;i<7;i++){drop(g.px>>8,player_y>>8,i);game_step(0,0);}CHECK(g.lives==3&&g.barrier==100&&g.shield>0);
 int t=g.tick;game_step(0,'p');CHECK(g.tick==t&&g.paused);game_step(0,'p');CHECK(!g.paused);g.energy=100;game_step(0,'d');CHECK(g.active>0&&g.energy==65);
 g.bullets[0].on=1;int bombs=ex.bombs;g.bullets[0].x=g.px;g.bullets[0].y=player_y;game_step(0,'b');CHECK(ex.bombs==bombs-1&&ex.bombAge>0&&!g.bullets[0].on);game_step(0,'o');CHECK(!ex.optionsOn);
 game_reset();g.mode=1;g.wave=5;g.clock=520;g.groups=3;game_step(0,0);CHECK(g.enemies[0].type==4);g.enemies[0].hp=40;game_step(0,0);CHECK(g.bossPhase==2);g.enemies[0].hp=15;game_step(0,0);CHECK(g.bossPhase==3);

 game_reset();g.mode=1;g.clock=100;g.groups=3;ex.optionsOn=0;g.shield=100000;g.enemies[0]=(Obj){.on=1,.x=160*Q,.y=100*Q,.base=160,.type=3,.hp=99};g.shots[0]=(Obj){.on=1,.x=160*Q,.y=114*Q,.vy=-4*Q,.type=6,.hp=4,.phase=-1};game_step(0,0);CHECK(g.shots[0].on&&g.shots[0].hp==3&&g.shots[0].vy>0&&absn(g.shots[0].vx)>0);int ehp=g.enemies[0].hp;game_step(0,0);CHECK(g.enemies[0].hp==ehp);for(int i=0;i<3;i++)ricochet(g.shots,g.enemies,0);CHECK(!g.shots[0].on&&g.shots[0].hp==0);
 for(int type=10;type<14;type++){game_reset();g.mode=1;g.groups=3;g.clock=100;g.shield=100000;ex.optionsOn=0;bug_group(type,0);int count=0;for(int i=0;i<MAXE;i++)count+=g.enemies[i].on;CHECK(count>=24);for(int tick=0;tick<150;tick++)game_step(0,0);for(int i=0;i<count;i++)CHECK(g.enemies[i].on&&g.enemies[i].type==type&&g.enemies[i].x>0&&g.enemies[i].x<320*Q&&g.enemies[i].y>24*Q&&g.enemies[i].y<130*Q);}
 game_reset();g.mode=1;g.clock=100;g.groups=3;g.shield=100000;ex.optionsOn=0;g.enemies[0]=(Obj){.on=1,.x=178*Q,.y=230*Q,.base=178,.type=0,.hp=1};g.enemies[1]=(Obj){.on=1,.x=160*Q,.y=40*Q,.base=160,.type=0,.hp=1};g.bullets[0]=(Obj){.on=1,.x=160*Q,.y=40*Q};game_step(0,'b');CHECK(!g.enemies[0].on&&g.enemies[1].on&&g.bullets[0].on);CHECK(ex.bombX==160&&ex.bombY==230);for(int i=0;i<40;i++)game_step(0,0);CHECK(!g.enemies[1].on&&!g.bullets[0].on);for(int x=0;x<=320;x+=320)for(int y=24;y<=256;y+=232)CHECK(bomb_reached(x,y,0));for(int i=0;i<40;i++)game_step(0,0);CHECK(!ex.bombAge);
 for(int y=0;y<1024;y++)CHECK(tunnel_left(y)>0&&tunnel_right(y)<320&&tunnel_right(y)-tunnel_left(y)>=60);
 game_reset();g.mode=1;for(int tick=0;tick<60000;tick++){for(int i=0;i<MAXE;i++)if(g.enemies[i].on){g.px=clamp(g.enemies[i].x,14*Q,306*Q);seen[g.enemies[i].type]=1;break;}if(ex.phase==5)g.px=tunnel_center(220)*Q;if(tick%150==0){g.energy=100;g.weapon=(tick/150)%7;}g.shield=100000;game_step(4,0);if(ex.phase>=0)phaseSeen[ex.phase]=1;CHECK(g.energy>=0&&g.energy<=100&&g.px>=14*Q&&g.px<=306*Q&&g.weapon<7);for(int i=0;i<MAXE;i++)if(g.enemies[i].on&&g.enemies[i].type==4&&(tick&127)==0)g.enemies[i].hp-=8;if(tick%97==0){ndraw=0;game_render(fb);}}
 for(int i=0;i<22;i++){if(!seen[i])fprintf(stderr,"Unseen enemy %d\n",i);CHECK(seen[i]);}for(int i=0;i<10;i++)CHECK(phaseSeen[i]);CHECK(weapons==255&&g.wave>30&&g.kills>500);
 headless=0;game_reset();g.weapon=2;sound(1,1,100,0x4000);sound_slide(1,1,0x2000,20);float audio[48000*2];mix_audio(audio,48000);CHECK(audioPeak>0&&audioPeak<1&&audioNonzero>1000);sound(2,2,100,0x3000);sound_slide(2,0,0,30);mix_audio(audio,48000);CHECK(SDL_GetAtomicInt(&audioErrors)==0&&SDL_GetAtomicInt(&audioDrops)==0);
 FILE*f=fopen("SELF-TEST.json","w");if(f){fprintf(f,"{\"passed_assertions\":%d,\"game_ticks\":60000,\"weapons\":8,\"enemy_types\":22,\"sectors\":10,\"sound_peak\":%.6f,\"sound_nonzero_samples\":%d,\"elapsed_ms\":%.2f}\n",checks,audioPeak,audioNonzero,ms_now()-begin);fclose(f);}printf("PASS %d checks; 60000 ticks, eight weapons, twenty-two enemies, ten sectors, audio finite and unclipped\n",checks);
}

static int demo,benchmark,stress,showcase,showcaseShots,shotCount;
static float demoSeconds=30;
static SDL_Gamepad*pad;
static double recentMS,recentCPU,before,start,acc,cpuSum,frameSum,maxCpu;
static int recentFrames,frames,ticks,cmd,focus=1,keysHeld,mouseFire,sampleCount;
static float recentFPS=60,recentCPUms,mx=160,my=230;
static double frameTimes[200000],cpuTimes[200000];
static SDL_Event event;
static int webCmd,webHeld;
EMSCRIPTEN_KEEPALIVE void web_command(int c){webCmd=c;}
EMSCRIPTEN_KEEPALIVE void web_held(int mask){webHeld=mask;}
EMSCRIPTEN_KEEPALIVE void web_pointer(float x,float y){mx=x;my=y;mouseControl=1;}
EMSCRIPTEN_KEEPALIVE void web_mute(void){muted=!muted;if(astream)SDL_SetAudioStreamGain(astream,muted?0:1);}
EMSCRIPTEN_KEEPALIVE int web_stat(int n){switch(n){case 0:return g.tick;case 1:return g.mode;case 2:return g.weapon;case 3:return ex.bombs;case 4:return ex.bombAge;case 5:return aliveParticles;case 6:return g.paused;case 7:return SDL_GetAtomicInt(&audioFrames);case 8:return audioNonzero;case 9:return SDL_GetAtomicInt(&audioErrors);case 10:return g.wave;case 11:return peakParticles;case 12:return (int)(audioPeak*1000000);case 13:return frames;case 14:return g.playerVX;case 15:return ex.phase;case 16:return ex.travel;case 17:return ex.bossSpawned;case 18:return muted;}return -1;}
static void browser_frame(void){
double frameStart=ms_now(),delta=(frameStart-before)/1000;before=frameStart;if(delta>.1)delta=.1;
 while(SDL_PollEvent(&event)){SDL_ConvertEventToRenderCoordinates(renderer,&event);if(event.type==SDL_EVENT_QUIT)cmd='p';if(event.type==SDL_EVENT_WINDOW_FOCUS_LOST&&!demo){focus=0;if(astream){SDL_ClearAudioStream(astream);SDL_PauseAudioStreamDevice(astream);}}if(event.type==SDL_EVENT_WINDOW_FOCUS_GAINED){focus=1;acc=0;if(astream)SDL_ResumeAudioStreamDevice(astream);}if(event.type==SDL_EVENT_MOUSE_MOTION){mx=event.motion.x/3;my=event.motion.y/3;mouseControl=1;}if(event.type==SDL_EVENT_MOUSE_BUTTON_DOWN&&event.button.button==SDL_BUTTON_LEFT){mouseFire=1;cmd=32;}if(event.type==SDL_EVENT_MOUSE_BUTTON_UP&&event.button.button==SDL_BUTTON_LEFT)mouseFire=0;
 if(event.type==SDL_EVENT_KEY_DOWN&&!event.key.repeat){SDL_Keycode k=event.key.key;if(k>=32&&k<127)cmd=(int)k;if(k==SDLK_ESCAPE)cmd='p';if(k==SDLK_F11){fullscreen=!fullscreen;SDL_SetWindowFullscreen(window,fullscreen);}if(k=='m'){muted=!muted;if(astream)SDL_SetAudioStreamGain(astream,muted?0:1);}if(k=='v'){vsync=!vsync;SDL_SetRenderVSync(renderer,vsync);}if(k==SDLK_F2){cap=cap==120?240:cap==240?0:120;}if(k==SDLK_F3)showStats=!showStats;if(k==SDLK_F4)quality=(quality+1)%3;if(k==SDLK_F12){screenshot("Screenshot.bmp");}if(k=='z'||k=='x'||k==SDLK_LEFT||k==SDLK_RIGHT||k==SDLK_UP||k==SDLK_DOWN||k=='w'||k=='a'||k=='s')mouseControl=0;}
 if(event.type==SDL_EVENT_GAMEPAD_BUTTON_DOWN){switch(event.gbutton.button){case SDL_GAMEPAD_BUTTON_SOUTH:cmd=32;break;case SDL_GAMEPAD_BUTTON_EAST:cmd='b';break;case SDL_GAMEPAD_BUTTON_WEST:cmd='d';break;case SDL_GAMEPAD_BUTTON_NORTH:cmd='o';break;case SDL_GAMEPAD_BUTTON_START:cmd='p';break;case SDL_GAMEPAD_BUTTON_LEFT_SHOULDER:cmd='1'+(g.weapon+7)%8;break;case SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER:cmd='1'+(g.weapon+1)%8;break;}}if(event.type==SDL_EVENT_GAMEPAD_ADDED&&!pad)pad=SDL_OpenGamepad(event.gdevice.which);if(event.type==SDL_EVENT_GAMEPAD_REMOVED&&pad&&SDL_GetGamepadID(pad)==event.gdevice.which){SDL_CloseGamepad(pad);pad=0;}}
 const bool*k=SDL_GetKeyboardState(0);keysHeld=webHeld|(k[SDL_SCANCODE_Z]||k[SDL_SCANCODE_LEFT]||k[SDL_SCANCODE_A]?1:0)|(k[SDL_SCANCODE_X]||k[SDL_SCANCODE_RIGHT]?2:0)|(k[SDL_SCANCODE_SPACE]||mouseFire?4:0);
 if(webCmd){cmd=webCmd;webCmd=0;}if(!focus){delta=0;acc=0;}acc+=delta;int steps=0;
 while(acc>=.02&&steps++<5){previous=g;previousEx=ex;previousY=player_y;int mask=keysHeld;
 if(demo){int phase=(int)((frameStart-start)/5000)%6;if(g.mode!=1||g.lives<1){game_reset();g.mode=1;}g.shield=100000;g.weapon=((int)((frameStart-start)/1500))%7;g.energy=100;int wave=phase==5?(stress?28:3):(stress?26:1)+phase;if(g.wave!=wave||((ex.phase==5)!=(phase==5))){g.wave=wave;g.clock=phase==5?300:0;g.groups=0;if(phase==5)ex.phase=-1;}for(int i=0;i<MAXE;i++)if(g.enemies[i].on&&g.enemies[i].y>24*Q){g.px=clamp(g.enemies[i].x,14*Q,306*Q);break;}if(phase==5)g.px=tunnel_center(210)*Q;mask=4;if(stress&&ticks%30==0)for(int j=0;j<3;j++)pc_burst(30+(ticks*31+j*97)%260,55+(ticks*17+j*41)%150,1);if(ticks%400==220)cmd='b';if((ticks%150)==0)pc_burst(40+(ticks*31)%240,70+(ticks*17)%140,ticks%300==0);}
 else if(mouseControl&&g.mode==1&&!g.paused)game_pointer((int)mx,(int)my);
if(showcase){g.wave=1;g.clock=100;g.groups=3;g.px=160*Q;player_y=220*Q;g.weapon=6;ex.optionsOn=0;mask=ticks>125?4:0;if(ticks==0){bug_group(10,0);for(int i=0;i<MAXE;i++)if(g.enemies[i].on){g.enemies[i].type=10+(i&3);g.enemies[i].hp=8;}}cmd=ticks==180?'b':0;}
 
 if(!mouseControl&&!g.paused){if(k[SDL_SCANCODE_UP]||k[SDL_SCANCODE_W])player_y-=4*Q;if(k[SDL_SCANCODE_DOWN]||k[SDL_SCANCODE_S])player_y+=4*Q;player_y=clamp(player_y,40*Q,240*Q);}
 if(pad){float ax=SDL_GetGamepadAxis(pad,SDL_GAMEPAD_AXIS_LEFTX)/32768.0f,ay=SDL_GetGamepadAxis(pad,SDL_GAMEPAD_AXIS_LEFTY)/32768.0f;if(fabsf(ax)>.15f||fabsf(ay)>.15f){mouseControl=0;g.px=clamp(g.px+(int)(ax*6*Q),14*Q,306*Q);player_y=clamp(player_y+(int)(ay*5*Q),40*Q,240*Q);}if(SDL_GetGamepadButton(pad,SDL_GAMEPAD_BUTTON_SOUTH)||SDL_GetGamepadAxis(pad,SDL_GAMEPAD_AXIS_RIGHT_TRIGGER)>12000)mask|=4;}
 game_step(mask|(cmd==32?4:0),cmd);cmd=0;acc-=.02;ticks++;
 }if(steps>5)acc=0;
 if(focus&&!g.paused)update_fx((float)delta);render_frame((float)fmin(acc/.02,1),(focus&&!g.paused)?(float)delta:0);
 char label[140];snprintf(label,sizeof(label),"%s  %s  FX %s  %d PARTICLES",muted?"MUTED":"STEREO",vsync?"VSYNC ON":"VSYNC OFF",quality==2?"ULTRA":quality==1?"HIGH":"LOW",aliveParticles);ui_text(12,780,label,(SDL_Color){135,189,224,255});
 if(!focus)ui_text(260,380,"CLICK WINDOW TO RESUME",(SDL_Color){255,255,255,255});
 if(showStats&&frames>0){snprintf(label,sizeof(label),"%.0f FPS  CPU %.2f MS",recentFPS,recentCPUms);ui_text(610,750,label,(SDL_Color){190,232,255,255});}
 double cpu=ms_now()-frameStart;cpuSum+=cpu;if(cpu>maxCpu)maxCpu=cpu;SDL_RenderPresent(renderer);double work=ms_now()-frameStart;double total=delta*1000;frameSum+=total;frames++;recentMS+=total;recentCPU+=cpu;recentFrames++;if(recentMS>=500){recentFPS=(float)(1000*recentFrames/recentMS);recentCPUms=(float)(recentCPU/recentFrames);recentMS=recentCPU=0;recentFrames=0;}if(sampleCount<200000){frameTimes[sampleCount]=total;cpuTimes[sampleCount]=cpu;sampleCount++;}
 if(showcase){if(ticks>=125&&showcaseShots==0){screenshot("Formation-Preview.bmp");showcaseShots=1;}if(ex.bombAge>=12&&showcaseShots==1){screenshot("Bomb-Early.bmp");showcaseShots=2;}if(ex.bombAge>=28&&showcaseShots==2){screenshot("Bomb-Late.bmp");showcaseShots=3;}}
 if(demo&&!benchmark&&((int)((frameStart-start)/1000)>shotCount+2)){char name[64];snprintf(name,sizeof(name),"Demo-%02d.bmp",shotCount);screenshot(name);shotCount+=5;}
 if(demo&&(frameStart-start)>demoSeconds*1000)cmd='p';
 
}

int main(int argc,char**argv){SDL_SetMainReady();SDL_SetHint(SDL_HINT_EMSCRIPTEN_KEYBOARD_ELEMENT,"#canvas");vsync=1;cap=0;
if(!SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_GAMEPAD))fatal(SDL_GetError());
 window=SDL_CreateWindow("Star Guard",1920,1600,0);if(!window)fatal(SDL_GetError());SDL_SetWindowSize(window,1920,1600);renderer=SDL_CreateRenderer(window,0);if(!renderer)fatal(SDL_GetError());SDL_SetWindowSize(window,1920,1600);SDL_SetRenderLogicalPresentation(renderer,960,800,SDL_LOGICAL_PRESENTATION_LETTERBOX);SDL_SetRenderVSync(renderer,1);init_graphics();
 SDL_AudioSpec spec={SDL_AUDIO_F32,2,48000};astream=SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,&spec,audio_callback,0);if(astream)SDL_ResumeAudioStreamDevice(astream);
game_reset();if(argc>1)g.wave=clamp(atoi(argv[1]),0,9)+1;previous=g;previousEx=ex;previousY=player_y;before=start=ms_now();
int count=0;SDL_JoystickID*ids=SDL_GetGamepads(&count);if(count)pad=SDL_OpenGamepad(ids[0]);SDL_free(ids);
emscripten_set_main_loop(browser_frame,0,1);return 0;}
