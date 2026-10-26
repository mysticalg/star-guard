from pathlib import Path
from PIL import Image
import json,hashlib
ROOT=Path(__file__).resolve().parents[1];SRC=ROOT/'art-source/giant-transform-v45';OUT=ROOT/'Assets/journeys'
records=[]
def place(dst,im,box):
 im=im.convert('RGBA');bounds=im.getchannel('A').point(lambda a:255 if a>16 else 0).getbbox();assert bounds;l,t,r,b=bounds;bounds=(max(0,l-8),max(0,t-8),min(im.width,r+8),min(im.height,b+8));im=im.crop(bounds);im.thumbnail((box[2]-32,box[3]-32),Image.Resampling.LANCZOS);x=box[0]+(box[2]-im.width)//2;y=box[1]+(box[3]-im.height)//2;dst.paste(im,(x,y));return [x-2,y-2,im.width+4,im.height+4]
forms=Image.new('RGBA',(6*512,2*320));uv=[]
for f in range(2):
 row=[]
 for m,key in enumerate(['ocean','hive','crystal','ice',None,'factory']):
  if key:
   source=SRC/f'{key}-form-{f+2}.png';r=place(forms,Image.open(source),(m*512,f*320,512,320));row.append([r[0]/forms.width,r[1]/forms.height,r[2]/forms.width,r[3]/forms.height]);records.append({'key':source.stem,'atlas':'giant-forms-v45.png','rect':r,'source_sha256':hashlib.sha256(source.read_bytes()).hexdigest()})
  else:row.append([0,0,0,0])
 uv.append(row)
forms.save(OUT/'giant-forms-v45.png')
worm=Image.new('RGBA',(3*384,3*384));wuv=[];old=Image.open(OUT/'giant-worm-v2.png')
for f in range(3):
 row=[]
 for c,key in enumerate(['head','body','tail']):
  source=SRC/f'worm-{key}-form-{f+1}.png';im=old.crop((c*256,0,(c+1)*256,256)) if f==0 else Image.open(source);r=place(worm,im,(c*384,f*384,384,384));row.append([r[0]/worm.width,r[1]/worm.height,r[2]/worm.width,r[3]/worm.height]);records.append({'key':f'worm-{key}-form-{f+1}','atlas':'worm-forms-v45.png','rect':r})
 wuv.append(row)
worm.save(OUT/'worm-forms-v45.png')
def matrix(name,values):
 return 'static const float '+name+'['+str(len(values))+']['+str(len(values[0]))+'][4]={\n'+',\n'.join('{'+','.join('{'+','.join(f'{n:.9f}f' for n in row)+'}' for row in group)+'}' for group in values)+'\n};\n'
mods=Image.new('RGBA',(6*256,2*256));muv=[]
for f in range(2):
 row=[]
 for m,key in enumerate(['ocean','hive','crystal','ice',None,'factory']):
  if key:
   source=SRC/f'{key}-module-{f+2}.png';r=place(mods,Image.open(source),(m*256,f*256,256,256));row.append([r[0]/mods.width,r[1]/mods.height,r[2]/mods.width,r[3]/mods.height]);records.append({'key':source.stem,'atlas':'giant-modules-v45.png','rect':r,'source_sha256':hashlib.sha256(source.read_bytes()).hexdigest()})
  else:row.append([0,0,0,0])
 muv.append(row)
mods.save(OUT/'giant-modules-v45.png')
(ROOT/'src/giant_form_uv.h').write_text(matrix('giantFormsUV',uv)+matrix('wormFormsUV',wuv)+matrix('giantModuleUV',muv))

for file in ['giant-forms-v45.png','worm-forms-v45.png','giant-modules-v45.png']:
 im=Image.open(OUT/file);records.append({'file':file,'sha256':hashlib.sha256((OUT/file).read_bytes()).hexdigest(),'size':im.size,'mode':im.mode});assert im.mode=='RGBA';assert im.getpixel((0,0))[3]==0
(SRC/'packed-manifest.json').write_text(json.dumps(records,indent=2))
print('Packed individually generated forms with exact alpha and transparent gutters')
