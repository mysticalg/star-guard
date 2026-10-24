/* One small audio sprite and one decoded buffer; warnings outrank pickup chatter. */
window.ShipVoice=(()=>{
  let context,gain,buffer,clips,source,current,queue=[],enabled=true,paused=false,muted=false,previous=null,played=0;
  const recent=new Map();
  async function load(){
    if(context)return;
    context=new (window.AudioContext||window.webkitAudioContext)();
    gain=context.createGain();gain.gain.value=.8;gain.connect(context.destination);
    await context.resume();
    const [meta,data]=await Promise.all([fetch('vo/announcements.json?v=27').then(r=>{if(!r.ok)throw Error('Voice metadata');return r.json();}),fetch('vo/announcements.mp3?v=27').then(r=>{if(!r.ok)throw Error('Voice audio');return r.arrayBuffer();})]);
    clips=meta.clips;buffer=await context.decodeAudioData(data);pump();
  }
  function stop(){if(source){source.onended=null;try{source.stop();}catch{}source=null;}current=null;}
  function pump(){
    if(!buffer||!enabled||paused||muted||source||!queue.length)return;
    const item=queue.shift(),clip=clips[item.key];if(!clip){pump();return;}
    current=item;source=context.createBufferSource();source.buffer=buffer;source.connect(gain);source.onended=()=>{source=null;current=null;pump();};
    source.start(0,clip.start,clip.duration);played++;
  }
  function say(key,priority=1){
    if(!enabled||muted)return;
    const now=performance.now(),cooldown=key==='energy-low'?30000:key==='hull-critical'?15000:1500;if(now-(recent.get(key)||-100000)<cooldown)return;
    recent.set(key,now);
    if(priority>=3){queue=queue.filter(x=>x.priority>=priority);if(current&&current.priority<priority)stop();}
    queue.push({key,priority});queue.sort((a,b)=>b.priority-a.priority);if(queue.length>4)queue.length=4;pump();
  }
  function start(){queue=[];stop();previous=null;recent.clear();load().then(()=>{context.resume();if(!queue.length&&!source)say("mission-start",2);pump();}).catch(()=>{const button=document.querySelector('#voice-toggle');if(button)button.textContent='Ship voice unavailable';});}
  function observe(stat){
    const s={mode:stat(1),sector:stat(15),tick:stat(55),pickup:stat(40),pickupTicks:stat(41),weapon:stat(2),ship:stat(56),warning:stat(33),morph:stat(46),form:stat(45),laser:stat(52),rear:stat(42),storm:stat(48),clear:stat(34),travel:stat(16),hull:stat(21),energy:stat(22)};
    muted=!!stat(18);const pause=!!stat(6)||document.hidden||!document.hasFocus();
    if(muted){queue=[];stop();}gain&&(gain.gain.value=muted?0:.8);
    if(pause!==paused){paused=pause;if(context){if(paused)context.suspend();else context.resume().then(pump);}}
    const p=previous;if(p&&s.tick<p.tick){previous=null;start();return;}
    if(!p){if(s.sector>=0)say('sector-'+s.sector,1);previous=s;return;}
    if(s.mode===2&&p.mode!==2)say('game-over',4);
    if(s.sector!==p.sector&&s.sector>=0)say('sector-'+s.sector,2);
    if(s.warning&&!p.warning)say('boss-approach',3);
    if(s.morph&&!p.morph)say(s.form===3?'final-form':'form-two',3);
    if(s.laser>0&&s.laser<=60&&(!p.laser||p.laser>60))say('super-laser',4);
    if(s.rear&&!p.rear)say('rear-attack',3);
    if(s.storm&&s.storm!==p.storm)say(['','asteroid-storm','ice-storm','fire-storm','ice-asteroid-storm','metal-asteroid-storm','radiation-zone','diamond-asteroid-storm','uranium-asteroid-storm'][s.storm],3);
    if(s.clear&&!p.clear)say('sector-clear',2);
    if(s.travel&&!p.travel)say('warp',2);
    if(s.hull<=25&&p.hull>25)say('hull-critical',3);
    if(s.energy<=10&&p.energy>10)say('energy-low',1);
    previous=s;
  }
  document.addEventListener('visibilitychange',()=>{if(context&&document.hidden){paused=true;context.suspend();}});
  return {start,observe,get speaking(){return !!source&&!paused&&!muted&&enabled;},toggle(){enabled=!enabled;if(!enabled){queue=[];stop();}return enabled;},get state(){return {enabled,ready:!!buffer,current:current?.key||null,queued:queue.length,played,paused,muted};}};
})();
