/* Original painted clouds and rocks; no new remote assets. */
static void draw_weather_atmosphere(void){Weather*w=&ex.env.weather;if(!w->kind||!w->stage)return;float strength=w->stage==1?((opening_front()?100:500)-w->left)/(float)(opening_front()?100:500):w->stage==3?w->left/(float)(opening_front()?100:500):1;int kind=w->kind;
 if(kind==WEATHER_STORM){journey_rect(play_min_x()-200,play_min_y()-200,play_max_x()-play_min_x()+400,play_max_y()-play_min_y()+400,4,8,20,(int)(100*strength));SDL_SetTextureColorMod(cloudTexture,45,55,75);SDL_SetTextureBlendMode(cloudTexture,SDL_BLENDMODE_BLEND);scrolling_layer(cloudTexture,380,1.8f,170*strength,1);scrolling_layer(cloudTexture,640,1.2f,130*strength,1);}
 if(kind==WEATHER_ACID||kind==WEATHER_ION||kind==WEATHER_ICE){SDL_SetTextureBlendMode(cloudTexture,SDL_BLENDMODE_BLEND);SDL_SetTextureColorMod(cloudTexture,kind==WEATHER_ACID?125:kind==WEATHER_ION?125:170,kind==WEATHER_ACID?150:kind==WEATHER_ION?125:195,kind==WEATHER_ACID?75:kind==WEATHER_ION?180:215);scrolling_layer(cloudTexture,40,1.6f,45*strength,1);scrolling_layer(cloudTexture,100,.9f,30*strength,1);}
 SDL_SetTextureColorMod(cloudTexture,255,255,255);
 if(organic_context()&&g.fx){/* Layered, low-alpha drifting spores, sampled from simulation time. */SDL_SetTextureBlendMode(smokeTexture,SDL_BLENDMODE_BLEND);SDL_SetTextureColorMod(smokeTexture,150,170,115);int count=reducedMotion?12:quality==2?48:28;for(int i=0;i<count;i++){unsigned h=(i+1)*2654435761u;float z=.4f+(h%100)/100.0f,t=visualTick*.02f*(reducedMotion?.35f:1),x=20+fmodf((h%997)*.31f+sinf(t*.25f+i)*12+320,300),y=24+fmodf((h%887)*.27f+t*(4+z*7),232),r=4+z*7;nquad=0;quad((x-r)*3,(y-r)*3,r*6,r*6,(SDL_FColor){.59f,.67f,.45f,.045f+z*.025f},0,0,1,1);flush(smokeTexture);}SDL_SetTextureColorMod(smokeTexture,255,255,255);}
}
/* Soft volumes occupy the hazardous sides; the marked centre remains readable. */
static void draw_weather_volumes(void){Weather*w=&ex.env.weather;if(!g.fx||!w->stage||!(w->kind==WEATHER_ACID||w->kind==WEATHER_ION||w->kind==WEATHER_FIRE||w->kind==WEATHER_ICE))return;float fade=w->stage==1?((opening_front()?100:500)-w->left)/(float)(opening_front()?100:500):w->stage==3?w->left/(float)(opening_front()?100:500):1,time=visualTick*.02f*(reducedMotion?.3f:1);SDL_SetTextureBlendMode(smokeTexture,SDL_BLENDMODE_BLEND);SDL_SetTextureAlphaMod(smokeTexture,255);int kind=w->kind;SDL_SetTextureColorMod(smokeTexture,kind==WEATHER_ACID?185:kind==WEATHER_ION?155:kind==WEATHER_FIRE?205:195,kind==WEATHER_ACID?225:kind==WEATHER_ION?140:kind==WEATHER_FIRE?100:220,kind==WEATHER_ACID?65:kind==WEATHER_ION?220:kind==WEATHER_FIRE?65:240);
 for(int i=0;i<24;i++){int side=i&1;float y=play_min_y()+fmodf(i/2*37+time*(kind==WEATHER_FIRE?-8:5)+1000,play_max_y()-play_min_y()),center=weather_center_at(y),r=24+(i%4)*5,x=center+(side?1:-1)*(weather_safe_half()+r+8+9*sinf(i+time*.2f));nquad=0;quad((x-r)*3,(y-r*.7f)*3,r*6,r*4.2f,(SDL_FColor){(kind==WEATHER_ACID?185:kind==WEATHER_ION?155:kind==WEATHER_FIRE?205:195)/255.0f,(kind==WEATHER_ACID?225:kind==WEATHER_ION?140:kind==WEATHER_FIRE?100:220)/255.0f,(kind==WEATHER_ACID?65:kind==WEATHER_ION?220:kind==WEATHER_FIRE?65:240)/255.0f,fade*(kind==WEATHER_ACID?.34f:kind==WEATHER_ICE?.23f:.17f)},0,0,1,1);flush(smokeTexture);}
 SDL_SetTextureColorMod(smokeTexture,255,255,255);
}
/* Fixed branching geometry:32-unit damaging trunk, soft cosmetic corona. */
/* Full radial texture at overlapping samples gives rounded feathered joins. */
static void storm_ribbon(float x,float y,float xx,float yy,float width,float tip,SDL_FColor colour){int steps=clamp((int)ceilf(hypotf(xx-x,yy-y)/fminf(10,fmaxf(1,width*.8f))),1,64);colour.a*=width>=42?.09f:.15f;for(int i=0;i<steps;i++){float t=i/(float)steps,r=width+(tip-width)*t,px=x+(xx-x)*t,py=y+(yy-y)*t;quad((px-r)*3,(py-r)*3,r*6,r*6,colour,0,0,1,1);}}
static void storm_core_joint(int x,int seed,int node,int side,float*px,float*py){int before=clamp(node-1,0,STORM_BOLT_SEGMENTS-1),after=clamp(node,0,STORM_BOLT_SEGMENTS-1);float dx1=storm_bolt_x(x,seed,before+1)-storm_bolt_x(x,seed,before),dy1=storm_bolt_y(before+1)-storm_bolt_y(before),len1=hypotf(dx1,dy1),dx2=storm_bolt_x(x,seed,after+1)-storm_bolt_x(x,seed,after),dy2=storm_bolt_y(after+1)-storm_bolt_y(after),len2=hypotf(dx2,dy2),nx=-dy1/len1-dy2/len2,ny=dx1/len1+dx2/len2,len=hypotf(nx,ny);nx/=len;ny/=len;float width=STORM_BOLT_RADIUS;*px=storm_bolt_x(x,seed,node)+side*nx*width;*py=storm_bolt_y(node)+side*ny*width;}
static void draw_storm_core(int x,int seed,float alpha){for(int n=0;n<STORM_BOLT_SEGMENTS;n++){float px[4],py[4];storm_core_joint(x,seed,n,1,px,py);storm_core_joint(x,seed,n+1,1,px+1,py+1);storm_core_joint(x,seed,n+1,-1,px+2,py+2);storm_core_joint(x,seed,n,-1,px+3,py+3);int first=nquad;quad(0,0,1,1,(SDL_FColor){.82f,.94f,1,alpha*.75f},0,0,1,1);if(nquad==first)return;for(int i=0;i<4;i++){vertices[first*4+i].position=(SDL_FPoint){px[i]*3,py[i]*3};vertices[first*4+i].tex_coord=(SDL_FPoint){i<2?0:1,.5f};}}}

static int renderedStormForks;
/* Cosmetic forks use connected taper meshes: wider roots, exactly pointed tips. */
static void storm_fork_mesh(const float*x,const float*y,const float*radius,int count,SDL_FColor colour){float nx[4],ny[4];for(int i=0;i<count;i++){int a=i?i-1:0,b=i+1<count?i+1:count-1;float dx=x[b]-x[a],dy=y[b]-y[a],len=fmaxf(.001f,hypotf(dx,dy));nx[i]=-dy/len*radius[i];ny[i]=dx/len*radius[i];}
 for(int i=0;i<count-1;i++){int first=nquad;quad(0,0,1,1,colour,0,0,1,1);if(nquad==first)return;SDL_FPoint points[4]={{(x[i]+nx[i])*3,(y[i]+ny[i])*3},{(x[i+1]+nx[i+1])*3,(y[i+1]+ny[i+1])*3},{(x[i+1]-nx[i+1])*3,(y[i+1]-ny[i+1])*3},{(x[i]-nx[i])*3,(y[i]-ny[i])*3}};for(int k=0;k<4;k++){vertices[first*4+k].position=points[k];vertices[first*4+k].tex_coord=(SDL_FPoint){k<2?0:1,.5f};}}}
static void storm_side_fork(float x,float y,float length,float dy,int side,unsigned hash,float alpha,int split){
 float px[4]={x,x+side*length*.32f,x+side*length*.68f,x+side*length},py[4]={y,y+dy*.25f+((int)(hash%13)-6),y+dy*.68f+((int)((hash>>4)%15)-7),y+dy};
 static const float corona[4]={11,8,4,0},core[4]={4,2.8f,1.6f,0};
 for(int i=0;i<3;i++)storm_ribbon(px[i],py[i],px[i+1],py[i+1],corona[i],corona[i+1],(SDL_FColor){.22f,.45f,1,alpha*.35f});
 storm_fork_mesh(px,py,core,4,(SDL_FColor){.5f,.78f,1,alpha*.54f});renderedStormForks++;
 if(split){float tx[2]={px[2],px[2]+side*(12+(int)(hash%17))},ty[2]={py[2],py[2]-14-((hash>>5)%16)},tr[2]={2.5f,0};storm_ribbon(tx[0],ty[0],tx[1],ty[1],6,0,(SDL_FColor){.3f,.55f,1,alpha*.25f});storm_fork_mesh(tx,ty,tr,2,(SDL_FColor){.45f,.72f,1,alpha*.48f});renderedStormForks++;}
}

static void draw_storm_forks(int x,int seed,float alpha){static const int nodes[9]={1,3,6,2,9,5,10,7,11};int count=quality==2?9:quality==1?7:5;
 for(int j=0;j<count;j++){unsigned hash=(seed+1)*2654435761u+(j+3)*2246822519u;int node=nodes[(j+seed%9)%9],side=(hash&1)?1:-1;float px=storm_bolt_x(x,seed,node),py=storm_bolt_y(node),length=22+((hash>>8)%42),dy=(int)((hash>>16)%63)-25;
 storm_side_fork(px,py,length,dy,side,hash,alpha,j%2==0);
 if(j%3==1)storm_side_fork(px,py,18+((hash>>12)%27),-dy*.65f-7,-side,hash>>2,alpha*.8f,0);
 }}
static void draw_storm_bolt(int x,int seed,int warning,int age){
 float alpha=warning?.20f:(18-age)/18.0f;if(reducedMotion)alpha*=.55f;
 SDL_SetTextureBlendMode(halo,SDL_BLENDMODE_ADD);nquad=0;
 for(int n=0;n<STORM_BOLT_SEGMENTS;n++){float ax=storm_bolt_x(x,seed,n),ay=storm_bolt_y(n),bx=storm_bolt_x(x,seed,n+1),by=storm_bolt_y(n+1);
  if(warning){storm_ribbon(ax,ay,bx,by,26,26,(SDL_FColor){.28f,.48f,1,alpha*.4f});}
  else{storm_ribbon(ax,ay,bx,by,70,70,(SDL_FColor){.18f,.28f,1,alpha*.18f});storm_ribbon(ax,ay,bx,by,48,48,(SDL_FColor){.25f,.55f,1,alpha*.4f});storm_ribbon(ax,ay,bx,by,32,32,(SDL_FColor){.48f,.78f,1,alpha*.75f});storm_ribbon(ax,ay,bx,by,16,16,(SDL_FColor){.82f,.94f,1,alpha});
  }

 }
 if(!warning){draw_storm_core(x,seed,alpha);draw_storm_forks(x,seed,alpha);}
 flush(halo);SDL_SetTextureBlendMode(halo,SDL_BLENDMODE_BLEND);
 if(warning)for(int n=0;n<STORM_BOLT_SEGMENTS;n+=2){float y=storm_bolt_y(n),xx=storm_bolt_x(x,seed,n);journey_line(xx-26,y,xx-26,y+5,155,205,255,150);journey_line(xx+26,y,xx+26,y+5,155,205,255,150);}
}
static void draw_weather_combat(void){Weather*w=&ex.env.weather;if(!w->kind)return;int t=ex.env.timer%3000;draw_weather_volumes();

 if(w->stage==2&&(w->kind==WEATHER_STORM||w->kind==WEATHER_ION)){int seed=lightning_seed(),side=seed&1,x=w->safeX+(side?100:-100),c=lightning_clock()%lightning_period();if(c>=5&&c<35)draw_storm_bolt(x,seed,1,0);if(c>=35&&c<53)draw_storm_bolt(x,seed,0,c-35);}

 for(int i=0;i<48;i++){WeatherActor*a=w->actors+i;if(!a->on)continue;float x=a->x/(float)Q,y=a->y/(float)Q,r=a->r;if(a->type==WEATHER_ASTEROID){journey_cell(worldObjects,g.wave==6&&(i&1)?2:0,0,4,2,x,y,r*2,r*2);}else if(a->type==WEATHER_ICE)journey_cell(worldObjects,1,0,4,2,x,y,r*2,r*2);else if(a->type==WEATHER_ACID){EnvironmentHazard h={.x=a->x,.y=a->y};organic_hazard_art(&h,2,r*2,r*2);}else{nquad=0;quad((x-r)*3,(y-r*1.5f)*3,r*6,r*9,(SDL_FColor){1,.45f,.12f,.8f},0,0,1,1);flush(halo);journey_cell(worldObjects,0,1,4,2,x,y,r*2,r*2);}}
}
