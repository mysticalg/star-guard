/* Licensed sample bus: decoded once, mixed by Web Audio, never in the render loop. */
window.SoundSamples=(()=>{
  let context,master,meter,bossNode=null,loading=null,lastRear=null,rearPlays=0,errors=0,phase=1,peak=0;
  const combatNames=["explosion-small","explosion-small-alt1","explosion-small-alt2","explosion-medium","explosion-medium-alt1","explosion-medium-alt2","explosion-large","explosion-large-alt1","explosion-large-alt2","explosion-boss","explosion","bomb","laser","bounce","metal","shield","rock","glass","organic","energy","tesla","gun","gun-alt","gun-rapid","gun-spread","gun-heavy","gun-ricochet","pickup","warning","warp","super","victory-blast","victory"],plays={},lastPlay={},active=new Set();
  let dropped=0,cavern=false,shotSerial=0;const explosionSerial=[0,0,0],teslaRates=[];
  const buffers={},rearSources=new Set(),wave=new Float32Array(256);
  async function start(){
    if(!context){context=new AudioContext();master=context.createGain();master.gain.value=.7;meter=context.createAnalyser();meter.fftSize=256;master.connect(meter);const limiter=context.createDynamicsCompressor();limiter.threshold.value=-12;limiter.knee.value=8;limiter.ratio.value=8;limiter.attack.value=.003;limiter.release.value=.12;meter.connect(limiter);limiter.connect(context.destination);}
    await context.resume();
    if(!loading)loading=Promise.all(['boss-reactor','rear-booster',...combatNames].map(async name=>{
      const response=await fetch('audio/sfx/'+name+'.ogg?v=28');if(!response.ok)throw Error('Sample '+name+' unavailable');
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
    let name=kind===21?['explosion-small','explosion-medium','explosion-large','explosion-boss'][Math.max(0,Math.min(3,triple))]:kind===2?(volume>100?'bomb':'explosion'):kind>=5&&kind<=10?materials[kind-5]:kind===15?materials[Math.max(0,Math.min(5,triple))]:kind===3?'laser':kind===11||kind===14?'bounce':kind===12?'super':kind===13?'victory-blast':kind===16||kind===22?'tesla':kind===1?(triple===1?'gun-spread':triple===2?'gun-rapid':triple===4?'gun-heavy':triple===6?'gun-ricochet':(shotSerial++%2?'gun-alt':'gun')):kind===4?'warp':kind===17?'pickup':kind===18?'warning':kind===20?'victory':null;
    if(kind===21&&triple<3){const variant=explosionSerial[triple]++%3;if(variant)name+='-alt'+variant;}
    if(!name||!buffers[name])return 1;
    const eventKey=kind===22?'tesla-chain':kind===21?'explosion-tier-'+triple:name;
    const now=context.currentTime,gap=name==='explosion-boss'?1:name.startsWith('explosion-large')?.09:name.startsWith('explosion-medium')?.065:name.startsWith('explosion-small')?.045:kind===22?.08:name==='tesla'?.24:name==='laser'?.085:name==='bounce'?.075:name==='bomb'?.12:name.startsWith('gun')?.035:name==='warp'?1:name==='victory'?3:.045;
    if(name==='explosion-boss'&&active.size>=24){const old=active.values().next().value;old.stop();active.delete(old);}
    if(name==='tesla'&&Array.from(active,s=>s.sampleName).filter(n=>n==='tesla'||n==='tesla-chain').length>=4){dropped++;return 1;}
    if(now-(lastPlay[eventKey]??-10)<gap||active.size>=24){dropped++;return 1;}
    lastPlay[eventKey]=now;
    const source=context.createBufferSource(),gain=context.createGain(),filter=context.createBiquadFilter(),stereo=context.createStereoPanner();
    source.buffer=buffers[name];source.sampleName=kind===22?eventKey:name;
    const reference=kind>=5&&kind<=10?[0x3600,0x4200,0x2300,0x4b00,0x2800,0x4800][kind-5]:kind===14?0x4900:kind===16||kind===22?0x4000:kind===3?0x2a00:0x3800;
    source.playbackRate.value=Math.max(.65,Math.min(1.6,Math.pow(2,(pitch-reference)/12288)));
    gain.gain.value=(name==='explosion-boss'?.8:name.startsWith('explosion-large')?.56:name.startsWith('explosion-medium')?.4:name.startsWith('explosion-small')?.25:kind===22?.18:name==='tesla'?.42:name==='victory'?.48:name==='warp'?.55:name==='gun-heavy'?.32:name==='gun-spread'?.27:name.startsWith('gun')?.23:name==='warning'?.32:name==='pickup'?.28:name==='bomb'?.65:name==='explosion'?.48:name==='laser'?.26:name==='bounce'?.19:.32)*Math.min(1.2,volume/65);
    filter.type='lowpass';filter.frequency.value=name==='explosion-boss'?3400:name==='tesla'?6200:10000;filter.Q.value=.4;
    stereo.pan.value=Math.max(-.85,Math.min(.85,pan));
    source.connect(filter);filter.connect(gain);gain.connect(stereo);stereo.connect(master);
    const nodes=[source,filter,gain,stereo],oscillators=[];
    if(name==='tesla'){
      source.playbackRate.value*=Math.pow(2,((Math.random()-.5)*(kind===22?8:3))/12);teslaRates.push(source.playbackRate.value);if(teslaRates.length>12)teslaRates.shift();
      filter.disconnect(gain);const dry=context.createGain(),wet=context.createGain(),lfo=context.createOscillator();dry.gain.value=.85;wet.gain.value=.15;lfo.frequency.value=.11;filter.connect(dry);dry.connect(gain);let previous=filter;
      for(const frequency of [550,1100,2200,4000]){const stage=context.createBiquadFilter(),depth=context.createGain();stage.type='allpass';stage.frequency.value=frequency+Math.sin(now*.69)*frequency*.12;stage.Q.value=.4;depth.gain.value=frequency*.12;lfo.connect(depth);depth.connect(stage.frequency);previous.connect(stage);previous=stage;nodes.push(stage,depth);}previous.connect(wet);wet.connect(gain);lfo.start();oscillators.push(lfo);nodes.push(dry,wet,lfo);
    }

    if(kind===3){gain.gain.value/=(triple?2.4:1.7);for(const ratio of (triple?[1.498307,1.781797]:[1.781797])){const harmonic=context.createBufferSource();harmonic.buffer=source.buffer;harmonic.playbackRate.value=source.playbackRate.value*ratio;harmonic.connect(filter);harmonic.start(now);nodes.push(harmonic);}}

    // Each reflected sample has its own short flanger; no shared resonance build-up.
    if(kind===11||kind===14){const delay=context.createDelay(.03),feedback=context.createGain(),wet=context.createGain(),lfo=context.createOscillator(),depth=context.createGain();delay.delayTime.value=.004;feedback.gain.value=.18;wet.gain.value=.24;lfo.frequency.value=.16+(pitch%7)*.015;depth.gain.value=.0015;lfo.connect(depth);depth.connect(delay.delayTime);gain.connect(delay);delay.connect(feedback);feedback.connect(delay);delay.connect(wet);wet.connect(stereo);lfo.start();oscillators.push(lfo);nodes.push(delay,feedback,wet,lfo,depth);}
    // A quiet, short reflection reinforces enclosed cavern acoustics.
    if(cavern){const delay=context.createDelay(.2),wet=context.createGain();delay.delayTime.value=.095;wet.gain.value=.18;gain.connect(delay);delay.connect(wet);wet.connect(stereo);nodes.push(delay,wet);}
    active.add(source);plays[kind===22?eventKey:name]=(plays[kind===22?eventKey:name]||0)+1;
    source.onended=()=>{setTimeout(()=>{for(const o of oscillators)o.stop();for(const n of nodes)n.disconnect();active.delete(source);},160);};if(kind===22)source.start(now,0,.13);else source.start();
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
  return {start,observe,event,get state(){return {teslaRates:[...teslaRates],teslaGain:.42,teslaPhaseWet:.15,sampleOnly:true,ready:Object.keys(buffers).length===combatNames.length+2,bossActive:!!bossNode,phase,rearPlays,plays:{...plays},active:active.size,activeNames:Array.from(active,s=>s.sampleName),dropped,activeRear:rearSources.size,masterGain:master?.gain.value||0,bossGain:bossNode?.g.gain.value||0,peak,errors};}};
})();
