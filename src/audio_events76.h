/* Accepted physical events only. Audio counters are outside gameplay/save state. */
static int audioRequests76[44],audioAdmitted76[44],audioLast76[51],audioEventTick76=-1;
static float audio_effect_pan76(float,float);
static float lastCuePan76[44];
static int hostileBeamTick76=-1000,enemyMegaEvents88,enemyMegaTick88=-1000;
static int pickupSerial76,pickupKind76,pickupFamily76=-1;
static void pc_audio_events_reset76(void){memset(audioRequests76,0,sizeof(audioRequests76));memset(audioAdmitted76,0,sizeof(audioAdmitted76));for(int i=0;i<51;i++)audioLast76[i]=-1000;audioEventTick76=-1;hostileBeamTick76=-1000;enemyMegaEvents88=0;enemyMegaTick88=-1000;pickupSerial76=pickupKind76=0;pickupFamily76=-1;}
static void combat_cue76(int kind,int type,int x,int y,int volume){if(kind<28||kind>41)return;audioRequests76[kind]++;int index=kind==40?49:kind==41?50:kind==28?40+(type&7):kind==35&&(type&4)?48:kind,gap=kind==41?4:kind==38?4:kind==33?30:kind==35?(type&4?2:40):kind==32?8:kind==31?4:kind==30?6:1;if(g.tick<audioLast76[index])audioLast76[index]=-1000;if(g.tick-audioLast76[index]<gap)return;audioLast76[index]=g.tick;audioAdmitted76[kind]++;lastCuePan76[kind]=audio_effect_pan76(x,y);enqueue((Command){.op=0,.ch=kind,.kind=kind,.type=type,.vol=volume,.pitch=kind==33?0x3000:0x3800,.pan=lastCuePan76[kind]});}
void pc_laser_charge76(int x,int y,int ticks){combat_cue76(37,ticks,x,y,76);}
void pc_enemy_mega_sound88(int x,int y){enemyMegaTick88=g.tick;enemyMegaEvents88++;enqueue((Command){.op=0,.ch=12,.kind=47,.type=6<<8,.vol=95,.pitch=0x3800,.pan=audio_effect_pan76(x,y)});}
void pc_hostile_laser76(int x,int y,int ticks){hostileBeamTick76=g.tick;audioRequests76[12]++;audioAdmitted76[12]++;enqueue((Command){.op=0,.ch=12,.kind=12,.type=clamp(ticks,1,250)<<8,.vol=90,.pitch=0x3800,.pan=audio_effect_pan76(x,y)});}
void pc_player_damage76(int kind,int x){combat_cue76(kind==2||kind==3?30:29,kind,x,player_y/Q,kind==2||kind==3?85:62);}
void pc_combat_hit76(Obj*o){if(o->type==4)combat_cue76(enemy_organic(o)?33:o->variant==14||o->variant==16?34:32,o->variant,o->x/Q,o->y/Q,enemy_organic(o)?72:52);else if(enemy_organic(o))combat_cue76(31,o->type,o->x/Q,o->y/Q,48);}
void pc_giant_hit76(int organic,int biomech,int x,int y){combat_cue76(organic?33:biomech?34:32,organic,x,y,organic?86:68);}
void pc_phase_sound76(int organic,int death,int x,int y){combat_cue76(35,organic|(death?4:0),x,y,death?100:80);}
void pc_pickup_voice76(int kind,int family){pickupKind76=kind;pickupFamily76=family;pickupSerial76++;}
static void audio_events_step76(void){if(g.mode!=1||g.paused||g.tick==audioEventTick76)return;audioEventTick76=g.tick;for(int i=0;i<MAXB;i++){Obj*b=g.bullets+i;if(b->serial&&b->audioBorn76==g.tick)combat_cue76(28,b->audioClass76|((b->type==6||b->type==8||b->type==9)?4:0),b->audioX76/Q,b->audioY76/Q,b->audioClass76&2?72:52);}}
static int audio_event_stat76(int n){if(n==12280)return pickupSerial76;if(n==12281)return pickupKind76;if(n==12282)return pickupFamily76;if(n>=12300&&n<12344)return audioRequests76[n-12300];if(n>=12400&&n<12444)return audioAdmitted76[n-12400];return 0;}
