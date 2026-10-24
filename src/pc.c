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
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#define STBI_NO_HDR
#define STBI_NO_LINEAR
#include "stb_image.h"
#include "game.c"
int pc_modern=1;
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
static Sprite sprites[113];
#include "modern_sprites.h"
static SDL_Texture*scenes[8],*cloudTexture,*combatTarget,*worldTarget,*bossFrames,*smokeTexture;
static Game previous,saved;static Expansion previousEx,savedEx;static int previousY;
static float sideBlend,backgroundClock,backgroundFrameDt,cameraZoom=1,cameraX,cameraY=72,visualTick;
static float warp_envelope(float alpha){if(!ex.travel)return 0;float tick=ex.travelTick;if(previousEx.travel==ex.travel)tick=previousEx.travelTick+(ex.travelTick-previousEx.travelTick)*alpha;float t=ex.travel==1?fminf(1,tick/65):1-fminf(1,tick/90);return t*t*(3-2*t);}
static int visibleStars,renderedCloudLayers;
static int side_mode(void){return (ex.phase<0?(g.wave-1)%12:ex.phase)&1;}
static void flight_delta(float sx,float sy,float*wx,float*wy){float a=sideBlend*1.57079632679f,c=cosf(a),ss=sinf(a),w=960-264*sideBlend,h=696+264*sideBlend;*wx=(sx*c+sy*ss)*3*320/w;*wy=(-sx*ss+sy*c)*3*232/h;}
static void flight_pointer(int*x,int*y){float wx,wy;if(cameraZoom>1){*x=(int)((*x*3-cameraX)/cameraZoom/3);*y=(int)((72+(*y*3-cameraY)/cameraZoom)/3);}flight_delta(*x-160,*y-140,&wx,&wy);*x=(int)(160+wx);*y=(int)(140+wy);}
static void flight_move(float sx,float sy){float wx,wy;flight_delta(sx,sy,&wx,&wy);g.px=clamp(g.px+(int)(wx*Q),14*Q,306*Q);player_y=clamp(player_y+(int)(wy*Q),40*Q,240*Q);}

typedef struct{int n;float x,y,angle,scale,opacity;} Draw;
static Draw draws[256];static int ndraw;
static SDL_Vertex vertices[NQ*4];static int indices[NQ*6],nquad;
static int muted,quality=2,headless,mouseControl,fullscreen,vsync=0,cap=120,showStats=1;
static unsigned int fxseed=1234567;
static int projectile_tint(const Obj*o,int hostile){if(hostile)return o->type==1?11:o->type==2?7:o->variant==1?10:o->variant==2?4:2;if(o->type==7){static const int prism[6]={9,8,5,6,3,2};return prism[o->heading%6];}static const int tint[9]={3,5,6,3,7,3,8,9,7};return tint[clamp(o->type,0,8)];}
static SDL_FColor projectile_colour(const Obj*o,int hostile){static const SDL_FColor colours[12]={{.12f,.45f,1,1},{1,.3f,.04f,1},{.7f,.12f,1,1},{.1f,.85f,1,1},{.35f,1,.08f,1},{1,.72f,.1f,1},{.4f,1,.2f,1},{1,.34f,.07f,1},{1,.15f,.75f,1},{.24f,.52f,1,1},{1,.12f,.15f,1},{1,.62f,.06f,1}};return colours[projectile_tint(o,hostile)];}
static float frand(void){fxseed=fxseed*1664525u+1013904223u;return (fxseed>>8)*(1.0f/16777216.0f);}
typedef struct{float x,y,px,py,vx,vy,age,life,size,r,g,b,spin,wind;} Spark;
static Spark sparks[NP];static int sparkHead,aliveParticles,peakParticles;
typedef struct{float x,y,age,life,radius;int big;} Wave;
static Wave waves[64];static int waveHead;
typedef struct{float x,y,vx,vy,age,life,size;} Smoke;
static Smoke smoke[160];static int smokeHead;
static void smoke_emit(float x,float y){Smoke*p=smoke+smokeHead;smokeHead=(smokeHead+1)%160;*p=(Smoke){x,y,(frand()-.5f)*8,2+frand()*6,0,1.2f+frand()*.8f,2.5f+frand()*2};}
static float lastExplosionX=160;
static float intensity(void){return fminf(6.0f,1.0f+(g.wave-1)*.2f+(ex.phase==3?.5f:0)+(g.bossPhase>1?.5f:0));}
static void emit(float x,float y,float vx,float vy,float life,float size,int tint){Spark*p=sparks+sparkHead;sparkHead=(sparkHead+1)%NP;static const float c[12][3]={{.12f,.45f,1},{1,.3f,.04f},{.7f,.12f,1},{.1f,.85f,1},{.35f,1,.08f},{1,.72f,.1f},{.4f,1,.2f},{1,.34f,.07f},{1,.15f,.75f},{.24f,.52f,1},{1,.12f,.15f},{1,.62f,.06f}};*p=(Spark){x,y,x,y,vx,vy,0,life,size,c[tint<0?0:tint>11?11:tint][0],c[tint<0?0:tint>11?11:tint][1],c[tint<0?0:tint>11?11:tint][2],(frand()-.5f)*5};}
static void wake_emit(float x,float y,float vx,float vy,float life,float size,int tint){emit(x,y,vx,vy,life,size,tint);sparks[(sparkHead+NP-1)%NP].wind=1;}
static float wake_speed(void){return ex.scrollSpeed*(50.0f/(4*256));}
void pc_burst(int x,int y,int big){lastExplosionX=(float)x;if(!g.fx)return;int n=(int)((big?330:100)*intensity()*(quality==2?1:quality==1?.5f:.15f));for(int i=0;i<n;i++){float a=frand()*2*PI,s=8+frand()*(big?130:65);emit(x,y,cosf(a)*s,sinf(a)*s,.25f+frand()*(big?1.8f:.9f),.35f+frand()*2.5f,(i&7)?1:3);}Wave*w=waves+waveHead;waveHead=(waveHead+1)%64;*w=(Wave){x,y,0,big?1.4f:.65f,big?130:60,big};}
void pc_bomb_start(int x,int y){for(int i=0;i<480;i++){float a=frand()*2*PI,s=25+frand()*180;emit(x,y,cosf(a)*s,sinf(a)*s*.8f,.3f+frand()*.8f,.6f+frand()*2,3);}}
void pc_impact(int x,int y,int tint){if(!g.fx)return;int n=(int)((quality==2?18:7)*intensity());for(int i=0;i<n;i++){float a=frand()*2*PI,s=10+frand()*55;emit(x,y,cosf(a)*s,sinf(a)*s,.12f+frand()*.35f,.4f+frand(),tint);}}
void pc_sprite(int n,float x,float y){if(ndraw>=256||n<0||n>=113)return;float angle=(n>=40&&n<50)||(n>=65&&n<67)?180:0;if((n>=51&&n<=57)||(n>=100&&n<=102))angle=-sideBlend*90;if(n==0){int rank=ship_level(),side=side_mode();n=rank>1?(rank<=4?(side?97:94)+rank-2:(side?107:103)+rank-5):side?50:0;angle=side?-90:0;}draws[ndraw++]=(Draw){n,x,y,angle};}

void pc_equipment_sprite(int n,float x,float y,float angle){if(ndraw<256&&n>=0&&n<113)draws[ndraw++]=(Draw){n,x,y,angle};}
void pc_hazard_sprite(int n,Obj*o,float x,float y){if(ndraw<256)draws[ndraw++]=(Draw){n,x,y,o->age*1.7f+o->phase,o->base/16.0f,1};}
void pc_charge_burst(int x,int y){if(!g.fx)return;for(int i=0;i<60;i++){float a=frand()*2*PI,s=12+frand()*65;emit(x,y,cosf(a)*s,sinf(a)*s,.15f+frand()*.45f,.4f+frand()*1.1f,(i&3)?1:3);}Wave*w=waves+waveHead;waveHead=(waveHead+1)%64;*w=(Wave){x,y,0,.45f,25,0};}
/* A stable hull in each phase: only blend during the actual transformation. */
static float smooth_unit(float t){t=fmaxf(0,fminf(1,t));return t*t*(3-2*t);}
void pc_enemy_sprite(int n,Obj*o,float x,float y){if(ndraw>=254||n<0||n>=67)return;float angle=o->heading/16.0f+(((n>=40&&n<50)||n>=65)?180:0);if(o->type!=4){draws[ndraw++]=(Draw){n,x,y,angle,1,1};return;}float form=o->power/256.0f;int stage=form>=1?1:0;float blend=smooth_unit(form-stage),growth=1+.42f*(stage+blend),breath=1+.004f*sinf(visualTick*.055f+o->variant);int from=stage?67+o->variant:n,to=67+stage*12+o->variant;if(blend<.9999f)draws[ndraw++]=(Draw){from,x,y,angle,growth*breath,1-blend};if(blend>.0001f)draws[ndraw++]=(Draw){to,x,y,angle,growth*breath,blend};}

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
/* Material-specific transients share the existing bounded voice/command pools. */
void pc_hit_sound(int material,int projectile,int x){static int last[6]={-100,-100,-100,-100,-100,-100},lastTick=-1,count;material=clamp(material,0,5);if(g.tick<last[material])last[material]=-100;if(lastTick!=g.tick){lastTick=g.tick;count=0;}if(g.tick-last[material]<2||count>=2)return;last[material]=g.tick;count++;static const int pitch[6]={0x3600,0x4200,0x2300,0x4b00,0x2800,0x4800};int shift=projectile==4?-1100:projectile==7?900:projectile==6?600:projectile==2?400:projectile*90;enqueue((Command){.op=0,.ch=12+material,.kind=5+material,.vol=material==2?40:32,.pitch=pitch[material]+shift+((g.tick%5)-2)*45,.pan=(x-160)/200.0f});}
void pc_transition(void){enqueue((Command){.op=0,.ch=9,.kind=4,.vol=85,.pitch=0x2200,.pan=0});}
void sound_slide(int ch,int type,int target,int duration){enqueue((Command){.op=1,.ch=ch,.type=type,.target=target,.duration=duration});}
void sound_mod(int ch,int volume,int pitch){enqueue((Command){.op=2,.ch=ch,.vol=volume,.pitch=pitch});}
static void audio_commands(void){int r=SDL_GetAtomicInt(&commandRead),w=SDL_GetAtomicInt(&commandWrite);while(r!=w){Command c=commands[r];r=(r+1)&255;
 if(c.op==0){if(!c.vol){for(int i=0;i<48;i++)if(voices[i].ch==c.ch)voices[i].on=0;continue;}int chosen=voiceHead++%48;for(int skip=0;skip<48&&voices[chosen].on&&voices[chosen].kind==4;skip++)chosen=voiceHead++%48;Voice*v=voices+chosen;float hz=pitch_hz(c.pitch);*v=(Voice){.on=1,.ch=c.ch,.kind=c.kind,.life=c.kind>=5?(c.kind==8?.2f:.12f):c.kind==4?4.3f:c.kind==2?1.3f:c.kind==3?.45f:.23f,.freq=hz,.target=hz,.amp=c.vol/127.0f*.17f,.pan=fmaxf(-.85f,fminf(.85f,c.pan))};}
 else{for(int j=0;j<48;j++){Voice*v=voices+(voiceHead-1-j+96)%48;if(!v->on||v->ch!=c.ch)continue;if(c.op==2){v->target=pitch_hz(c.pitch);v->slide=.025f;v->life=fmaxf(v->life,v->age+.08f);}else if(c.type){v->target=pitch_hz(c.target);v->slide=fmaxf(.01f,c.duration*.008f);}else{v->volTarget=c.target/127.0f*.17f;v->volSlide=fmaxf(.01f,c.duration*.008f);}break;}}
 }SDL_SetAtomicInt(&commandRead,r);}
static void mix_audio(float*out,int frames){audio_commands();for(int n=0;n<frames;n++){float l=0,r=0;for(int i=0;i<48;i++){Voice*v=voices+i;if(!v->on)continue;v->age+=1/48000.0f;if(v->age>=v->life){v->on=0;continue;}if(v->slide>0){v->freq+=(v->target-v->freq)/fmaxf(1,v->slide*48000);v->slide-=1/48000.0f;}if(v->volSlide>0){v->amp+=(v->volTarget-v->amp)/fmaxf(1,v->volSlide*48000);v->volSlide-=1/48000.0f;}
 float env=fminf(v->age*900,1)*fminf((v->life-v->age)*80,1),sample;
 noiseSeed=noiseSeed*1664525u+1013904223u;float noise=(noiseSeed>>8)*(2/16777216.0f)-1;
 v->phase+=v->freq/48000;if(v->phase>=1)v->phase-=floorf(v->phase);v->phase2+=1/48000.0f;
 if(v->kind==5){sample=(sinf(2*PI*v->phase)+.4f*sinf(2*PI*v->phase*2.71f)+noise*.12f)*expf(-v->age*38);}
 else if(v->kind==6){sample=sinf(2*PI*v->phase+2.5f*sinf(2*PI*v->phase*1.4f))*expf(-v->age*22);}
 else if(v->kind==7){v->noise+=.18f*(noise-v->noise);sample=(v->noise*2.8f+sinf(2*PI*v->phase)*.22f)*expf(-v->age*35);}
 else if(v->kind==8){sample=(sinf(2*PI*v->phase)+.45f*sinf(2*PI*v->phase*3.89f))*expf(-v->age*27);}
 else if(v->kind==9){v->noise+=.3f*(noise-v->noise);sample=(v->noise*1.8f+noise*.3f)*expf(-v->age*25);}
 else if(v->kind==10){sample=(sinf(2*PI*v->phase)+noise*.2f)*expf(-v->age*55);}
 else if(v->kind==4){float t=v->age/v->life,sweep=sinf(PI*t);v->noise+=(.015f+.35f*sweep)*(noise-v->noise);sample=(noise*.25f+v->noise*1.5f+sinf(2*PI*v->phase)*.08f)*powf(fmaxf(0,sweep),.8f);}
 else if(v->kind==2){v->noise+=.07f*(noise-v->noise);sample=(v->noise*2.4f+sinf(2*PI*v->phase)*expf(-v->age*12)*.65f)*expf(-v->age*3.4f);}
 else if(v->kind==3){sample=(sinf(2*PI*v->phase+1.8f*sinf(2*PI*v->phase*2))+sinf(2*PI*(v->phase+v->phase2*9))*.45f)*.65f;}
 else sample=(sinf(2*PI*v->phase+1.2f*sinf(2*PI*v->phase*2))*.8f+noise*.12f)*expf(-v->age*17);
 sample*=env*v->amp;l+=sample*(1-v->pan)*.7071f;r+=sample*(1+v->pan)*.7071f;
 }l=l/(1+fabsf(l));r=r/(1+fabsf(r));if(!isfinite(l)||!isfinite(r)){l=r=0;SDL_AddAtomicInt(&audioErrors,1);}audioPeak=fmaxf(audioPeak,fmaxf(fabsf(l),fabsf(r)));if(fabsf(l)+fabsf(r)>.00001f)audioNonzero++;out[n*2]=l;out[n*2+1]=r;}SDL_AddAtomicInt(&audioFrames,frames);}
static void SDLCALL audio_callback(void*unused,SDL_AudioStream*s,int additional,int total){(void)unused;(void)total;float block[512*2];int frames=additional/8;while(frames>0){int n=frames>512?512:frames;mix_audio(block,n);SDL_PutAudioStreamData(s,block,n*8);frames-=n;}}
static void quad(float x,float y,float w,float h,SDL_FColor c,float u,float v,float uw,float vh){if(nquad>=NQ)return;int k=nquad*4;vertices[k]=(SDL_Vertex){{x,y},c,{u,v}};vertices[k+1]=(SDL_Vertex){{x+w,y},c,{u+uw,v}};vertices[k+2]=(SDL_Vertex){{x+w,y+h},c,{u+uw,v+vh}};vertices[k+3]=(SDL_Vertex){{x,y+h},c,{u,v+vh}};nquad++;}
/* Cancel the camera's nonuniform scale on local artwork, keeping world positions intact. */
static void unwarp_quad(float cx,float cy){float sx=960/(960-264*sideBlend),sy=696/(696+264*sideBlend);for(int i=(nquad-1)*4;i<nquad*4;i++){vertices[i].position.x=cx+(vertices[i].position.x-cx)*sx;vertices[i].position.y=cy+(vertices[i].position.y-cy)*sy;}}
static void flush(SDL_Texture*t){if(nquad)SDL_RenderGeometry(renderer,t,vertices,nquad*4,indices,nquad*6);nquad=0;}
static void light(float x,float y,float size,float r,float gg,float b,float alpha){quad((x-size)*3,(y-size)*3,size*6,size*6,(SDL_FColor){r,gg,b,alpha},0,0,1,1);unwarp_quad(x*3,y*3);}
static void streak(float x,float y,float vx,float vy,float length,SDL_FColor colour,float scale){float speed=sqrtf(vx*vx+vy*vy);if(speed<1)return;float dx=vx/speed,dy=vy/speed,cx=(x-dx*length*.4f)*3,cy=(y-dy*length*.4f)*3;int begin=nquad;quad(cx-length*1.5f,cy-2*scale,length*3,4*scale,colour,0,0,1,1);if(nquad==begin)return;for(int i=begin*4;i<begin*4+4;i++){float px=vertices[i].position.x-cx,py=vertices[i].position.y-cy;vertices[i].position.x=cx+px*dx-py*dy;vertices[i].position.y=cy+px*dy+py*dx;}unwarp_quad(cx,cy);}
static void beam_ribbon(float x1,float y1,float x2,float y2,float width,SDL_FColor colour){float dx=x2-x1,dy=y2-y1,len=sqrtf(dx*dx+dy*dy);if(len<.01f)return;float nx=-dy/len,ny=dx/len,sx=(960-264*sideBlend)/960,sy=(696+264*sideBlend)/696;float w=width/fmaxf(.1f,sqrtf(nx*nx*sx*sx+ny*ny*sy*sy));int first=nquad;quad(0,0,1,1,colour,0,0,1,1);if(nquad==first)return;SDL_FPoint points[4]={{(x1+nx*w)*3,(y1+ny*w)*3},{(x2+nx*w)*3,(y2+ny*w)*3},{(x2-nx*w)*3,(y2-ny*w)*3},{(x1-nx*w)*3,(y1-ny*w)*3}};for(int i=0;i<4;i++){vertices[first*4+i].position=points[i];vertices[first*4+i].tex_coord=(SDL_FPoint){.5f,i<2?0:1};}}
static void rainbow_beam(float x,float end){float powerScale=1+.18f*(power_level(g.weapon)-1);static const float rainbow[6][3]={{1,.12f,.18f},{1,.55f,.08f},{.9f,1,.12f},{.15f,1,.45f},{.1f,.65f,1},{.7f,.2f,1}};for(float y=26;y<end;y+=6){float next=fminf(end,y+6),w1=sinf(y*.12f+g.tick*.25f)*.28f,w2=sinf(next*.12f+g.tick*.25f)*.28f;for(int band=0;band<6;band++){float offset=(band-2.5f)*.7f*powerScale;beam_ribbon(x+offset+w1,y,x+offset+w2,next,.35f*powerScale,(SDL_FColor){rainbow[band][0],rainbow[band][1],rainbow[band][2],.52f});}beam_ribbon(x+w1,y,x+w2,next,.12f,(SDL_FColor){1,1,1,.5f});if(((int)y&15)<6)light(x,y,3,.25f,.55f,1,.06f);}}
static void init_graphics(void){for(int i=0;i<NQ;i++){int k=i*6,a=i*4;indices[k]=a;indices[k+1]=a+1;indices[k+2]=a+2;indices[k+3]=a;indices[k+4]=a+2;indices[k+5]=a+3;}
 for(int c=0;c<256;c++){int r=17*(((c&16)>>1)+(c&4)+(c&3)),gg=17*(((c&64)>>3)+((c&32)>>3)+(c&3)),b=17*(((c&128)>>4)+((c&8)>>1)+(c&3));palette[c]=0xff000000u|(b<<16)|(gg<<8)|r;}
 base=SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA32,SDL_TEXTUREACCESS_STREAMING,320,256);SDL_SetTextureScaleMode(base,SDL_SCALEMODE_NEAREST);
 int aw,ah,channels;unsigned char*pixels=stbi_load("Assets/modern/atlas.png",&aw,&ah,&channels,4);if(!pixels)exit(2);
 atlas=SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA32,SDL_TEXTUREACCESS_STATIC,aw,ah);SDL_UpdateTexture(atlas,0,pixels,aw*4);SDL_SetTextureBlendMode(atlas,SDL_BLENDMODE_BLEND);SDL_SetTextureScaleMode(atlas,SDL_SCALEMODE_LINEAR);stbi_image_free(pixels);memcpy(sprites,modernSprites,sizeof(sprites));pixels=stbi_load("Assets/modern/boss-frames.png",&aw,&ah,&channels,4);if(!pixels)exit(3);bossFrames=SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA32,SDL_TEXTUREACCESS_STATIC,aw,ah);SDL_UpdateTexture(bossFrames,0,pixels,aw*4);SDL_SetTextureBlendMode(bossFrames,SDL_BLENDMODE_BLEND);SDL_SetTextureScaleMode(bossFrames,SDL_SCALEMODE_LINEAR);stbi_image_free(pixels);
 const char*files[10]={"space.jpg","cavern.jpg","verdant.jpg","dunes.jpg","frost.jpg","magma.jpg","orbit.jpg","ground.jpg","clouds.png","smoke.png"};for(int i=0;i<10;i++){char path[120];snprintf(path,sizeof(path),"Assets/modern/%s",files[i]);pixels=stbi_load(path,&aw,&ah,&channels,4);if(!pixels)exit(3);SDL_Texture*t=SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA32,SDL_TEXTUREACCESS_STATIC,aw,ah);SDL_UpdateTexture(t,0,pixels,aw*4);SDL_SetTextureBlendMode(t,SDL_BLENDMODE_BLEND);SDL_SetTextureScaleMode(t,SDL_SCALEMODE_LINEAR);stbi_image_free(pixels);if(i<8)scenes[i]=t;else if(i==8)cloudTexture=t;else smokeTexture=t;}
 SDL_SetTextureBlendMode(base,SDL_BLENDMODE_BLEND);
 uint32_t glowPixels[64*64];for(int j=0;j<64;j++)for(int i=0;i<64;i++){float dx=(i-31.5f)/31.5f,dy=(j-31.5f)/31.5f,d=dx*dx+dy*dy;float a=fmaxf(0,1-d);a=a*a*a;glowPixels[j*64+i]=((uint32_t)(a*255)<<24)|0xffffff;}
 fxTarget=SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA32,SDL_TEXTUREACCESS_TARGET,1920,1600);if(!fxTarget)exit(3);SDL_SetTextureBlendMode(fxTarget,SDL_BLENDMODE_ADD);SDL_SetTextureScaleMode(fxTarget,SDL_SCALEMODE_LINEAR);
 halo=SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA32,SDL_TEXTUREACCESS_STATIC,64,64);SDL_UpdateTexture(halo,0,glowPixels,64*4);SDL_SetTextureBlendMode(halo,SDL_BLENDMODE_ADD);SDL_SetTextureScaleMode(halo,SDL_SCALEMODE_LINEAR);int cw,ch;SDL_GetRenderOutputSize(renderer,&cw,&ch);combatTarget=SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA32,SDL_TEXTUREACCESS_TARGET,cw,ch);if(!combatTarget)exit(3);SDL_SetTextureBlendMode(combatTarget,SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_ONE,SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,SDL_BLENDOPERATION_ADD,SDL_BLENDFACTOR_ONE,SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,SDL_BLENDOPERATION_ADD));SDL_SetTextureScaleMode(combatTarget,SDL_SCALEMODE_LINEAR);worldTarget=SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA32,SDL_TEXTUREACCESS_TARGET,cw,ch);if(!worldTarget)exit(3);SDL_SetTextureBlendMode(worldTarget,SDL_BLENDMODE_NONE);SDL_SetTextureScaleMode(worldTarget,SDL_SCALEMODE_LINEAR);
}
static int boss_damage_level(const Obj*o){return o->base>0?clamp((o->base-o->hp)*6/o->base,0,5):0;}
static void boss_damage_point(const Obj*o,int n,float*x,float*y){float a=(o->heading/16.0f+180)*PI/180,xx=((n*17+7)%23-11),yy=((n*23+3)%37-18);float sx=960/(960-264*sideBlend),sy=696/(696+264*sideBlend);*x=o->x/256.0f+(xx*cosf(a)-yy*sinf(a))*sx;*y=o->y/256.0f+(xx*sinf(a)+yy*cosf(a))*sy;}
static void draw_hull_damage(void){nquad=0;for(int i=0;i<MAXE;i++){Obj*o=g.enemies+i;if(!o->on||o->type!=4)continue;int damage=boss_damage_level(o);for(int n=0;n<damage;n++){float x,y;boss_damage_point(o,n,&x,&y);quad((x-2.6f)*3,(y-1.6f)*3,15.6f,9.6f,(SDL_FColor){.035f,.025f,.02f,.82f},0,0,0,0);unwarp_quad(x*3,y*3);}}flush(NULL);}
static int enemy_engines(const Obj*o){return o->on&&o->type!=9&&o->type!=23&&o->type!=24&&o->y>12*Q&&o->y<280*Q;}
static void engine_port(const Obj*o,int port,float*x,float*y,float*dx,float*dy){int boss=o->type==4;float a=(o->heading/16.0f+(boss?180:0))*PI/180,form=o->power/256.0f,across=(port&1?1:-1)*(boss?14+(port/2)*12+form*5:4),rear=boss?22+form*10:o->type>=10&&o->type<14?7:11;*dx=-sinf(a);*dy=cosf(a);*x=o->x/(float)Q+across*cosf(a)-rear*sinf(a);*y=o->y/(float)Q+across*sinf(a)+rear*cosf(a);}
static void draw_smoke(void){if(!g.fx)return;nquad=0;for(int i=0;i<160;i++){Smoke*p=smoke+i;if(p->life<=0)continue;float t=p->age/p->life,r=p->size*(1+t*2.5f),alpha=fminf(1,t*9)*(1-t)*.48f;quad((p->x-r)*3,(p->y-r)*3,r*6,r*6,(SDL_FColor){.65f,.67f,.7f,alpha},0,0,1,1);unwarp_quad(p->x*3,p->y*3);}flush(smokeTexture);}
static float trailTime,bombRenderRadius;
static void update_fx(float dt){for(int i=0;i<160;i++){Smoke*p=smoke+i;if(p->life<=0)continue;p->age+=dt;if(p->age>=p->life){p->life=0;continue;}p->x+=p->vx*dt;p->y+=(p->vy+wake_speed())*dt;p->vx*=1-dt*.3f;}aliveParticles=0;for(int i=0;i<NP;i++){Spark*p=sparks+i;if(p->life<=0)continue;p->age+=dt;if(p->age>=p->life){p->life=0;continue;}p->px=p->x;p->py=p->y;float drag=1/(1+dt*1.3f),vx=p->vx;p->vx=(p->vx-p->vy*p->spin*dt*.1f)*drag;p->vy=(p->vy+vx*p->spin*dt*.1f)*drag;p->x+=p->vx*dt;p->y+=(p->vy+p->wind*wake_speed())*dt;aliveParticles++;}if(aliveParticles>peakParticles)peakParticles=aliveParticles;for(int i=0;i<64;i++)if(waves[i].life>0){waves[i].age+=dt;if(waves[i].age>waves[i].life)waves[i].life=0;}}
static void draw_fx(float dt){float power=intensity();int level=quality==2?2:1;trailTime+=dt;int emissions=(int)(trailTime*120);if(emissions>4)emissions=4;if(emissions)trailTime-=emissions/120.0f;
 if(g.fx&&g.mode==1){for(int e=0;e<emissions;e++){for(int i=0;i<MAXS+MAXB;i++){Obj*o=i<MAXS?g.shots+i:g.bullets+i-MAXS;if(!o->on)continue;int tint=projectile_tint(o,i>=MAXS);for(int k=0;k<(i<MAXS&&o->type==7?1:level);k++)emit(o->x/256.0f+(frand()-.5f)*2,o->y/256.0f+(frand()-.5f)*2,-o->vx/256.0f*4+(frand()-.5f)*12,-o->vy/256.0f*3,i<MAXS&&o->type==7?.08f:.12f+power*.07f,i<MAXS&&o->type==7?.22f:.5f+frand(),tint);}
 for(int i=0;i<MAXE;i++){Obj*o=g.enemies+i;if(!enemy_engines(o))continue;int boss=o->type==4,ports=boss?2+2*(o->power/256):2,boost=(o->age+i*13)%100<22;for(int port=0;port<ports;port++){if(!boss&&quality<2&&((e+port)&1))continue;float x,y,dx,dy;engine_port(o,port,&x,&y,&dx,&dy);wake_emit(x+(frand()-.5f),y+(frand()-.5f),dx*(boost?95:45)+(frand()-.5f)*9,dy*(boost?95:45)+(frand()-.5f)*9,boost?.30f:.19f,boss?(boost?1.4f:.9f):(boost?.8f:.45f),o->type==25?7:boss?9:3);}}
 for(int j=0;j<level*3;j++){wake_emit(g.px/256.0f+(j&1?2:-2),player_y/256.0f+7,(frand()-.5f)*12,30+frand()*45,.18f+frand()*.18f,.4f+frand()*1.2f,3);}
 for(int i=0;i<MAXE;i++){Obj*o=g.enemies+i;if(!o->on||o->type!=4)continue;int damage=boss_damage_level(o);for(int n=0;n<damage;n++){float x,y;boss_damage_point(o,n,&x,&y);if(frand()<.05f+damage*.014f)smoke_emit(x,y);if(frand()<.12f+damage*.035f)wake_emit(x,y,(frand()-.5f)*65,-10+frand()*45,.12f+frand()*.4f,.3f+frand()*.65f,(n&1)?1:3);}}
 if(ex.travel&&quality==2)for(int k=0;k<10;k++)emit(frand()*320,frand()*232+24,0,160+frand()*500,.3f+frand()*.4f,.3f+frand(),(k&1)?0:2);
 }}
 SDL_Rect clip={0,72,960,696};SDL_SetRenderClipRect(renderer,&clip);nquad=0;
 if(g.fx){for(int i=0;i<NP;i++){Spark*p=sparks+i;if(p->life<=0)continue;float fade=1-p->age/p->life;light(p->x,p->y,p->size*(1+fade*1.4f),p->r,p->g,p->b,fade*.65f);if(quality==2&&p->age<.08f)light(p->x,p->y,p->size*.38f,1,1,1,fade);}
 for(int i=0;i<64;i++){Wave*w=waves+i;if(!w->life)continue;float t=w->age/w->life,r=w->radius*t,fade=(1-t)*(1-t);light(w->x,w->y,10*(1-t)+2,1,.55f,.12f,fade*.8f);int n=quality==2?96:40;for(int j=0;j<n;j++){float a=j*2*PI/n;light(w->x+cosf(a)*r,w->y+sinf(a)*r,.6f+2*(1-t),.25f,.65f,1,fade*.65f);}}
 for(int i=0;i<MAXS+MAXB;i++){Obj*o=i<MAXS?g.shots+i:g.bullets+i-MAXS;if(!o->on)continue;int hostile=i>=MAXS,t=o->type;float x=o->x/256.0f,y=o->y/256.0f,scale=hostile?1:1+.18f*(clamp(o->power,1,4)-1);SDL_FColor c=projectile_colour(o,hostile);if(!hostile&&t==8){light(x,y,4,c.r,c.g,c.b,.35f);continue;}if(!hostile&&t==7){float speed=sqrtf((float)o->vx*o->vx+(float)o->vy*o->vy);if(speed>0){float dx=o->vx/speed,dy=o->vy/speed,tx=x-dx*32*scale,ty=y-dy*32*scale;beam_ribbon(tx,ty,x,y,1.6f*scale,(SDL_FColor){c.r,c.g,c.b,.14f});beam_ribbon(tx,ty,x,y,.6f*scale,(SDL_FColor){c.r,c.g,c.b,.65f});for(int echo=0;echo<3;echo++){Obj hue=*o;hue.heading+=echo+1;SDL_FColor cc=projectile_colour(&hue,0);float wobble=sinf(visualTick*.09f+echo*2.1f)*(1.2f+echo*.6f)*scale,along=(echo+1)*5*scale;beam_ribbon(tx-dy*wobble-dx*along,ty+dx*wobble-dy*along,x-dy*wobble-dx*along,y+dx*wobble-dy*along,.24f*scale,(SDL_FColor){cc.r,cc.g,cc.b,.28f});}beam_ribbon(tx,ty,x,y,.13f*scale,(SDL_FColor){.9f,.98f,1,.9f});}light(x,y,4.2f*scale,c.r,c.g,c.b,.35f);continue;}if(hostile&&t==2){light(x,y,7,c.r,c.g,c.b,.35f);for(int n=0;n<12;n++){float a=n*PI/6+visualTick*.08f;light(x+cosf(a)*3,y+sinf(a)*3,.7f,c.r,c.g,c.b,.8f);}light(x,y,1,1,.9f,.4f,.8f);continue;}streak(x,y,o->vx,o->vy,(hostile?4:5)*scale,c,scale);light(x,y,3.8f*scale,c.r,c.g,c.b,.7f);light(x,y,.55f*scale,1,1,1,.8f);if(hostile&&t==1){quad((x-1)*3,(y-1)*3,6,6,(SDL_FColor){1,.78f,.15f,1},.5f,.5f,0,0);unwarp_quad(x*3,y*3);}}

 for(int i=0;i<MAXE;i++){Obj*o=g.enemies+i;if(!enemy_engines(o))continue;int boss=o->type==4,ports=boss?2+2*(o->power/256):2;float boost=(o->age+i*13)%100<22?1.5f:1;for(int port=0;port<ports;port++){float x,y,dx,dy;engine_port(o,port,&x,&y,&dx,&dy);light(x,y,(boss?4:2.3f)*boost,.15f,.55f,1,.25f);light(x,y,boss?1.1f:.6f,.75f,.95f,1,.8f);beam_ribbon(x,y,x+dx*(boss?10:6)*boost,y+dy*(boss?10:6)*boost,boss?.8f:.4f,(SDL_FColor){.2f,.65f,1,.45f});}}
 for(int i=0;i<8;i++){Obj*o=g.drops+i;if(!o->on)continue;float x=o->x/256.0f,y=o->y/256.0f,pulse=.5f+.5f*sinf(g.tick*.14f+i);int warm=o->type==5||o->type==9||o->type==11;light(x,y,10+pulse*3,warm?1:.2f,warm?.55f:.75f,warm?.1f:1,.12f+pulse*.1f);for(int j=0;j<3;j++){float a=g.tick*.06f+j*2*PI/3;light(x+cosf(a)*9,y+sinf(a)*7,.5f,.65f,.9f,1,.7f);}}
 for(int i=0;i<MAXE;i++){Obj*o=g.enemies+i;if(!o->on||o->type!=4)continue;float bx=o->x/256.0f,by=o->y/256.0f,form=o->power/256.0f,pulse=.5f+.5f*sinf(visualTick*.065f+o->variant),red=o->variant%3==0?1:.2f,green=o->variant%3==1?1:.5f,blue=o->variant%3==0?.22f:1;light(bx,by,19+form*6+pulse*3,red,green,blue,.11f);light(bx,by,7+pulse*2,red,green,blue,.4f);light(bx,by,1.2f,1,.95f,.8f,.7f);for(int side=-1;side<=1;side+=2){float angle=(o->heading/16.0f+180)*PI/180,dx=side*(18+form*7),dy=-12,x=bx+dx*cosf(angle)-dy*sinf(angle),y=by+dx*sinf(angle)+dy*cosf(angle);light(x,y,9+form*3,red,green,blue,.16f+pulse*.06f);light(x,y,2,1,.8f,.4f,.45f);}for(int n=0;n<boss_damage_level(o);n++){float x,y;boss_damage_point(o,n,&x,&y);float flicker=.4f+.2f*sinf(visualTick*.37f+n*3);light(x,y,2.3f,1,.25f,.04f,flicker);light(x,y,.55f,1,.9f,.45f,flicker);}}
 for(int i=0;i<MAXE;i++){Obj*o=g.enemies+i;if(!o->on||o->type!=4||o->attack<=0||o->attack>85)continue;float x=o->x/(float)Q,y=o->y/(float)Q,dx=o->vx/(float)Q-x,dy=o->vy/(float)Q-y,d=sqrtf(dx*dx+dy*dy);if(d<1)continue;float exx=x+dx/d*500,ey=y+dy/d*500;if(o->attack<=60){beam_ribbon(x,y,exx,ey,.22f,(SDL_FColor){1,.15f,.25f,.45f});light(x,y,8+o->attack*.12f,1,.15f,.4f,.4f);}else{beam_ribbon(x,y,exx,ey,5,(SDL_FColor){.75f,.1f,1,.17f});beam_ribbon(x,y,exx,ey,2.2f,(SDL_FColor){1,.15f,.5f,.6f});beam_ribbon(x,y,exx,ey,.65f,(SDL_FColor){1,.9f,1,.95f});light(x,y,17,1,.15f,.7f,.55f);}}
 for(int i=0;i<MAXH;i++){Obj*o=ex.obstacles+i;if(!o->on||o->type<4)continue;float x=o->x/(float)Q,y=o->y/(float)Q;SDL_FColor c=o->type==5?(SDL_FColor){.2f,.8f,1,.28f}:o->type==6?(SDL_FColor){1,.3f,.05f,.35f}:(SDL_FColor){.65f,.5f,.35f,.12f};beam_ribbon(x,y,x-o->vx*.025f,y-o->vy*.025f,2,c);light(x,y,o->base*1.3f,c.r,c.g,c.b,c.a);}
 if(ship_level()>1){float x=g.px/(float)Q,y=player_y/(float)Q;for(int side=-1;side<=1;side+=2){light(x+side*(3+ship_level()),y+6,3+ship_level(),.12f,.7f,1,.17f);light(x+side*(3+ship_level()),y+6,.6f,.8f,1,1,.65f);}}
 if(ex.satellite)light(satellite_x(),satellite_y(),7,.7f,.35f,1,.2f);
 for(int i=0;i<MAXE;i++){Obj*o=g.enemies+i;if(!o->on)continue;float x=o->x/256.0f,y=o->y/256.0f;if(o->type==4&&(o->shieldCharge>0||(o->attack>60&&o->attack<=85))){float r=38+o->power/12.0f;for(int k=0;k<64;k++){float a=k*2*PI/64+visualTick*.012f;light(x+cosf(a)*r,y+sinf(a)*r*.72f,1.1f,.15f,.75f,1,.4f);}}if(o->type==4&&o->morph>0){float r=24+(55-o->morph)*.65f;light(x,y,35, .25f,.7f,1,.18f);for(int n=0;n<32;n++){float a=n*2*PI/32+visualTick*.04f;light(x+cosf(a)*r,y+sinf(a)*r*.75f,.95f,.3f,.8f,1,.7f);}}if(o->type>=23&&o->age%90>=55&&o->age%90<70)light(x-7,y,3,1,.2f,.05f,.7f);if(enemy_guarded(o)&&o->type!=4)for(int j=0;j<32;j++){float a=j*2*PI/32+g.tick*.02f;light(x+cosf(a)*18,y+sinf(a)*18,.5f,.2f,.65f,1,.35f);}if((o->type==17&&o->age>=80&&o->age<100)||(o->type==21&&o->age%150>=80&&o->age%150<115))beam_ribbon(x,y,g.px/256.0f,player_y/256.0f,.15f,(SDL_FColor){1,.25f,.12f,.35f});}
 if(ex.rearWarning)for(int n=0;n<3;n++){float x=clamp((g.px>>8)+(n-1)*62,28,292),y=249;beam_ribbon(x-3,y-3,x,y,.6f,(SDL_FColor){1,.3f,.1f,.7f});beam_ribbon(x,y,x+3,y-3,.6f,(SDL_FColor){1,.3f,.1f,.7f});}
 if(g.beam)for(int lane=ex.multi?-1:0;lane<=(ex.multi?1:0);lane++)rainbow_beam(g.px/256.0f+lane*28,player_y/256.0f-7);
 if(ex.bombAge){flush(halo);float r=bombRenderRadius,fade=fminf(1,(61-ex.bombAge)/10.0f),cx=(float)ex.bombX,cy=(float)ex.bombY;quad((cx-r)*3,(cy-r*.8f)*3,r*6,r*4.8f,(SDL_FColor){.08f,.35f,1,.11f*fade},0,0,1,1);for(int j=0;j<256;j++){float a=j*2*PI/256;light(cx+cosf(a)*r,cy+sinf(a)*r*.8f,5.5f,.1f,.5f,1,.8f*fade);light(cx+cosf(a)*r*.96f,cy+sinf(a)*r*.768f,2,.55f,.9f,1,.4f*fade);}}
 }flush(halo);
 if(ex.bombAge){SDL_FPoint points[257];SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_ADD);float fade=fminf(1,(61-ex.bombAge)/10.0f);for(int line=-1;line<=1;line++){float r=bombRenderRadius+line*2;for(int j=0;j<=256;j++){float a=j*2*PI/256;points[j]=(SDL_FPoint){(ex.bombX+cosf(a)*r)*3,(ex.bombY+sinf(a)*r*.8f)*3};}SDL_SetRenderDrawColor(renderer,line?60:235,line?160:255,255,(Uint8)((line?140:255)*fade));SDL_RenderLines(renderer,points,257);}SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);}
 SDL_SetRenderClipRect(renderer,0);
}
static float wrap_positive(float x,float period){return x-floorf(x/period)*period;}
/* Mirrored repeats meet at identical edge pixels, avoiding jumps when a layer wraps. */
static void scrolling_layer(SDL_Texture*t,float speed,float scale,int alpha,int clouds){
 int taps=!clouds&&speed>=300?(quality>=2?4:quality?2:1):1;
 for(int sample=0;sample<taps;sample++){float shutter=(taps==1?0:(float)(taps-1-sample)/(taps-1))*.35f*backgroundFrameDt*fminf(8,fmaxf(.45f,ex.scrollSpeed/192.0f));float clock=backgroundClock-shutter;
 float w=1114*scale,h=742*scale,a=sideBlend*PI*.5f,dx=-sinf(a)*clock*speed,dy=cosf(a)*clock*speed;
 float x=wrap_positive(dx,w)-w,y=72+wrap_positive(dy,h)-h;
 SDL_SetTextureAlphaMod(t,(Uint8)clamp(alpha/(sample+1),0,255));
 int col0=-1-(int)floorf(dx/w),row0=-1-(int)floorf(dy/h);
 for(int row=0;row<3;row++)for(int col=0;col<3;col++){
  SDL_FRect dest={x+col*w,y+row*h,w,h};if(dest.x>960||dest.y>768||dest.x+w<0||dest.y+h<72)continue;
  int flip=(((col+col0)&1)?SDL_FLIP_HORIZONTAL:0)|(((row+row0)&1)?SDL_FLIP_VERTICAL:0);
  SDL_RenderTextureRotated(renderer,t,0,&dest,0,0,(SDL_FlipMode)flip);
 }
 }
 (void)clouds;
}
static void scene_plate(int phase,int alpha){phase=clamp(phase,0,11);int index=phase>=6?phase-4:0;SDL_Texture*t=scenes[index];if(!t)return;
 SDL_SetTextureColorMod(t,phase==1?190:phase==4?245:255,phase==1?170:phase==4?180:255,phase==4?190:255);
 scrolling_layer(t,phase==10?780:phase==11?38:phase>=6?13:3,1.2f,alpha,0);
 if(cloudTexture){renderedCloudLayers+=2;
 if(phase==10){SDL_SetTextureBlendMode(cloudTexture,SDL_BLENDMODE_BLEND);SDL_SetTextureColorMod(cloudTexture,215,235,255);scrolling_layer(cloudTexture,300,1.7f,alpha*.12f,1);scrolling_layer(cloudTexture,650,1.15f,alpha*.18f,1);}
 else if(phase>=6&&phase!=10){SDL_SetTextureBlendMode(cloudTexture,SDL_BLENDMODE_BLEND);SDL_SetTextureColorMod(cloudTexture,255,phase==7?218:255,phase==7?185:255);scrolling_layer(cloudTexture,880,1.35f,alpha*.70f,1);scrolling_layer(cloudTexture,1920,.85f,alpha*.95f,1);}
 else{SDL_SetTextureBlendMode(cloudTexture,SDL_BLENDMODE_ADD);SDL_SetTextureColorMod(cloudTexture,55,130,225);scrolling_layer(cloudTexture,100,1.7f,alpha*.40f,1);SDL_SetTextureColorMod(cloudTexture,150,60,230);scrolling_layer(cloudTexture,360,1.15f,alpha*.48f,1);SDL_SetTextureBlendMode(cloudTexture,SDL_BLENDMODE_BLEND);}
 }
}
static void draw_star_layers(int alpha){
 float a=sideBlend*PI*.5f,dx=-sinf(a),dy=cosf(a);nquad=0;visibleStars=0;
 for(int layer=0;layer<3;layer++)for(int i=0;i<80;i++){
  unsigned hash=(unsigned)(i+layer*97+1)*2654435761u;float speed=12+layer*29,size=.8f+layer*.65f;
  float x=wrap_positive((hash%10007)*.096f+dx*backgroundClock*speed-(g.px/256.0f-160)*layer*.08f,960);
  float y=72+wrap_positive(((hash>>12)%10007)*.0695f+dy*backgroundClock*speed,696);
  float bright=(.45f+layer*.2f)*alpha/255.0f;quad(x-size*3,y-size*3,size*6,size*6,(SDL_FColor){.65f,.83f,1,bright*.8f},0,0,1,1);
  quad(x-size*.5f,y-size*.5f,size,size,(SDL_FColor){.9f,.96f,1,bright},0,0,1,1);visibleStars++;
 }flush(halo);
}
static void draw_modern_world(void){SDL_Rect clip={0,72,960,696};SDL_SetRenderClipRect(renderer,&clip);int phase=ex.phase<0?0:ex.phase;renderedCloudLayers=0;if(ex.entryY){scene_plate(ex.oldPhase,255);scene_plate(phase,(ex.entryY-24)*255/232);}else scene_plate(phase,255);
 SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);SDL_SetRenderDrawColor(renderer,1,6,16,45);SDL_FRect shade={0,72,960,696};SDL_RenderFillRect(renderer,&shade);
 int sky=(phase<=5||phase==10)?255:0;if(ex.entryY){int blend=clamp((ex.entryY-24)*255/232,0,255);sky=((ex.oldPhase<=5||ex.oldPhase==10)?255-blend:0)+((phase<=5||phase==10)?blend:0);}visibleStars=0;if(sky)draw_star_layers(sky);
 SDL_SetRenderClipRect(renderer,0);
}
static void draw_modern_terrain(void){SDL_Rect clip={0,72,960,696};SDL_SetRenderClipRect(renderer,&clip);
 if(ex.phase==11){nquad=0;quad(288*3,72,32*3,696,(SDL_FColor){.38f,.43f,.48f,1},.2f,.1f,.12f,.8f);flush(scenes[1]);SDL_SetRenderDrawColor(renderer,35,48,58,255);for(int n=0;n<14;n++){float y=72+fmodf(n*64+backgroundClock*300,768);SDL_RenderLine(renderer,290*3,y,320*3,y);}SDL_SetRenderDrawColor(renderer,143,177,187,255);SDL_RenderLine(renderer,288*3,72,288*3,768);}
 if(ex.phase==5||(ex.oldPhase==5&&ex.entryY)){SDL_SetTextureColorMod(scenes[1],255,255,255);nquad=0;
 for(int y=24;y<256;y+=2)if(tunnel_visible(y)){float l=tunnel_left(y),r=tunnel_right(y),ll=tunnel_left(y+2),rr=tunnel_right(y+2),v=fmodf(((ex.backgroundY>>8)+y)*4.8f,1024)/1024.0f,vh=fminf(9.6f/1024,1-v);SDL_FColor colour={.75f,.82f,.88f,1};
 quad(0,y*3,l*3,6,colour,0,v,l/320,vh);vertices[nquad*4-2].position.x=ll*3;vertices[nquad*4-2].tex_coord.x=ll/320;
 quad(r*3,y*3,(320-r)*3,6,colour,r/320,v,(320-r)/320,vh);vertices[nquad*4-1].position.x=rr*3;vertices[nquad*4-1].tex_coord.x=rr/320;
 }flush(scenes[1]);}

SDL_SetRenderClipRect(renderer,0);}
static int lerpi(int a,int b,float t){return (int)(a+(b-a)*t);}
static void interp_objects(Obj*a,Obj*b,int n,float t){for(int i=0;i<n;i++)if(a[i].on&&b[i].on&&a[i].type==b[i].type){a[i].x=lerpi(b[i].x,a[i].x,t);a[i].y=lerpi(b[i].y,a[i].y,t);if(a[i].type==4)a[i].power=lerpi(b[i].power,a[i].power,t);int turn=(a[i].heading-b[i].heading+8640)%5760-2880;a[i].heading=(b[i].heading+(int)(turn*t)+5760)%5760;}}
static void render_frame(float alpha,float dt){visualTick=previous.tick+(g.tick-previous.tick)*alpha;backgroundFrameDt=fminf(dt,.033f);if(!g.paused&&g.mode==1)backgroundClock+=dt*fminf(8,fmaxf(.45f,ex.scrollSpeed/192.0f));sideBlend+=(side_mode()-sideBlend)*fminf(1,dt*4);if(fabsf(sideBlend-side_mode())<.0005f)sideBlend=side_mode();saved=g;savedEx=ex;int sy=player_y;g.px=lerpi(previous.px,g.px,alpha);player_y=lerpi(previousY,player_y,alpha);interp_objects(g.enemies,previous.enemies,MAXE,alpha);interp_objects(g.shots,previous.shots,MAXS,alpha);interp_objects(g.bullets,previous.bullets,MAXB,alpha);interp_objects(g.drops,previous.drops,8,alpha);
 int delta=(ex.backgroundY-previousEx.backgroundY)&131071;if(delta>65535)delta-=131072;ex.backgroundY=(previousEx.backgroundY+(int)(delta*alpha))&131071;for(int i=0;i<NSTARS;i++){int d=g.stars[i].y-previous.stars[i].y;if(d<0)d+=232*Q;g.stars[i].y=(previous.stars[i].y+(int)(d*alpha))%(232*Q);}
 bombRenderRadius=ex.bombAge?((previousEx.bombAge?previousEx.bombRadius:0)+(ex.bombRadius-(previousEx.bombAge?previousEx.bombRadius:0))*alpha):0;ndraw=0;game_render(fb);for(int i=0;i<81920;i++)rgba[i]=fb[i]?palette[fb[i]]:0;SDL_UpdateTexture(base,0,rgba,320*4);SDL_SetRenderTarget(renderer,combatTarget);SDL_SetRenderLogicalPresentation(renderer,960,800,SDL_LOGICAL_PRESENTATION_STRETCH);SDL_SetRenderDrawColor(renderer,0,0,0,0);SDL_RenderClear(renderer);draw_modern_terrain();SDL_FRect area={0,0,960,768};SDL_RenderTexture(renderer,base,0,&area);
 SDL_Rect clip={0,72,960,696};SDL_SetRenderClipRect(renderer,&clip);nquad=0;SDL_Texture*batchTexture=atlas;for(int i=0;i<ndraw;i++){Draw*d=draws+i;SDL_Texture*texture=d->n>=67&&d->n<91?bossFrames:atlas;if(texture!=batchTexture){flush(batchTexture);batchTexture=texture;}Sprite*s=sprites+d->n;float scale=(d->n==0||d->n==50||(d->n>=94&&d->n<=99)||(d->n>=103&&d->n<=110))?.5f:((d->n>=51&&d->n<=57)||(d->n>=100&&d->n<=102))?.95f+.10f*sinf(g.tick*.14f+d->x):d->scale>0?d->scale:1;quad((d->x-s->w*.5f*scale)*3,(d->y-s->h*.5f*scale)*3,(s->w*3*scale)*(d->n>=28&&d->n<32?(1+.09f*sinf(g.tick*.7f+d->x)):1),s->h*3*scale,(SDL_FColor){1,1,1,d->opacity>0?d->opacity:1},s->u,s->v,s->uw,s->vh);if(d->angle!=0){float angle=d->angle*PI/180,c=cosf(angle),ss=sinf(angle),cx=d->x*3,cy=d->y*3;for(int v=(nquad-1)*4;v<nquad*4;v++){float xx=vertices[v].position.x-cx,yy=vertices[v].position.y-cy;vertices[v].position.x=cx+xx*c-yy*ss;vertices[v].position.y=cy+xx*ss+yy*c;}}unwarp_quad(d->x*3,d->y*3);}flush(batchTexture);draw_hull_damage();draw_smoke();SDL_SetRenderClipRect(renderer,0);SDL_SetRenderTarget(renderer,fxTarget);SDL_SetRenderLogicalPresentation(renderer,960,800,SDL_LOGICAL_PRESENTATION_STRETCH);SDL_SetRenderDrawColor(renderer,0,0,0,255);SDL_RenderClear(renderer);draw_fx(dt);SDL_SetRenderTarget(renderer,combatTarget);SDL_SetRenderLogicalPresentation(renderer,960,800,SDL_LOGICAL_PRESENTATION_STRETCH);SDL_FRect fxArea={0,0,960,800};SDL_RenderTexture(renderer,fxTarget,0,&fxArea);

 float envelope=warp_envelope(alpha);cameraZoom=1+(8+2.5f*sideBlend)*envelope;SDL_SetRenderTarget(renderer,envelope>0?worldTarget:0);SDL_SetRenderLogicalPresentation(renderer,960,800,envelope>0?SDL_LOGICAL_PRESENTATION_STRETCH:SDL_LOGICAL_PRESENTATION_LETTERBOX);SDL_SetRenderDrawColor(renderer,3,5,14,255);SDL_RenderClear(renderer);draw_modern_world();float tw,th;SDL_GetTextureSize(combatTarget,&tw,&th);SDL_FRect source={0,th*72/800,tw,th*696/800};float w=960-264*sideBlend,h=696+264*sideBlend;SDL_FRect destination={480-w/2,420-h/2,w,h};SDL_Rect worldClip={0,72,960,696};SDL_SetRenderClipRect(renderer,&worldClip);SDL_RenderTextureRotated(renderer,combatTarget,&source,&destination,sideBlend*90,0,SDL_FLIP_NONE);SDL_SetRenderClipRect(renderer,0);
 cameraX=0;cameraY=72;if(envelope>0){float a=sideBlend*PI*.5f,xx=(g.px/256.0f*3-480)*w/960,yy=(player_y/256.0f*3-420)*h/696,sx=480+xx*cosf(a)-yy*sinf(a),sy=420+xx*sinf(a)+yy*cosf(a);float focusX=sx+(480-sx)*envelope,focusY=sy+(420-sy)*envelope;cameraX=fmaxf(960*(1-cameraZoom),fminf(0,focusX-sx*cameraZoom));cameraY=72+fmaxf(696*(1-cameraZoom),fminf(0,focusY-72-(sy-72)*cameraZoom));SDL_SetRenderTarget(renderer,0);SDL_SetRenderLogicalPresentation(renderer,960,800,SDL_LOGICAL_PRESENTATION_LETTERBOX);SDL_SetRenderDrawColor(renderer,3,5,14,255);SDL_RenderClear(renderer);float fw,fh;SDL_GetTextureSize(worldTarget,&fw,&fh);SDL_FRect crop={0,fh*72/800,fw,fh*696/800},zoomed={cameraX,cameraY,960*cameraZoom,696*cameraZoom};SDL_SetRenderClipRect(renderer,&worldClip);SDL_RenderTexture(renderer,worldTarget,&crop,&zoomed);float ax,ay;SDL_GetTextureSize(atlas,&ax,&ay);for(int which=0;which<2;which++){float weight=which?sideBlend:1-sideBlend;if(weight<.001f)continue;int rank=ship_level();Sprite*ship=sprites+(rank>1?(rank<=4?(which?97:94)+rank-2:(which?107:103)+rank-5):(which?50:0));SDL_FRect ss={ship->u*ax,ship->v*ay,ship->uw*ax,ship->vh*ay},dst={cameraX+sx*cameraZoom-ship->w*.75f*cameraZoom,cameraY+(sy-72)*cameraZoom-ship->h*.75f*cameraZoom,ship->w*1.5f*cameraZoom,ship->h*1.5f*cameraZoom};SDL_SetTextureAlphaMod(atlas,(Uint8)(255*weight));SDL_RenderTextureRotated(renderer,atlas,&ss,&dst,which?0:sideBlend*90,0,SDL_FLIP_NONE);}SDL_SetTextureAlphaMod(atlas,255);SDL_SetRenderDrawColor(renderer,2,5,12,(Uint8)(envelope*210));SDL_FRect bars[2]={{0,72,960,envelope*22},{0,768-envelope*22,960,envelope*22}};SDL_RenderFillRects(renderer,bars,2);SDL_SetRenderClipRect(renderer,0);}
 g=saved;ex=savedEx;player_y=sy;
}
static void ui_text(int x,int y,const char*s,SDL_Color c){SDL_SetRenderDrawColor(renderer,c.r,c.g,c.b,c.a);while(*s){int glyph=-1;for(int i=0;fontchars[i];i++)if(fontchars[i]==*s){glyph=i;break;}if(glyph>=0)for(int j=0;j<7;j++)for(int k=0;k<5;k++)if(font[glyph*7+j]&(1<<(4-k))){SDL_FRect r={x+k*2.0f,y+j*2.0f,2,2};SDL_RenderFillRect(renderer,&r);}x+=12;s++;}}
static void fatal(const char*s){SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"Star Guard PC",s,window);exit(1);}
static void screenshot(const char*name){SDL_Surface*s=SDL_RenderReadPixels(renderer,0);if(s){SDL_SaveBMP(s,name);SDL_DestroySurface(s);}}
static double ms_now(void){return SDL_GetPerformanceCounter()*1000.0/SDL_GetPerformanceFrequency();}
static int compare_double(const void*a,const void*b){double x=*(const double*)a,y=*(const double*)b;return (x>y)-(x<y);}
static void selftest(void){
 headless=1;int seen[26]={0},weapons=0,checks=0,phaseSeen[12]={0};double begin=ms_now();
 #define CHECK(x) do{if(!(x)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);exit(4);}checks++;}while(0)
 game_reset();CHECK(g.unlocked==255&&ex.bombs==3&&ex.options==0);game_step(4,0);CHECK(g.mode==1);
 for(int w=0;w<8;w++){game_reset();g.mode=1;g.shield=100000;ex.optionsOn=0;g.weapon=w;g.cool=0;g.energy=100;game_step(4,0);if(w==3||w==5)CHECK(g.beam>0&&g.energy<100);else{int n=0;for(int i=0;i<MAXS;i++)n+=g.shots[i].on;CHECK(n==(w==1||w==7?3:1));}weapons|=1<<w;}
 game_reset();g.mode=1;g.shield=0;bullet(g.px,player_y,0,0);game_step(0,0);CHECK(g.barrier==65&&g.shield>0);g.shield=0;g.barrier=0;bullet(g.px,player_y,0,0);game_step(0,0);CHECK(g.lives==2);
 for(int i=0;i<7;i++){drop(g.px>>8,player_y>>8,i);game_step(0,0);}CHECK(g.lives==3&&g.barrier==100&&g.shield>0);
 int t=g.tick;game_step(0,'p');CHECK(g.tick==t&&g.paused);game_step(0,'p');CHECK(!g.paused);g.energy=100;game_step(0,'d');CHECK(g.active>0&&g.energy==65);
 g.bullets[0].on=1;int bombs=ex.bombs;g.bullets[0].x=g.px;g.bullets[0].y=player_y;game_step(0,'b');CHECK(ex.bombs==bombs-1&&ex.bombAge>0&&!g.bullets[0].on);game_step(0,'o');CHECK(!ex.optionsOn);
 game_reset();g.mode=1;g.wave=5;g.clock=2400;g.groups=SECTOR_WAVES;ex.rearSpawned=1;for(int i=0;i<101;i++)game_step(0,0);CHECK(g.enemies[0].type==4);for(int i=0;i<121;i++)game_step(0,0);g.enemies[0].hp=g.enemies[0].base/2;game_step(0,0);CHECK(g.bossPhase==2);g.enemies[0].hp=g.enemies[0].base/5;game_step(0,0);CHECK(g.bossPhase==3);

 game_reset();g.mode=1;g.clock=100;g.groups=SECTOR_WAVES;ex.optionsOn=0;g.shield=100000;g.enemies[0]=(Obj){.on=1,.x=160*Q,.y=100*Q,.base=160,.type=3,.hp=99};g.shots[0]=(Obj){.on=1,.x=160*Q,.y=114*Q,.vy=-4*Q,.type=6,.hp=4,.phase=-1};game_step(0,0);CHECK(g.shots[0].on&&g.shots[0].hp==3&&g.shots[0].vy>0&&absn(g.shots[0].vx)>0);int ehp=g.enemies[0].hp;game_step(0,0);CHECK(g.enemies[0].hp==ehp);for(int i=0;i<3;i++)ricochet(g.shots,g.enemies,0);CHECK(!g.shots[0].on&&g.shots[0].hp==0);
 for(int type=10;type<14;type++){game_reset();g.mode=1;g.groups=SECTOR_WAVES;g.clock=100;g.shield=100000;ex.optionsOn=0;bug_group(type,0);int count=0;for(int i=0;i<MAXE;i++)count+=g.enemies[i].on;CHECK(count>=24);for(int tick=0;tick<150;tick++)game_step(0,0);for(int i=0;i<count;i++)CHECK(g.enemies[i].on&&g.enemies[i].type==type&&g.enemies[i].x>0&&g.enemies[i].x<320*Q&&g.enemies[i].y>24*Q&&g.enemies[i].y<130*Q);}
 game_reset();g.mode=1;g.clock=100;g.groups=SECTOR_WAVES;g.shield=100000;ex.optionsOn=0;g.enemies[0]=(Obj){.on=1,.x=178*Q,.y=230*Q,.base=178,.type=0,.hp=1};g.enemies[1]=(Obj){.on=1,.x=160*Q,.y=40*Q,.base=160,.type=0,.hp=1};g.bullets[0]=(Obj){.on=1,.x=160*Q,.y=40*Q};game_step(0,'b');CHECK(!g.enemies[0].on&&g.enemies[1].on&&g.bullets[0].on);CHECK(ex.bombX==160&&ex.bombY==230);for(int i=0;i<40;i++)game_step(0,0);CHECK(!g.enemies[1].on&&!g.bullets[0].on);for(int x=0;x<=320;x+=320)for(int y=24;y<=256;y+=232)CHECK(bomb_reached(x,y,0));for(int i=0;i<40;i++)game_step(0,0);CHECK(!ex.bombAge);
 for(int y=0;y<1024;y++)CHECK(tunnel_left(y)>0&&tunnel_right(y)<320&&tunnel_right(y)-tunnel_left(y)>=60);
 game_reset();g.mode=1;for(int tick=0;tick<60000;tick++){for(int i=0;i<MAXE;i++)if(g.enemies[i].on)seen[g.enemies[i].type]=1;for(int i=0;i<MAXE;i++)if(g.enemies[i].on){g.px=clamp(g.enemies[i].x,14*Q,306*Q);seen[g.enemies[i].type]=1;break;}if(ex.phase==5)g.px=tunnel_center(220)*Q;if(tick%150==0){g.energy=100;g.weapon=(tick/150)%7;}g.shield=100000;game_step(4,0);if(ex.phase>=0)phaseSeen[ex.phase]=1;CHECK(g.energy>=0&&g.energy<=100&&g.px>=14*Q&&g.px<=306*Q&&g.weapon<8);for(int i=0;i<MAXE;i++)if(g.enemies[i].on&&g.enemies[i].type==4&&(tick&127)==0)g.enemies[i].hp-=8;if(tick%97==0){ndraw=0;game_render(fb);}}
 for(int i=0;i<26;i++){if(!seen[i])fprintf(stderr,"Unseen enemy %d\n",i);CHECK(seen[i]);}for(int i=0;i<12;i++)CHECK(phaseSeen[i]);CHECK(weapons==255&&g.wave>12&&g.kills>400);
 headless=0;game_reset();g.weapon=2;sound(1,1,100,0x4000);sound_slide(1,1,0x2000,20);float audio[48000*2];mix_audio(audio,48000);CHECK(audioPeak>0&&audioPeak<1&&audioNonzero>1000);sound(2,2,100,0x3000);sound_slide(2,0,0,30);mix_audio(audio,48000);CHECK(SDL_GetAtomicInt(&audioErrors)==0&&SDL_GetAtomicInt(&audioDrops)==0);
 FILE*f=fopen("SELF-TEST.json","w");if(f){fprintf(f,"{\"passed_assertions\":%d,\"game_ticks\":60000,\"weapons\":8,\"enemy_types\":25,\"sectors\":12,\"sound_peak\":%.6f,\"sound_nonzero_samples\":%d,\"elapsed_ms\":%.2f}\n",checks,audioPeak,audioNonzero,ms_now()-begin);fclose(f);}printf("PASS %d checks; 60000 ticks, eight weapons, twenty-six enemies, twelve sectors, audio finite and unclipped\n",checks);
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
EMSCRIPTEN_KEEPALIVE int web_stat(int n){switch(n){case 0:return g.tick;case 1:return g.mode;case 2:return g.weapon;case 3:return ex.bombs;case 4:return ex.bombAge;case 5:return aliveParticles;case 6:return g.paused;case 7:return SDL_GetAtomicInt(&audioFrames);case 8:return audioNonzero;case 9:return SDL_GetAtomicInt(&audioErrors);case 10:return g.wave;case 11:return peakParticles;case 12:return (int)(audioPeak*1000000);case 13:return frames;case 14:return g.playerVX;case 15:return ex.phase;case 16:return ex.travel;case 17:return ex.bossSpawned;case 18:return muted;case 19:return g.score;case 20:return g.lives;case 21:return g.barrier;case 22:return g.energy;case 23:{int count=0;for(int i=0;i<MAXE;i++)count+=g.enemies[i].on;return count;}case 24:for(int i=0;i<MAXE;i++)if(g.enemies[i].on&&g.enemies[i].type==4)return g.enemies[i].variant+1;return 0;case 25:for(int i=0;i<MAXE;i++)if(g.enemies[i].on&&g.enemies[i].type==4)return g.enemies[i].hp*100/g.enemies[i].base;return 0;case 26:return ex.optionsOn?ex.options:0;case 27:return (int)recentFPS;case 28:return side_mode();case 29:return g.px;case 30:return player_y;case 31:return visibleStars;case 32:return (int)(backgroundClock*1000);case 33:return ex.bossWarning;case 34:return ex.bossSpawned&&!route_occupied();case 35:for(int i=0;i<MAXE;i++)if(g.enemies[i].on&&g.enemies[i].type==4)return g.enemies[i].y;return -99999;case 36:return renderedCloudLayers;case 37:return power_level(g.weapon);case 38:return ex.satellite;case 39:return ex.dropper;case 40:return ex.pickupKind;case 41:return ex.pickupTicks;case 42:return ex.rearWarning;case 43:return (int)(cameraZoom*1000);case 48:return storm_kind();case 49:return g.clock;case 50:return g.groups;case 51:return SECTOR_WAVES;case 52:for(int i=0;i<MAXE;i++)if(g.enemies[i].on&&g.enemies[i].type==4)return g.enemies[i].attack;return 0;case 53:{int n=0;for(int i=0;i<MAXH;i++)n+=ex.obstacles[i].on;return n;}case 61:return g.weaponCharge[g.weapon];case 62:return power_needed(g.weapon);case 60:for(int i=0;i<MAXE;i++)if(g.enemies[i].on&&g.enemies[i].type==4)return g.enemies[i].shieldCharge;return 0;case 56:return ship_level();case 57:return g.acquiredWeapons;case 58:return g.salvagePicked;case 59:return g.lastLootValue;case 55:return g.missionTicks;case 45:return g.bossPhase;case 46:for(int i=0;i<MAXE;i++)if(g.enemies[i].on&&g.enemies[i].type==4)return g.enemies[i].morph;return 0;case 47:for(int i=0;i<MAXE;i++)if(g.enemies[i].on&&g.enemies[i].type==4)return clamp((g.enemies[i].power+128)/256,0,2);return 0;case 44:for(int i=0;i<MAXE;i++)if(g.enemies[i].on&&g.enemies[i].type==4)return boss_damage_level(g.enemies+i);return 0;}return -1;}
static void browser_frame(void){
double frameStart=ms_now(),delta=(frameStart-before)/1000;before=frameStart;if(delta>.1)delta=.1;
 while(SDL_PollEvent(&event)){SDL_ConvertEventToRenderCoordinates(renderer,&event);if(event.type==SDL_EVENT_QUIT)cmd='p';if(event.type==SDL_EVENT_WINDOW_FOCUS_LOST&&!demo){focus=0;if(astream){SDL_ClearAudioStream(astream);SDL_PauseAudioStreamDevice(astream);}}if(event.type==SDL_EVENT_WINDOW_FOCUS_GAINED){focus=1;acc=0;if(astream)SDL_ResumeAudioStreamDevice(astream);}if(event.type==SDL_EVENT_MOUSE_MOTION){mx=event.motion.x/3;my=event.motion.y/3;if(ticks>5)mouseControl=1;}if(event.type==SDL_EVENT_MOUSE_BUTTON_DOWN&&event.button.button==SDL_BUTTON_LEFT){mouseFire=1;cmd=32;}if(event.type==SDL_EVENT_MOUSE_BUTTON_UP&&event.button.button==SDL_BUTTON_LEFT)mouseFire=0;if(event.type==SDL_EVENT_MOUSE_BUTTON_DOWN&&event.button.button==SDL_BUTTON_RIGHT)cmd='b';
 if(event.type==SDL_EVENT_KEY_DOWN&&!event.key.repeat){SDL_Keycode k=event.key.key;if(k>=32&&k<127&&(k!=32||!cmd))cmd=(int)k;if(k==SDLK_ESCAPE)cmd='p';if(k==SDLK_F11){fullscreen=!fullscreen;SDL_SetWindowFullscreen(window,fullscreen);}if(k=='m'){muted=!muted;if(astream)SDL_SetAudioStreamGain(astream,muted?0:1);}if(k=='v'){vsync=!vsync;SDL_SetRenderVSync(renderer,vsync);}if(k==SDLK_F2){cap=cap==120?240:cap==240?0:120;}if(k==SDLK_F3)showStats=!showStats;if(k==SDLK_F4)quality=(quality+1)%3;if(k==SDLK_F12){screenshot("Screenshot.bmp");}if(k=='z'||k=='x'||k==SDLK_LEFT||k==SDLK_RIGHT||k==SDLK_UP||k==SDLK_DOWN||k=='w'||k=='a'||k=='s')mouseControl=0;}
 if(event.type==SDL_EVENT_GAMEPAD_BUTTON_DOWN){switch(event.gbutton.button){case SDL_GAMEPAD_BUTTON_SOUTH:cmd=32;break;case SDL_GAMEPAD_BUTTON_EAST:cmd='b';break;case SDL_GAMEPAD_BUTTON_WEST:cmd='d';break;case SDL_GAMEPAD_BUTTON_NORTH:cmd='o';break;case SDL_GAMEPAD_BUTTON_START:cmd='p';break;case SDL_GAMEPAD_BUTTON_LEFT_SHOULDER:cmd='1'+(g.weapon+7)%8;break;case SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER:cmd='1'+(g.weapon+1)%8;break;}}if(event.type==SDL_EVENT_GAMEPAD_ADDED&&!pad)pad=SDL_OpenGamepad(event.gdevice.which);if(event.type==SDL_EVENT_GAMEPAD_REMOVED&&pad&&SDL_GetGamepadID(pad)==event.gdevice.which){SDL_CloseGamepad(pad);pad=0;}}
 const bool*k=SDL_GetKeyboardState(0);keysHeld=webHeld|(k[SDL_SCANCODE_SPACE]||mouseFire?4:0);
 if(webCmd){cmd=webCmd;webCmd=0;}if(!focus){delta=0;acc=0;}acc+=delta;int steps=0;
 while(acc>=.02&&steps++<5){previous=g;previousEx=ex;previousY=player_y;int mask=keysHeld;
 if(demo){int phase=(int)((frameStart-start)/5000)%6;if(g.mode!=1||g.lives<1){game_reset();g.mode=1;}g.shield=100000;g.weapon=((int)((frameStart-start)/1500))%7;g.energy=100;int wave=phase==5?(stress?28:3):(stress?26:1)+phase;if(g.wave!=wave||((ex.phase==5)!=(phase==5))){g.wave=wave;g.clock=phase==5?300:0;g.groups=0;if(phase==5)ex.phase=-1;}for(int i=0;i<MAXE;i++)if(g.enemies[i].on&&g.enemies[i].y>24*Q){g.px=clamp(g.enemies[i].x,14*Q,306*Q);break;}if(phase==5)g.px=tunnel_center(210)*Q;mask=4;if(stress&&ticks%30==0)for(int j=0;j<3;j++)pc_burst(30+(ticks*31+j*97)%260,55+(ticks*17+j*41)%150,1);if(ticks%400==220)cmd='b';if((ticks%150)==0)pc_burst(40+(ticks*31)%240,70+(ticks*17)%140,ticks%300==0);}
 else if(mouseControl&&g.mode==1&&!g.paused){int px=(int)mx,py=(int)my;flight_pointer(&px,&py);game_pointer(px,py);}
if(showcase){g.wave=1;g.clock=100;g.groups=SECTOR_WAVES;g.px=160*Q;player_y=220*Q;g.weapon=6;ex.optionsOn=0;mask=ticks>125?4:0;if(ticks==0){bug_group(10,0);for(int i=0;i<MAXE;i++)if(g.enemies[i].on){g.enemies[i].type=10+(i&3);g.enemies[i].hp=8;}}cmd=ticks==180?'b':0;}

 if(!mouseControl&&!g.paused){int sx=(k[SDL_SCANCODE_RIGHT]||k[SDL_SCANCODE_X])-(k[SDL_SCANCODE_LEFT]||k[SDL_SCANCODE_Z]||k[SDL_SCANCODE_A]);int sy=(k[SDL_SCANCODE_DOWN]||k[SDL_SCANCODE_S])-(k[SDL_SCANCODE_UP]||k[SDL_SCANCODE_W]);flight_move(sx*6,sy*4);}
 if(pad){float ax=SDL_GetGamepadAxis(pad,SDL_GAMEPAD_AXIS_LEFTX)/32768.0f,ay=SDL_GetGamepadAxis(pad,SDL_GAMEPAD_AXIS_LEFTY)/32768.0f;if(fabsf(ax)>.15f||fabsf(ay)>.15f){mouseControl=0;flight_move(ax*6,ay*5);}if(SDL_GetGamepadButton(pad,SDL_GAMEPAD_BUTTON_SOUTH)||SDL_GetGamepadAxis(pad,SDL_GAMEPAD_AXIS_RIGHT_TRIGGER)>12000)mask|=4;}
 game_step(mask|(cmd==32?4:0),cmd);cmd=0;acc-=.02;ticks++;
 }if(steps>5)acc=0;
 if(focus&&!g.paused)update_fx((float)delta);render_frame((float)fmin(acc/.02,1),(focus&&!g.paused)?(float)delta:0);
 double cpu=ms_now()-frameStart;cpuSum+=cpu;if(cpu>maxCpu)maxCpu=cpu;SDL_RenderPresent(renderer);double work=ms_now()-frameStart;double total=delta*1000;frameSum+=total;frames++;recentMS+=total;recentCPU+=cpu;recentFrames++;if(recentMS>=500){recentFPS=(float)(1000*recentFrames/recentMS);recentCPUms=(float)(recentCPU/recentFrames);recentMS=recentCPU=0;recentFrames=0;}if(sampleCount<200000){frameTimes[sampleCount]=total;cpuTimes[sampleCount]=cpu;sampleCount++;}
 if(showcase){if(ticks>=125&&showcaseShots==0){screenshot("Formation-Preview.bmp");showcaseShots=1;}if(ex.bombAge>=12&&showcaseShots==1){screenshot("Bomb-Early.bmp");showcaseShots=2;}if(ex.bombAge>=28&&showcaseShots==2){screenshot("Bomb-Late.bmp");showcaseShots=3;}}
 if(demo&&!benchmark&&((int)((frameStart-start)/1000)>shotCount+2)){char name[64];snprintf(name,sizeof(name),"Demo-%02d.bmp",shotCount);screenshot(name);shotCount+=5;}
 if(demo&&(frameStart-start)>demoSeconds*1000)cmd='p';

}

int main(int argc,char**argv){SDL_SetMainReady();SDL_SetHint(SDL_HINT_EMSCRIPTEN_KEYBOARD_ELEMENT,"#canvas");vsync=1;cap=0;
int renderWidth=argc>2?clamp(atoi(argv[2]),1920,3840):2880,renderHeight=renderWidth*5/6;
if(!SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_GAMEPAD))fatal(SDL_GetError());
 window=SDL_CreateWindow("Star Guard",renderWidth,renderHeight,0);if(!window)fatal(SDL_GetError());SDL_SetWindowSize(window,renderWidth,renderHeight);renderer=SDL_CreateRenderer(window,0);if(!renderer)fatal(SDL_GetError());SDL_SetWindowSize(window,renderWidth,renderHeight);SDL_SetRenderLogicalPresentation(renderer,960,800,SDL_LOGICAL_PRESENTATION_LETTERBOX);SDL_SetRenderVSync(renderer,1);init_graphics();
 SDL_AudioSpec spec={SDL_AUDIO_F32,2,48000};astream=SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,&spec,audio_callback,0);if(astream)SDL_ResumeAudioStreamDevice(astream);
game_reset();g.mode=1;if(argc>1)g.wave=clamp(atoi(argv[1]),0,11)+1;sideBlend=side_mode();previous=g;previousEx=ex;previousY=player_y;before=start=ms_now();
int count=0;SDL_JoystickID*ids=SDL_GetGamepads(&count);if(count)pad=SDL_OpenGamepad(ids[0]);SDL_free(ids);
emscripten_set_main_loop(browser_frame,0,1);return 0;}
