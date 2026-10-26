static float play_view_zoom(void){return (ex.giant.active||ex.giant.defeated)&&ex.giant.archetype!=4&&g.viewZoom>0&&g.viewZoom<1?g.viewZoom:1;}
static int play_min_x(void){return (int)(160-146/play_view_zoom());}
static int play_max_x(void){return (int)(160+146/play_view_zoom());}
static int play_min_y(void){return (int)(140-100/play_view_zoom());}
static int play_max_y(void){return (int)(140+100/play_view_zoom());}
static float giant_world_scale(void){return ex.giant.archetype==4?1:ex.giant.scale>0?ex.giant.scale:1;}
static int giant_world_x(int x){return 160+(int)((x-160+ex.giant.rootX)*giant_world_scale());}
static int giant_world_y(int y){return 100+(int)((y-100+ex.giant.rootY)*giant_world_scale());}
#include "lightning65.h"
static int journey_number(void){return clamp(g.wave-1,0,MISSION_COUNT-1);}
static int journey_kind(void){int m=journey_number();return journeyKinds[m][clamp(ex.section,0,journeyCounts[m]-1)];}
static int organic_context(void){int k=journey_kind();return k==ENV_ROOT||k==ENV_HIVE||k==ENV_MOUTH||k==ENV_BEAST||k==ENV_TEETH;}
static int journey_depth(void){int depth=0;for(int m=0;m<=journey_number();m++)for(int s=0;s<(m==journey_number()?clamp(ex.section,0,journeyCounts[m]):journeyCounts[m]);s++)depth+=journeyDepthWeight[m][s];return depth;}
static int mission_phase(int wave){int m=clamp(wave-1,0,5);return journeyPhases[m][clamp(ex.section,0,journeyCounts[m]-1)];}
static int mission_index(int phase){for(int m=0;m<6;m++)for(int s=0;s<journeyCounts[m];s++)if(journeyPhases[m][s]==phase)return m;return 0;}
static int section_final(void){return ex.section==journeyCounts[journey_number()]-1;}
static int mission_report_ready(void){return section_final()&&ex.encounterTier==2&&ex.victory>0;}
static int section_has_boss(void){int m=journey_number();return (!ex.miniDone&&journeyMinis[m][ex.section]>=0)||(journeyGiantAt[m]==ex.section&&!ex.giant.defeated);}
/* Ten-second forecast, thirty-second front, ten-second clear-air recovery.
   Profile follows the authored biome; it never changes organic actor identity. */
static int opening_front(void){return g.wave==1&&ex.section==0;}
static int opening_storm_now(void){return opening_front()&&ex.env.timer>=1000&&ex.env.timer<1500;}
static int lightning_period(void){return opening_front()?60:90;}
static int lightning_clock(void){return ex.env.timer-(opening_front()?1100:500);}
static int lightning_seed(void){return lightning_clock()/lightning_period();}
static int weather_profile(void){if(g.wave==1)return opening_storm_now()?WEATHER_STORM:WEATHER_NONE;switch(journey_kind()){case ENV_SKY:case ENV_OCEAN:return WEATHER_STORM;case ENV_SPACE:case ENV_WALL:return WEATHER_ASTEROID;case ENV_ROOT:case ENV_HIVE:case ENV_MOUTH:case ENV_BEAST:case ENV_TEETH:return WEATHER_ACID;case ENV_EM:case ENV_REACTOR:case ENV_WRECK:return WEATHER_ION;case ENV_ASH:case ENV_BASE:case ENV_ESCAPE:return WEATHER_FIRE;case ENV_BLIZZARD:case ENV_ICE:case ENV_CRYSTAL:return WEATHER_ICE;}return WEATHER_NONE;}
static int weather_front(void){return ex.env.timer%3000<2500;}
static int weather_exclusive(void){int k=journey_kind();return !ex.bossSpawned&&!ex.giant.active&&(opening_storm_now()||ex.env.timer<2500&&k==ENV_SPACE);}
static int environment_exclusive(void){int k=journey_kind();return weather_exclusive()||k==ENV_WALL||k==ENV_MOUTH||k==ENV_TEETH||k==ENV_ESCAPE||k==ENV_BLIZZARD||k==ENV_ASH||k==ENV_EM||k==ENV_BASE;}
static int custom_environment(void){int k=journey_kind();return k!=ENV_SKY&&k!=ENV_SPACE&&k!=ENV_ASH&&k!=ENV_FORTRESS;}
/* Active time belongs to the whole mission; the escape budget remains after the finale. */
static int mission_active_target(void){return missionActiveSeconds[journey_number()]*50;}
static int mission_escape_budget(void){int m=journey_number(),ticks=0;for(int s=journeyGiantAt[m]+1;s<journeyCounts[m];s++)ticks+=journeySeconds[m][s]*50;return ticks;}
static int finale_progress_pending(void){int m=journey_number();return ex.section==journeyGiantAt[m]&&(ex.miniDone||journeyMinis[m][ex.section]<0)&&ex.progressTicks<mission_active_target()-mission_escape_budget();}
static int section_formation_clock(void){return g.clock-(opening_front()?clamp(g.clock-1000,0,500):journey_kind()==ENV_SPACE?2500:0);}
static int section_wave_target(void){if(environment_exclusive())return 0;int duration=section_duration();if(!duration)return finale_progress_pending()?g.groups+1:0;int combat=duration-(opening_front()?500:journey_kind()==ENV_SPACE?2500:0),target=clamp(combat/(formation_interval()+12)+1,4,64);return finale_progress_pending()&&g.clock>=duration?g.groups+1:target;}
static int section_duration(void){if(journey_kind()==ENV_WALL)return 0;int m=journey_number();if(ex.miniDone&&ex.section==journeyGiantAt[m]&&journeyMinis[m][ex.section]>=0)return 0;return journeySeconds[m][ex.section]*50;}
static int journey_exit_gate(int timer){int start=section_duration()-500;if(start<400)start=400;return (timer-start)*3/5-40;}
static int mission_progress_ready(void){return !section_final()||ex.progressTicks>=mission_active_target();}
static int ship_endurance(void){static const int pct[3]={100,120,85};return pct[clamp(g.chassis,0,2)];}
static int ship_armour_damage(int damage){static const int armour[3]={100,80,130};static const int gear[4]={100,110,125,115};int effective=armour[g.chassis]*gear[clamp(g.armourGear,0,3)];return (damage*10000+effective-1)/effective;}
static int player_attack_scale(int weapon){int warm=clamp((ex.fatigue[clamp(weapon,0,8)]-150)*10/150,0,10);return (100-warm)*(ex.env.empActive?80:100)/100;}
static void journey_advance(void){
 if(!mission_progress_ready())return;
 globules_clear();
 ex.oldKind=journey_kind();ex.missionHandover=0;if(section_final()){ex.completedMissionMask|=1<<(g.wave-1);salvage_flush();}
 if(!section_final()){ex.section++;ex.sectionHandover=1;}
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

static void journey_commit_refit(void){if(!ex.refit)return;g.wave=ex.refitNext;ex.section=0;ex.refit=ex.refitNext=0;ex.missionTicks=ex.progressTicks=0;ex.missionHandover=1;ex.sectionHandover=1;g.clock=g.groups=0;ex.bossSpawned=ex.bossWarning=ex.clearTicks=ex.victory=0;ex.miniDone=ex.encounterTier=0;ex.giant=(Giant){0};ex.env=(Environment){.kind=-1};g.shield=120;}
