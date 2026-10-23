static const char*boss_name(int v){static const char*n[10]={"STAR DREADNOUGHT","ION ORACLE","IRON HARVESTER","RIFT LANCER","SIEGE CITADEL","CAVERN WARDEN","SKY MANTA","DUNE SCORPION","FROST CROWN","MAGMA HYDRA"};return n[clamp(v,0,9)];}
static void stage_boss_step(Obj*o){int v=o->variant,p=o->hp*3>o->base*2?1:o->hp*3>o->base?2:3;g.bossPhase=p;
 if(p!=o->phase){o->phase=p;g.shake=5;pc_impact(o->x>>8,o->y>>8,3);}
 int amplitude=v==5?12:v==6?47:20+v*2;o->x=160*Q+sn(o->age/(p==3?1:2)+v*17)*amplitude;o->y=(v==6?74:58)*Q+sn(o->age+64)*(v==3?9:4);
 int cooldown=clamp(45-v-p*4,18,45);if(o->age%cooldown)return;int x=o->x,y=o->y+20*Q;
 switch(v){
 case 0:for(int a=-2;a<=2;a++)bullet(x,y,a*230,390+p*70);aim(x,y,650);break;
 case 1:for(int a=0;a<256;a+=32)bullet(x,y,sn(a+o->age*3)*580/1024,sn(a+o->age*3+64)*580/1024);break;
 case 2:aim(x-25*Q,y,680);aim(x+25*Q,y,680);for(int a=-1;a<=1;a++)bullet(x,y,a*190,420);break;
 case 3:for(int a=-3;a<=3;a++)bullet(x,y,sn(o->age*5+a*15)*450/1024,480+absn(a)*70);break;
 case 4:for(int a=0;a<256;a+=16)bullet(x,y,sn(a+o->age)*550/1024,sn(a+o->age+64)*550/1024);break;
 case 5:{int gap=60+remn(o->age*3,200);for(int a=25;a<310;a+=24)if(absn(a-gap)>34)bullet(a*Q,90*Q,0,400+p*45);break;}
 case 6:for(int wing=-1;wing<=1;wing+=2){for(int a=-1;a<=1;a++)bullet(x+wing*30*Q,y,wing*260+a*150,500+p*50);}break;
 case 7:for(int a=0;a<5;a++)bullet(x+(a-2)*10*Q,y,(a-2)*230+sn(o->age*3)/4,480);aim(x,y,720);break;
 case 8:for(int a=-2;a<=2;a++){bullet(x-22*Q,y,a*190,480);bullet(x+22*Q,y,-a*190,600);}break;
 default:for(int h=-1;h<=1;h++){aim(x+h*24*Q,y,550+h*65);bullet(x+h*24*Q,y,sn(o->age*4+h*43)/2,510);}break;
 }
}
