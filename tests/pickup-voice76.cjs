const vm=require('node:vm'),fs=require('node:fs'),assert=require('node:assert/strict');
let clock=0,stopped=0;const sources=[];
class Context{constructor(){this.destination={};}resume(){return Promise.resolve()}suspend(){return Promise.resolve()}createGain(){return{gain:{value:1},connect(){}}}decodeAudioData(data){return Promise.resolve({role:data.file.includes('pickup-names')?'pickup':'mission'})}createBufferSource(){const s={connect(){},start(_,start,duration){this.startAt=start;this.duration=duration},stop(){stopped++}};sources.push(s);return s;}}
const box={window:{AudioContext:Context},performance:{now:()=>clock},document:{hidden:false,hasFocus:()=>true,addEventListener(){},querySelector(){return null}},console,fetch:async file=>({ok:true,json:async()=>JSON.parse(fs.readFileSync('docs/'+file.split('?')[0],'utf8')),arrayBuffer:async()=>({file})})};
vm.runInNewContext(fs.readFileSync('docs/ship-voice.js','utf8'),box);const voice=box.window.ShipVoice;
const values={1:1,15:-1,55:1,21:100,22:100,12280:0,12281:0,12282:-1},stat=n=>values[n]??0;
const settle=async()=>{for(let i=0;i<50;i++)await Promise.resolve()},finish=()=>sources.at(-1).onended?.();
(async()=>{
 const names=JSON.parse(fs.readFileSync('docs/vo/pickup-names-v76.json','utf8'));assert.equal(names.nameOnly,true);for(const c of Object.values(names.clips)){assert(!/acquired|online|restored|warning|\d/i.test(c.text));assert(c.duration>0&&c.duration<1.5);}
 voice.start();await settle();assert(voice.state.ready&&voice.state.pickupReady);finish();voice.observe(stat);
 values[12280]=1;values[12281]=10;values[12282]=8;values[2]=0;voice.observe(stat);assert.equal(voice.state.current,'family-8','speak the actual unequipped Tesla pickup, not current Pulse');assert.equal(sources.at(-1).buffer.role,'pickup');assert.equal(sources.at(-1).startAt,names.clips['family-8'].start);assert.equal(voice.state.pickupPlayed,1);
 values[12280]=2;voice.observe(stat);assert.equal(voice.state.queued,0,'rapid repeated pickup names dedupe, including time zero');assert.equal(voice.state.pickupPlayed,1);finish();
 clock=1000;values[12280]=3;values[12281]=5;values[12282]=-1;voice.observe(stat);assert.equal(voice.state.current,'pickup-5');values[33]=1;voice.observe(stat);assert.equal(voice.state.current,'boss-approach');assert.equal(sources.at(-1).buffer.role,'mission');assert(stopped>0,'warnings preempt low priority pickup names');
 values[18]=1;voice.observe(stat);assert.equal(voice.state.current,null);assert.equal(voice.speaking,false);values[18]=0;values[33]=0;voice.observe(stat);clock=2000;values[12280]=4;values[12281]=2;voice.observe(stat);assert.equal(voice.state.current,'pickup-2');assert.equal(voice.toggle(),false);assert.equal(voice.state.current,null);assert.equal(voice.toggle(),true);
 console.log('PASS19 local name-only clips; actual picked family, no legacy verbose pickup, rapid dedupe, mission/warning priority, mute and voice controls');
})().catch(e=>{console.error(e);process.exit(1)});
