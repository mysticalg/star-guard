static float play_view_zoom(void){return (ex.giant.active||ex.giant.defeated)&&ex.giant.archetype!=4&&g.viewZoom>0&&g.viewZoom<1?g.viewZoom:1;}
static int play_min_x(void){return (int)(160-146/play_view_zoom());}
static int play_max_x(void){return (int)(160+146/play_view_zoom());}
static int play_min_y(void){return (int)(140-100/play_view_zoom());}
static int play_max_y(void){return (int)(140+100/play_view_zoom());}
static float giant_world_scale(void){return ex.giant.archetype==4?1:ex.giant.scale>0?ex.giant.scale:1;}
static int giant_world_x(int x){return 160+(int)((x-160)*giant_world_scale());}
static int giant_world_y(int y){return 100+(int)((y-100)*giant_world_scale());}
static int journey_number(void){return clamp(g.wave-1,0,MISSION_COUNT-1);}
static int journey_kind(void){int m=journey_number();return journeyKinds[m][clamp(ex.section,0,journeyCounts[m]-1)];}
static int journey_depth(void){int depth=ex.section;for(int m=0;m<journey_number();m++)depth+=journeyCounts[m];return depth;}
static int mission_phase(int wave){int m=clamp(wave-1,0,5);return journeyPhases[m][clamp(ex.section,0,journeyCounts[m]-1)];}
static int mission_index(int phase){for(int m=0;m<6;m++)for(int s=0;s<journeyCounts[m];s++)if(journeyPhases[m][s]==phase)return m;return 0;}
static int section_final(void){return ex.section==journeyCounts[journey_number()]-1;}
static int section_has_boss(void){int m=journey_number();return (!ex.miniDone&&journeyMinis[m][ex.section]>=0)||(journeyGiantAt[m]==ex.section&&!ex.giant.defeated);}
static int environment_exclusive(void){int k=journey_kind();return k==ENV_WALL||k==ENV_MOUTH||k==ENV_TEETH||k==ENV_ESCAPE||k==ENV_BLIZZARD||k==ENV_ASH||k==ENV_EM||k==ENV_BASE;}
static int custom_environment(void){int k=journey_kind();return k!=ENV_SKY&&k!=ENV_SPACE&&k!=ENV_ASH&&k!=ENV_FORTRESS;}
static int section_wave_target(void){return environment_exclusive()?0:clamp(section_duration()/formation_interval()+1,4,18);}
static int section_duration(void){if(ex.miniDone&&ex.section==journeyGiantAt[journey_number()])return 700;if(journey_number()==0){static const int coast[3]={1400,750,500};return coast[clamp(ex.section,0,2)];}int k=journey_kind();if(k==ENV_WALL)return 1300;if(k==ENV_MOUTH)return 700;if(k==ENV_TEETH)return 900;if(k==ENV_ESCAPE)return 1200;if(k==ENV_OCEAN)return 1500;return 1400*(170-3*clamp(journey_depth()-2,0,14))/170;}
static int ship_endurance(void){static const int pct[3]={100,120,85};return pct[clamp(g.chassis,0,2)];}
static int ship_armour_damage(int damage){static const int armour[3]={100,80,130};static const int gear[4]={100,110,125,115};int effective=armour[g.chassis]*gear[clamp(g.armourGear,0,3)];return (damage*10000+effective-1)/effective;}
static int player_attack_scale(int weapon){int warm=clamp((ex.fatigue[clamp(weapon,0,8)]-150)*10/150,0,10);return (100-warm)*(ex.env.empActive?80:100)/100;}
static void journey_advance(void){
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

static void journey_commit_refit(void){if(!ex.refit)return;g.wave=ex.refitNext;ex.section=0;ex.refit=ex.refitNext=0;ex.missionTicks=0;ex.missionHandover=1;g.clock=g.groups=0;ex.bossSpawned=ex.bossWarning=ex.clearTicks=ex.victory=0;ex.miniDone=ex.encounterTier=0;ex.giant=(Giant){0};ex.env=(Environment){.kind=-1};g.shield=120;}
