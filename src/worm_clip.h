/* Clip the rotating organic silhouette to the same two-unit corridor mesh as
   collision. UVs remain attached to anatomy as it emerges from a wall hole. */
static int worm_clip_plane(SDL_Vertex*in,int count,SDL_Vertex*out,float ax,float ay,float c){
 int used=0;for(int i=0;i<count;i++){SDL_Vertex a=in[i],b=in[(i+1)%count];float da=ax*a.position.x+ay*a.position.y+c,db=ax*b.position.x+ay*b.position.y+c;
  if(da>=0&&used<16)out[used++]=a;
  if((da>=0)!=(db>=0)){float t=da/(da-db);SDL_Vertex v=a;v.position.x+=t*(b.position.x-a.position.x);v.position.y+=t*(b.position.y-a.position.y);v.tex_coord.x+=t*(b.tex_coord.x-a.tex_coord.x);v.tex_coord.y+=t*(b.tex_coord.y-a.tex_coord.y);if(used<16)out[used++]=v;}
 }return used;
}
static void worm_clip_quad(const SDL_Vertex*source){
 float minY=source[0].position.y,maxY=minY;for(int i=1;i<4;i++){minY=fminf(minY,source[i].position.y);maxY=fmaxf(maxY,source[i].position.y);}
 int first=clamp((int)floorf(minY/6)*2,24,256),last=clamp((int)ceilf(maxY/6)*2,24,256);
 for(int y=first;y<last;y+=2){SDL_Vertex a[16],b[16];memcpy(a,source,4*sizeof(*source));int count=4;float yy=y*3,l=tunnel_left(y)*3,r=tunnel_right(y)*3,ls=(tunnel_left(y+2)*3-l)/6,rs=(tunnel_right(y+2)*3-r)/6;
  count=worm_clip_plane(a,count,b,0,1,-yy);count=worm_clip_plane(b,count,a,0,-1,yy+6);
  count=worm_clip_plane(a,count,b,1,-ls,ls*yy-l);count=worm_clip_plane(b,count,a,-1,rs,r-rs*yy);
  for(int i=1;i+1<count;i++){int at=nquad;quad(0,0,1,1,a[0].color,0,0,1,1);if(nquad==at)return;vertices[at*4]=a[0];vertices[at*4+1]=a[i];vertices[at*4+2]=vertices[at*4+3]=a[i+1];}
 }
}
