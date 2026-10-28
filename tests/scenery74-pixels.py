from pathlib import Path
import json,sys,cv2,numpy as np
root=Path(sys.argv[1] if len(sys.argv)>1 else '../scenery74-native')
states=json.loads((root/'results.json').read_text());rows=[]
for r in states:
 name,mode=r['name'],r['mode'];a=cv2.imread(str(root/f'{name}-{mode}-a.png'));b=cv2.imread(str(root/f'{name}-{mode}-b.png'))
 h,w=a.shape[:2];x0,x1=int(w*.015),int(w*.985);y0,y1=int(h*.26),int(h*.87)
 # The worm body and stationary illumination dominate central correlations.
 # A visibly clear right-edge scenery strip excludes those foreground objects.
 if name in ['beast78','reactor55','reactor32']:x0,x1=int(w*.815),int(w*.992)
 gray=lambda im:cv2.cvtColor(im[y0:y1,x0:x1],cv2.COLOR_BGR2GRAY).astype(np.float32)
 first,second=gray(a),gray(b);window=cv2.createHanningWindow((first.shape[1],first.shape[0]),cv2.CV_32F)
 (dx,dy),response=cv2.phaseCorrelate(first,second,window)
 expected=np.array(r['b']['ground'])-r['a']['ground'];expected*=np.array([w/960,h/800])
 row=dict(name=name,mode=mode,pixel_shift=[dx,dy],correlation=response,projected_shift=expected.tolist(),roi=[x0,y0,x1,y1]);rows.append(row)
 print(json.dumps(row))
 if mode=='after':
  assert response>.15,(name,'scenery correlation too weak',response)
  assert np.linalg.norm([dx,dy])>40,(name,'backdrop remains too slow')
  assert np.linalg.norm(np.array([dx,dy])-expected)<1,(name,'actual texture disagrees with projected motion')
for name in dict.fromkeys(r['name'] for r in rows):
 pair={r['mode']:r for r in rows if r['name']==name}
 assert np.linalg.norm(pair['after']['pixel_shift'])>4*max(.25,np.linalg.norm(pair['before']['pixel_shift'])),name
(root/'pixel-motion.json').write_text(json.dumps(rows,indent=2))
print('PASS actual scenery ROI displacement in every 200ms comparison; no boss sprite used as background motion')
