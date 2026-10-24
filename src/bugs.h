/* Compact insect squadrons: banked entry, hovering ranks, individual dive sorties. */
static void bug_group(int type,int formation){int n=g.wave>=4?32:24;for(int i=0;i<n;i++){Obj*o=slot(g.enemies,MAXE);if(!o)break;int col=i&7,row=i>>3;o->type=type;o->hp=type==13?3:1+(g.wave>=12);o->phase=i+((formation&1)<<5);o->base=36+col*35;o->vx=(42+row*21)*Q;o->vy=g.px;o->x=((formation&1)?344:-24)*Q;o->y=-28*Q;}}
static void bug_step(Obj*o){int ordinal=o->phase&31,delay=(ordinal&7)*3+(ordinal>>3)*6,t=o->age-delay,sortie=245+ordinal*9+(o->type-10)*13;if(t<0){o->y=-28*Q;return;}
 if(t<95){int u=t*256/95,from=(o->phase&32)?344:-24;o->x=from*Q+(o->base-from)*u+((sn(t*3+ordinal*7)*9*(256-u))>>8);o->y=-28*Q+((o->vx+28*Q)*u>>8)-sn(t*2)*5;}
 else if(t<sortie){o->x=o->base*Q+sn((g.tick>>1)+o->phase*5)*2;o->y=o->vx+(sn(g.tick+ordinal*8)*3>>1);}
 else{int d=t-sortie;if(d==0)o->vy=g.px;if(o->type==12&&o->variant<2){int u=d*256/150;o->x=o->base*Q+sn(u*2)*18;o->y=o->vx+sn(u>>1)*28;if(d>=150){o->variant++;if(o->variant<2)o->age=delay+100;}}
  else{if(o->type==12)d-=150;o->x=o->base*Q+((o->vy-o->base*Q)*clamp(d,0,80)/80)+sn(d*3+o->phase)*6;o->y=o->vx+d*(o->type==11?760:550);}}
 if(t>110&&((o->age+ordinal*13)&127)==80&&o->y>30*Q&&o->y<205*Q)aim(o->x,o->y,420+(g.wave<<2));
}
static void ricochet(Obj*s,Obj*enemy,int id){float nx=(float)(s->x-enemy->x),ny=(float)(s->y-enemy->y),len=sqrtf(nx*nx+ny*ny);if(len<1){nx=0;ny=1;len=1;}nx/=len;ny/=len;float vx=s->vx,vy=s->vy,dot=vx*nx+vy*ny;vx-=2*dot*nx;vy-=2*dot*ny;
 /* A small alternating cant avoids perfectly vertical rebounds. */
 float angle=((g.tick+id)&1)?.32f:-.32f,c=cosf(angle),snv=sinf(angle);s->vx=(int)(vx*c-vy*snv);s->vy=(int)(vx*snv+vy*c);int r=enemy->type==4?34:enemy->type>=10?11:16;s->x=enemy->x+(int)(nx*r*Q);s->y=enemy->y+(int)(ny*r*Q);s->base=3;s->phase=id;if(--s->hp<=0)s->on=0;pc_impact(s->x>>8,s->y>>8,1);sound(1,1,65,0x3800+(4-s->hp)*0x300);
}
static int bomb_reached(int x,int y,int padding){int dx=x-ex.bombX,dy=y-ex.bombY,r=ex.bombRadius+padding;return dx*dx*64+dy*dy*100<=r*r*64;}
static void bomb_step(void){if(!ex.bombAge)return;ex.bombRadius=ex.bombAge*9;
 for(int i=0;i<MAXE;i++){Obj*o=g.enemies+i;if(o->on&&!(o->type==4&&o->age<=120)&&bomb_reached(o->x>>8,o->y>>8,o->type==4?30:10))killed(o);}
 for(int i=0;i<MAXB;i++){Obj*o=g.bullets+i;if(o->on&&bomb_reached(o->x>>8,o->y>>8,3)){o->on=0;pc_impact(o->x>>8,o->y>>8,3);}}
 for(int i=0;i<12;i++){Obj*o=ex.obstacles+i;int contact=o->on&&bomb_reached(o->x>>8,o->y>>8,o->base);if(o->on&&o->type==2){int gx=o->x>>8,gy=o->y>>8,yy=clamp(ex.bombY,gy-6,gy+6);contact=bomb_reached(clamp(ex.bombX,0,gx-52),yy,0)||bomb_reached(clamp(ex.bombX,gx+53,320),yy,0);}if(contact){o->on=0;g.score+=25;pc_impact(o->x>>8,o->y>>8,3);}}
 if(++ex.bombAge>60){ex.bombAge=0;ex.bombRadius=0;}
}
