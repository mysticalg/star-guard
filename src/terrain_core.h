/* Fine solid terrain. Every physical query and alpha texel uses density>0. */
static unsigned int terrain_hash(int x,int y){unsigned int n=(unsigned)x*374761393u+(unsigned)y*668265263u;n=(n^(n>>13))*1274126177u;return n^(n>>16);}
static int terrainScrollSpeed=153;
static int wall_origin_y(void){return TERRAIN_ORIGIN+terrain.scrollQ/Q;}
static int terrain_ready(void){return journey_kind()!=ENV_WALL||wall_origin_y()>player_y/Q+30;}
static int terrain_initial_material(int x,int y){int yy=y+(sn(x*3)*4+sn(x*7+37)*2)/1024;yy=yy*448/TERRAIN_H;if(g.wave==3&&ex.section==3)return yy<112?3:yy<280?2:yy<364?1:0;if(g.wave==6&&ex.section==1)return yy<98?3:yy<238?2:yy<308?1:0;if(g.wave==6&&ex.section==5)return yy<84?3:yy<196?2:yy<238?3:yy<308?2:yy<378?1:0;return yy<140?3:yy<280?2:yy<364?1:0;}
/* Penetration stays calibrated for the stock cutter; the denser grain fringes
   take an extra point of real damage without halving low-power drilling depth. */
static int terrain_cut_resistance(int m){static const int resist[4]={3,4,6,8};return resist[clamp(m,0,3)];}
static int terrain_material(int x,int y){return terrain.material[clamp(y,0,TERRAIN_H-1)*TERRAIN_W+clamp(x,0,TERRAIN_W-1)];}
static int terrain_hardness(int m){static const int hp[4]={4,5,7,9};return hp[clamp(m,0,3)];}
static int terrain_index(int x,int y){x+=8;y-=wall_origin_y();return x>=0&&x<TERRAIN_W&&y>=0&&y<TERRAIN_H?y*TERRAIN_W+x:-1;}
static int terrain_solid(int x,int y){int i=terrain_index(x,y);return terrain.active&&i>=0&&terrain.density[i]>0;}
static void terrain_dirty(int x,int y){if(x<terrain.dirtyLeft)terrain.dirtyLeft=x;if(x>terrain.dirtyRight)terrain.dirtyRight=x;if(y<terrain.dirtyTop)terrain.dirtyTop=y;if(y>terrain.dirtyBottom)terrain.dirtyBottom=y;terrain.revision++;}
static void terrain_write(int i,int hp){int old=terrain.density[i];if(old==hp)return;int x=i%TERRAIN_W,y=i/TERRAIN_W,c=y/28*12+x/28;terrain.density[i]=(u8)hp;
 if(old&&!hp){terrain.solid--;terrain.removed++;terrain.opened[i]=(unsigned short)(ex.env.timer+1);if(--terrain.solidCount[c]==0&&!terrain.scored[c]){terrain.scored[c]=1;g.score+=15;}}
 if(!old&&hp){terrain.solid++;terrain.restored++;terrain.solidCount[c]++;terrain.opened[i]=0;}
 terrain_dirty(x,y);
}
static void terrain_enter(void){unsigned int serial=terrain.serial+1;memset(&terrain,0,sizeof(terrain));terrain.serial=serial;terrain.active=1;terrain.sideOverride=-1;terrain.advanceQ=terrainScrollSpeed;terrain.solid=TERRAIN_N;terrain.lastShipX=g.px/Q;terrain.lastShipY=player_y/Q;for(int i=0;i<TERRAIN_N;i++){terrain.material[i]=(u8)terrain_initial_material(i%TERRAIN_W,i/TERRAIN_W);terrain.density[i]=(u8)terrain_hardness(terrain.material[i]);}for(int i=0;i<TERRAIN_CELLS;i++)terrain.solidCount[i]=784;terrain.dirtyLeft=terrain.dirtyTop=0;terrain.dirtyRight=TERRAIN_W-1;terrain.dirtyBottom=TERRAIN_H-1;terrain.revision=1;}
static int wall_x(int i){return 6+i%12*28;}
static int wall_y(int i){return wall_origin_y()+14+i/12*28;}
static int wall_remaining(void){return terrain.active?terrain.solid*192/TERRAIN_N:0;}
static void terrain_debris(int x,int y,int material){for(int n=0;n<5;n++){unsigned int h=terrain_hash(terrain.impacts*7+n,x*17+y);TerrainChip*c=terrain.chips+terrain.chipHead;terrain.chipHead=(terrain.chipHead+1)%TERRAIN_DEBRIS;*c=(TerrainChip){x*Q,y*Q,(int)(h%241)-120,-80-(int)(h>>8)%180,18+(int)(h>>16)%18,material};}}
static int terrain_chip(int wx,int wy,int radius,int energy){int cx=wx+8,cy=wy-wall_origin_y(),removed=0;terrain.impacts++;ex.env.cellHits++;int material=terrain_material(clamp(cx,0,TERRAIN_W-1),clamp(cy,0,TERRAIN_H-1));
 for(int y=clamp(cy-radius-1,0,TERRAIN_H-1);y<=clamp(cy+radius+1,0,TERRAIN_H-1);y++)for(int x=clamp(cx-radius-1,0,TERRAIN_W-1);x<=clamp(cx+radius+1,0,TERRAIN_W-1);x++){int dx=x-cx,dy=y-cy,r=radius+(int)(terrain_hash(x,y)%3)-1,d=dx*dx+dy*dy;if(d>r*r)continue;int i=y*TERRAIN_W+x,hp=terrain.density[i];if(!hp)continue;int force=clamp(energy*(r*r-d+radius)/(r*r+radius),1,energy);int hard=terrain_cut_resistance(terrain_material(x,y)),depth=energy>=6?clamp(radius*energy/hard,2,radius):clamp(energy*3/hard,1,radius);int excavated=dx*dx*depth*depth+dy*dy*radius*radius<=radius*radius*depth*depth;int next=excavated?0:clamp(hp-force,0,255);if(!next)removed++;terrain_write(i,next);}
 /* Unsupported single-grain remnants crumble into the same carved void. */
 for(int pass=0;pass<2;pass++)for(int y=clamp(cy-radius-1,1,TERRAIN_H-2);y<=clamp(cy+radius+1,1,TERRAIN_H-2);y++)for(int x=clamp(cx-radius-1,1,TERRAIN_W-2);x<=clamp(cx+radius+1,1,TERRAIN_W-2);x++){int i=y*TERRAIN_W+x;if(!terrain.density[i])continue;int neighbours=0;for(int yy=-1;yy<=1;yy++)for(int xx=-1;xx<=1;xx++)if(xx||yy)neighbours+=terrain.density[i+yy*TERRAIN_W+xx]>0;if(neighbours<=4){terrain_write(i,0);removed++;}}
 terrain_debris(wx,wy,material);pc_hit_sound(material==0?2:material==1?3:material==2?0:5,g.weapon,wx);return removed;
}
/* One-unit DDA prevents even fast torpedoes crossing thin remaining material. */
static int terrain_trace(int x,int y,int xx,int yy,int*out){int dx=xx-x,dy=yy-y,steps=absn(dx)>absn(dy)?absn(dx):absn(dy);if(!steps)steps=1;if(steps>1024)steps=1024;for(int s=0;s<=steps;s++){int px=x+dx*s/steps,py=y+dy*s/steps;if(terrain_solid(px,py)){out[0]=px;out[1]=py;return 1;}}return 0;}
static int terrain_projectile(Obj*s,int previousX,int previousY,int hostile){if(journey_kind()!=ENV_WALL||!terrain.active||!s->on)return 0;int hit[2];if(!terrain_trace(previousX/Q,previousY/Q,s->x/Q,s->y/Q,hit))return 0;s->x=hit[0]*Q;s->y=hit[1]*Q;s->on=0;if(!hostile){int radius=s->type==9?12:s->type==4?9:s->type==8?10:s->variant==101?3:s->type==1||s->type==2?6:11;int energy=s->type==9?9:s->type==8?5:clamp(projectile_damage(s)*variant_damage_scale(clamp(s->type,0,8))/100/32,1,9);terrain_chip(hit[0],hit[1],radius,energy);if(s->type==9){ex.torpedoBlasts++;pc_torpedo_sound(1,hit[0]);}else if(s->type==4){ex.missileBlasts++;pc_torpedo_sound(1,hit[0]);}}return 1;}
static int terrain_hull_solid(int x,int y){int rx=flight_side_mode()?6:8,ry=flight_side_mode()?12:10;if(ex.cavernTurn){rx=8;ry=12;}for(int dy=-ry;dy<=ry;dy++)for(int dx=-rx;dx<=rx;dx++)if(dx*dx*ry*ry+dy*dy*rx*rx<=rx*rx*ry*ry&&terrain_solid(x+dx,y+dy))return 1;return 0;}
static void terrain_clear_box(int x,int y,int w,int h){for(int yy=y;yy<y+h;yy++)for(int xx=x;xx<x+w;xx++){int i=terrain_index(xx,yy);if(i>=0&&terrain.density[i])terrain_write(i,0);}}
static void terrain_respawn_clear(void){if(journey_kind()!=ENV_WALL||!terrain.active)return;terrain_clear_box(g.px/Q-14,player_y/Q-100,29,120);terrain.lastShipX=g.px/Q;terrain.lastShipY=player_y/Q;}
static void wall_ship_lost(void){if(ex.env.wallGrace||g.mode!=1)return;ex.lastDamageTick=g.tick;ex.lastDamageAmount=100;ex.lastDamageKind=5;ex.lastDamageSerial++;ex.lastDamageFatal=1;g.lives--;g.barrier=75;g.shield=100;parasites_new_hull();burst(g.px/Q,player_y/Q,1);int safe=-1,best=10000;for(int x=20;x<=300;x+=2)if(!terrain_hull_solid(x,230)&&absn(x-g.px/Q)<best){safe=x;best=absn(x-g.px/Q);}if(safe>=0)g.px=safe*Q;player_y=230*Q;terrain_respawn_clear();if(g.lives<=0){g.mode=g.continues?3:2;g.continueTicks=450;g.continueReady=0;}}
static void wall_regrowth(void){/* Striped, fixed-tick work; only crystal strictly BEHIND the ship. */
 for(int y=g.tick&15;y<TERRAIN_H;y+=16){int wy=y+wall_origin_y();if(wy<=player_y/Q+60)continue;for(int x=0;x<TERRAIN_W;x++){int i=y*TERRAIN_W+x;if(terrain.density[i]||!terrain.opened[i]||terrain_material(x,y)!=3||ex.env.timer+1-terrain.opened[i]<100)continue;if(absn(x-8-g.px/Q)<30&&absn(wy-player_y/Q)<65)continue;terrain_write(i,terrain_hardness(3));}}
}
static int terrain_beam_tip(int lane){if(journey_kind()!=ENV_WALL||!terrain.active)return 26;int hit[2],tip=26;for(int side=-1;side<=1;side++){int offset=side*beam_half_width();if(terrain_trace(g.px/Q+lane*6+offset,player_y/Q-7,beam_center(lane,26)+offset,26,hit)&&hit[1]>tip)tip=hit[1];}return tip;}
/* Denser authored strata approach more slowly, before reaching the hull.
   This is material pacing, never a collision grace or an automatic clearance. */
static int terrain_advance_target(void){if(terrainScrollSpeed!=153)return terrainScrollSpeed;static const int speed[4]={154,128,96,64};int x=clamp(g.px/Q+8,0,TERRAIN_W-1),y=clamp(player_y/Q-100-wall_origin_y(),0,TERRAIN_H-1);return speed[terrain_material(x,y)];}
static void asteroid_wall_step(int mask){terrain.previousScrollQ=terrain.scrollQ;int target=terrain_advance_target();terrain.advanceQ+=clamp(target-terrain.advanceQ,-1,1);terrain.scrollQ+=terrain.advanceQ;if(terrain.turnTest&&ex.env.timer==400){terrain.sideOverride=terrain.sideOverride==1?0:1;ex.cavernTurn=75;}wall_regrowth();for(int n=0;n<TERRAIN_DEBRIS;n++){TerrainChip*c=terrain.chips+n;if(c->life){c->life--;c->x+=c->vx;c->y+=c->vy;c->vy+=9;}}
 /* Beam rays stop at their first surface; carving is once per four simulation ticks. */
 if(g.beam&&(g.tick&3)==0)for(int lane=ex.multi?-1:0;lane<=(ex.multi?1:0);lane++)for(int side=-1;side<=1;side++){int hit[2],offset=side*beam_half_width();if(terrain_trace(g.px/Q+lane*6+offset,player_y/Q-7,beam_center(lane,26)+offset,26,hit))terrain_chip(hit[0],hit[1],3+clamp(beam_half_width()/3,0,5),3);}
 if(g.weapon==8&&(mask&4)&&ex.empTicks&&g.tick%6==0)for(int n=-1;n<=1;n++){int hit[2];if(terrain_trace(g.px/Q,player_y/Q-8,g.px/Q+n*35,player_y/Q-125,hit))terrain_chip(hit[0],hit[1],5,2);}
 if(ex.bombAge&&terrain.novaSerial!=ex.bombSerial){int hit[2];if(terrain_trace(ex.bombX,ex.bombY,ex.bombX,ex.bombY-ex.bombRadius,hit)){terrain_chip(hit[0],hit[1],26,18);terrain.novaSerial=ex.bombSerial;}}
 ex.env.cutter=0;if((mask&4)&&(g.weaponHeat[g.weapon]||g.weaponEnergy[g.weapon]<2)){ex.env.cutter=1;if(g.tick%10==0){int hit[2];if(terrain_trace(g.px/Q,player_y/Q-8,g.px/Q,player_y/Q-140,hit))terrain_chip(hit[0],hit[1],7,4);}}
 int x=g.px/Q,y=player_y/Q,dx=x-terrain.lastShipX,dy=y-terrain.lastShipY,steps=absn(dx)>absn(dy)?absn(dx):absn(dy);if(!steps)steps=1;for(int n=0;n<=steps;n++)if(terrain_hull_solid(terrain.lastShipX+dx*n/steps,terrain.lastShipY+dy*n/steps)){wall_ship_lost();break;}terrain.lastShipX=g.px/Q;terrain.lastShipY=player_y/Q;
}
