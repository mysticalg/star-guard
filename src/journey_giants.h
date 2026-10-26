/* Independent collision targets; hull art is not a collision rectangle. HP uses hundredths. */
static int giant_live(int n){return ex.giant.parts[n].hp>0;}
static int giant_open(int n){Giant*b=&ex.giant;int c=b->timer%240;if(!b->active||b->timer<120||!giant_live(n)||giant_add_count())return 0;
 if(b->archetype==4)return b->parts[n].visible&&(n!=6||b->partsDestroyed>=4);
 if(n==6)return !giant_live(0)&&!giant_live(1)&&!giant_live(2)&&!giant_live(3)&&!giant_live(7)&&!giant_live(8)&&(b->archetype!=1||c>=120)&&(b->archetype!=4||c>=90);
 if(n>=7)return 1;
 if(n>=2&&n<=5&&(giant_live(0)||giant_live(1)))return 0;
 if(b->archetype==2&&n<2)return n==((b->timer/160)&1);
 if(b->archetype==3&&n<2)return c>=80&&c<200;
 return 1;}
static void giant_layout(void){Giant*b=&ex.giant;static const int xy[9][2]={{64,53},{256,53},{64,145},{256,145},{110,44},{210,44},{160,114},{110,156},{210,156}};for(int i=0;i<9;i++){GiantPart*p=b->parts+i;p->x=giant_world_x(xy[i][0]);p->y=giant_world_y(xy[i][1]);p->w=(int)((i==6?38:28)*giant_world_scale());p->h=(int)((i==6?38:30)*giant_world_scale());}}
static void giant_start(void){Giant*b=&ex.giant;*b=(Giant){.active=1,.phase=1,.archetype=g.wave-1,.zoom=.55f,.scale=1,.rayX=160};ex.encounterTier=2;ex.bossSpawned=1;
 static const int xy[9][2]={{64,53},{256,53},{64,145},{256,145},{110,44},{210,44},{160,114},{110,156},{210,156}};
 for(int i=0;i<9;i++){int hp=i==6?16000+g.wave*1800:i<2?2600:1800;b->parts[i]=(GiantPart){.hp=i>=7?0:hp,.maxHp=hp,.x=xy[i][0],.y=xy[i][1],.w=i==6?38:28,.h=i==6?38:30,.role=i};}
 if(b->archetype==4){for(int i=0;i<9;i++){b->parts[i].hp=b->parts[i].maxHp=i==6?24000:2200;b->parts[i].w=i==6?40:32;b->parts[i].h=i==6?40:32;}}
 for(int i=0;i<9;i++){b->parts[i].hp=b->parts[i].hp*(100+3*(g.pilotLevel-1))/100;b->parts[i].maxHp=b->parts[i].maxHp*(100+3*(g.pilotLevel-1))/100;}
 if(b->archetype!=4)giant_layout();else{b->scale=1;b->zoom=.68f;}
 if(b->archetype==5){int reduction=clamp(ex.baseDamage,0,8)*150;b->parts[0].hp-=reduction;b->parts[1].hp-=reduction;}
}
static void giant_raw_hit(int n,int damage){Giant*b=&ex.giant;GiantPart*p=b->parts+n;if(!giant_open(n))return;int old=p->hp;p->hp=clamp(p->hp-damage,0,p->maxHp);if(p->hp<old)p->damageUntil=g.tick+5;phoenix_charge((old-p->hp)/50);if(old&&p->hp==0){b->partsDestroyed++;g.score+=n==6?5000:500;burst(p->x,p->y,1);}
 if(b->archetype!=4&&n==6&&p->hp>0&&b->phase<4&&p->hp*3<p->maxHp*2){b->phase=4;for(int i=7;i<9;i++){b->parts[i].hp=b->parts[i].maxHp=2200;}}
 if(n==6&&p->hp*3<p->maxHp)b->phase=5;
 if(n==6&&!p->hp){ex.giantRewardMask|=1<<(g.wave-1);b->active=0;b->defeated=1;ex.victory=1;ex.victoryX=160;ex.victoryY=104;ex.clearScore=g.score-ex.stageScore;ex.clearKills=g.kills-ex.stageKills;ex.clearSalvage=g.salvagePicked-ex.stageSalvage;ex.clearBonus=3000;g.score+=ex.clearBonus;for(int i=0;i<MAXB;i++)g.bullets[i].on=0;}
}
static void giant_hit(int n,int damage){if(giant_open(n))giant_raw_hit(n,player_hit_roll(damage*variant_damage_scale(g.weapon)/100,ex.giant.parts[n].x,ex.giant.parts[n].y));}
static void giant_electric_hit(int n,int damage){if(!giant_open(n))return;GiantPart*p=ex.giant.parts+n;int rolled=player_hit_roll(damage*variant_damage_scale(8)/100,p->x,p->y);p->dotTicks=150;p->dotDamage=rolled/10;if(!p->dotClock)p->dotClock=50;giant_raw_hit(n,rolled*70/100);}
static void giant_fan(int x,int y,int speed,int spread){for(int n=-1;n<=1;n++)bullet(x*Q,y*Q,n*spread,speed);}
static void worm_positions(void){Giant*b=&ex.giant;int c=b->timer%600,reverse=(b->timer/600)&1;static const int order[9]={1,2,3,4,5,6,0,7,8};int travel=b->phase>=4?210:260;for(int i=0;i<9;i++){GiantPart*p=b->parts+i;float growth=b->phase>=4?1.35f:b->phase>=2?1.15f:1;p->w=p->h=(int)((i==6?40:32)*growth);int local=c-90-order[i]*26;float u=local/(float)travel;int start=tunnel_left(100)+6,end=tunnel_right(100)-6;int x=start+(int)((end-start)*u),y=100+(sn((int)(u*256))*(b->phase>=2?38:24)>>10);p->x=reverse?end-(x-start):x;p->y=y;p->visible=local>=0&&local<=travel&&p->x>tunnel_left(y)+8&&p->x<tunnel_right(y)-8;int nextY=100+(sn((int)((u+.02f)*256))*(b->phase>=2?38:24)>>10);p->angle=(int)(atan2f(nextY-y,(reverse?-1:1)*(end-start)*.02f)*57.29578f)+90;}}
static void giant_step(int mask){(void)mask;Giant*b=&ex.giant;if(!b->active)return;b->timer++;int aoe=b->timer%360;if(aoe==100){b->aoeX=clamp(g.px/Q,play_min_x()+36,play_max_x()-36);b->aoeY=clamp(player_y/Q,play_min_y()+30,play_max_y()-15);}if(aoe==180){b->blastSerial++;int dx=g.px/Q-b->aoeX,dy=player_y/Q-b->aoeY;if(dx*dx+dy*dy<32*32)hurt_at(g.px/Q,player_y/Q,0,-1);pc_charge_burst(b->aoeX,b->aoeY);}int t=b->timer,c=t%240,a=b->archetype;
 if((!b->addWaves&&t>=240)||(b->addWaves==1&&b->phase>=4&&t-b->lastAddTick>=180&&!giant_add_count())){for(int n=0;n<3;n++)environment_spawn(20+a,80+n*80,38,0,180,13,5+g.wave/2);b->addWaves++;b->lastAddTick=t;pc_emp_sound(1,160);}
 if(a==4){b->phase=b->parts[6].hp*3<b->parts[6].maxHp*2?4:b->partsDestroyed>=2?2:1;}else if(b->phase<4)b->phase=giant_live(0)||giant_live(1)?1:giant_live(2)||giant_live(3)?2:3;b->zoom=a==4?(b->phase>=4?.52f:b->phase>=2?.60f:.68f):(b->phase>=4?.38f:b->phase>=2?.46f:.55f);if(a!=4){if(b->scale<=0)b->scale=1;float target=b->phase>=4?2.7f:b->phase>=3?2.25f:b->phase>=2?2:1.6f;b->scale+=(target-b->scale)*.025f;giant_layout();}
 /* Ocean projectors flood side lanes; each destroyed projector removes its own wave. */
 if(a==0){if(c>=40&&c<100)ex.env.warning=10;if(c>=100&&c<220){int y=giant_world_y(30+(c-100)*2);for(int i=0;i<2;i++)if(giant_live(i)&&absn(player_y/Q-y)<14*giant_world_scale()&&absn(g.px/Q-giant_world_x(i?252:68))<48*giant_world_scale())hurt_at(g.px/Q,player_y/Q,0,-1);}}
 /* Brood chambers spawn actual pursuers; surviving chambers mend the shell, never a dead part. */
 if(a==1){if(c==120)for(int i=0;i<2;i++)if(giant_live(i)){Obj*o=slot(g.enemies,MAXE);if(o)*o=(Obj){.on=1,.type=0,.x=b->parts[i].x*Q,.y=(b->parts[i].y+b->parts[i].h/2)*Q,.hp=2,.base=b->parts[i].x};}if(c==0)for(int i=2;i<6;i++)if(giant_live(i))b->parts[i].hp=clamp(b->parts[i].hp+100,0,b->parts[i].maxHp);if(c==180)giant_fan(160,130,340,240);}
 /* Crystal reflectors alternate exposed sides. Closed reflectors return a readable enemy shot. */
 if(a==2&&t%120==0)for(int i=0;i<2;i++)if(giant_live(i))giant_fan(b->parts[i].x,110,280,i?240:130);
 /* Cooling vents open after their marked ice lanes. Breaking a vent prevents its next lane. */
 if(a==3){if(c<80)ex.env.warning=11;if(c==80)for(int i=0;i<2;i++)if(giant_live(i))for(int n=-1;n<=1;n++)environment_spawn(4,b->parts[i].x+n*18,b->parts[i].y+(int)(20*giant_world_scale()),0,260,8,1);}
 /* Parasitic worm: each segment follows the same time-delayed path through gut holes. */
 if(a==4){worm_positions();if(t%600<90)ex.env.warning=16;if(b->parts[6].visible&&t%60==0&&b->phase>=2)giant_fan(b->parts[6].x,b->parts[6].y,280,150);}
 /* Factory capacitors aim, telegraph, then discharge. Destroying either cuts beam width. */
 if(a==5){if(c==30)b->rayX=clamp(g.px/Q,play_min_x()+22,play_max_x()-22);if(c>=30&&c<100)ex.env.warning=13;if(c>=100&&c<140){int width=(int)((8+12*(giant_live(0)+giant_live(1)))*giant_world_scale());if(absn(g.px/Q-b->rayX)<width)hurt_at(g.px/Q,player_y/Q,0,-1);}}
 if(t>120){for(int i=2;i<6;i++)if(a!=4&&giant_live(i)&&t%(i<4?150:210)==i*13)aim(b->parts[i].x*Q,b->parts[i].y*Q,310+(b->phase>=4?60:0));}
 for(int i=0;i<9;i++){GiantPart*p=b->parts+i;if(p->dotTicks){p->dotTicks--;if(--p->dotClock<=0){p->dotClock=50;giant_raw_hit(i,p->dotDamage);}}if(!p->hp||(a==4&&!p->visible))continue;for(int n=0;n<MAXS;n++){Obj*s=g.shots+n;if(!s->on||s->base>0||absn(s->x/Q-p->x)>p->w/2+3||absn(s->y/Q-p->y)>p->h/2+3)continue;int open=giant_open(i);if(open)giant_hit(i,(s->type==9?800:projectile_damage(s))*player_attack_scale(g.weapon)/100);else if(a==2&&i<2)bullet(p->x*Q,(p->y+20)*Q,(g.px/Q-p->x)*2,300);s->on=0;pc_impact(p->x,p->y,open?1:3);break;}
  if(g.weapon==8&&(mask&4)&&g.weaponFireCool[8]==weapon_delay(18))giant_electric_hit(i,220*player_attack_scale(8)/100);
  if(!b->active)return;
  if(g.beam&&(g.tick&3)==0&&p->y<player_y/Q&&beam_hits(p->x,p->y,p->w/2))giant_hit(i,(ex.phoenixTicks?600:120)*player_attack_scale(g.weapon)/100);
  if(ex.bombAge==25)giant_hit(i,1200);
  for(int n=0;n<ex.options&&n<5;n++)if(!ex.podContact[n]&&absn(option_x(n)-p->x)<p->w/2+6&&absn(option_y(n)-p->y)<p->h/2+6){giant_hit(i,25);ex.podContact[n]=20;}
  if(absn(g.px/Q-p->x)<p->w/2+5&&absn(player_y/Q-p->y)<p->h/2+5)hurt_at(g.px/Q,player_y/Q,0,-1);
 }
}
