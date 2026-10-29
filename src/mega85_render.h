/* One paid, full-width primary aperture. Gold corona is light; the white core
 * stays inside the real collision width. Pods call the same bounded renderer. */
static void mega85_stream(float x,float y,float xx,float yy,float width,float intensity){
 float pulse=reducedMotion?1:1+.018f*sinf(visualTick*.17f);
 beam_ribbon(x,y,xx,yy,width*1.55f*pulse,(SDL_FColor){1,.36f,.055f,.1f*intensity});
 beam_ribbon(x,y,xx,yy,width*1.15f,(SDL_FColor){1,.52f,.1f,.23f*intensity});
 beam_ribbon(x,y,xx,yy,width*.94f,(SDL_FColor){1,.66f,.2f,.4f*intensity});
 beam_ribbon(x,y,xx,yy,width*.55f,(SDL_FColor){1,.8f,.4f,.3f*intensity});
 beam_ribbon(x,y,xx,yy,fmaxf(.45f,width*.13f),(SDL_FColor){1,1,1,.94f*intensity});
 float dx=xx-x,dy=yy-y,len=fmaxf(1,hypotf(dx,dy)),nx=-dy/len,ny=dx/len;
 for(int strand=-2;strand<=2;strand++)if(strand){float off=strand*width*.24f;beam_ribbon(x+nx*off,y+ny*off,xx+nx*off,yy+ny*off,.25f,(SDL_FColor){1,.9f,.66f,.52f*intensity});}
 for(int side=-1;side<=1;side+=2)beam_ribbon(x+nx*width*.82f*side,y+ny*width*.82f*side,xx+nx*width*.82f*side,yy+ny*width*.82f*side,.3f,(SDL_FColor){1,.72f,.2f,.7f*intensity});
 light(xx,yy,width*.7f,1,.7f,.22f,.35f*intensity);light(x,y,width*.48f,1,.9f,.5f,.4f*intensity);
}
