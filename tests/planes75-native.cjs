const {chromium}=require('C:/Users/drhoo/.codex/skills/develop-web-game/scripts/node_modules/playwright'),fs=require('fs'),assert=require('assert/strict');
(async()=>{
 const out=process.env.OUT||(process.env.LIVE?'../release75-live-planes':'../planes75-native'),base=process.env.LIVE?'https://mysticalg.github.io/star-guard/':'http://127.0.0.1:8876/';fs.mkdirSync(out,{recursive:true});
 const browser=await chromium.launch({args:['--use-angle=d3d11','--enable-gpu','--ignore-gpu-blocklist','--mute-audio']}),rows=[];
 const cases=process.env.SELECTED?[['fighter',0,6,1,200,0],['cave-blend',0,5,3,200,512],['organic-loop',12,6,1,350,0]]:[['fighter',0,6,1,200,0],['flank',1,6,1,150,0],['s-dive',2,6,1,170,0],['heavy-patrol',3,6,1,350,0],['strafe',5,6,1,120,0],['escort-weave',6,6,1,140,0],['minelayer',7,6,1,110,0],['organic-loop',12,6,1,350,0],['phase-transit',18,6,1,350,0],['cave-blend',0,5,3,200,512]];
 for(const [name,type,mission,section,tick,extra]of cases){let baseline;
  for(const disabled of [1,0]){
   const p=await browser.newPage({viewport:{width:1280,height:1000}}),errors=[];p.on('pageerror',e=>errors.push(e.message));await p.route('**/api/**',r=>r.fulfill({json:{scores:[]}}));await p.clock.install();
   await p.goto(base+`?release=75&preview=${mission}.${section}&at=800&visual=${2097152+type*4194304+extra+(disabled?1048576:0)}`);
   await p.waitForFunction(()=>window.Module&&Module._web_stat&&!document.querySelector('#start').disabled,{},{timeout:90000});assert.equal(await p.locator('#build-version').textContent(),'Release 75');await p.locator('#resolution').selectOption('1920',{force:true});await p.clock.pauseAt((await p.evaluate(()=>Date.now()))+100);await p.locator('#start').click({force:true});await p.keyboard.press('F8');
   await p.clock.runFor((tick-10)*20);while(await p.evaluate(()=>Module._web_stat(0))<tick)await p.clock.runFor(5);
   await p.addStyleTag({content:'#game-message{display:none!important}'});
   const sample=()=>p.evaluate(()=>{const s=Module._web_stat;return{tick:s(0),ghosts:s(12100),projectiles:s(12101),debris:s(12102),side:s(244),zoom:s(43),actors:Array.from({length:64},(_,i)=>[s(2000+i),s(2100+i),s(2200+i),s(2300+i),s(2700+i)]).filter(a=>a[0]),god:s(11810),quality:s(11005)}});
   const a=await sample();assert.equal(a.tick,tick);if(disabled)baseline=a;else {assert.deepEqual(a.actors,baseline.actors);assert.ok(a.ghosts>0&&a.ghosts<=160,JSON.stringify({name,a}));}assert.equal(a.god,1);
   const stem=name+(disabled?'-off':'-on');await p.screenshot({path:out+'/'+stem+'-a.png',clip:await p.locator('#canvas').boundingBox()});await p.clock.runFor(180);while(await p.evaluate(()=>Module._web_stat(0))<tick+10)await p.clock.runFor(5);const b=await sample();assert.equal(b.tick,tick+10);await p.screenshot({path:out+'/'+stem+'-b.png',clip:await p.locator('#canvas').boundingBox()});
   if(!disabled&&name==='fighter'){
    await p.keyboard.press('p');await p.clock.runFor(40);const paused=await sample();await p.clock.runFor(700);assert.deepEqual(await sample(),paused);await p.keyboard.press('p');await p.clock.runFor(40);
    await p.evaluate(()=>Module._web_motion(1));await p.clock.runFor(40);assert.equal((await sample()).ghosts,0);await p.screenshot({path:out+'/fighter-reduced-motion.png',clip:await p.locator('#canvas').boundingBox()});
    await p.evaluate(()=>Module._web_motion(0));await p.clock.runFor(160);assert.ok((await sample()).ghosts>0);
    for(let n=0;n<3;n++){await p.keyboard.press('F4');await p.clock.runFor(160);const q=await sample();if(q.quality===120)assert.equal(q.ghosts,0);else assert.ok(q.ghosts<=(q.quality===240?160:64));}
    await p.evaluate(()=>Module._web_held(4));await p.clock.runFor(600);const firing=await sample();assert.ok(firing.projectiles>0&&firing.projectiles<=160);await p.screenshot({path:out+'/fighter-projectile-exposure.png',clip:await p.locator('#canvas').boundingBox()});
    await p.keyboard.press('F7');await p.clock.runFor(40);assert.equal((await sample()).ghosts,0);
   }
   assert.deepEqual(errors,[]);rows.push({name,disabled,a,b,errors});fs.writeFileSync(out+'/results.json',JSON.stringify(rows,null,2));console.log('PASS',stem,a.ghosts,a.projectiles);await p.close();
  }
 }
 await browser.close();console.log('PASS paired fixed-tick painted hull exposure and continuous routes',rows.length);
})().catch(e=>{console.error(e);process.exit(1)});
