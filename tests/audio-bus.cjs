const vm=require('node:vm'),fs=require('node:fs'),assert=require('node:assert/strict');
const input=process.argv[2]||'docs/sample-sfx.js';
let clock=0,sourceStarts=0,oscillatorStops=0,holdTesla=false;
function param(){return {value:0,setTargetAtTime(v){this.value=v;}};}
function node(){return {gain:param(),frequency:param(),Q:param(),pan:param(),playbackRate:{value:1},delayTime:param(),threshold:param(),knee:param(),ratio:param(),attack:param(),release:param(),connect(){},disconnect(){},start(){sourceStarts++;},stop(){oscillatorStops++;},getFloatTimeDomainData(a){a.fill(0);}};}
class Context{
 get currentTime(){return clock;}get state(){return 'running';}get sampleRate(){return 48000;}
 constructor(){this.destination=node();}
 resume(){return Promise.resolve();}
 createGain(){return node();}createAnalyser(){return node();}createDynamicsCompressor(){return node();}
 createBiquadFilter(){return node();}createStereoPanner(){return node();}createBufferSource(){return node();}
 createDelay(){return node();}createOscillator(){return node();}
 decodeAudioData(data){return holdTesla&&data.name.includes('/tesla.ogg')?new Promise(()=>{}):Promise.resolve({duration:.01});}
}
async function setup(holding=false){clock=0;sourceStarts=0;holdTesla=holding;const window={};const sandbox={window,AudioContext:Context,Float32Array,Math,console,document:{hidden:false},setTimeout:()=>0,fetch:async name=>({ok:true,arrayBuffer:async()=>({name})})};vm.runInNewContext(fs.readFileSync(input,'utf8'),sandbox);const loading=window.SoundSamples.start();if(holding){for(let i=0;i<200;i++)await Promise.resolve();}else await loading;return window.SoundSamples;}
(async()=>{
 for(const renderHz of [60,120,240,1000]){
  const bus=await setup();let nextEvent=.08;
  // Audio clock progresses; intentionally never deliver onended or timer callbacks.
  // This reproduces delayed main-thread cleanup without pretending to render at this rate.
  for(let frame=0;clock<8.01;frame++){
   clock=frame/renderHz;
   while(nextEvent<=clock+1e-8&&nextEvent<=8.001){bus.event(1,65,0x3800,0,0);nextEvent+=.08;}
  }
  const played=Object.values(bus.state.plays).reduce((a,b)=>a+b,0);
  console.log(JSON.stringify({syntheticRenderHz:renderHz,requested:100,played,dropped:bus.state.dropped,active:bus.state.active}));
  assert.equal(played,100,'completed samples must not consume the voice budget when callbacks are delayed');
  assert.equal(bus.state.dropped,0);assert.ok(bus.state.active<=1);
  assert.ok(bus.state.cleanupPending<=3,'finished effect graphs must also be reclaimed without completion callbacks');
 }
 const bus=await setup();clock=1;
 for(let i=0;i<100;i++)bus.event(1,65,0x3800,0,0);
 assert.ok(Object.values(bus.state.plays).reduce((a,b)=>a+b,0)<=2,'same-time category throttling remains bounded');
 assert.ok(bus.state.dropped>=98);
 const partial=await setup(true);clock=1;
 partial.event(1,65,0x3800,0,0);
 assert.equal(Object.values(partial.state.plays).reduce((a,b)=>a+b,0),1,'a delayed Tesla decode must not silence an already decoded gun sample');
 const stops=oscillatorStops;partial.event(14,65,0x4900,0,0);
 assert.ok(oscillatorStops>stops,'effect oscillators stop on the audio clock without waiting for JS completion');
 console.log('PASS clock-driven voice retirement at synthetic 60/120/240/1000 cadence; event cooldown remains bounded');
})().catch(e=>{console.error(e);process.exitCode=1;});
