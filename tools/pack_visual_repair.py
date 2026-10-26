"""Build padded atlases from generated RGBA sources without chroma removal.

Alpha thresholds locate occupied bounds only; original RGBA pixels are copied
unchanged into the crop, then resized. No pixel is keyed or alpha-rewritten.
"""
from pathlib import Path
import json, shutil, hashlib, re
from PIL import Image, ImageOps
import numpy as np
from scipy.ndimage import label, find_objects
ROOT=Path(__file__).resolve().parents[1]
SOURCE=ROOT/'art-source/visual-repair-v41'
OUT=ROOT/'Assets/journeys'
manifest=json.loads((SOURCE/'generation-manifest.json').read_text())
records={r['key']:r for r in manifest['records']}
def read(key):
 r=records[key];p=SOURCE/(key+'.png')
 if not p.exists():shutil.copy2(r['source'],p)
 return Image.open(p).convert('RGBA')
def bounds(im):
 # Ignore sub-visible alpha noise for locating a crop, not for altering pixels.
 a=im.getchannel('A').point(lambda a:255 if a>16 else 0)
 box=a.getbbox();assert box
 l,t,r,b=box;return im.crop((max(0,l-6),max(0,t-6),min(im.width,r+6),min(im.height,b+6)))
def fitted(im,size=256,pad=16):
 im=ImageOps.contain(bounds(im),(size-pad*2,size-pad*2),Image.Resampling.LANCZOS)
 dst=Image.new('RGBA',(size,size));dst.paste(im,((size-im.width)//2,(size-im.height)//2));return dst
def objects(im):
 a=np.array(im.getchannel('A'));labs,_=label(a>32)
 boxes=[]
 for k,sl in enumerate(find_objects(labs)):
  if np.count_nonzero(labs[sl]==k+1)<100:continue
  boxes.append((sl[1].start,sl[0].start,sl[1].stop,sl[0].stop))
 return boxes
def ships():
 dst=Image.new('RGBA',(1536,768))
 for ship,key in enumerate(['ship-blue','ship-green','ship-red']):
  im=read(key);boxes=objects(im)
  # Every selected source has six disconnected full subjects. Sort by row then
  # x using their centres, not nominal grid boundaries crossed by Phoenix fins.
  assert len(boxes)==6,(key,boxes)
  boxes=sorted(boxes,key=lambda b:(b[1]+b[3])/2)
  for row in range(3):
   pair=sorted(boxes[row*2:row*2+2],key=lambda b:b[0])
   for view,(l,t,r,b) in enumerate(pair):
    crop=im.crop((max(0,l-4),max(0,t-4),min(im.width,r+4),min(im.height,b+4)))
    dst.paste(fitted(crop,pad=18),((ship*2+view)*256,row*256))
 dst.save(OUT/'player-ships-v2.png')
 uv=[]
 for ship in range(3):
  for row in range(3):
   for view in range(2):
    x=(ship*2+view)*256;y=row*256
    a=dst.crop((x,y,x+256,y+256)).getchannel('A').point(lambda a:255 if a>16 else 0)
    l,t,r,b=a.getbbox();l=max(0,l-3);t=max(0,t-3);r=min(256,r+3);b=min(256,b+3)
    uv.append('{'+','.join(f'{v:.9f}f' for v in [(x+l)/1536,(y+t)/768,(r-l)/1536,(b-t)/768])+'}')
 (ROOT/'src/player_art_uv.h').write_text('static const float playerArtUV[18][4]={\n'+',\n'.join(uv)+'\n};\n')
 # Menu preview follows the actual normal top-view chassis.
 preview=Image.new('RGBA',(768,256))
 for ship in range(3):preview.paste(dst.crop((ship*512,0,ship*512+256,256)),(ship*256,0))
 preview.save(ROOT/'docs/images/journey-ships-v2.png')
def hulls():
 dst=Image.new('RGBA',(1536,1024))
 for m,key in [(0,'ocean'),(1,'hive'),(2,'crystal'),(3,'ice'),(5,'factory')]:
  dst.paste(fitted(read('hull-'+key+'-solid'),512,30),((m%3)*512,(m//3)*512))
 dst.save(OUT/'giant-hulls-v2.png')
def hazards():
 dst=Image.new('RGBA',(1024,512))
 for i,key in enumerate(['wall-rock','wall-ice','wall-metal','wall-crystal','meteor','root','wave','burrow']):dst.paste(fitted(read(key),256,12),((i%4)*256,(i//4)*256))
 dst.save(OUT/'world-objects-v2.png')
def bosses():
 old=Image.open(ROOT/'Assets/modern/boss-frames.png').convert('RGBA');dst=Image.new('RGBA',old.size)
 # 39 existing frames, 12 families x2 plus five families x3.
 for y in range(6):
  for x in range(8):
   cell=old.crop((x*256,y*256,x*256+256,y*256+256))
   if cell.getchannel('A').getbbox():dst.paste(fitted(cell,256,12),(x*256,y*256))
 for family,key in [(15,'prism'),(16,'isotope')]:
  im=read(key)
  for stage in range(3):
   cell=im.crop((stage*im.width//3,0,(stage+1)*im.width//3,im.height))
   cell=cell.rotate(180) # Engine's boss heading convention points up in source.
   index=32+(family-12)*3+stage
   dst.paste(fitted(cell,256,12),((index%8)*256,(index//8)*256))
 dst.save(ROOT/'Assets/modern/boss-frames-v2.png')
def biology():
 for name,keys in [('organic-parasites-v2.png',['parasite-tick','parasite-leech','parasite-wing','parasite-egg']),('giant-worm-v2.png',['worm-head','worm-body','worm-tail'])]:
  dst=Image.new('RGBA',(len(keys)*256,256))
  for i,key in enumerate(keys):dst.paste(fitted(read(key),256,16),(i*256,0))
  dst.save(OUT/name)
  if name=='giant-worm-v2.png':
   uv=[]
   for i in range(3):
    a=dst.crop((i*256,0,i*256+256,256)).getchannel('A').point(lambda a:255 if a>16 else 0);l,t,r,b=a.getbbox();l=max(0,l-3);t=max(0,t-3);r=min(256,r+3);b=min(256,b+3)
    uv.append('{'+','.join(f'{v:.9f}f' for v in [(i*256+l)/768,t/256,(r-l)/768,(b-t)/256])+'}')
   (ROOT/'src/worm_art_uv.h').write_text('static const float wormArtUV[3][4]={\n'+',\n'.join(uv)+'\n};\n')
ships();hulls();hazards();bosses();biology()
audit=[]
for path in [OUT/'player-ships-v2.png',OUT/'giant-hulls-v2.png',OUT/'world-objects-v2.png',ROOT/'Assets/modern/boss-frames-v2.png',OUT/'organic-parasites-v2.png',OUT/'giant-worm-v2.png']:
 im=Image.open(path);audit.append({'file':str(path.relative_to(ROOT)),'size':im.size,'mode':im.mode,'sha256':hashlib.sha256(path.read_bytes()).hexdigest()})
(SOURCE/'packed-manifest.json').write_text(json.dumps(audit,indent=2))
print(json.dumps(audit,indent=2))

for record in manifest['records']:
 path=SOURCE/(record['key']+'.png')
 if path.exists():record['project_source']=str(path.relative_to(ROOT));record['source_sha256']=hashlib.sha256(path.read_bytes()).hexdigest()
(SOURCE/'generation-manifest.json').write_text(json.dumps(manifest,indent=2))
