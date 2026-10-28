#define main star_guard_main
#include "../src/pc.c"
#undef main
static int checks;
#define V(x) do{checks++;if(!(x)){fprintf(stderr,"FAIL line%d tick%d type%d %s\n",__LINE__,g.tick,type,#x);return 1;}}while(0)
static Obj old75[MAXE];static Game mutationGame75;static Expansion mutationExpansion75;
static void arena75(int cave){headless=1;game_reset();g.mode=1;g.fx=0;g.devInvincible=1;g.wave=6;ex.section=cave?5:0;ex.phase=ex.oldPhase=mission_phase(6);ex.intro=ex.entryY=ex.travel=0;ex.phaseAge=g.clock=100;g.groups=section_wave_target();environment_enter();ex.enemySpeed=256;}
int main(void){int type=0;
 arena75(0);regularSpawnBudget=1;group(0,0);regularSpawnBudget=-1;Obj a=g.enemies[0],b=a;b.x=280*Q;b.y=150*Q;g.enemies[0].on=0;regularSpawnBudget=1;group(0,0);regularSpawnBudget=-1;a=g.enemies[0];V(a.serial&&a.serial!=b.serial);Obj shown=a;interp_objects(&shown,&b,1,.5f);V(shown.x==a.x&&shown.y==a.y);b=a;b.x-=8*Q;shown=a;interp_objects(&shown,&b,1,.5f);V(shown.x==a.x-4*Q);
 for(int cave=0;cave<2;cave++)for(int pattern=0;pattern<8;pattern++)for(type=0;type<29;type++)if(type!=4&&type!=8&&type!=9&&type!=22&&type!=23&&type!=24&&type!=26){
  arena75(cave);regularSpawnBudget=8;group(type,pattern);regularSpawnBudget=-1;int count=enemy_count(),serials=0;
  for(int i=0;i<MAXE;i++)if(g.enemies[i].on){V(g.enemies[i].serial);serials++;for(int j=i+1;j<MAXE;j++)if(g.enemies[j].on)V(g.enemies[i].serial!=g.enemies[j].serial);}
  V(serials==count);double maximum=0;int live=0;
  for(int t=0;t<1200&&enemy_count();t++){
   memcpy(old75,g.enemies,sizeof(old75));game_step(0,0);
   for(int i=0;i<MAXE;i++){Obj*o=g.enemies+i,*p=old75+i;if(!o->on||!p->on||o->serial!=p->serial||o->type!=type)continue;
    int in=o->x>14*Q&&o->x<306*Q&&o->y>24*Q&&o->y<256*Q&&p->x>14*Q&&p->x<306*Q&&p->y>24*Q&&p->y<256*Q;
    if(in){float d=hypotf(o->x-p->x,o->y-p->y)/Q;if(d>maximum)maximum=d;if(d>=19)fprintf(stderr,"jump cave%d pattern%d actor%d old(%d,%d) new(%d,%d) age%d strength%d side%d base%d d%.3f\n",cave,pattern,i,p->x,p->y,o->x,o->y,o->age,o->strength,o->caveSide,o->base,d);V(d<19);if(!enemy_organic(o))V(absn((o->heading-p->heading+8640)%5760-2880)<=144);live++;}
    if(cave&&o->y>24*Q&&o->y<256*Q){float l,r;if(cavern_interval(o->y/(float)Q,pc_enemy_radius(o),o->caveSide,&l,&r))V(o->x>=ceilf(l*Q)&&o->x<=floorf(r*Q));V(o->caveSide==p->caveSide);}
   }
   if(ex.bossSpawned)break;
  }
  if(type!=15&&type!=19&&type!=21&&type!=3)V(enemy_count()==0||ex.bossSpawned);V(live>0);
 }
 /* Actual open-field flank leaves and re-enters before its finite departure. */
 arena75(0);regularSpawnBudget=1;group(1,0);regularSpawnBudget=-1;int exited=0,reentered=0;
 for(int t=0;t<300&&g.enemies[0].on;t++){game_step(0,0);Obj*o=g.enemies;if(o->y>24*Q&&o->y<256*Q){if(o->x<0)exited=1;if(exited&&o->x>14*Q)reentered=1;}}V(exited&&reentered&&!g.enemies[0].on);V(g.score==0&&g.kills==0);
 /* History cannot alter simulation, survive a recycled object, or grow on paused frames. */
 arena75(0);g.fx=1;type=0;regularSpawnBudget=1;group(0,0);regularSpawnBudget=-1;u32 id=g.enemies[0].serial;
 for(int t=0;t<10;t++){game_step(0,0);mutationGame75=g;mutationExpansion75=ex;motion_step75();V(!memcmp(&mutationGame75,&g,sizeof(g))&&!memcmp(&mutationExpansion75,&ex,sizeof(ex)));}MotionTrack75*track=motion_find75(id);V(track&&track->count==6);int head=track->head,tick=g.tick;u32 seed=g.seed;
 for(int t=0;t<500;t++)motion_step75();V(track->head==head&&g.tick==tick&&g.seed==seed);
 MotionPose75 pose;V(motion_pose75(track,4,.5f,&pose));V(isfinite(pose.x)&&isfinite(pose.y));
 g.enemies[0].on=0;regularSpawnBudget=1;group(0,0);regularSpawnBudget=-1;game_step(0,0);motion_step75();V(!motion_find75(id));V(!motion_pose75(motion_find75(g.enemies[0].serial),2,1,&pose));
 game_step(0,2009);for(int t=0;t<60;t++){game_pointer(80+t*3,200);game_step(0,0);motion_step75();}for(int i=0;i<5;i++){MotionTrack75*f=motion_find75(0xffffffe0u+i);V(f&&f->count==6);}game_reset();V(!motion_find75(0xffffffffu)&&motionTick75==-1);
 printf("PASS %d planes75 checks: identity-safe interpolation, all formations/types in open/cavern routes, heading continuity, branch clearance, finite flank re-entry, fixed-tick history and lifecycle\n",checks);return 0;
}
