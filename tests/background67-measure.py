from pathlib import Path
from PIL import Image
import numpy as np,json
f=Path('../background67-motion');out=[]
def grey(name):
 a=np.asarray(Image.open(f/name).convert('RGB'),dtype=np.float32);return a[:,:,0]*.2126+a[:,:,1]*.7152+a[:,:,2]*.0722
for scene,axis,box in [('vertical',1,(450,200,800,450)),('cave-side',0,(250,260,700,330)),('giant-arena',1,(250,590,400,660))]:
 for version in ['before','after']:
  a=grey(scene+'-'+version+'-a.png');b=grey(scene+'-'+version+'-b.png');x1,y1,x2,y2=box;ref=a[y1:y2,x1:x2];ref=ref-ref.mean();scores=[]
  for shift in range(-150,151):
   dx=shift if axis==0 else 0;dy=shift if axis==1 else 0
   if y1+dy<75 or y2+dy>690 or x1+dx<0 or x2+dx>a.shape[1]:continue
   block=b[y1+dy:y2+dy,x1+dx:x2+dx];block=block-block.mean();score=float(np.sum(ref*block)/max(.0001,np.linalg.norm(ref)*np.linalg.norm(block)));scores.append((score,shift))
  best=max(scores);pa=grey(scene+'-'+version+'-paused-a.png');pb=grey(scene+'-'+version+'-paused-b.png');pdelta=np.abs(pa[y1:y2,x1:x2]-pb[y1:y2,x1:x2]);out.append(dict(scene=scene,version=version,ROI=box,axis='x' if axis==0 else 'y',textureShiftPixels=best[1],normalizedCorrelation=best[0],pauseMeanAbsoluteLumaDelta=float(pdelta.mean()),pauseMaxDelta=float(pdelta.max())))
Path('../background67-motion-metrics.json').write_text(json.dumps(out,indent=2));print(json.dumps(out,indent=2))
