from pathlib import Path
import json,subprocess,wave,hashlib
import numpy as np
root=Path('art-source/voice-v83');names=json.loads((root/'names.json').read_text());rate=44100;pieces=[];clips={};cursor=0;edits=[]
for key,text in names['clips'].items():
 source=root/(key+'-original.wav');pcm=subprocess.check_output(['ffmpeg','-v','error','-i',str(source),'-af','highpass=f=160,lowpass=f=6500','-ac','1','-ar',str(rate),'-f','f32le','pipe:1']);full=np.frombuffer(pcm,dtype='<f4').copy();active=np.flatnonzero(abs(full)>.0003);assert len(active)
 # Trim only the outside of the FULL utterance, never stop at an internal word gap.
 first=max(0,int(active[0]) -441);last=min(len(full),int(active[-1])+2426);a=full[first:last].copy();a-=a.mean();n=min(200,len(a)//8);a[:n]*=np.linspace(0,1,n);a[-n:]*=np.linspace(1,0,n);a*=.7/max(abs(a));clips[key]=dict(start=cursor/rate,duration=len(a)/rate,text=text);pieces.extend([a,np.zeros(int(rate*.12),dtype=np.float32)]);cursor+=len(a)+int(rate*.12)
 edits.append(dict(key=key,text=text,sourceFile=source.name,sourceSHA256=hashlib.sha256(source.read_bytes()).hexdigest(),sourceRate=rate,firstSample=first,lastSample=last,threshold=.0003,clipSeconds=len(a)/rate,fullUtterance=True))
master=np.concatenate(pieces);masterFile=root/'pickup-names-master.wav'
with wave.open(str(masterFile),'wb') as w:w.setnchannels(1);w.setsampwidth(2);w.setframerate(rate);w.writeframes((master*32767).astype('<i2').tobytes())
output=Path('docs/vo/pickup-names-v83.mp3');subprocess.run(['ffmpeg','-v','error','-y','-i',str(masterFile),'-c:a','libmp3lame','-b:a','128k',str(output)],check=True)
decoded=np.frombuffer(subprocess.check_output(['ffmpeg','-v','error','-i',str(output),'-ar',str(rate),'-ac','1','-f','f32le','pipe:1']),dtype='<f4')
for row in edits:
 c=clips[row['key']];start=round(c['start']*rate);end=start+round(c['duration']*rate);a=master[start:end];b=decoded[start:end];assert len(a)==len(b)
 corr=float(np.dot(a,b)/np.sqrt(np.dot(a,a)*np.dot(b,b)));assert corr>.98,(row['key'],corr);row['encodedClipCorrelation']=corr
Path('docs/vo/pickup-names-v83.json').write_text(json.dumps(dict(voice=names['voice'],origin='Locally synthesized with Windows SAPI; original full utterances preserved in art-source/voice-v83',nameOnly=True,clips=clips),indent=2),encoding='utf-8');(root/'edits-and-encoded-coverage.json').write_text(json.dumps(edits,indent=2),encoding='utf-8')
print('PASS',len(clips),'complete utterances; each actual MP3 offset correlation>.98 with full prepared speech; seconds',cursor/rate,'bytes',output.stat().st_size)
