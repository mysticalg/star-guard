static float play_view_zoom(void){return (ex.giant.active||ex.giant.defeated)&&ex.giant.archetype!=4&&g.viewZoom>0&&g.viewZoom<1?g.viewZoom:1;}
static int play_min_x(void){return (int)(160-146/play_view_zoom());}
static int play_max_x(void){return (int)(160+146/play_view_zoom());}
static int play_min_y(void){return (int)(140-100/play_view_zoom());}
static int play_max_y(void){return (int)(140+100/play_view_zoom());}
static float giant_world_scale(void){return ex.giant.archetype==4?1:ex.giant.scale>0?ex.giant.scale:1;}
static int giant_world_x(int x){return 160+(int)((x-160+ex.giant.rootX)*giant_world_scale());}
static int giant_world_y(int y){return 100+(int)((y-100+ex.giant.rootY)*giant_world_scale());}
static int journey_number(void){return clamp(g.wave-1,0,MISSION_COUNT-1);}
static int journey_kind(void){int m=journey_number();return journeyKinds[m][clamp(ex.section,0,journeyCounts[m]-1)];}
static int organic_context(void){int k=journey_kind();return k==ENV_ROOT||k==ENV_HIVE||k==ENV_MOUTH||k==ENV_BEAST||k==ENV_TEETH;}
static int journey_depth(void){int depth=ex.section;for(int m=0;m<journey_number();m++)depth+=journeyCounts[m];return depth;}
static int mission_phase(int wave){int m=clamp(wave-1,0,5);return journeyPhases[m][clamp(ex.section,0,journeyCounts[m]-1)];}
static int mission_index(int phase){for(int m=0;m<6;m++)for(int s=0;s<journeyCounts[m];s++)if(journeyPhases[m][s]==phase)return m;return 0;}
static int section_final(void){return ex.section==journeyCounts[journey_number()]-1;}
static int section_has_boss(void){int m=journey_number();return (!ex.miniDone&&journeyMinis[m][ex.section]>=0)||(journeyGiantAt[m]==ex.section&&!ex.giant.defeated);}
static int environment_exclusive(void){int k=journey_kind();return k==ENV_WALL||k==ENV_MOUTH||k==ENV_TEETH||k==ENV_ESCAPE||k==ENV_BLIZZARD||k==ENV_ASH||k==ENV_EM||k==ENV_BASE;}
static int custom_environment(void){int k=journey_kind();return k!=ENV_SKY&&k!=ENV_SPACE&&k!=ENV_ASH&&k!=ENV_FORTRESS;}
/* 150s active progression per mission, never 150s per scenery section. */
static int finale_progress_pending(void){int m=journey_number();return ex.section==journeyGiantAt[m]&&(ex.miniDone||journeyMinis[m][ex.section]<0)&&ex.progressTicks<7500;}
static int section_wave_target(void){if(environment_exclusive())return 0;int target=clamp(section_duration()/(formation_interval()+12)+1,4,32);return finale_progress_pending()&&g.clock>=section_duration()?g.groups+1:target;}
static int section_duration(void){int m=journey_number();static const int duration[6][6]={{2200,1800,1400},{2200,2000,1400},{1800,1300,1800,1400},{2300,1700,2000},{2400,700,2200,1200},{1800,1400,1500,1600,1400,1200}};static const int postMini[6]={1500,1400,1200,1400,2000,0};if(ex.miniDone&&ex.section==journeyGiantAt[m]&&postMini[m])return postMini[m];return duration[m][clamp(ex.section,0,journeyCounts[m]-1)];}
static int ship_endurance(void){static const int pct[3]={100,120,85};return pct[clamp(g.chassis,0,2)];}
static int ship_armour_damage(int damage){static const int armour[3]={100,80,130};static const int gear[4]={100,110,125,115};int effective=armour[g.chassis]*gear[clamp(g.armourGear,0,3)];return (damage*10000+effective-1)/effective;}
static int player_attack_scale(int weapon){int warm=clamp((ex.fatigue[clamp(weapon,0,8)]-150)*10/150,0,10);return (100-warm)*(ex.env.empActive?80:100)/100;}
static void journey_advance(void){
 globules_clear();
 ex.oldKind=journey_kind();ex.missionHandover=0;if(section_final()){ex.completedMissionMask|=1<<(g.wave-1);salvage_flush();}
 if(!section_final())ex.section++;
 else if(g.wave<MISSION_COUNT){ex.refit=1;ex.refitNext=g.wave+1;ex.victory=ex.bossSpawned=ex.clearTicks=0;return;}
 else{ex.campaignWon=1;g.mode=2;g.score+=10000;return;}
 g.clock=g.groups=0;ex.bossSpawned=ex.bossWarning=ex.clearTicks=ex.victory=0;ex.miniDone=ex.encounterTier=0;
 ex.giant=(Giant){0};ex.env=(Environment){.kind=-1};g.shield=120;
}
static void journey_after_encounter(void){
 if(ex.encounterTier==1){ex.miniDone=1;ex.encounterTier=0;ex.bossSpawned=ex.bossWarning=ex.clearTicks=ex.victory=0;if(journeyGiantAt[journey_number()]==ex.section){g.clock=g.groups=0;return;}}
 journey_advance();
}
static void giant_start(void);
static void environment_step(int);

static int giant_add_count(void){int n=0;for(int i=0;i<32;i++)n+=ex.env.hazards[i].on&&ex.env.hazards[i].type>=20&&ex.env.hazards[i].type<=25;return n;}

static void journey_commit_refit(void){if(!ex.refit)return;g.wave=ex.refitNext;ex.section=0;ex.refit=ex.refitNext=0;ex.missionTicks=ex.progressTicks=0;ex.missionHandover=1;g.clock=g.groups=0;ex.bossSpawned=ex.bossWarning=ex.clearTicks=ex.victory=0;ex.miniDone=ex.encounterTier=0;ex.giant=(Giant){0};ex.env=(Environment){.kind=-1};g.shield=120;}
