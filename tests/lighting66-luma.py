from pathlib import Path
from PIL import Image
import json
folder=Path('../lighting66-same-state');out=[]
luma=lambda z:.2126*z[0]+.7152*z[1]+.0722*z[2]
for name,mode,boxes in [('side-shots',1,[(350,440,750,455),(350,385,750,400)]),('side-beam',1,[(350,440,750,455),(350,385,750,400)]),('side-idle',2,[(260,385,410,400),(150,385,220,400)]),('vertical',1,[(500,240,515,530)]),('blend',1,[(550,290,570,310)]),('zoom',1,[(490,330,500,390)]),('storm',3,[(290,250,310,450)]),('explosion',5,[(130,460,180,480)])]:
 a=Image.open(folder/(name+'-0.png')).convert('RGB');b=Image.open(folder/(name+'-'+str(mode)+'.png')).convert('RGB')
 for box in boxes:
  lit=sum(map(luma,a.crop(box).getdata()))/((box[2]-box[0])*(box[3]-box[1]));unlit=sum(map(luma,b.crop(box).getdata()))/((box[2]-box[0])*(box[3]-box[1]));out.append(dict(scene=name,isolatedSource='lamp' if mode==2 else 'storm' if name=='storm' else 'explosion' if name=='explosion' else 'real player weapon',terrainROI=box,sourceOff=unlit,sourceOn=lit,delta=lit-unlit,ratio=lit/max(.001,unlit)))
Path('../lighting66-terrain-luma.json').write_text(json.dumps(out,indent=2));print(json.dumps(out,indent=2))

assert all(r["delta"]>0 for r in out)
