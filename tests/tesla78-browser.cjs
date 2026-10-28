const {chromium}=require('C:/Users/drhoo/.codex/skills/develop-web-game/scripts/node_modules/playwright');
const fs=require('fs'),assert=require('assert/strict');
(async()=>{
 const live=!!process.env.LIVE,out=live?'../release78-live-combat':'../tesla78-browser',url=live?'https://mysticalg.github.io/star-guard/':'http://127.0.0.1:8876/';fs.mkdirSync(out,{recursive:true});
 const b=await chromium.launch({args:['--use-angle=d3d11','--enable-gpu','--ignore-gpu-blocklist','--mute-audio']}),reports=[];
 for(const ship of [0,2]){
  const p=await b.newPage({viewport:{width:1280,height:1000}}),errors=[],writes=[];p.on('pageerror',e=>errors.push(e.message));await p.route('**/api/**',r=>{if(r.request().method()!=='GET')writes.push(r.request().url());return r.fulfill({json:{scores:[]}})});
  await p.clock.install();await p.goto(url+'?release=78');await p.waitForFunction(()=>window.Module&&Module._web_stat&&!document.querySelector('#start').disabled,{},{timeout:90000});assert.equal(await p.locator('#build-version').textContent(),'Release 78');
  await p.locator('#ship').selectOption(''+ship,{force:true});await p.locator('#sector').selectOption('0',{force:true});await p.locator('#resolution').selectOption('1920',{force:true});await p.clock.pauseAt((await p.evaluate(()=>Date.now()))+100);await p.locator('#start').click({force:true});await p.clock.runFor(40);
  async function cmd(code){await p.evaluate(c=>Module._web_command(c),code);await p.clock.runFor(40)}
  await p.keyboard.press('F8');await p.clock.runFor(40);assert.equal(await p.evaluate(()=>Module._web_stat(11810)),1);const blockedNormalStartWrites=writes.splice(0);await cmd(57);assert.equal(await p.evaluate(()=>Module._web_stat(2)),8);
  const read=()=>p.evaluate(()=>{const s=Module._web_stat,actors=[];for(let i=0;i<64;i++)if(s(2000+i)&&s(2200+i)>24&&s(2200+i)<230)actors.push({type:s(2000+i)-1,x:s(2100+i),y:s(2200+i),hp:s(10680+i)});return{tick:s(0),mission:s(10),section:s(160),groups:s(50),rank:s(37),weapon:s(2),energy:s(108),heat:s(118),kills:s(233),links:s(89),critical:s(88),god:s(11810),dev:s(11710),version:s(11800),damage:s(11601),side:s(311),zoom:s(43),lives:s(20),pause:s(6),actors}});
  let s=await read();assert.equal(s.version,78);assert.equal(s.damage,185);assert.equal(s.rank,1);
  while(s.tick<662){await p.clock.runFor(Math.max(1,Math.min(150,(662-s.tick)*20)));s=await read()}
  assert.equal(s.energy,100);const begin=s;await p.evaluate(()=>Module._web_held(4));let midShot=false,maxLinks=0;
  for(let t=0;t<450;t++){
   s=await read();maxLinks=Math.max(maxLinks,s.links);const target=s.actors.sort((a,c)=>c.y-a.y)[0];if(target)await p.evaluate(([x,y])=>Module._web_pointer(x,y),[target.x,Math.max(90,Math.min(230,target.y+70))]);
   if(!midShot&&s.energy<65&&s.actors.length){await p.screenshot({path:out+'/ship'+ship+'-tesla-actual.png',clip:await p.locator('#canvas').boundingBox()});midShot=true}
   if(s.heat||!s.energy)break;await p.clock.runFor(20);
  }
  assert(s.heat>0&&s.energy===0);assert.equal(s.lives,begin.lives);assert(s.kills>begin.kills);assert(midShot);const end=s;
  await p.evaluate(()=>Module._web_held(0));await cmd(112);s=await read();assert.equal(s.pause,1);await p.clock.runFor(400);assert.deepEqual(await read(),s);await cmd(112);
  const bank={start:begin,end,ticks:end.tick-begin.tick,kills:end.kills-begin.kills,maxLinks};
  if(ship===0){await p.keyboard.press('F9');await p.clock.runFor(40);s=await read();assert.equal(s.rank,4);assert.equal(s.energy,100);const full=await p.evaluate(()=>({bombs:Module._web_stat(3),lives:Module._web_stat(20),dev:Module._web_stat(11710)}));assert.equal(full.bombs,5);assert.equal(full.lives,5);assert.equal(full.dev,1);
   await p.keyboard.press('F10');await p.clock.runFor(40);s=await read();assert.equal(s.mission,2);assert.equal(s.god,1);assert.equal(s.dev,1);await p.screenshot({path:out+'/secret-keys-mission-skip.png',clip:await p.locator('#canvas').boundingBox()});
  }
  assert.deepEqual(errors,[]);assert.deepEqual(writes,[]);reports.push({ship,bank,blockedNormalStartWrites,secretKeys:ship===0?'F8 persistent/F9fullpower/F10nextmission unranked verified':'F8 persistent',errors,writes});fs.writeFileSync(out+'/results.json',JSON.stringify(reports,null,2));console.log('PASS Tesla78 browser ship',ship,'bankticks',bank.ticks,'kills',bank.kills,'maxlinks',maxLinks);await p.close();
 }
 await b.close();
})().catch(e=>{console.error(e);process.exit(1)});
