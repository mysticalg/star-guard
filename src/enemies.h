/* Eight specialist enemies. Their own age clock drives readable tells and attacks. */
static int enemy_guarded(const Obj*o){return (o->type==4&&(o->age<=120||o->morph>0||o->shieldCharge>0||(o->attack>60&&o->attack<=85)))||(o->type==15&&o->age%140<85)||(o->type==18&&o->age%130>=85&&o->age%130<105);}
static void specialist_group(int type,int formation){int n=(type>=23?4:type==19?3:type==15?4:6)+mission_pressure()/6;for(int i=0;i<n;i++){Obj*o=slot(g.enemies,MAXE);if(!o)break;*o=(Obj){.on=1,.type=type,.hp=(type==19?8:type==15?6:3)+enemy_bonus(),.x=(38+i*244/(n-1))*Q,.y=(-24-i*22)*Q,.base=38+i*244/(n-1),.phase=i*23+formation*41};}}
static void ground_group(int type,int formation){for(int n=0;n<4+mission_pressure()/7;n++){Obj*o=slot(g.enemies,MAXE);if(o)*o=(Obj){.on=1,.type=type,.hp=(type==23?5:4)+enemy_bonus(),.x=(type==23?276:273)*Q,.y=(-32-n*66-formation*8)*Q,.base=293,.phase=n*17};}}
static void specialist_step(Obj*o){int t=o->age,x=o->x,y=o->y;
 switch(o->type){
 case 26:if(t<65||t>260){o->y+=t>260?1100:420;}else{int dx=g.px-x,dy=player_y-y,d=absn(dx)+absn(dy);if(!d)d=1;int speed=900;o->vx=clamp(o->vx+clamp(dx*speed/d-o->vx,-40,40),-900,900);o->vy=clamp(o->vy+clamp(dy*speed/d-o->vy,-40,40),-900,900);o->x+=o->vx;o->y+=o->vy;}break;
 case 25:o->x=o->base*Q+sn(t*2+o->phase)*5;if(t<100)o->y+=360;else if(t<155)o->y+=40;else o->y-=800;if(t==105||t==145){Obj*b=bullet(o->x,o->y,0,30);if(b){b->type=t==145?6:2;if(b->type==6)b->hp=3;}pc_impact(o->x/Q,o->y/Q,1);}break;
 case 23:case 24:o->x=(o->type==23?276:273)*Q;o->y+=o->type==23?400:300;if(y>30*Q&&y<235*Q&&t%90==70){aim(x-7*Q,y,490);if(o->type==23){aim(x-7*Q,y-5*Q,560);}}break;
 case 14:o->x=o->base*Q+sn(t*3+o->phase)*7;o->y+=t<170?180:520;if(t%75==40&&y>28*Q)for(int a=-1;a<=1;a++)bullet(x,y,a*340,460);break;
 case 15:o->x=o->base*Q+sn(t+o->phase)*3;o->y+=100;if(t%140==90&&y>30*Q)for(int a=-2;a<=2;a++)armored_bullet(x,y,a*170,390);break;
 case 16:o->x=160*Q+sn(t*2+o->phase)*30;o->y+=235;if(t%80==45&&y>32*Q&&y<200*Q)hazard(3,x>>8,y>>8);break;
 case 17:if(t<100){o->x=o->base*Q+sn(t*2+o->phase)*4;o->y+=170;if(t==80){int d=absn(g.px-x)+absn(player_y-y);o->vx=divn((g.px-x)*1450,d);o->vy=divn((player_y-y)*1450,d);}}else{o->x+=o->vx;o->y+=o->vy;if(t>200)o->y+=900;}break;
 case 18:o->y+=165;if(t%130==85){pc_impact(x>>8,y>>8,3);o->base=24+remn(o->base+113,272);}o->x=o->base*Q+sn(t*2+o->phase)*3;if(t%130==110&&y>24*Q)aim(o->x,o->y,700);break;
 case 19:o->x=o->base*Q+sn(t+o->phase)*4;o->y+=110;if(t%100==65&&y>30*Q){armored_aim(x-12*Q,y,450);armored_aim(x+12*Q,y,450);}break;
 case 20:if(t<300){o->x=o->base*Q+sn(t*3+o->phase)*9;o->y+=t<100?200:45;}else o->y+=640;if(t%36==0&&y>30*Q){int a=t*5+o->phase;bullet(x,y,sn(a)*550/1024,sn(a+64)*550/1024);}break;
 case 22:o->x=(ex.phase==5?tunnel_lane(o->y>>8,o->phase&1)*Q:o->base*Q)+sn(t*2+o->phase)*(ex.phase==5?2:3);o->y-=480+(t>220?300:0);if(t>80&&t%95==50&&y>32*Q&&y<220*Q)aim(o->x,o->y,480);break;
 case 21:o->x=o->base*Q+sn(t+o->phase)*6;o->y+=120;if(t%150==80){o->vx=g.px;o->vy=player_y;}if(t%150==115&&y>28*Q){int dx=o->vx-x,dy=o->vy-y,d=absn(dx)+absn(dy);bullet(x,y,divn(dx*1400,d),divn(dy*1400,d));sound(4,1,55,0x4800);}break;
 }
}
static void carrier_release(const Obj*parent){if(parent->type!=19)return;for(int n=-1;n<=1;n+=2){Obj*o=slot(g.enemies,MAXE);if(o)*o=(Obj){.on=1,.type=17,.hp=2,.x=parent->x+n*14*Q,.y=parent->y,.base=(parent->x>>8)+n*14,.age=99,.vx=n*600,.vy=750};}}
