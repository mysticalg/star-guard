from pathlib import Path
import json,sys
import numpy as np
from PIL import Image
root=Path(sys.argv[1] if len(sys.argv)>1 else '../illumination73-firing')
states=json.loads((root/'results.json').read_text());rows=[]
for name in dict.fromkeys(r['name'] for r in states):
 records={r['mode']:r for r in states if r['name']==name}
 for key in ['tick','paused','side','zoom']:
  assert len({r[key] for r in records.values()})==1,(name,key)
 assert records['before']['sources']==records['after']['sources'],name
 images={m:np.asarray(Image.open(root/f'{name}-{m}.png').convert('RGB'),dtype=float) for m in records}
 luma={m:im@np.array([.2126,.7152,.0722]) for m,im in images.items()}
 h,w=luma['after'].shape;yy,xx=np.indices((h,w));region=(yy>h*.15)&(yy<h*.86)&(xx>w*.04)&(xx<w*.96)
 old=(luma['before']-luma['control']>3)&region;new=(luma['after']-luma['control']>3)&region
 # Several simultaneous sources overlap; use the single-source test for radius
 # and require at least 50% more actual lit scene area during normal firing.
 ratio=float(new.sum()/max(1,old.sum()));assert ratio>1.5,(name,ratio)
 rows.append(dict(name=name,tick=records['after']['tick'],old_lit_pixels=int(old.sum()),new_lit_pixels=int(new.sum()),area_ratio=ratio))
(root/'firing-metrics.json').write_text(json.dumps(rows,indent=2));print('PASS actual firing matched-state lit scene area',json.dumps(rows))
