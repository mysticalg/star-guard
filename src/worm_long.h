static int swept_ellipse(float,float,float,float,float,float);
/* A long physical chain follows the head at four-tick intervals. The underground
   return is deliberately much larger than the viewport; camera never fits it. */
#define WORM_PERIOD 900
#define WORM_EXPOSED 180
#define WORM_LAG 4
static float worm_catmull(float p0,float p1,float p2,float p3,float t){return .5f*(2*p1+(-p0+p2)*t+(2*p0-5*p1+4*p2-p3)*t*t+(-p0+3*p1-3*p2+p3)*t*t*t);}
static float worm_center_sample(int y){return (floor_surface_side(y,-1)+floor_surface_side(y,1))*.5f;}
static float worm_center(float y){int lo=(int)floorf(y/16)*16;return worm_catmull(worm_center_sample(lo-16),worm_center_sample(lo),worm_center_sample(lo+16),worm_center_sample(lo+32),(y-lo)/16);}
static void worm_curve(float c,int form,float*x,float*y){float center=worm_center(130),left=center-100,right=center+100;
 if(c<WORM_EXPOSED){static const float points[14][2]={{-140,0},{-100,0},{-60,0},{-55,-65},{0,0},{55,65},{65,0},{55,-65},{0,0},{-55,65},{0,90},{70,82},{100,0},{200,82}};float u=c*(11.0f/WORM_EXPOSED);int i=clamp((int)u,0,10);float t=u-i,xx=worm_catmull(points[i][0],points[i+1][0],points[i+2][0],points[i+3][0],t),yy=worm_catmull(points[i][1],points[i+1][1],points[i+2][1],points[i+3][1],t);if(form==1)yy=-yy;*y=130+yy;*x=worm_center(*y)+xx;}
 else{float u=(c-WORM_EXPOSED)/(WORM_PERIOD-WORM_EXPOSED),v=1-u;*x=v*v*v*right+3*v*v*u*1400+3*v*u*u*(-1080)+u*u*u*left;*y=130+3*v*v*u*970+3*v*u*u*970;}
}
static WormSegment worm_sample(int timer,int index,int form){float time=timer-120-index*WORM_LAG;int cycle=(int)floorf(time/WORM_PERIOD);float c=time-cycle*WORM_PERIOD,x,y,xx,yy,ahead=c+.1f;worm_curve(c,form,&x,&y);if(ahead>=WORM_PERIOD)ahead-=WORM_PERIOD;worm_curve(ahead,form,&xx,&yy);float dx=xx-x,dy=yy-y,angle=atan2f(dy,dx)*57.2957795f+90;int visible=c<WORM_EXPOSED&&time>=0&&y>=30&&y<=235&&x>tunnel_left((int)y)-12&&x<tunnel_right((int)y)+12;return (WormSegment){x,y,angle,visible};}
static void worm_path(int form,int i,int*x,int*y,int*visible,int*angle){WormSegment s=worm_sample(ex.giant.timer,i==6?0:i+1,form);*x=(int)s.x;*y=(int)s.y;*visible=s.visible;*angle=(int)s.angle;}
static void worm_positions(void){Giant*b=&ex.giant;for(int i=0;i<WORM_SEGMENTS;i++){WormSegment s=worm_sample(b->timer,i,b->form);if(b->morph){WormSegment old=worm_sample(b->timer,i,b->oldForm);float t=giant_morph_fraction();s.x=old.x+(s.x-old.x)*t;s.y=old.y+(s.y-old.y)*t;float angle=fmodf(s.angle-old.angle+540,360)-180;s.angle=old.angle+angle*t;s.visible=s.visible||old.visible;}b->worm[i]=s;}WormSegment*h=b->worm;GiantPart*p=b->parts+6;p->x=(int)h->x;p->y=(int)h->y;p->visible=h->visible;p->angle=(int)h->angle;p->w=44+b->form*8;p->h=48+b->form*8;for(int i=0;i<9;i++)if(i!=6)b->parts[i].hp=0;}
static void worm_ship_lost(void){if(player_is_protected()||g.mode!=1)return;ex.lastDamageTick=g.tick;ex.lastDamageAmount=g.barrier;ex.lastDamageKind=6;ex.lastDamageSerial++;ex.lastDamageFatal=1;g.lives--;parasites_new_hull();g.barrier=75;g.shield=250;g.respawnTicks=250;g.shake=8;burst(g.px/Q,player_y/Q,1);pc_hit_sound(3,4,g.px/Q);player_respawn_safe();if(g.lives<=0){g.mode=g.continues?3:2;g.continueTicks=450;g.continueReady=0;}}
static void worm_body_contact(void){Giant*b=&ex.giant;if(player_is_protected())return;float px=g.px/(float)Q,py=player_y/(float)Q,ox=ex.damagePrevX/(float)Q,oy=ex.damagePrevY/(float)Q;
 for(int i=0;i<WORM_SEGMENTS;i++){WormSegment*s=b->worm+i;if(!s->visible)continue;WormSegment prev=worm_sample(b->timer-1,i,b->form);float r=i?12+b->form*2:16+b->form*3;if(swept_ellipse((prev.visible?prev.x:s->x)-ox,(prev.visible?prev.y:s->y)-oy,s->x-px,s->y-py,r+player_hull_rx(),r+player_hull_ry())){worm_ship_lost();return;}
 if(i&&b->worm[i-1].visible){WormSegment*next=b->worm+i-1;float dx=next->x-s->x,dy=next->y-s->y;if(dx*dx+dy*dy<2500&&swept_ellipse(s->x-px,s->y-py,next->x-px,next->y-py,r+player_hull_rx(),r+player_hull_ry())){worm_ship_lost();return;}}
 }
}

static int worm_shot_contact(const Obj*s,const GiantPart*p){WormSegment prev=worm_sample(ex.giant.timer-1,0,ex.giant.form);float x=s->age?s->previousX/(float)Q:s->x/(float)Q,y=s->age?s->previousY/(float)Q:s->y/(float)Q;return swept_ellipse(x-(prev.visible?prev.x:p->x),y-(prev.visible?prev.y:p->y),s->x/(float)Q-p->x,s->y/(float)Q-p->y,p->w*.38f+3,p->h*.4f+3);}
