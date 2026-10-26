const{chromium}=require('C:/Users/drhoo/.codex/skills/develop-web-game/scripts/node_modules/playwright'),fs=require('fs'),assert=require('assert/strict');
(async()=>{const out='../opening65-native-smart';fs.mkdirSync(out,{recursive:true});const b=await chromium.launch({args:['--use-angle=d3d11','--enable-gpu','--ignore-gpu-blocklist']}),p=await b.newPage({viewport:{width:1280,height:1000}}),errors=[];p.on('pageerror',e=>errors.push(e.message));await p.route('**/api/**',r=>r.fulfill({json:{scores:[]}}));await p.clock.install();await p.goto('http://127.0.0.1:8876/');await p.waitForFunction(()=>window.Module&&Module._web_stat&&window.PilotBridge&&!document.querySelector('#start').disabled,{},{timeout:60000});await p.evaluate(()=>document.querySelector('#resolution').add(new Option('QA960','960')));await p.locator('#resolution').selectOption('960',{force:true});await p.locator('#ship').selectOption('2',{force:true});await p.locator('#start').click();await p.clock.pauseAt((await p.evaluate(()=>Date.now()))+100);await p.evaluate(()=>Module._web_held(4));const rows=[];let continues=0,lastCapture=-1;
for(let i=0;i<1600;i++){const s=await p.evaluate(()=>{const s=Module._web_stat;return{tick:s(0),mode:s(1),active:s(10901),clock:s(49),env:s(164),weather:s(10400),stage:s(10401),strikes:s(10412),tesla:s(10907),lives:s(20),armour:s(21),waves:s(50),npcs:s(23),mini:s(24),kills:s(233),continues:s(74),audio:WeatherAudio.state()}});if(s.mode===2){rows.push(s);break;}if(s.mode===3){await p.evaluate(()=>Module._web_command(13));continues++;await p.clock.runFor(100);continue;}
const marker=s.clock<1000?0:s.clock<1500?1:s.clock<4500?2:3;if(marker!==lastCapture&&s.clock>100){lastCapture=marker;await p.screenshot({path:out+'/phase-'+marker+'.png',clip:await p.locator('#canvas').boundingBox()});rows.push(s);fs.writeFileSync(out+'/progress.json',JSON.stringify({continues,rows,last:s,errors},null,2));}
if(s.mini){rows.push(s);break;}
await p.evaluate(()=>{
const s=Module._web_stat,px=s(29)/256,py=s(30)/256,bullets=[];for(let n=0;n<128;n++)if(s(3000+n))bullets.push({x:s(3200+n)/256,y:s(3400+n)/256,vx:s(3600+n)/256,vy:s(3800+n)/256});
let best=-1e9,bx=px,by=py,near=0;for(const b of bullets)if(Math.hypot(b.x-px,b.y-py)<70)near++;
for(const dx of[-24,-12,0,12,24])for(const dy of[-12,0,12]){const x=Math.max(20,Math.min(300,px+dx)),y=Math.max(180,Math.min(238,py+dy));let danger=0,min=1000;
for(const b of bullets)for(const k of[0,3,6,10]){const d=Math.hypot(x-b.x-b.vx*k,y-b.y-b.vy*k);min=Math.min(min,d);danger+=250/(d*d+16);}
for(let n=0;n<64;n++)if(s(2000+n)){const type=s(2000+n)-1,ex=s(2100+n),ey=s(2200+n),d=Math.hypot(ex-x,ey-y);danger+=500/(d*d+20);if(d<22)danger+=20;}
const score=Math.min(min,70)-danger*35-Math.hypot(dx,dy)*.08-Math.abs(x-160)*.018;if(score>best){best=score;bx=x;by=y;}}
Module._web_pointer(bx,by);Module._web_held(4);
if(s(21)<65&&!s(189))Module._web_command(104);
else if(s(150)>=5000)Module._web_command(120);
else if(near>12&&s(3)>0&&!s(4))Module._web_command(98);
else if(near>8&&s(22)>=35&&!s(497))Module._web_command(100);
else if(near>10&&s(142)>0)Module._web_command(101);
else if(near>14&&s(98)>0)Module._web_command(99);
});await p.clock.runFor(100);
if(i%20===0){rows.push(s);fs.writeFileSync(out+'/progress.json',JSON.stringify({continues,rows,last:s,errors},null,2));}}
const last=await p.evaluate(()=>({clock:Module._web_stat(49),active:Module._web_stat(10901),mini:Module._web_stat(24),mode:Module._web_stat(1)}));fs.writeFileSync(out+'/results.json',JSON.stringify({fixture:'Continuous normal launch, stock Bastion with actual Pulse/pointer/H/D/Nova/E/chaff/Phoenix/continue controls only, no previews/skips/God/HP changes; native1920 render (production minimum), accelerated browser clock, no hardware-FPS or no-death claim.',continues,rows,last,errors},null,2));assert(last.active>=4500);assert.equal(errors.length,0);console.log('PASS continuous native first90s+',last,'continues',continues);await b.close()})().catch(e=>{console.error(e);process.exit(1)});
