/* Bundled field recordings and local biome beds; no external requests or gameplay RNG. */
window.WeatherAudio=(()=>{
 let context,bed,filter,gain,rainSource,rainGain,thunderBus,loading,pending=null;
 let lastStrike=0,lastTick=-1,currentKind=0,thunderCount=0,lastThunderTick=-1,cursor=0,lastVariant=-1,error='';
 let buffers=[],booms=[],rainReady=false,history=[];
 const files=['weather-thunder-v65.ogg','weather-thunder-v65-2.ogg','weather-thunder-v65-3.ogg'];
 function stopBooms(){for(const b of booms){b.volume.gain.setTargetAtTime(0,context.currentTime,.025);try{b.source.stop(context.currentTime+.08)}catch{}}booms=[];pending=null;}
 function silence(){if(!context)return;gain.gain.setTargetAtTime(0,context.currentTime,.025);rainGain.gain.setTargetAtTime(0,context.currentTime,.025);stopBooms();}
 function unlock(){
  if(!context){
   context=window.GameAudio76?.get().context||new AudioContext();const noise=context.createBuffer(1,context.sampleRate*2,context.sampleRate),data=noise.getChannelData(0);let seed=621017;
   for(let i=0;i<data.length;i++){seed=(Math.imul(seed,1664525)+1013904223)|0;data[i]=(seed>>>0)/2147483648-1;}
   bed=context.createBufferSource();bed.buffer=noise;bed.loop=true;filter=context.createBiquadFilter();filter.type='lowpass';gain=context.createGain();gain.gain.value=0;bed.connect(filter).connect(gain).connect(window.GameAudio76?.get().input||context.destination);bed.start();
   rainGain=context.createGain();rainGain.gain.value=0;rainGain.connect(window.GameAudio76?.get().input||context.destination);
   const limiter=context.createDynamicsCompressor();limiter.threshold.value=-8;limiter.knee.value=3;limiter.ratio.value=20;limiter.attack.value=.003;limiter.release.value=.3;thunderBus=context.createGain();thunderBus.gain.value=.7;thunderBus.connect(limiter).connect(window.GameAudio76?.get().input||context.destination);
   const load=async file=>{const r=await fetch(new URL('audio/'+file,document.baseURI));if(!r.ok)throw Error(file+' '+r.status);return context.decodeAudioData(await r.arrayBuffer());};
   loading=Promise.all([Promise.all(files.map(load)).then(a=>buffers=a),load('weather-rain-v65.ogg').then(buffer=>{rainSource=context.createBufferSource();rainSource.buffer=buffer;rainSource.loop=true;rainSource.connect(rainGain);rainSource.start();rainReady=true;})]).catch(e=>{error=e.message;console.error(e)});
  }
  context.resume().catch(()=>{});
 }
 document.addEventListener('pointerdown',unlock,{passive:true});document.addEventListener('keydown',unlock);
 document.addEventListener('visibilitychange',()=>{if(document.hidden)silence()});window.addEventListener?.('blur',silence);
 function observe(s){
  if(!context)return;const tick=s(0),kind=s(10400),stage=s(10401),paused=s(6)||s(18)||s(1)!==1||document.hidden||!document.hasFocus(),speaking=!!window.ShipVoice?.speaking;
  currentKind=kind;booms=booms.filter(b=>context.currentTime<b.end);
  gain.gain.setTargetAtTime(!paused&&stage===2?[0,0,.008,.018,.012,.02,.018][kind]||0:0,context.currentTime,paused?.025:.25);
  filter.frequency.setTargetAtTime([200,260,200,950,2400,550,1500][kind]||200,context.currentTime,.25);
  const rain=!paused&&s(77)>0;rainGain.gain.setTargetAtTime(rain?(stage===2&&kind===1?.18:.06)*(booms.length?.6:1)*(speaking?.55:1):0,context.currentTime,paused?.025:.35);
  thunderBus.gain.setTargetAtTime(speaking?.56:.7,context.currentTime,.08);
  const strike=s(10412),strikeTick=s(10413);if(tick<lastTick){pending=null;lastStrike=0;}if(paused)stopBooms();
  if(!paused&&kind===1&&strike&&strike!==lastStrike&&tick>lastTick)pending={at:strikeTick+15};
  if(pending&&!paused&&tick>=pending.at&&buffers.length){
   const variant=cursor++%buffers.length,source=context.createBufferSource(),volume=context.createGain();source.buffer=buffers[variant];volume.gain.value=.7;
   if(booms.length>=3){const oldest=booms.shift();oldest.volume.gain.setTargetAtTime(0,context.currentTime,.04);try{oldest.source.stop(context.currentTime+.18)}catch{}}
   source.connect(volume).connect(thunderBus);source.start();const end=context.currentTime+source.buffer.duration;source.stop(end);booms.push({source,volume,end});source.onended=()=>{booms=booms.filter(b=>b.source!==source);source.disconnect();volume.disconnect();};
   pending=null;thunderCount++;lastThunderTick=tick;lastVariant=variant;history.push({strike,tick,variant,file:files[variant],delayTicks:tick-strikeTick});if(history.length>12)history.shift();
  }
  lastStrike=strike;lastTick=tick;
 }
 return{observe,get ready(){return loading},state:()=>({unlocked:!!context,kind:currentKind,gain:gain?.gain.value||0,rainGain:rainGain?.gain.value||0,rainReady,thunderCount,lastThunderTick,strikeSerial:lastStrike,voices:booms.length,lastVariant,variants:buffers.length,history:history.map(x=>({...x})),durations:buffers.map(b=>b.duration),waveform:'recorded-natural-claps-approved-DaniloSFX-primary',ready:buffers.length===3,error,delayTicks:15})};
})();
