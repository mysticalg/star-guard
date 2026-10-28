#define main star_guard_main
#include "../src/pc.c"
#undef main
static int checks;
#define V(x) do{checks++;if(!(x)){fprintf(stderr,"FAIL audio events line%d %s\n",__LINE__,#x);return 1;}}while(0)
static void arena(void){headless=1;game_reset();g.mode=1;g.fx=0;g.clock=-100000;g.groups=section_wave_target();ex.intro=ex.travel=ex.entryY=0;ex.phase=ex.oldPhase=mission_phase(1);g.shield=0;}
int main(void){
 arena();Obj organic={.on=1,.type=10,.hp=20,.x=160*Q,.y=100*Q,.entryTicks=12};damage_enemy(&organic,100);V(audioRequests76[31]==1);damage_enemy(&organic,0);V(audioRequests76[31]==1);
 Obj boss={.on=1,.type=4,.hp=100,.base=100,.phase=1,.x=160*Q,.y=100*Q,.age=200,.openingTicks=30};damage_enemy(&boss,100);V(audioRequests76[32]==0);boss.openingTicks=0;boss.shieldCharge=1000;damage_enemy(&boss,100);V(audioRequests76[32]==0);boss.shieldCharge=0;damage_enemy(&boss,100);V(audioRequests76[32]==1);
 g.devInvincible=1;hurt_amount(160,230,0,1,85,2);V(!audioRequests76[29]&&!audioRequests76[30]);g.devInvincible=0;hurt_amount(160,230,0,1,85,2);V(audioRequests76[30]==1&&!audioRequests76[29]);g.shield=0;g.barrier=100;g.tick+=20;hurt_amount(160,230,0,1,35,1);V(audioRequests76[29]==1);
 arena();g.devInvincible=1;regularSpawnBudget=8;group(0,0);regularSpawnBudget=-1;for(int t=0;t<400;t++){game_step(0,0);audio_events_step76();int before=audioRequests76[28];audio_events_step76();V(audioRequests76[28]==before);}V(audioAdmitted76[28]>0&&audioAdmitted76[28]<=audioRequests76[28]);V(!hostileSource76);
 arena();drop(g.px/Q,player_y/Q,5);game_step(0,0);V(pickupSerial76==1&&pickupKind76==5);V(g.drops[0].on==0);int serial=pickupSerial76;game_step(0,0);V(pickupSerial76==serial);
 sideBlend=0;float pan=audio_effect_pan76(90,100);sideBlend=1;V(fabsf(audio_effect_pan76(90,100)-pan)>.1f);
 arena();g.devInvincible=1;Obj laser={.on=1,.type=27,.hp=10,.entryTicks=12,.x=160*Q,.y=80*Q};for(int t=1;t<=54;t++){g.tick++;aircraft_weapon_step(&laser);V(audioRequests76[37]==1);V(audioRequests76[12]==(t<40?0:t<54?t-39:14));V(audio_stat76(12261)==(t>=40&&t<54));}V(laser.weaponShots==1);
 arena();g.devInvincible=1;Obj mini={.on=1,.type=4,.variant=1,.hp=100,.base=100,.phase=1,.age=280,.x=160*Q,.y=58*Q};stage_boss_step(&mini);V(mini.attack==1&&audioRequests76[37]==1&&!audioRequests76[12]);for(int t=2;t<=86;t++){mini.age++;g.tick++;stage_boss_step(&mini);V(mini.attack==t);V(audioRequests76[12]==(t<61?0:t<=85?t-60:25));V(audio_stat76(12261)==(t>=61&&t<=85));}V(audioRequests76[37]==1);
 arena();ex.giant.archetype=5;Obj*gb=giant_audio_bullet76(50*Q,80*Q,0,200);V(gb&&gb->audioClass76==6);V(gb->audioBorn76==g.tick);
 arena();g.tick++;giant_beam_damage(20,40,20,240,8);V(audioRequests76[12]==1&&audio_stat76(12261));g.tick++;V(!audio_stat76(12261));game_reset();V(!audio_stat76(12261));
 printf("PASS %d actual audio-event checks: health loss/entry/shield gates, physical crash versus bullet hit, protected silence, real hostile fire/no repeated tick emission, actual pickup metadata\n",checks);return 0;
}
