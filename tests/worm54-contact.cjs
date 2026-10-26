const {chromium}=require('C:/Users/drhoo/.codex/skills/develop-web-game/scripts/node_modules/playwright');
const fs=require('fs'),assert=require('assert/strict');
(async()=>{
 const dir=process.env.OUTPUT_DIR||'../worm54-contact';fs.mkdirSync(dir,{recursive:true});
 const browser=await chromium.launch(),page=await browser.newPage({viewport:{width:1280,height:1000}});
 await page.route('**/api/**',r=>r.fulfill({json:{scores:[]}}));
 await page.goto((process.env.GAME_URL||'http://127.0.0.1:8876/')+'?preview=5.3&at=1500&finale=1');
 await page.waitForFunction(()=>window.PilotBridge&&!document.querySelector('#start').disabled);
 await page.locator('#resolution').selectOption('1920');await page.locator('#start').click();
 await page.evaluate(()=>{window.safeEntry=setInterval(()=>{const s=Module._web_stat,z=s(43)/1000,x=Math.max(s(5141)+15,Math.min(s(5140)-15,105));Module._web_pointer(160+(x-160)*z,140+99*z);},20)});
 await page.waitForFunction(()=>Module._web_stat(313)===0&&Module._web_stat(20)===3);
 const before=await page.evaluate(()=>{
  clearInterval(safeEntry);
  const s=Module._web_stat;
  for(let i=5;i<168;i++)if(s(6400+i)){
   const x=s(6000+i)/256,y=s(6200+i)/256,z=s(43)/1000;
   Module._web_pointer(160+(x-160)*z,140+(y-140)*z);
   return {tick:s(0),armour:s(21),lives:s(20),shield:s(313),segment:i,x,y};
  }
  throw Error('No exposed body segment');
 });
 await page.waitForFunction(()=>Module._web_stat(492)===6&&Module._web_stat(20)===2);
 await page.evaluate(()=>Module._web_command(112));await page.waitForFunction(()=>Module._web_stat(6));
 await page.addStyleTag({content:'#game-message{visibility:hidden!important}'});
 const after=await page.evaluate(()=>({tick:Module._web_stat(0),armour:Module._web_stat(21),lives:Module._web_stat(20),shield:Module._web_stat(313),kind:Module._web_stat(492),fatal:Module._web_stat(496)}));
 assert.equal(before.armour,100);assert.equal(before.shield,0);assert.equal(after.fatal,1);
 await page.locator('#canvas').screenshot({path:dir+'/body-contact.png'});
 fs.writeFileSync(dir+'/result.json',JSON.stringify({fixture:'Unranked encounter, normal entry grace expired, pointer input into an exposed segment',before,after},null,2));
 console.log({before,after});await browser.close();
})();
