/* Licensed sample bus: decoded once, mixed by Web Audio, never in the render loop. */
window.SoundSamples=(()=>{
  let audioQuiet76=false,drive76=null,context,master,meter,bossNode=null,loading=null,lastRear=null,rearPlays=0,errors=0,phase=1,peak=0,currentPeak=0,masterTarget=.7;
  const combatNames=["explosion-small","explosion-small-alt1","explosion-small-alt2","explosion-medium","explosion-medium-alt1","explosion-medium-alt2","explosion-large","explosion-large-alt1","explosion-large-alt2","explosion-boss","explosion","bomb","laser","bounce","metal","metal-alt1","metal-alt2","shield","rock","rock-alt1","rock-alt2","glass","glass-alt1","glass-alt2","organic","energy","tesla","gun","gun-alt","gun-rapid","gun-spread","gun-heavy","gun-ricochet","pickup","warning","warp","super","victory-blast","victory","torpedo-launch","torpedo-blast","chaff-deploy"],plays={},lastPlay={},active=new Set();
  let dropped=0,budgetDropped=0,unavailable=0,retired=0,requested=0,ended=0,cavern=false,shotSerial=0;const explosionSerial=[0,0,0],teslaRates=[],materialSerial={};
  const flybyNames=['boss-flyby-engine','boss-flyby-organic','boss-flyby-ice'];
  const recordingNames76=['engine-jet-loop-v76','machine-energy-loop-v76','laser-sustain-v76','laser-charge-v76','laser-tail-v76','laser-reflect-v76','metal-crash-v76','hull-hit-v76','mechanical-actuation-v76','enemy-heavy-fire-v76','organic-roar-a-v76','organic-roar-b-v76','organic-roar-c-v76','organic-growl-v76','organic-attack-a-v76','organic-attack-b-v76','organic-squelch-v76'];
  const files76={bomb:'explosion-large-alt2.ogg',explosion:'explosion-medium.ogg','victory-blast':'explosion-large.ogg',laser:'laser-discharge-v76.ogg',bounce:'laser-reflect-v76.ogg',shield:'shield-absorb-v76.ogg',organic:'organic-wet-hit-v76.ogg',energy:'laser-discharge-v76.ogg',super:'laser-discharge-v76.ogg','rear-booster':'engine-jet-boost-v76.ogg','torpedo-launch':'engine-jet-boost-v76.ogg','chaff-deploy':'mechanical-actuation-v76.ogg',warp:'engine-jet-boost-v76.ogg','laser-tail-v76':'laser-discharge-v76.ogg','boss-flyby-engine':'engine-jet-boost-v76.ogg','boss-flyby-organic':'organic-roar-a-v76.ogg','boss-flyby-ice':'glass-alt1.ogg'};
  const allSampleNames76=[...new Set(['rear-booster',...combatNames,...recordingNames76,...flybyNames])];
  const buffers={},tails=new Set(),rearSources=new Set(),wave=new Float32Array(256);
  async function start(){
    if(!context){context=window.GameAudio76?.get().context||new AudioContext();master=context.createGain();master.gain.value=.7;meter=context.createAnalyser();meter.fftSize=256;master.connect(meter);const limiter=context.createDynamicsCompressor();limiter.threshold.value=-12;limiter.knee.value=8;limiter.ratio.value=8;limiter.attack.value=.003;limiter.release.value=.12;meter.connect(limiter);limiter.connect(window.GameAudio76?.get().input||context.destination);}
    await context.resume();
    if(!drive76&&window.RecordedDrive76)drive76=window.RecordedDrive76.create(context,master,buffers,(_,pan,signature)=>signature==='cryo'?event(38,28,0x4b00,pan,3):event(36,48,0x3800,pan,0));
    if(!loading)loading=(async()=>{const byFile=new Map();for(const name of allSampleNames76){const file=files76[name]||name+'.ogg';if(!byFile.has(file))byFile.set(file,[]);byFile.get(file).push(name);}const files=[...byFile.keys()];let next=0;async function worker(){while(next<files.length){const file=files[next++];let failure;for(let attempt=0;attempt<3;attempt++){try{const response=await fetch('audio/sfx/'+file+'?v=76');if(!response.ok)throw Error('Sample '+file+' unavailable');const decoded=await context.decodeAudioData(await response.arrayBuffer());for(const name of byFile.get(file))buffers[name]=decoded;failure=null;break;}catch(error){failure=error;await new Promise(resolve=>setTimeout(resolve,150*(attempt+1)));}}if(failure)throw failure;}}await Promise.all(Array.from({length:4},worker));})().catch(error=>{errors++;console.error(error);});
    return loading;
  }
  function stopBoss(){if(!bossNode)return;const old=bossNode;bossNode=null;old.g.gain.setTargetAtTime(0,context.currentTime,.1);old.s.stop(context.currentTime+.4);}
  function startBoss(){const s=context.createBufferSource(),g=context.createGain(),f=context.createBiquadFilter();s.buffer=buffers['boss-reactor'];s.loop=true;g.gain.value=0;f.type='lowpass';f.frequency.value=500;s.connect(f);f.connect(g);g.connect(master);s.onended=()=>{s.disconnect();f.disconnect();g.disconnect();};s.start();bossNode={s,g,f};}
  function rear(x){if(!buffers['rear-booster'])return;if(rearSources.size>=3){const old=rearSources.values().next().value;old.stop();rearSources.delete(old);}const s=context.createBufferSource(),g=context.createGain(),pan=context.createStereoPanner();s.buffer=buffers['rear-booster'];s.playbackRate.value=.94+(rearPlays%4)*.04;g.gain.value=.38;pan.pan.value=Math.max(-.8,Math.min(.8,(x-160)/200));s.connect(g);g.connect(pan);pan.connect(master);rearSources.add(s);s.onended=()=>{rearSources.delete(s);s.disconnect();g.disconnect();pan.disconnect();};rearPlays++;s.start();}
  function retireSamples76(now){for(const playing of active)if(playing.endsAt<=now){active.delete(playing);tails.add(playing);retired++;}for(const playing of tails)if(playing.endsAt+.16<=now)playing.dispose();}
  // Called at the actual C collision/fire event, rather than polling counters.
  // A fixed voice budget and category cooldowns bound large chain reactions.
  function event(kind,volume,pitch,pan,triple){
    requested++;if(!context){unavailable++;return 0;}if(audioQuiet76)return 1;
    if(kind===39&&drive76&&!drive76.leaseBeam('player',pan,volume,'cryo'))return 1;
    if(kind===3&&drive76&&!drive76.leaseBeam('player',pan,volume,!!triple))return 1;
    if(kind===12&&drive76&&!drive76.leaseBeam('hostile',pan,volume,false,Math.max(.04,Math.min(5,(triple>>>8)/50||.28))))return 1;
    const added76=kind===38?(triple===2?'glass-alt1':triple===3?'glass-alt2':'glass'):kind===39?'glass-alt2':kind===28?((triple&3)===3?'organic-growl-v76':(triple&1)?(shotSerial++%2?'organic-attack-a-v76':'organic-attack-b-v76'):(triple&4)?'energy':(triple&2)?'enemy-heavy-fire-v76':'gun-alt'):kind===29?'hull-hit-v76':kind===30?'metal-crash-v76':kind===31?'organic':kind===32?'mechanical-actuation-v76':kind===33?'organic-roar-'+['a','b','c'][shotSerial++%3]+'-v76':kind===34?'energy':kind===35?(triple&1?(triple&4?'organic-roar-c-v76':'organic-growl-v76'):'mechanical-actuation-v76'):kind===36?'laser-tail-v76':kind===37?'laser-charge-v76':null;
    const materials=['metal','shield','rock','glass','organic','energy'];
    let name=added76||(kind===27?flybyNames[(triple&7)===1||(triple&7)===4?1:(triple&7)===2?2:0]:kind===26?'hull-hit-v76':kind===25?'chaff-deploy':kind===23?'torpedo-launch':kind===24?'torpedo-blast':kind===21?['explosion-small','explosion-medium','explosion-large','explosion-boss'][Math.max(0,Math.min(3,triple))]:kind===2?(volume>100?'bomb':'explosion'):kind>=5&&kind<=10?materials[kind-5]:kind===15?materials[Math.max(0,Math.min(5,triple))]:kind===3?'laser':kind===14?materials[Math.max(0,Math.min(5,triple))]:kind===11?'bounce':kind===12?'super':kind===13?'victory-blast':kind===16||kind===22?'tesla':kind===1?(triple===1?'gun-spread':triple===2?'gun-rapid':triple===4?'gun-heavy':triple===6?'gun-ricochet':(shotSerial++%2?'gun-alt':'gun')):kind===4?'warp':kind===17?'pickup':kind===18?'warning':kind===20?'victory':null);
    if(kind===21&&triple<3){const variant=explosionSerial[triple]++%3;if(variant)name+='-alt'+variant;}
    let materialBase='';if(((kind>=5&&kind<=10)||kind===14||kind===15)&&['metal','rock','glass'].includes(name)){materialBase=name;const variant=(materialSerial[name]||0)%3;materialSerial[name]=(materialSerial[name]||0)+1;if(variant)name+='-alt'+variant;}
    if(!name)return 1;if(!buffers[name]){unavailable++;return 0;}
    const eventKey=kind===38?'cryo-'+triple:kind===39?'cryo-start':kind===28?'npc-fire-'+(triple&7):kind===35?'recorded-35-'+(triple&4?'death':'phase'):kind>=29&&kind<=37?'recorded-'+kind:kind===14?'reflection-'+triple:kind===27?'boss-flyby':kind===26?'critical-hit':kind===22?'tesla-chain':kind===21?'explosion-tier-'+triple:materialBase||name;
    const now=context.currentTime,gap=kind===38?(triple===2?.07:.14):kind===39?.12:kind===28?((triple&3)===3?.6:(triple&1)?.11:.065):kind===29?.065:kind===30?.2:kind===31?.12:kind===32?.16:kind===33?.8:kind===34?.25:kind===35?.8:kind===36?.08:kind===37?.18:kind===27?.4:kind===26?0:name==='torpedo-launch'?.2:name==='torpedo-blast'?.2:name==='explosion-boss'?1:name.startsWith('explosion-large')?.09:name.startsWith('explosion-medium')?.065:name.startsWith('explosion-small')?.045:kind===22?.08:name==='tesla'?.24:name==='laser'?.085:name==='bounce'?.075:name==='bomb'?.12:name.startsWith('gun')?.035:name==='warp'?1:name==='victory'?3:.045;
    // Audio playback can finish before the main thread delivers onended.
    // Admission follows the audio clock, never delayed cleanup timers.
    retireSamples76(now);
    const priority=kind===18?4:kind===29||kind===30||kind===35||kind===37?3:kind===33||name==='explosion-boss'?2:kind===1||kind===3||kind===11||kind===12||kind===16||kind===23||kind===24||kind===26||kind===27?1:0;
    if(priority&&active.size>=24){const old=[...active].sort((a,b)=>(a.priority||0)-(b.priority||0))[0];if(old&&priority>(old.priority||0)){try{old.stop();}catch{}old.dispose();}}

    if(name==='tesla'&&Array.from(active,s=>s.sampleName).filter(n=>n==='tesla'||n==='tesla-chain').length>=4){dropped++;budgetDropped++;return 1;}
    if(now-(lastPlay[eventKey]??-10)<gap){dropped++;return 1;}
    if(active.size>=24){dropped++;budgetDropped++;return 1;}
    lastPlay[eventKey]=now;
    const source=context.createBufferSource(),gain=context.createGain(),filter=context.createBiquadFilter(),stereo=context.createStereoPanner();
    source.buffer=buffers[name];source.sampleName=kind===14||kind===22||kind===28?eventKey:name;source.priority=priority;
    const reference=kind===38||kind===39?0x4b00:kind>=5&&kind<=10?[0x3600,0x4200,0x2300,0x4b00,0x2800,0x4800][kind-5]:kind===14?0x4900:kind===26?0x5900:kind===16||kind===22?0x4000:kind===3?0x2a00:0x3800;
    source.playbackRate.value=Math.max(.65,Math.min(1.6,Math.pow(2,(pitch-reference)/12288)));
    if(kind===37&&triple>0)source.playbackRate.value=Math.max(.25,Math.min(2,source.buffer.duration/(triple/50)));
    gain.gain.value=(kind===38?(triple===2?.34:triple===3?.08:.15):kind===39?.10:kind===28?((triple&2)?.28:.19):kind===29?.33:kind===30?.56:kind===31?.16:kind===32?.22:kind===33?.38:kind===34?.20:kind===35?.35:kind===36?.15:kind===37?.16:kind===27?.7:name==='torpedo-launch'?.3:name==='torpedo-blast'?.6:name==='explosion-boss'?.8:name.startsWith('explosion-large')?.56:name.startsWith('explosion-medium')?.4:name.startsWith('explosion-small')?.25:kind===22?.18:name==='tesla'?.42:name==='victory'?.48:name==='warp'?.55:name==='gun-heavy'?.32:name==='gun-spread'?.27:name.startsWith('gun')?.23:name==='warning'?.32:name==='pickup'?.28:name==='bomb'?.65:name==='explosion'?.48:name==='laser'?.26:name==='bounce'?.19:.32)*Math.min(1.2,volume/65);
    filter.type='lowpass';filter.frequency.value=name==='explosion-boss'?3400:name==='tesla'?6200:10000;filter.Q.value=.4;
    stereo.pan.value=Math.max(-.85,Math.min(.85,pan));
    source.connect(filter);filter.connect(gain);gain.connect(stereo);stereo.connect(master);
    if(kind===27){const direction=triple&8?-1:1,rate=source.playbackRate.value;source.playbackRate.setValueAtTime(rate*1.15,now);source.playbackRate.linearRampToValueAtTime(rate*.76,now+.6);filter.type='bandpass';filter.Q.value=.65;filter.frequency.setValueAtTime(450,now);filter.frequency.exponentialRampToValueAtTime((triple&7)===1?1800:3400,now+.24);filter.frequency.exponentialRampToValueAtTime(380,now+.65);stereo.pan.setValueAtTime(Math.max(-.85,Math.min(.85,pan-direction*.65)),now);stereo.pan.linearRampToValueAtTime(Math.max(-.85,Math.min(.85,pan+direction*.65)),now+.65);}

    const nodes=[source,filter,gain,stereo],oscillators=[];
    if(name==='tesla'){
      source.playbackRate.value*=Math.pow(2,((Math.random()-.5)*(kind===22?8:3))/12);teslaRates.push(source.playbackRate.value);if(teslaRates.length>12)teslaRates.shift();
      filter.disconnect(gain);const dry=context.createGain(),wet=context.createGain(),lfo=context.createOscillator();dry.gain.value=.85;wet.gain.value=.15;lfo.frequency.value=.11;filter.connect(dry);dry.connect(gain);let previous=filter;
      for(const frequency of [550,1100,2200,4000]){const stage=context.createBiquadFilter(),depth=context.createGain();stage.type='allpass';stage.frequency.value=frequency+Math.sin(now*.69)*frequency*.12;stage.Q.value=.4;depth.gain.value=frequency*.12;lfo.connect(depth);depth.connect(stage.frequency);previous.connect(stage);previous=stage;nodes.push(stage,depth);}previous.connect(wet);wet.connect(gain);lfo.start();oscillators.push(lfo);nodes.push(dry,wet,lfo);
    }



    // Each reflected sample has its own short flanger; no shared resonance build-up.
    if(kind===11||kind===14){const delay=context.createDelay(.03),feedback=context.createGain(),wet=context.createGain(),lfo=context.createOscillator(),depth=context.createGain();delay.delayTime.value=.004;feedback.gain.value=.18;wet.gain.value=.24;lfo.frequency.value=.16+(pitch%7)*.015;depth.gain.value=.0015;lfo.connect(depth);depth.connect(delay.delayTime);gain.connect(delay);delay.connect(feedback);feedback.connect(delay);delay.connect(wet);wet.connect(stereo);lfo.start();oscillators.push(lfo);nodes.push(delay,feedback,wet,lfo,depth);}
    // A quiet, short reflection reinforces enclosed cavern acoustics.
    if(cavern){const delay=context.createDelay(.2),wet=context.createGain();delay.delayTime.value=.095;wet.gain.value=.18;gain.connect(delay);delay.connect(wet);wet.connect(stereo);nodes.push(delay,wet);}
    source.endsAt=now+(kind===27?1.1:kind===22?.13:source.buffer.duration/source.playbackRate.value);
    for(const oscillator of oscillators)oscillator.stop(source.endsAt+.16);
    active.add(source);plays[kind===14||kind===22||kind===26||kind===28||kind>=29&&kind<=39?eventKey:name]=(plays[kind===14||kind===22||kind===26||kind===28||kind>=29&&kind<=39?eventKey:name]||0)+1;
    source.dispose=()=>{if(source.cleaned)return;source.cleaned=true;active.delete(source);tails.delete(source);for(const n of nodes)n.disconnect();};
    source.onended=()=>{ended++;active.delete(source);if(!source.cleaned){tails.add(source);setTimeout(source.dispose,160);}};if(kind===22)source.start(now,0,.13);else if(kind===27){source.start();gain.gain.setTargetAtTime(0,now+.7,.07);source.stop(now+1.1);}else source.start();
    return 1;
  }
  function observe(stat){
    if(!context)return;cavern=!!stat(81);const now=context.currentTime,quiet=!!stat(18)||!!stat(6)||document.hidden||blurred;audioQuiet76=quiet;retireSamples76(now);
    const target=quiet?0:window.ShipVoice?.speaking?.45:.7;if(target!==masterTarget){master.gain.cancelScheduledValues(now);master.gain.setTargetAtTime(target,now,.04);masterTarget=target;}
    phase=Math.max(1,Math.min(3,stat(45)));stopBoss();
    if(drive76){const sources=[];for(let i=0;i<Math.min(6,stat(12200));i++){const n=12210+i*6;sources.push({id:stat(n),kind:stat(n+1),drive:stat(n+2)/1000,pan:stat(n+3)/1000,near:stat(n+4)/1000});}drive76.observe({quiet:quiet||stat(1)!==1,scene:stat(10)+':'+stat(160),tick:stat(0),playerBeam:!!stat(12260),hostileBeam:!!stat(12261),sources});}
    const serial=stat(90);if(lastRear!==null&&serial>lastRear&&!quiet)for(let n=0;n<Math.min(2,serial-lastRear);n++)rear(stat(91));lastRear=serial;
    meter.getFloatTimeDomainData(wave);currentPeak=0;for(const v of wave)currentPeak=Math.max(currentPeak,Math.abs(v));peak=Math.max(peak,currentPeak);
  }
  let blurred=false;addEventListener('blur',()=>{blurred=true;audioQuiet76=true;drive76?.stopAll();if(master){master.gain.cancelScheduledValues(context.currentTime);master.gain.setTargetAtTime(0,context.currentTime,.025);masterTarget=0;}});addEventListener('focus',()=>{blurred=false;});document.addEventListener('visibilitychange',()=>{if(document.hidden&&master){audioQuiet76=true;drive76?.stopAll();master.gain.cancelScheduledValues(context.currentTime);master.gain.setTargetAtTime(0,context.currentTime,.025);masterTarget=0;}});
  return {start,observe,event,get state(){return {teslaRates:[...teslaRates],teslaGain:.42,teslaPhaseWet:.15,sampleOnly:true,generatedFlybys:false,recordedFlybys:true,quiet:audioQuiet76,drive:drive76?.state||null,contextState:context?.state||"not-started",audioClock:context?.currentTime||0,sampleRate:context?.sampleRate||0,requested,unavailable,budgetDropped,retired,ended,cleanupPending:tails.size,ready:allSampleNames76.every(name=>!!buffers[name]),bossActive:!!bossNode,phase,rearPlays,plays:{...plays},active:active.size,totalVoices:active.size+rearSources.size+(drive76?.state.sources||0),audibleLimit:35,activeNames:Array.from(active,s=>s.sampleName),dropped,activeRear:rearSources.size,masterTarget,currentPeak,masterGain:master?.gain.value||0,bossGain:bossNode?.g.gain.value||0,peak,errors};}};
})();
