static const char*boss_name(int v){static const char*n[12]={"STAR DREADNOUGHT","ION ORACLE","IRON HARVESTER","RIFT LANCER","SIEGE CITADEL","CAVERN WARDEN","SKY MANTA","DUNE SCORPION","FROST CROWN","MAGMA HYDRA","ORBITAL SENTINEL","GROUND LEVIATHAN"};return n[clamp(v,0,11)];}
static void stage_boss_step(Obj*o){int v=o->variant;
 if(o->age<=120){float t=o->age/120.0f,e=t*t*(3-2*t);o->x=160*Q;o->y=(int)((-64+(v==6?138:122)*e)*Q);return;}
 int p=o->hp*3>o->base*2?1:o->hp*3>o->base?2:3;g.bossPhase=p;
 if(p!=o->phase){o->attack=0;o->phase=p;o->morph=55;g.shake=5;pc_impact(o->x>>8,o->y>>8,3);sound(5,1,100,0x2200);sound_slide(5,1,0x5200,55);}o->power+=clamp((p-1)*256-o->power,-6,6);int transforming=o->morph>0;if(o->morph)o->morph--;
 /* Keep the motion clock continuous across morphs and phases. */
 float amplitude=(v==5?12:v==6?47:20+v*2)*(1+o->power/256.0f*.08f),settle=clamp(o->age-120,0,60)/60.0f;settle=settle*settle*(3-2*settle);o->x=160*Q+(int)(sinf(o->age*.0122718463f+v*.417f)*amplitude*Q*settle);o->y=((v==6?74:58)+o->power/12)*Q+(int)(sinf(o->age*.0245436926f+1.57079633f)*(v==3?9:4)*Q*settle);if(transforming)return;
 if(o->attack){o->attack++;if(o->attack>=110)o->attack=0;}else if(o->age>180&&o->age%(p==3?180:240)==0){if((v+o->age/240)%2==0){o->attack=1;o->vx=g.px;o->vy=player_y;sound(5,1,95,0x2400);sound_slide(5,1,0x6800,40);}else for(int k=-1;k<=1;k++){Obj*b=bullet(o->x+k*12*Q,o->y+28*Q,k*130,300+p*40);if(b)b->type=2;}}
 if(o->attack>60&&o->attack<=85){float x=o->x/(float)Q,y=o->y/(float)Q,dx=o->vx/(float)Q-x,dy=o->vy/(float)Q-y,d=sqrtf(dx*dx+dy*dy);if(d>1){float px=g.px/(float)Q-x,py=player_y/(float)Q-y,along=(px*dx+py*dy)/d,perp=fabsf(px*dy-py*dx)/d;if(along>0&&perp<8)hurt_at(g.px/Q,player_y/Q,0,1);}}if(o->attack>0&&o->attack<=85)return;
 int cooldown=clamp(45-v-p*6,14,45);if(o->age%cooldown)return;int x=o->x,y=o->y+(20+o->power/16)*Q;
 if(p>=2)for(int side=-1;side<=1;side+=2){aim(x+side*(p==3?74:62)*Q,y-8*Q,490+p*65);if(p==3)bullet(x+side*(p==3?74:62)*Q,y,side*330,540);}
 switch(v){
 case 0:for(int a=-2;a<=2;a++)bullet(x,y,a*230,390+p*70);aim(x,y,650);break;
 case 1:for(int a=0;a<256;a+=32)bullet(x,y,sn(a+o->age*3)*580/1024,sn(a+o->age*3+64)*580/1024);break;
 case 2:aim(x-25*Q,y,680);aim(x+25*Q,y,680);for(int a=-1;a<=1;a++)bullet(x,y,a*190,420);break;
 case 3:for(int a=-3;a<=3;a++)bullet(x,y,sn(o->age*5+a*15)*450/1024,480+absn(a)*70);break;
 case 4:for(int a=0;a<256;a+=16)bullet(x,y,sn(a+o->age)*550/1024,sn(a+o->age+64)*550/1024);break;
 case 5:{int gap=60+remn(o->age*3,200);for(int a=25;a<310;a+=24)if(absn(a-gap)>34)bullet(a*Q,90*Q,0,400+p*45);break;}
 case 6:for(int wing=-1;wing<=1;wing+=2){for(int a=-1;a<=1;a++)bullet(x+wing*30*Q,y,wing*260+a*150,500+p*50);}break;
 case 7:for(int a=0;a<5;a++)bullet(x+(a-2)*10*Q,y,(a-2)*230+sn(o->age*3)/4,480);aim(x,y,720);break;
 case 10:for(int a=-2;a<=2;a++){bullet(x-27*Q,y,a*210,450+p*45);bullet(x+27*Q,y,-a*210,450+p*45);}break;
 case 11:for(int a=-1;a<=1;a++){aim(x+a*24*Q,y,500+p*50);bullet(x+a*24*Q,y,a*180+sn(o->age*3)/3,430);}break;
 case 8:for(int a=-2;a<=2;a++){bullet(x-22*Q,y,a*190,480);bullet(x+22*Q,y,-a*190,600);}break;
 default:for(int h=-1;h<=1;h++){aim(x+h*24*Q,y,550+h*65);bullet(x+h*24*Q,y,sn(o->age*4+h*43)/2,510);}break;
 }
}
