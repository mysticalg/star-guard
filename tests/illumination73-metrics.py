from pathlib import Path
import json,sys
import numpy as np
from PIL import Image

root=Path(sys.argv[1] if len(sys.argv)>1 else '../illumination73-native')
states=json.loads((root/'results.json').read_text())
results=[]
for name in dict.fromkeys(s['name'] for s in states):
    records={s['mode']:s for s in states if s['name']==name}
    for key in ['tick','paused','side','zoom','x','y']:
        assert len({r[key] for r in records.values()})==1,(name,key)
    assert records['before']['sources']==records['after']['sources']==records['control']['sources']+1,name
    images={mode:np.asarray(Image.open(root/f'{name}-{mode}.png').convert('RGB'),dtype=float) for mode in records}
    luma={mode:im@np.array([.2126,.7152,.0722]) for mode,im in images.items()}
    height,width=luma['after'].shape
    yy,xx=np.indices((height,width));cx=records['after']['x']*width/960;cy=records['after']['y']*height/800
    distance=np.hypot(xx-cx,yy-cy)
    # DOM status and energy/ammo overlays extend below the 72px game header.
    # Exclude their entire upper strip so font timing cannot count as terrain.
    region=(yy>=120*height/800)&(yy<700*height/800)&(distance>12)
    before=luma['before']-luma['control'];after=luma['after']-luma['control']
    old=(before>3)&region;new=(after>3)&region
    count_before=int(old.sum());count_after=int(new.sum());ratio=count_after/max(1,count_before)
    assert ratio>=2.5,(name,ratio)
    far=region&(distance>450)
    assert not far.any() or np.max(np.abs(after[far]))<=1,(name,'far scene changed')
    annuli=[]
    for radius in range(12,420,12):
        mask=region&(distance>=radius)&(distance<radius+12)
        if mask.sum()>10:annuli.append({'radius':radius+6,'old_luma_gain':float(before[mask].mean()),'new_luma_gain':float(after[mask].mean())})
    results.append({'name':name,'old_lit_texture_pixels':count_before,'new_lit_texture_pixels':count_after,'area_ratio':ratio,'old_radius95':float(np.quantile(distance[old],.95)),'new_radius95':float(np.quantile(distance[new],.95)),'far_scene_unchanged':True,'annuli':annuli})
(root/'footprint-metrics.json').write_text(json.dumps(results,indent=2))
print('PASS actual matched-state texture illumination footprint:',json.dumps([{k:r[k] for k in ['name','area_ratio','old_radius95','new_radius95']} for r in results]))
