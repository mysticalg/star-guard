const {chromium}=require('C:/Users/drhoo/.codex/skills/develop-web-game/scripts/node_modules/playwright'),fs=require('fs'),assert=require('assert/strict'),P=require('../docs/pilot-store.js');
(async()=>{
const dir='../save64-browser';fs.mkdirSync(dir,{recursive:true});
const fixture=P.fresh();Object.assign(fixture,{xp:3300,credits:5432,parts:99,cores:7,selectedShip:2,armour:2,skin:1});fixture.armourOwned[2]=true;fixture.skins[1]=true;fixture.owned[2][0][1]=1;fixture.loadouts[2][0]=1;fixture.loadouts[2][3]=null;P.validate(fixture);assert.equal(P.level(fixture),6);
const business=p=>{const q={...p};delete q.revision;return q;};
const b=await chromium.launch(),c=await b.newContext({viewport:{width:1280,height:1000}}),errors=[];
await c.route('**/api/**',r=>r.fulfill({json:{scores:[]}}));
await c.addInitScript(({key,value})=>{if(localStorage.getItem(key)===null)localStorage.setItem(key,value)},{key:P.KEY,value:JSON.stringify(fixture)});
const p=await c.newPage();p.on('pageerror',e=>errors.push(e.message));await p.goto('http://127.0.0.1:8876/');
await p.waitForFunction(()=>window.PilotBridge&&!document.querySelector('#start').disabled,{},{timeout:60000});
assert.deepEqual(business(await p.evaluate(()=>PilotBridge.state)),business(fixture));
await p.reload();await p.waitForFunction(()=>window.PilotBridge&&!document.querySelector('#start').disabled,{},{timeout:60000});
assert.deepEqual(business(await p.evaluate(()=>PilotBridge.state)),business(fixture));
await p.clock.install();await p.locator('#start').click();await p.clock.pauseAt((await p.evaluate(()=>Date.now()))+700);await p.clock.runFor(600);
const engine=await p.evaluate(()=>({level:Module._web_stat(236),armour:Module._web_stat(237),ship:Module._web_stat(180),sections:Module._web_stat(161),paused:Module._web_stat(6)}));
assert.equal(engine.level,6);assert.equal(engine.armour,2);assert.equal(engine.ship,2);assert.equal(engine.sections,3);
await p.evaluate(()=>Module._web_command(112));await p.clock.runFor(100);assert.equal(await p.evaluate(()=>Module._web_stat(6)),1);await p.clock.runFor(600);
const exported=await p.evaluate(()=>PilotBridge.export());P.validate(JSON.parse(exported));const saved=JSON.parse(exported);const inventory=p=>{const v=business(p);delete v.run;delete v.runSerial;return v;};assert.deepEqual(inventory(saved),inventory(fixture));assert.equal(saved.runSerial,1);assert.equal(saved.run.mission,1);
fs.writeFileSync(dir+'/synthetic-pilot-export.json',exported);
await p.clock.resume();const q=await c.newPage();q.on('pageerror',e=>errors.push(e.message));await q.goto('http://127.0.0.1:8876/');await q.waitForFunction(()=>window.PilotBridge&&!document.querySelector('#start').disabled,{},{timeout:60000});assert.deepEqual(business(await q.evaluate(()=>PilotBridge.state)),business(saved));assert.equal(errors.length,0);
fs.writeFileSync(dir+'/results.json',JSON.stringify({syntheticFixture:true,realUserProfileAccessed:false,level:6,engine,menuReload:true,pauseExport:true,newPagePersistence:true,businessFieldsUnchanged:true,errors},null,2));console.log('PASS isolated synthetic LV6 pilot: menu reload, actual engine loadout/armour/chassis, pause/export and second-page persistence',engine);await b.close();
})().catch(e=>{console.error(e);process.exit(1)});



