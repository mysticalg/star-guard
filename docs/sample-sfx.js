/* Small CC0 sample bus: decoded once, mixed by Web Audio, never in the render loop. */
window.SoundSamples=(()=>{
  let context,master,meter,bossNode=null,loading=null,lastRear=null,rearPlays=0,errors=0,phase=1,peak=0;
  const combatNames=["explosion","bomb","laser","bounce","metal","shield","rock","glass","organic","energy","tesla","gun","pickup","warning","warp","super","victory-blast","victory"],plays={},lastPlay={},active=new Set();
  let dropped=0,cavern=false;
  const buffers={},rearSources=new Set(),wave=new Float32Array(256);
  async function start(){
    if(!context){context=new AudioContext();master=context.createGain();master.gain.value=.7;meter=context.createAnalyser();meter.fftSize=256;master.connect(meter);const limiter=context.createDynamicsCompressor();limiter.threshold.value=-12;limiter.knee.value=8;limiter.ratio.value=8;limiter.attack.value=.003;limiter.release.value=.12;meter.connect(limiter);limiter.connect(context.destination);}
    await context.resume();
    if(!loading)loading=Promise.all(['boss-reactor','rear-booster',...combatNames].map(async name=>{
      const response=await fetch('audio/sfx/'+name+'.ogg?v=27');if(!response.ok)throw Error('Sample '+name+' unavailable');
      buffers[name]=await context.decodeAudioData(await response.arrayBuffer());
    })).catch(error=>{errors++;console.error(error);});
    return loading;
  }
  function stopBoss(){if(!bossNode)return;const old=bossNode;bossNode=null;old.g.gain.setTargetAtTime(0,context.currentTime,.1);old.s.stop(context.currentTime+.4);}
  function startBoss(){const s=context.createBufferSource(),g=context.createGain(),f=context.createBiquadFilter();s.buffer=buffers['boss-reactor'];s.loop=true;g.gain.value=0;f.type='lowpass';f.frequency.value=500;s.connect(f);f.connect(g);g.connect(master);s.onended=()=>{s.disconnect();f.disconnect();g.disconnect();};s.start();bossNode={s,g,f};}
  function rear(x){if(!buffers['rear-booster'])return;if(rearSources.size>=3){const old=rearSources.values().next().value;old.stop();rearSources.delete(old);}const s=context.createBufferSource(),g=context.createGain(),pan=context.createStereoPanner();s.buffer=buffers['rear-booster'];s.playbackRate.value=.94+(rearPlays%4)*.04;g.gain.value=.38;pan.pan.value=Math.max(-.8,Math.min(.8,(x-160)/200));s.connect(g);g.connect(pan);pan.connect(master);rearSources.add(s);s.onended=()=>{rearSources.delete(s);s.disconnect();g.disconnect();pan.disconnect();};rearPlays++;s.start();}
  // Called at the actual C collision/fire event, rather than polling counters.
  // A fixed voice budget and category cooldowns bound large chain reactions.
  function event(kind,volume,pitch,pan,triple){
    if(!context||!buffers.tesla)return 0;
    const materials=['metal','shield','rock','glass','organic','energy'];
    let name=kind===2?(volume>100?'bomb':'explosion'):kind>=5&&kind<=10?materials[kind-5]:kind===15?materials[Math.max(0,Math.min(5,triple))]:kind===3?'laser':kind===11||kind===14?'bounce':kind===12?'super':kind===13?'victory-blast':kind===16?'tesla':kind===1?'gun':kind===4?'warp':kind===17?'pickup':kind===18?'warning':kind===20?'victory':null;
    if(!name||!buffers[name])return 1;
    const now=context.currentTime,gap=name==='tesla'?.24:name==='laser'?.085:name==='bounce'?.075:name==='bomb'?.12:name==='gun'?.045:name==='warp'?1:name==='victory'?3:.045;
    if(now-(lastPlay[name]??-10)<gap||active.size>=24){dropped++;return 1;}
    lastPlay[name]=now;
    const source=context.createBufferSource(),gain=context.createGain(),filter=context.createBiquadFilter(),stereo=context.createStereoPanner();
    source.buffer=buffers[name];
    const reference=kind>=5&&kind<=10?[0x3600,0x4200,0x2300,0x4b00,0x2800,0x4800][kind-5]:kind===14?0x4900:kind===16?0x4000:kind===3?0x2a00:0x3800;
    source.playbackRate.value=Math.max(.65,Math.min(1.6,Math.pow(2,(pitch-reference)/12288)));
    gain.gain.value=(name==='tesla'?.72:name==='victory'?.48:name==='warp'?.55:name==='gun'?.22:name==='warning'?.32:name==='pickup'?.28:name==='bomb'?.65:name==='explosion'?.48:name==='laser'?.26:name==='bounce'?.19:.32)*Math.min(1.2,volume/65);
    filter.type='lowpass';filter.frequency.value=name==='tesla'?6200:10000;filter.Q.value=.4;
    stereo.pan.value=Math.max(-.85,Math.min(.85,pan));
    source.connect(filter);filter.connect(gain);gain.connect(stereo);stereo.connect(master);
    const nodes=[source,filter,gain,stereo],oscillators=[];
    if(kind===3){gain.gain.value/=(triple?2.4:1.7);for(const ratio of (triple?[1.498307,1.781797]:[1.781797])){const harmonic=context.createBufferSource();harmonic.buffer=source.buffer;harmonic.playbackRate.value=source.playbackRate.value*ratio;harmonic.connect(filter);harmonic.start(now);nodes.push(harmonic);}}

    // Each reflected sample has its own short flanger; no shared resonance build-up.
    if(kind===11||kind===14){const delay=context.createDelay(.03),feedback=context.createGain(),wet=context.createGain(),lfo=context.createOscillator(),depth=context.createGain();delay.delayTime.value=.004;feedback.gain.value=.18;wet.gain.value=.24;lfo.frequency.value=.16+(pitch%7)*.015;depth.gain.value=.0015;lfo.connect(depth);depth.connect(delay.delayTime);gain.connect(delay);delay.connect(feedback);feedback.connect(delay);delay.connect(wet);wet.connect(stereo);lfo.start();oscillators.push(lfo);nodes.push(delay,feedback,wet,lfo,depth);}
    // A quiet, short reflection reinforces enclosed cavern acoustics.
    if(cavern){const delay=context.createDelay(.2),wet=context.createGain();delay.delayTime.value=.095;wet.gain.value=.18;gain.connect(delay);delay.connect(wet);wet.connect(stereo);nodes.push(delay,wet);}
    active.add(source);plays[name]=(plays[name]||0)+1;
    source.onended=()=>{setTimeout(()=>{for(const o of oscillators)o.stop();for(const n of nodes)n.disconnect();active.delete(source);},160);};source.start();
    return 1;
  }
  function observe(stat){
    if(!context)return;cavern=!!stat(81);const now=context.currentTime,quiet=!!stat(18)||!!stat(6)||document.hidden;
    master.gain.setTargetAtTime(quiet?0:.7,now,.04);
    const boss=stat(24)>0&&stat(1)===1&&stat(35)>-20*256;
    phase=Math.max(1,Math.min(3,stat(45)));
    if(boss&&buffers['boss-reactor']){if(!bossNode)startBoss();bossNode.g.gain.setTargetAtTime(.28+phase*.04,now,.5);bossNode.f.frequency.setTargetAtTime(400+phase*100,now,.5);bossNode.s.playbackRate.setTargetAtTime(.92+phase*.04,now,.5);}else stopBoss();
    const serial=stat(90);if(lastRear!==null&&serial>lastRear&&!quiet)for(let n=0;n<Math.min(2,serial-lastRear);n++)rear(stat(91));lastRear=serial;
    meter.getFloatTimeDomainData(wave);for(const v of wave)peak=Math.max(peak,Math.abs(v));
  }
  return {start,observe,event,get state(){return {sampleOnly:true,ready:Object.keys(buffers).length===combatNames.length+2,bossActive:!!bossNode,phase,rearPlays,plays:{...plays},active:active.size,dropped,activeRear:rearSources.size,masterGain:master?.gain.value||0,bossGain:bossNode?.g.gain.value||0,peak,errors};}};
})();
