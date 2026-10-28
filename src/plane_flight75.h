/* Formation-wide routes: no random position changes, extra sorties or clock changes.
 * Y speed and all weapons remain owned by each existing type's simulation. */
static void plane_flight75(Obj*o){
 int t=o->age,p=o->flightPattern,phase=p*29,side=cavern_guard_active()?(o->caveSide?1:-1):(o->base<160?-1:1);
 float enter=fminf(1,t/50.0f);enter=enter*enter*(3-2*enter);
 float x=0,amp=.8f+(p%3)*.2f;
 switch(o->type){
 case 0:x=sn(t+phase)*14.0f/1024;break; /* banked paired/V ranks */
 case 1:{float u=fmaxf(0,fminf(1,(t-40)/170.0f)),arc=sinf(u*3.14159265f);float edge=side<0?o->base+38:358-o->base;x=side*edge*arc*arc;break;} /* flank out, arc back through entry lane */
 case 2:x=sn(t*2+phase)*28.0f/1024;break; /* diving S attack */
 case 3:x=sn((t>>1)+phase)*32.0f/1024;break; /* broad heavy patrol */
 case 5:x=(sn(t+phase)*28.0f+sn(t*2+phase)*9.0f)/1024;break; /* echelon strafe */
 case 6:x=sn(t*2+phase+(o->flightMember&1)*128)*20.0f/1024;break; /* paired escort weave */
 case 7:x=sn((t>>1)+phase)*12.0f/1024;break; /* steady minelayer patrol */
 default:return;
 }
 if(cavern_guard_active())x*=.18f; /* actual canonical guard retains the chosen branch */
 o->x=(int)((o->base+x*enter*amp)*Q);
}
