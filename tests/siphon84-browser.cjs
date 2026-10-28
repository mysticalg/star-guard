const {chromium}=require('C:/Users/drhoo/.codex/skills/develop-web-game/scripts/node_modules/playwright');
const fs=require('fs'),assert=require('assert/strict');
(async()=>{
 const out=process.env.OUT||'../siphon84-browser';fs.mkdirSync(out,{recursive:true});
 const b=await chromium.launch({args:['--use-angle=d3d11','--enable-gpu','--ignore-gpu-blocklist','--mute-audio']});
 const cases=[{name:'vertical',ship:0,scene:'1.1',flags:2097152},{name:'side',ship:1,scene:'1.2',flags:90177536},{name:'mobile-reduced',ship:2,scene:'1.2',flags:56623104,mobile:true},{name:'zoom',ship:1,scene:'6.11',flags:2097168,finale:3}];
 const selected=c=>(!process.env.CASES||process.env.CASES.split(',').includes(c.name))&&(!process.env.RANKS||process.env.RANKS.split(',').includes(''+c.rank));
 const reports=(process.env.CASES||process.env.RANKS)&&fs.existsSync(out+'/results.json')?JSON.parse(fs.readFileSync(out+'/results.json')).filter(x=>!selected(x.case)):[];
 for(const c of cases.flatMap(x=>[1,4].map(rank=>({...x,rank}))).filter(selected)){
  const p=await b.newPage({viewport:{width:c.mobile?390:1280,height:1000}}),errors=[],writes=[];
  p.on('pageerror',e=>errors.push(e.message));await p.route('**/api/**',r=>{if(r.request().method()!=='GET')writes.push(r.request().url());return r.fulfill({json:{scores:[]}})});
  await p.clock.install();await p.goto((process.env.BASE||'http://127.0.0.1:8878/')+'?release=84&preview='+c.scene+'&at=800&visual='+c.flags+(c.finale?'&finale='+c.finale:''));
  await p.waitForFunction(()=>window.PilotBridge&&window.Module?._web_stat&&!document.querySelector('#start').disabled,{},{timeout:90000});
  assert.equal(await p.locator('#build-version').textContent(),'Release 84');
  await p.evaluate(()=>{const state=PilotStore.fresh();state.credits=10000;state.parts=1000;state.cores=100;state.xp=9000;for(let ship=0;ship<3;ship++){state.owned[ship][3][3]=0;state.loadouts[ship][3]=3;}const r=PilotBridge.import(JSON.stringify(state));if(!r.ok)throw Error(r.message)});
  await p.locator('[data-flight=ship]').click();await p.locator('#ship').selectOption(''+c.ship,{force:true});await p.getByRole('combobox',{name:/^Starting slot 4 /}).selectOption('3',{force:true});
  await p.locator('#resolution').selectOption('1920',{force:true});await p.clock.pauseAt((await p.evaluate(()=>Date.now()))+100);await p.locator('#start').click({force:true});
  await p.keyboard.press('F8');await p.clock.runFor(40);await p.keyboard.press('4');await p.clock.runFor(200);if(c.mobile)await p.evaluate(()=>Module._web_motion(1));
  if(c.rank===4){await p.keyboard.press('F9');await p.clock.runFor(40);await p.keyboard.press('o');await p.clock.runFor(200);}
  const read=()=>p.evaluate(()=>{const s=Module._web_stat,actors=[];for(let i=0;i<64;i++)if(s(2000+i))actors.push({i,type:s(2000+i)-1,x:s(2100+i),y:s(2200+i),hp:s(2300+i),disabled:s(14400+i),paid:s(14500+i),frozen:s(14000+i)});return{tick:s(0),version:s(11800),weapon:s(2),variant:s(213),rank:s(37),bank:s(22),hull:s(21),heat:s(96),paused:s(6),god:s(11810),dev:s(11710),side:s(244)/1000,zoom:s(43)/1000,returned:s(14600),window:s(14601),disarms:s(14602),drawn:s(14605),freezes:s(14300),actors}});
  let state=await read();assert.equal(state.variant,3);assert.equal(state.rank,c.rank);assert((await p.locator('#hud-weapon').textContent()).includes('Siphon'));
  const aim=async(a,s)=>{const x=Math.max(12,Math.min(308,a.x)),y=Math.max(50,Math.min(245,a.y+65)),angle=s.side*Math.PI/2,xx=(x*3-480)*(960-264*s.side)/960*s.zoom,yy=(y*3-420)*(696+264*s.side)/696*s.zoom;await p.evaluate(([x,y])=>Module._web_pointer(x,y),[(480+xx*Math.cos(angle)-yy*Math.sin(angle))/3,(420+xx*Math.sin(angle)+yy*Math.cos(angle))/3]);};
  const idle=state;let active=null;await p.evaluate(()=>Module._web_held(4));
  for(let n=0;n<450;n++){state=await read();assert(state.window<=8&&state.drawn<=(c.mobile?12:64)&&state.bank<=100);if(state.returned>0&&state.drawn>0&&state.actors.some(a=>a.disabled)){active=state;break;}const target=state.actors.filter(a=>a.type!==4&&a.y>30&&a.y<190&&a.x>12&&a.x<308).sort((a,b)=>b.y-a.y)[0];if(target)await aim(target,state);await p.clock.runFor(20);}
  fs.writeFileSync(out+'/'+c.name+'-rank'+c.rank+'-probe.json',JSON.stringify({case:c,idle,state,active},null,2));assert(active,'actual credited return and disabled target');assert.equal(active.freezes,0);assert.equal(active.hull,idle.hull);
  await p.screenshot({path:out+'/'+c.name+'-rank'+c.rank+'-return.png',clip:await p.locator('#canvas').boundingBox()});await p.evaluate(()=>Module._web_held(0));await p.clock.runFor(40);const released=await read(),amount=released.returned;
  await p.clock.runFor(60);assert.equal((await read()).returned,amount,'release cannot earn passive energy');
  await p.evaluate(()=>Module._web_command(112));for(let n=0;n<80;n++){await p.clock.runFor(1);if((await read()).paused)break;}
  const paused=await read();assert(paused.paused);await p.clock.runFor(500);assert.deepEqual(await read(),paused);
  await p.evaluate(()=>Module._web_command(112));await p.clock.runFor(40);await p.keyboard.press('1');await p.clock.runFor(600);const switched=await read();assert.equal(switched.weapon,0);assert.equal(switched.returned,amount);assert.equal(switched.drawn,0);assert.equal(switched.freezes,0);
  assert.deepEqual(errors,[]);assert.deepEqual(writes,[]);reports.push({case:c,idle,active,released,paused,switched,errors,writes});fs.writeFileSync(out+'/results.json',JSON.stringify(reports,null,2));await p.close();console.log('PASS actual Siphon return/disarm/release/pause/switch',c.name,c.rank);
 }
 await b.close();
})().catch(e=>{console.error(e);process.exit(1)});
