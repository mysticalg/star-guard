static const u8 landscape_sky[4][16]={{9, 10, 10, 11, 11, 164, 164, 43, 196, 196, 197, 197, 198, 198, 199, 199}, {12, 3, 6, 7, 7, 56, 56, 56, 57, 57, 51, 92, 92, 93, 93, 94}, {9, 10, 10, 11, 42, 164, 165, 196, 197, 197, 198, 198, 199, 248, 248, 249}, {2, 2, 2, 2, 2, 5, 6, 6, 6, 6, 7, 7, 7, 7, 7, 18}};
static const u8 landscape_ridge[4][4]={{165, 11, 40, 2}, {58, 56, 6, 2}, {216, 166, 11, 9}, {7, 6, 2, 1}};
static const u8 landscape_cloud[4]={249, 95, 253, 57};
static int row_phase(int y){return ex.entryY&&y>=ex.entryY?ex.oldPhase:ex.phase;}
static void landscape_pixel(u8*b,int x,int y,int c,int phase){if(row_phase(y)==phase)pixel(b,x,y,c);}
static void draw_landscapes(u8*b){int scroll=ex.landScroll>>8;
 for(int y=24;y<256;y++){int phase=row_phase(y);if(phase<6)continue;int v=phase-6,c=landscape_sky[v][clamp((y-24)*15/210,0,15)];solid_span(b,y,0,320,c);}
 for(int phase=6;phase<10;phase++){if(phase!=ex.phase&&(!ex.entryY||phase!=ex.oldPhase))continue;int v=phase-6;
 /* A distant sun and high clouds move slowly; each mountain layer has its own velocity. */
 int sunx=245-(scroll/24)%38,suny=phase==9?93:76;
 for(int yy=-18;yy<=18;yy++)for(int xx=-18;xx<=18;xx++)if(xx*xx+yy*yy<324)landscape_pixel(b,sunx+xx,suny+yy,phase==9?87:phase==7?223:255,phase);
 for(int layer=0;layer<2;layer++)for(int cloud=0;cloud<6;cloud++){int cx=((cloud*79-scroll/(layer?3:7)+v*23)%440+440)%440-60,cy=45+((cloud*37+v*13)%78)+layer*28;
 for(int yy=-8;yy<=8;yy++){int width=28-(yy*yy)/3;for(int xx=-width;xx<=width;xx++)landscape_pixel(b,cx+xx,cy+yy,landscape_cloud[v]-(layer?0:0),phase);}}
 for(int x=0;x<320;x++){for(int layer=0;layer<4;layer++){int world=x+scroll/(layer==0?8:layer==1?4:layer==2?2:1),height=layer==0?155:layer==1?188:layer==2?220:246;
 int amplitude=layer==0?30:layer==1?22:layer==2?12:5;
 int peak=height+(sn(world/(layer==0?2:1)+v*27)*amplitude>>10)+(sn(world*3+layer*57)*amplitude>>12);
 if(phase==7)peak=height+(sn(world/2+v*17)*amplitude>>10);if(phase==8&&layer<2)peak-=absn(sn(world*2))*amplitude/1500;
 for(int y=clamp(peak,24,256);y<256;y++){int c=landscape_ridge[v][layer];if(phase==8&&y<peak+3&&layer<3)c=255;if(phase==9&&layer==3&&((world+y*7)&31)<3)c=87;landscape_pixel(b,x,y,c,phase);}
 }}
 }
}
static void draw_stage_boss(u8*b,Obj*o,int x,int y){int v=o->variant;if(v==0){pc_sprite(2,o->x/256.0f,o->y/256.0f);return;}
 int colours[10]={143,235,7,139,143,87,235,223,235,87},c=colours[v];
 /* Distinct silhouettes: orbital oracle, armoured harvester, lance, citadel,
    rock warden, broad manta, scorpion, ice crown and three-headed hydra. */
 for(int yy=-22;yy<=22;yy++){int w=v==1?24-absn(yy)/2:v==2?32-absn(yy)/3:v==3?8+(yy+22)/2:v==4?36-((absn(yy)/6)&1)*9:v==5?24-absn(yy)/3:v==6?43-absn(yy)*3/2:v==7?26-absn(yy)/2:v==8?30-absn(yy):20-absn(yy)/3;
 for(int xx=-w;xx<=w;xx++){int edge=absn(xx)>w-3||absn(yy)>19;pixel(b,x+xx,y+yy,edge?c:((xx+yy+g.tick/5)&15)<2?c:8);}}
 int pods=v==9?3:v==1?4:v==8?5:2;
 for(int n=0;n<pods;n++){int a=(v==1?g.tick*2:0)+n*256/pods,px=x+(sn(a)*(v==6?36:29)>>10),py=y+(sn(a+64)*(v==9?6:18)>>10);if(v==9){px=x+(n-1)*25;py=y+18;}
 ring(b,px,py,v==8?7:9,v==7?1:v==9?2:3,g.tick*2);glow(b,px,py,v==9?1:3,3);pixel(b,px,py,255);
 if(v==7||v==8)stroke(b,px,py,px+(n&1?10:-10),py-19,c);}
 if(v==1||v==4)ring(b,x,y,37,v==4?2:3,g.tick);if(v==6){stroke(b,x-40,y,x-23,y+27,c);stroke(b,x+40,y,x+23,y+27,c);}if(v==7)stroke(b,x,y-21,x+sn(g.tick*3)/70,y-40,c);
 int bar=clamp(o->hp*66/o->base,0,66);solid_span(b,y-29,x-33,x+33,8);solid_span(b,y-29,x-33,x-33+bar,c);
}
