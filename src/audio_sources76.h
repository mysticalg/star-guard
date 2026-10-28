/* Passive, fixed-tick audio selection. No gameplay state or RNG is changed. */
typedef struct{u32 id;int kind,type;float drive,pan,near,priority;} AudioSource76;
static AudioSource76 audioSources76[6];static int audioSourceCount76,audioSourceTick76=-1;
void pc_audio_reset76(void){pc_audio_events_reset76();pc_ice_reset83();pc_siphon_reset84();audioSourceCount76=0;audioSourceTick76=-1;}
static void audio_point76(float*x,float*y){*x=480+(*x*3-480)*journeyZoom;*y=420+(*y*3-420)*journeyZoom;flight_screen_point(x,y);}
static float audio_effect_pan76(float x,float y){audio_point76(&x,&y);return fmaxf(-.85f,fminf(.85f,(x-480)/560));}
static void audio_offer76(AudioSource76 a){int at=audioSourceCount76;for(int i=0;i<audioSourceCount76;i++)if(a.priority>audioSources76[i].priority){at=i;break;}if(at>=6)return;for(int i=clamp(audioSourceCount76,0,5);i>at;i--)audioSources76[i]=audioSources76[i-1];audioSources76[at]=a;audioSourceCount76=clamp(audioSourceCount76+1,0,6);}
static void audio_sources_step76(void){
 if(g.mode!=1||ex.travel||ex.refit||ex.sectionHandover||ex.cavernTurn){audioSourceCount76=0;audioSourceTick76=-1;return;}
 if(g.tick==audioSourceTick76)return;audioSourceTick76=g.tick;u32 prior[6];int priorCount=audioSourceCount76;for(int i=0;i<priorCount;i++)prior[i]=audioSources76[i].id;audioSourceCount76=0;
 float px=g.px/(float)Q,py=player_y/(float)Q;audio_point76(&px,&py);
 for(int i=0;i<MAXE;i++){Obj*o=g.enemies+i;if(!enemy_engines(o)||!o->serial)continue;float x=o->x/(float)Q,y=o->y/(float)Q,speed=hypotf(o->x-o->previousX,o->y-o->previousY)/Q;if(speed<.06f)continue;audio_point76(&x,&y);if(x<-90||x>1050||y<36||y>830)continue;
  float distance=hypotf(x-px,y-py),near=fmaxf(0,1-distance/1150),drive=fminf(1.4f,.18f+speed*.36f),priority=near+(o->type==4?.3f:0);for(int j=0;j<priorCount;j++)if(prior[j]==o->serial)priority+=.09f;
  int hum=o->type==18||o->type==21||(o->type==15&&enemy_guarded(o))||(o->type==4&&(o->variant==8||o->variant==12||o->variant==14||o->variant==15||o->variant==16));
  audio_offer76((AudioSource76){o->serial,hum?2:1,o->type,drive,fmaxf(-.85f,fminf(.85f,(x-480)/560)),near*near,priority});
 }
 if(ex.giant.active&&!ex.giant.ice83.frozen&&!organic_context()){
  Giant*b=&ex.giant;float x=b->parts[6].x,y=b->parts[6].y;audio_point76(&x,&y);float speed=hypotf(b->rootX-b->previousRootX,b->rootY-b->previousRootY),drive=fminf(1.25f,.35f+speed*.25f);
  audio_offer76((AudioSource76){0x40000000u+(u32)g.wave,b->archetype==0?1:2,100+b->archetype,drive,fmaxf(-.8f,fminf(.8f,(x-480)/560)),.85f,2});
 }
}
static int audio_stat76(int n){if(n>=12280)return audio_event_stat76(n);if(n==12260)return g.beam;if(n==12261)return g.mode==1&&!ex.travel&&!ex.refit&&g.tick==hostileBeamTick76;if(n==12200)return audioSourceCount76;if(n<12210||n>=12246)return 0;int index=(n-12210)/6,field=(n-12210)%6;if(index>=audioSourceCount76)return 0;AudioSource76*s=audioSources76+index;return field==0?(int)s->id:field==1?s->kind:field==2?(int)(s->drive*1000):field==3?(int)(s->pan*1000):field==4?(int)(s->near*1000):s->type;}
