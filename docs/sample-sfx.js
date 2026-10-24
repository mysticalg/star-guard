/* Small CC0 sample bus: decoded once, mixed by Web Audio, never in the render loop. */
window.SoundSamples=(()=>{
  let context,master,meter,bossNode=null,loading=null,lastRear=null,rearPlays=0,errors=0,phase=1,peak=0;
  const buffers={},rearSources=new Set(),wave=new Float32Array(256);
  async function start(){
    if(!context){context=new AudioContext();master=context.createGain();master.gain.value=.7;meter=context.createAnalyser();meter.fftSize=256;master.connect(meter);meter.connect(context.destination);}
    await context.resume();
    if(!loading)loading=Promise.all(['boss-reactor','rear-booster'].map(async name=>{
      const response=await fetch('audio/sfx/'+name+'.ogg?v=26');if(!response.ok)throw Error('Sample '+name+' unavailable');
      buffers[name]=await context.decodeAudioData(await response.arrayBuffer());
    })).catch(error=>{errors++;console.error(error);});
    return loading;
  }
  function stopBoss(){if(!bossNode)return;const old=bossNode;bossNode=null;old.g.gain.setTargetAtTime(0,context.currentTime,.1);old.s.stop(context.currentTime+.4);}
  function startBoss(){const s=context.createBufferSource(),g=context.createGain(),f=context.createBiquadFilter();s.buffer=buffers['boss-reactor'];s.loop=true;g.gain.value=0;f.type='lowpass';f.frequency.value=500;s.connect(f);f.connect(g);g.connect(master);s.onended=()=>{s.disconnect();f.disconnect();g.disconnect();};s.start();bossNode={s,g,f};}
  function rear(x){if(!buffers['rear-booster'])return;if(rearSources.size>=3){const old=rearSources.values().next().value;old.stop();rearSources.delete(old);}const s=context.createBufferSource(),g=context.createGain(),pan=context.createStereoPanner();s.buffer=buffers['rear-booster'];s.playbackRate.value=.94+(rearPlays%4)*.04;g.gain.value=.38;pan.pan.value=Math.max(-.8,Math.min(.8,(x-160)/200));s.connect(g);g.connect(pan);pan.connect(master);rearSources.add(s);s.onended=()=>{rearSources.delete(s);s.disconnect();g.disconnect();pan.disconnect();};rearPlays++;s.start();}
  function observe(stat){
    if(!context)return;const now=context.currentTime,quiet=!!stat(18)||!!stat(6)||document.hidden;
    master.gain.setTargetAtTime(quiet?0:.7,now,.04);
    const boss=stat(24)>0&&stat(1)===1&&stat(35)>-20*256;
    phase=Math.max(1,Math.min(3,stat(45)));
    if(boss&&buffers['boss-reactor']){if(!bossNode)startBoss();bossNode.g.gain.setTargetAtTime(.28+phase*.04,now,.5);bossNode.f.frequency.setTargetAtTime(400+phase*100,now,.5);bossNode.s.playbackRate.setTargetAtTime(.92+phase*.04,now,.5);}else stopBoss();
    const serial=stat(90);if(lastRear!==null&&serial>lastRear&&!quiet)for(let n=0;n<Math.min(2,serial-lastRear);n++)rear(stat(91));lastRear=serial;
    meter.getFloatTimeDomainData(wave);for(const v of wave)peak=Math.max(peak,Math.abs(v));
  }
  return {start,observe,get state(){return {ready:Object.keys(buffers).length===2,bossActive:!!bossNode,phase,rearPlays,activeRear:rearSources.size,masterGain:master?.gain.value||0,bossGain:bossNode?.g.gain.value||0,peak,errors};}};
})();
