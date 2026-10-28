const vm=require('node:vm'),fs=require('node:fs'),assert=require('node:assert/strict');
let clock=0;const sources=[];
const param=()=>({value:0,setTargetAtTime(v){this.value=v},cancelScheduledValues(){}});
const node=()=>({gain:param(),frequency:param(),Q:param(),pan:param(),playbackRate:param(),connect(){},disconnect(){this.disconnected=true}});
const context={get currentTime(){return clock},createBufferSource(){const n={...node(),start(){this.playing=true},stop(at){this.stops=(this.stops||0)+1;this.stopAt=at;this.playing=at>clock}};sources.push(n);return n},createGain:node,createStereoPanner:node,createBiquadFilter:node};
const box={window:{}};vm.runInNewContext(fs.readFileSync('docs/recorded-drive76.js','utf8'),box);const buffers={'laser-sustain-v76':{duration:1.7},'machine-energy-loop-v76':{duration:2.1},'engine-jet-loop-v76':{duration:4.9}},tails=[];
const drive=box.window.RecordedDrive76.create(context,node(),buffers,(...args)=>tails.push(args));
const frame=(ids,extra={})=>({scene:'1/0',quiet:false,sources:ids.map((id,i)=>({id,kind:i%2+1,drive:.5+i*.1,near:.7,pan:i*.2-.5})),...extra});
drive.observe(frame([1,2,3,4,5,6,7,8,9]));assert.equal(drive.state.engines,6);assert.equal(drive.state.sources,6);assert(drive.state.active.every(e=>e.gain>0&&e.gain<.2&&Math.abs(e.pan)<=.85));assert(drive.state.active.some(e=>e.sample==='machine-energy-loop-v76'));
const start=drive.state.starts;for(let i=0;i<500;i++)drive.observe(frame([1,2,3,4,5,6]));assert.equal(drive.state.starts,start,'unchanged source IDs must not allocate each rendered frame');
assert.equal(drive.leaseBeam('player',.3,70),true);assert.equal(drive.leaseBeam('player',.2,70),false);assert.equal(drive.leaseBeam('hostile',-.4,90,false,.28),true);assert.equal(drive.leaseBeam('overflow',0,90),false);assert.equal(drive.state.beams,2);assert.equal(drive.state.sources,8);
for(let t=0;t<120;t++){clock+=.02;drive.observe(frame([100+t*6,101+t*6,102+t*6,103+t*6,104+t*6,105+t*6]));assert(drive.state.sources<=8);assert(drive.state.engines<=6);}
assert.equal(drive.state.beams,0);assert.equal(tails.length,2);assert(drive.state.evicted>0);assert.equal(drive.state.peak,8);
drive.observe(frame([901],{quiet:true}));assert.equal(drive.state.sources,0);assert(sources.every(s=>!s.playing||s.stopAt<=clock));assert(sources.every(s=>s.disconnected));
clock+=.2;drive.observe(frame([901,902]));assert.equal(drive.state.engines,2);drive.observe(frame([],{scene:'2/0'}));assert.equal(drive.state.engines,0);assert.equal(drive.state.sources,0);drive.observe(frame([910]));assert.equal(drive.state.engines,1);drive.stopAll();assert.equal(drive.state.sources,0);
console.log('PASS recorded-drive76: finite8-source graph,6engines/2beams, actual drive/pan, no frame allocations for stable IDs, death/churn/release/scene/pause/mute cleanup and resume');
