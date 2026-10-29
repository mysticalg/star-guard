/* The cached physical segment owns both edges; drawing never advances a clock. */
static void draw_enemy_mega88(const Obj*o){
 const EnemyMega88*m=&o->mega88;
 if(m->stamp!=g.tick||!m->phase||enemy_mega_blocked88(o))return;
 float dx=m->bx-m->ax,dy=m->by-m->ay,d=hypotf(dx,dy);if(d<1)return;dx/=d;dy/=d;
 float nx=-dy*m->half,ny=dx*m->half;
 if(m->phase==1){
  for(int k=0;k<d;k+=16){float end=fminf(d,k+8);
   journey_line(m->ax+dx*k+nx,m->ay+dy*k+ny,m->ax+dx*end+nx,m->ay+dy*end+ny,255,100,110,175);
   journey_line(m->ax+dx*k-nx,m->ay+dy*k-ny,m->ax+dx*end-nx,m->ay+dy*end-ny,255,100,110,175);
  }
  journey_line(m->ax,m->ay,m->bx,m->by,255,145,95,125);
 }else if(m->phase==2){
  float a=m->phase==3?(ENEMY_MEGA_WARNING88+ENEMY_MEGA_FIRE88+ENEMY_MEGA_TAIL88-o->weaponClock)/(float)ENEMY_MEGA_TAIL88:1;
  nquad=0;beam_ribbon(m->ax,m->ay,m->bx,m->by,m->half+5,(SDL_FColor){1,.05f,.12f,.09f*a});
  beam_ribbon(m->ax,m->ay,m->bx,m->by,m->half,(SDL_FColor){1,.13f,.08f,.32f*a});
  beam_ribbon(m->ax,m->ay,m->bx,m->by,7,(SDL_FColor){1,.57f,.25f,.65f*a});
  beam_ribbon(m->ax,m->ay,m->bx,m->by,2.5f,(SDL_FColor){1,.95f,.82f,.95f*a});flush(halo);
  journey_line(m->ax+nx,m->ay+ny,m->bx+nx,m->by+ny,255,70,75,(int)(215*a));
  journey_line(m->ax-nx,m->ay-ny,m->bx-nx,m->by-ny,255,70,75,(int)(215*a));
 }
 if(m->phase==3){float fade=(ENEMY_MEGA_WARNING88+ENEMY_MEGA_FIRE88+ENEMY_MEGA_TAIL88-o->weaponClock)/(float)ENEMY_MEGA_TAIL88;nquad=0;beam_ribbon(m->ax,m->ay,m->bx,m->by,1,(SDL_FColor){1,.7f,.4f,.12f*fade});flush(halo);}
 float rad=m->phase==1?5+o->weaponClock*.11f:11;
 for(int n=0;n<16;n++){float a=n*PI/8,b=(n+1)*PI/8;journey_line(m->ax+cosf(a)*rad,m->ay+sinf(a)*rad,m->ax+cosf(b)*rad,m->ay+sinf(b)*rad,255,180,120,200);}
}
