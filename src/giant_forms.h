/* Shared encounter state; each boss supplies its own anatomy and attack plan. */
static float giant_morph_fraction(void){Giant*b=&ex.giant;float t=1-b->morph/60.0f;return t*t*(3-2*t);}
static float giant_body_shift(void){return ex.giant.rootX;}
static float giant_body_heave(void){return ex.giant.rootY;}
/* Slow, predictable encounter paths; all artwork and physical modules share this root. */
static void giant_move_root(void){Giant*b=&ex.giant;if(b->archetype==4){b->rootX=b->rootY=0;return;}static const float amp[6]={28,24,36,26,0,30},rate[6]={.42f,.36f,.30f,.5f,0,.44f};float t=b->timer/50.0f,form=b->morph?b->oldForm+(b->form-b->oldForm)*giant_morph_fraction():b->form;float a=amp[b->archetype]*(1+.15f*form);b->rootX=sinf(t*rate[b->archetype])*a;b->rootY=sinf(t*rate[b->archetype]*(b->archetype==1?2:1.4f))*(b->archetype==5?8:5);}
static void giant_layout(void){Giant*b=&ex.giant;static const int base[9][2]={{64,130},{256,130},{98,65},{222,65},{110,44},{210,44},{160,114},{110,156},{210,156}};
 static const int middle[6][9][2]={
 {{64,53},{256,53},{42,137},{278,137},{104,72},{216,72},{160,119},{45,120},{275,120}},
 {{64,53},{256,53},{52,142},{268,142},{112,98},{208,98},{160,122},{64,90},{256,90}},
 {{64,53},{256,53},{45,114},{275,114},{105,163},{215,163},{160,110},{60,80},{260,80}},
 {{64,53},{256,53},{48,140},{272,140},{115,70},{205,70},{160,118},{48,96},{272,96}},
 {{64,53},{256,53},{64,145},{256,145},{110,44},{210,44},{160,114},{110,156},{210,156}},
 {{64,53},{256,53},{40,145},{280,145},{104,65},{216,65},{160,117},{40,98},{280,98}}};
 static const int late[6][9][2]={
 {{64,53},{256,53},{42,137},{278,137},{104,72},{216,72},{160,114},{48,112},{272,112}},
 {{64,53},{256,53},{52,142},{268,142},{112,98},{208,98},{160,125},{65,132},{255,132}},
 {{64,53},{256,53},{45,114},{275,114},{105,163},{215,163},{151,108},{68,100},{267,143}},
 {{64,53},{256,53},{48,140},{272,140},{115,70},{205,70},{160,116},{52,135},{268,135}},
 {{64,53},{256,53},{64,145},{256,145},{110,44},{210,44},{160,114},{110,156},{210,156}},
 {{64,53},{256,53},{40,145},{280,145},{104,65},{216,65},{160,112},{42,116},{278,116}}};
 float t=b->morph?giant_morph_fraction():1,scale=giant_world_scale();for(int i=0;i<9;i++){GiantPart*p=b->parts+i;const int*to=b->form==0?base[i]:b->form==1?middle[b->archetype][i]:late[b->archetype][i];const int*from=b->oldForm==0?base[i]:b->oldForm==1?middle[b->archetype][i]:late[b->archetype][i];float x=from[0]+(to[0]-from[0])*t,y=from[1]+(to[1]-from[1])*t;if(b->archetype==5){float cycle=b->timer*.035f+i*.8f;x+=sinf(cycle)*(i==6?1.5f:3+b->form*2);y+=cosf(cycle*.8f)*(i==6?2:4+b->form);p->angle=i==6?0:(int)(sinf(cycle)*5);}float sway=giant_body_shift();p->x=160+(int)((x-160+sway)*scale);p->y=100+(int)((y-100+giant_body_heave())*scale);p->w=(int)((i==6?38:i>=7?42:b->form&&i>=4?34:28)*scale);p->h=(int)((i==6?38:i>=7?(b->archetype==1?50:44):b->form&&i>=4?40:30)*scale);}}
static void giant_set_form(int form){Giant*b=&ex.giant;if(form<=b->form||!b->active)return;b->oldForm=b->form;b->form=form;b->morph=60;b->formAge=0;b->attackKind=b->attackClock=0;b->transformSerial++;
 for(int i=0;i<MAXB;i++)g.bullets[i].on=0;for(int i=0;i<MAXS;i++)g.shots[i].on=0;for(int i=0;i<32;i++)ex.env.hazards[i].on=0;
 int first=b->archetype==4?7:form==1?4:7;for(int i=first;i<first+2;i++){GiantPart*p=b->parts+i;p->hp=p->maxHp=(form==1?1600:2200)*(100+3*(g.pilotLevel-1))/100;p->dotTicks=p->dotClock=p->damageUntil=0;}
 pc_impact(160,100,3);sound(5,1,90,0x2200);sound_slide(5,1,0x5200,60);
}
static void giant_check_form(void){Giant*b=&ex.giant;if(!b->active)return;if(b->archetype==4){if(b->partsDestroyed>=2&&b->form<1)giant_set_form(1);if(b->parts[6].hp*3<=b->parts[6].maxHp*2&&b->form<2)giant_set_form(2);b->phase=b->form==2?4:b->form==1?2:1;}else{if(b->phase<4)b->phase=giant_live(0)||giant_live(1)?1:giant_live(2)||giant_live(3)||giant_live(4)||giant_live(5)?2:3;if(b->phase>=2&&b->form<1)giant_set_form(1);if(b->phase>=4&&b->form<2)giant_set_form(2);}}
