const {chromium}=require('C:/Users/drhoo/.codex/skills/develop-web-game/scripts/node_modules/playwright'),fs=require('fs'),assert=require('assert/strict');
(async()=>{
 const base=process.env.BASE||'http://127.0.0.1:8882/',out=process.env.OUT||'../enemy88-browser';fs.mkdirSync(out,{recursive:true});
 const b=await chromium.launch({args:['--use-angle=d3d11','--enable-gpu','--ignore-gpu-blocklist','--mute-audio']}),reports=[];
 const cases=[{name:'vertical-hit',scene:'1.1',hit:true},{name:'vertical-dodge',scene:'1.1',dodge:true},{name:'side-dodge',scene:'1.2',dodge:true},{name:'shield-pickup',scene:'1.1',shield:true},{name:'god',scene:'1.1',god:true},{name:'cavern',scene:'2.3',dodge:true},{name:'zoom',scene:'6.11',god:true,finale:true},{name:'mobile-reduced',scene:'1.2',dodge:true,mobile:true,reduced:true}].filter(c=>!process.env.CASES||process.env.CASES.split(',').includes(c.name));
 for(const c of cases){
  const p=await b.newPage({viewport:c.mobile?{width:390,height:844}:{width:1280,height:1000}}),errors=[],writes=[],rows=[];p.on('pageerror',e=>errors.push(e.message));await p.route('**/api/**',r=>{if(r.request().method()!=='GET')writes.push(r.request().method());return r.fulfill({json:{scores:[]}})});await p.clock.install();
  const flags=2097152+(27<<22)+(c.shield?16384:0)+(c.finale?16:0);await p.goto(base+'?release=88&preview='+c.scene+'&at=1&visual='+flags+(c.finale?'&finale=2':''));
  await p.waitForFunction(()=>window.Module&&Module._web_stat&&!document.querySelector('#start').disabled,{},{timeout:90000});assert.equal(await p.locator('#build-version').textContent(),'Release 88');await p.locator('#resolution').selectOption('1920',{force:true});await p.clock.pauseAt((await p.evaluate(()=>Date.now()))+100);await p.locator('#start').click({force:true});await p.clock.runFor(40);
  if(c.reduced)await p.evaluate(()=>Module._web_motion(1));
  if(c.god){await p.keyboard.press('F8');await p.clock.runFor(40)}
  const read=()=>p.evaluate(()=>{const s=Module._web_stat,actors=[];for(let i=0;i<64;i++)if(s(2000+i)===28){const n=16000+i*12;actors.push({i,x:s(2100+i),y:s(2200+i),clock:s(n),phase:s(n+1),fired:s(n+2),hit:s(n+3),ax:s(n+4)/100,ay:s(n+5)/100,bx:s(n+6)/100,by:s(n+7)/100,half:s(n+8)/100,safe:s(n+9),length:s(n+10)/100,done:s(n+11)})}return{tick:s(0),version:s(11800),lives:s(20),hull:s(21),shield:s(313),pickup:s(140),god:s(11810),pause:s(6),side:s(244)/1000,zoom:s(43)/1000,x:s(29)/256,y:s(30)/256,actors}});
  async function point(x,y,s){const a=s.side*Math.PI/2,xx=(x*3-480)*(960-264*s.side)/960*s.zoom,yy=(y*3-420)*(696+264*s.side)/696*s.zoom;await p.evaluate(([x,y])=>Module._web_pointer(x,y),[(480+xx*Math.cos(a)-yy*Math.sin(a))/3,(420+xx*Math.sin(a)+yy*Math.cos(a))/3]);}
  let id=null,warning=null,active=null,done=null,pause=false,picked=false,dodged=false;
  const initial=await read();await point(160,230,initial);
  for(let n=0;n<700;n++){
   await p.clock.runFor(20);const s=await read();rows.push(s);assert.equal(s.version,88);picked ||=s.pickup>0;const a=id===null?s.actors.find(x=>x.phase===1):s.actors.find(x=>x.i===id);
   assert(s.actors.filter(x=>x.phase===1||x.phase===2).length<=1,'one warning/active Mega lane');
   if(a&&a.phase===1&&!warning){id=a.i;warning={...s,chosen:a};await p.screenshot({path:out+'/'+c.name+'-warning.png',clip:await p.locator('#canvas').boundingBox()});if(c.dodge){await point(a.safe,230,s);dodged=true}}
   if(a&&a.phase===2&&!active){active={...s,chosen:a};await p.screenshot({path:out+'/'+c.name+'-active.png',clip:await p.locator('#canvas').boundingBox()});}
   if(a&&a.phase===2&&!pause&&a.fired>=15){await p.keyboard.press('p');await p.clock.runFor(40);const before=await read();await p.clock.runFor(1000);const after=await read();assert.equal(after.pause,1);assert.equal(after.tick,before.tick);assert.equal(after.actors.find(x=>x.i===id).fired,before.actors.find(x=>x.i===id).fired);await p.keyboard.press('p');await p.clock.runFor(40);pause=true}
   if(a&&a.phase===3){done={...s,chosen:a};await p.screenshot({path:out+'/'+c.name+'-tail.png',clip:await p.locator('#canvas').boundingBox()});break;}
  }
  fs.writeFileSync(out+'/'+c.name+'-probe.json',JSON.stringify({initial,warning,active,done,pause,picked,dodged,rows,errors,writes},null,2));
  assert(warning&&active&&done&&pause,'actual warning/fire/tail/pause cycle');assert.equal(done.chosen.fired,150);assert.equal(done.chosen.clock,211);assert.equal(warning.chosen.half,22);
  if(c.finale)assert(active.zoom<.6,'actual boss camera pullback');
  if(c.hit){assert.equal(done.lives,initial.lives-1);assert.equal(done.chosen.hit,1)}else assert.equal(done.lives,initial.lives,'safe lane or real protection prevents loss');
  if(c.shield)assert(picked,'actual Shield pickup collected');if(c.god)assert.equal(done.god,1);if(c.dodge)assert(dodged);assert.deepEqual(errors,[]);assert.deepEqual(writes,[]);
  reports.push({case:c,warning,active,done,pause,picked,dodged,errors,writes});fs.writeFileSync(out+'/results.json',JSON.stringify(reports,null,2));console.log('PASS actual enemyMega88',c.name,'150fireTicks/lives',done.lives,'safe',warning.chosen.safe);await p.close();
 }
 await b.close();
})().catch(e=>{console.error(e);process.exit(1)});
