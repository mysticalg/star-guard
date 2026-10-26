/* Local procedural ambience; no network, media generation or gameplay RNG. */
window.WeatherAudio=(()=>{
 let context,bed,filter,gain,lastStrike=0,lastTick=-1,currentKind=0;
 function unlock(){if(!context){context=new AudioContext();const noise=context.createBuffer(1,context.sampleRate*2,context.sampleRate),data=noise.getChannelData(0);let seed=621017;for(let i=0;i<data.length;i++){seed=(Math.imul(seed,1664525)+1013904223)|0;data[i]=(seed>>>0)/2147483648-1;}bed=context.createBufferSource();bed.buffer=noise;bed.loop=true;filter=context.createBiquadFilter();filter.type='lowpass';gain=context.createGain();gain.gain.value=0;bed.connect(filter).connect(gain).connect(context.destination);bed.start();}context.resume().catch(()=>{});}
 document.addEventListener('pointerdown',unlock,{passive:true});document.addEventListener('keydown',unlock);
 function observe(s){if(!context)return;const tick=s(0),kind=s(10400),stage=s(10401),paused=s(6)||s(18)||s(1)!==1||document.hidden||!document.hasFocus();currentKind=kind;
  gain.gain.setTargetAtTime(!paused&&stage===2?[0,.026,.008,.018,.012,.02,.018][kind]||0:0,context.currentTime,.25);
  filter.frequency.setTargetAtTime([200,260,200,950,2400,550,1500][kind]||200,context.currentTime,.25);
  const strike=s(10410);if(!paused&&stage===2&&kind===1&&strike&&strike!==lastStrike&&tick>lastTick){const boom=context.createBufferSource(),low=context.createBiquadFilter(),volume=context.createGain();boom.buffer=bed.buffer;low.type='lowpass';low.frequency.value=180;volume.gain.setValueAtTime(.0001,context.currentTime);volume.gain.exponentialRampToValueAtTime(.3,context.currentTime+.025);volume.gain.exponentialRampToValueAtTime(.0001,context.currentTime+1.8);boom.connect(low).connect(volume).connect(context.destination);boom.start();boom.stop(context.currentTime+1.9);}
  lastStrike=strike;lastTick=tick;
 }
 return{observe,state:()=>({unlocked:!!context,kind:currentKind,gain:gain?.gain.value||0})};
})();
