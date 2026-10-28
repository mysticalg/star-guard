typedef struct{int tick,wave,section,kind;float x,y,vx,vy,size,spin;} IceShard83;
static IceShard83 iceShards83[96];static unsigned iceShardHead83,iceFxSerial83;
static void pc_ice_sound83(int,int,int);
void pc_ice_reset83(void){memset(iceShards83,0,sizeof(iceShards83));iceShardHead83=iceFxSerial83=0;}
void pc_ice_shift83(int gap){for(int i=0;i<96;i++)if(iceShards83[i].kind)iceShards83[i].tick+=gap;}
void pc_ice_event83(int kind,int x,int y,int radius){pc_ice_sound83(kind,x,y);if(!g.fx)return;unsigned serial=++iceFxSerial83;int count=kind==2?(reducedMotion?14:32):reducedMotion?5:12;for(int i=0;i<count;i++){unsigned h=serial*2654435761u+i*2246822519u;float a=(h&65535)*2*PI/65536,speed=(kind==2?24:9)+(h>>16)%35;IceShard83*p=iceShards83+iceShardHead83++%96;*p=(IceShard83){g.tick,g.wave,ex.section,kind,x+cosf(a)*radius*.35f,y+sinf(a)*radius*.35f,cosf(a)*speed,sinf(a)*speed,kind==2?1.3f+(h%9)*.2f:.7f+(h%5)*.2f,(h%11-5.f)*.6f};}}
