from pathlib import Path
import subprocess,json,wave,hashlib
import numpy as np
root=Path('art-source/audio-v76');out=Path('../audio76-candidates');out.mkdir(exist_ok=True)
# Source edits only. No synthetic oscillators/noise replace the licensed recordings.
spec=[
 ('engine-jet-loop','C-V-704945-public-hq-original.mp3',58,5,70,7500,True),
 ('engine-jet-boost','C-V-704945-public-hq-original.mp3',15.8,1.6,80,8000,False),
 ('machine-energy-loop','Why_O-633172-public-hq-original.mp3',1.2,2.2,90,1800,True),
 ('laser-sustain','Why_O-633172-public-hq-original.mp3',.8,1.8,350,8500,True),
 ('laser-charge','Why_O-633172-public-hq-original.mp3',.04,.48,160,6500,False),
 ('laser-discharge','Why_O-633172-public-hq-original.mp3',3.2,.42,200,9000,False),
 ('laser-reflect','Why_O-633172-public-hq-original.mp3',4.4,.16,800,9500,False),
 ('shield-absorb','Why_O-633172-public-hq-original.mp3',2.6,.22,120,3400,False),
 ('metal-crash','moxobna-40158-public-hq-original.mp3',.48,1.55,65,8500,False),
 ('hull-hit','moxobna-40158-public-hq-original.mp3',1.48,.33,110,6200,False),
 ('mechanical-actuation','moxobna-40158-public-hq-original.mp3',.94,.5,160,7200,False),
 ('enemy-heavy-fire','qubodup-239138-public-hq-original.mp3',2.72,.2,95,9000,False),
 ('organic-roar-a','Kodack-256602-public-hq-original.mp3',.46,2.7,70,6500,False),
 ('organic-roar-b','Kodack-256602-public-hq-original.mp3',5.56,3.3,65,6500,False),
 ('organic-roar-c','Kodack-256602-public-hq-original.mp3',11.28,2.0,80,7200,False),
 ('organic-growl','Joao_Janz-483782-public-hq-original.mp3',.04,2.15,80,7000,False),
 ('organic-attack-a','Joao_Janz-483782-public-hq-original.mp3',.28,.3,240,8000,False),
 ('organic-attack-b','Kodack-256602-public-hq-original.mp3',16.1,.32,300,8500,False),
 ('organic-wet-hit','decadylan-825806-public-hq-original.mp3',.25,.34,130,8500,False),
 ('organic-squelch','decadylan-825806-public-hq-original.mp3',2.2,.6,90,8000,False),
]
rows=[]
for name,source,start,duration,hi,lo,loop in spec:
 cmd=['ffmpeg','-v','error','-ss',str(start),'-t',str(duration),'-i',str(root/source),'-af',f'highpass=f={hi},lowpass=f={lo}','-ac','1','-ar','48000','-f','f32le','pipe:1']
 a=np.frombuffer(subprocess.check_output(cmd),dtype='<f4').copy();a-=a.mean()
 if loop:
  # Crossfade the recorded tail into its own head, then continue at the seam.
  n=4800;t=np.linspace(0,1,n,dtype=np.float32);a=np.concatenate([a[n:-n],a[-n:]*(1-t)+a[:n]*t])
 else:
  n=min(240,len(a)//8);m=min(4800,len(a)//4);a[:n]*=np.linspace(0,1,n);a[-m:]*=np.linspace(1,0,m)
 peak=float(np.max(abs(a)));assert peak>.001,(name,peak)
 a*=.72/peak;wav=out/(name+'-v76.wav')
 with wave.open(str(wav),'wb') as w:w.setnchannels(1);w.setsampwidth(2);w.setframerate(48000);w.writeframes((a*32767).astype('<i2').tobytes())
 ogg=out/(name+'-v76.ogg');subprocess.run(['ffmpeg','-v','error','-y','-i',str(wav),'-c:a','libvorbis','-q:a','5',str(ogg)],check=True)
 subprocess.run(['ffmpeg','-v','error','-i',str(ogg),'-f','null','-'],check=True)
 rows.append(dict(name=name,source=source,start=start,sourceDuration=duration,highpassHz=hi,lowpassHz=lo,loop=loop,loopSeamCrossfadeSeconds=.1 if loop else 0,seconds=len(a)/48000,pcmPeak=float(max(abs(a))),pcmRMS=float(np.sqrt(np.mean(a*a))),sha256=hashlib.sha256(ogg.read_bytes()).hexdigest(),bytes=ogg.stat().st_size))
(out/'edits-and-decode.json').write_text(json.dumps(rows,indent=2),encoding='utf-8')
print('Prepared',len(rows),'licensed recording candidates; originals preserved. NOT integrated, audition/mix verification pending.')
