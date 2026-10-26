from PIL import Image
import numpy as np,re,json,hashlib
from pathlib import Path
source=Path('Assets/journeys/player-ships-v2.png');im=np.asarray(Image.open(source).convert('RGBA'));H,W=im.shape[:2];uvs=[list(map(float,re.findall(r'[-\d.]+',r))) for r in re.findall(r'\{([^{}]+)\}',Path('src/player_art_uv.h').read_text())];rows=[]
# Match the actual player quad, local side-view rotation and nonuniform unwarp.
# Camera rotation and zoom are common to hull and attachments and preserve overlap.
for ship in range(3):
 for row in range(2):
  for blend in [0,.25,.5,.75,1]:
   sx=960/(960-264*blend);sy=696/(696+264*blend)
   for view,weight in [(0,1-blend),(1,blend)]:
    if weight<.2:continue
    u,v,du,dv=uvs[ship*6+row*2+view];x0,y0=int(round(u*W)),int(round(v*H));ww,hh=int(round(du*W)),int(round(dv*H));ys,xs=np.where(im[y0:y0+hh,x0:x0+ww,3]>=96);aw=(32 if view else 26) if row else (24 if view else 16);ah=(16 if view else 24) if row else (12 if view else 20);px=((xs+.5)/ww-.5)*aw;py=((ys+.5)/hh-.5)*ah
    if view:px,py=py,-px
    px=px*sx;py=py*sy
    for n,(x,y) in enumerate([(-3,-2),(3,-2),(0,-5)]):
     d=float(np.sqrt((px-x)**2+(py-y)**2).min());assert d<2.0,(ship,row,blend,view,n,d);rows.append({'ship':ship,'phoenix':row,'blend':blend,'view':view,'globule':n,'nearestOpaqueWorldDistance':round(d,4)})
worst=max(rows,key=lambda r:r['nearestOpaqueWorldDistance']);out={'checks':len(rows),'hullSourceSHA256':hashlib.sha256(source.read_bytes()).hexdigest(),'globuleBodyWorldSize':[7,8],'worst':worst,'rows':rows};Path('../globules60-hull-alpha.json').write_text(json.dumps(out,indent=2));print('PASS',len(rows),'actual opaque-hull contact probes; worst',worst)
