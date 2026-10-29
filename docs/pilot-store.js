/* Local pilot economy. Pure transactions are shared by the browser and tests. */
(function(root){
 const KEY='star-guard-pilot-v3',PREVIOUS_KEY='star-guard-pilot-v2',LEGACY_KEY='star-guard-pilot-v1',CAP=10;
 const premium84={3:{name:'Siphon',price:2400,level:6},5:{name:'Mega Death Laser',price:6400,level:10}};
 const validVariant84=(w,v)=>integer(v,3)&&(v<3||!!premium84[w]);
 const variantCount84=w=>premium84[w]?4:3;
 const clone=x=>JSON.parse(JSON.stringify(x)),integer=(x,max)=>Number.isInteger(x)&&x>=0&&x<=max;
 function fresh(){return {version:3,revision:0,credits:300,parts:0,cores:0,xp:0,selectedShip:0,owned:Array.from({length:3},()=>Array.from({length:9},()=>[0,-1,-1,-1])),loadouts:Array.from({length:3},()=>Array(9).fill(0)),armourOwned:[true,false,false,false],armour:0,skins:[true,false,false],skin:0,rng:0x51a9c3,runSerial:0,rewarded:[],run:null};}
 function level(p){let xp=p.xp,n=1;while(n<CAP&&xp>=n*200){xp-=n*200;n++;}return n;}
 function validate(p){
  if(!p||![1,2,3].includes(p.version)||!integer(p.revision,1e9)||!integer(p.credits,1e7)||!integer(p.parts,1e6)||!integer(p.cores,1e4)||!integer(p.xp,1e8)||!integer(p.selectedShip,2)||!integer(p.rng,0xffffffff)||!integer(p.runSerial,1e9))throw Error('Invalid pilot fields');
  if(!Array.isArray(p.owned)||p.owned.length!==3||!Array.isArray(p.loadouts)||p.loadouts.length!==3)throw Error('Invalid inventory');
  for(let s=0;s<3;s++){if(p.owned[s].length!==9||p.loadouts[s].length!==9)throw Error('Invalid slot count');for(let w=0;w<9;w++){if(!Array.isArray(p.owned[s][w])||p.owned[s][w].length!==(p.version===1?3:4)||p.owned[s][w].some(q=>!Number.isInteger(q)||q< -1||q>3))throw Error('Invalid gear quality');if(p.version>=2&&p.owned[s][w][3]>=0&&(!premium84[w]||(p.version===2&&w!==3)))throw Error('Unsupported premium item');const v=p.loadouts[s][w];if(v!==null&&(!validVariant84(w,v)||(p.version===1&&v===3)||p.owned[s][w][v]<0))throw Error('Equipped item is not owned');}if(p.owned[s][0][0]<0||p.loadouts[s][0]===null)throw Error('Primary loaner missing');}
  if(!Array.isArray(p.armourOwned)||p.armourOwned.length!==4||p.armourOwned.some(x=>typeof x!=='boolean')||!p.armourOwned[0]||!integer(p.armour,3)||!p.armourOwned[p.armour])throw Error('Invalid armour');
  if(!Array.isArray(p.skins)||p.skins.length!==3||p.skins.some(x=>typeof x!=='boolean')||!p.skins[0]||!integer(p.skin,2)||!p.skins[p.skin])throw Error('Invalid cosmetic');
  if(!Array.isArray(p.rewarded)||p.rewarded.length>256||p.rewarded.some(x=>typeof x!=='string'||x.length>100))throw Error('Invalid reward ledger');
  if(p.run!==null){const r=p.run;if(!integer(r.id,1e9)||!integer(r.mission,6)||r.mission<1||!integer(r.lastKills,1e7)||!integer(r.lastParts,1e7)||!integer(r.killsPaid,200)||!integer(r.partsPaid,200))throw Error('Invalid sortie counters');}
  if(p.version===1)for(const ship of p.owned)for(const slot of ship)slot.push(-1);if(p.version<3)p.version=3;return p;
 }
 const weaponPrice=(w,v)=>v===3?(premium84[w]?.price??Infinity):v===0?60+w*8:v===1?120+w*10:180+w*12;
 const armourPrices=[0,220,400,1800],armourStats=[{name:'Standard plating',armour:100,energy:100},{name:'Laminate plating',armour:110,energy:103},{name:'Bulwark plating',armour:125,energy:106},{name:'Adaptive plating',armour:115,energy:100}];
 function armourSale(id){return Math.floor(Math.min(armourPrices[id],[0,120,240,400][id])*.4);}
 function recipe(p,s,w,v){const q=p.owned[s][w][v];if(q<0&&v===3)return {credits:premium84[w].price,parts:w===5?160:80,cores:w===5?5:2,level:premium84[w].level,quality:0};if(q<0)return {credits:30+v*50,parts:6+v*3,cores:0,level:1,quality:0};if(q===3)return null;return [{credits:100,parts:10,cores:0,level:3,quality:1},{credits:300,parts:24,cores:1,level:10,quality:2},{credits:700,parts:45,cores:3,level:10,quality:3}][q];}
 function random(p,max){p.rng=(Math.imul(p.rng,1664525)+1013904223)>>>0;return p.rng%max;}
 function spend(p,c){if(p.credits<c.credits||p.parts<c.parts||p.cores<c.cores)throw Error('Insufficient credits or materials');p.credits-=c.credits;p.parts-=c.parts;p.cores-=c.cores;}
 function apply(p,a){
  const s=a.ship??p.selectedShip,w=a.slot,v=a.variant;let message='';
  if(['buy','sell','craft','equip'].includes(a.type)&&(!integer(s,2)||!integer(w,8)||!validVariant84(w,v)))throw Error('Incompatible weapon slot');
  if(a.type==='buy'){if(v===3&&level(p)<premium84[w].level)throw Error('Requires pilot level '+premium84[w].level);if(p.owned[s][w][v]>=0)throw Error('Already owned');spend(p,{credits:weaponPrice(w,v),parts:0,cores:0});p.owned[s][w][v]=0;message='Weapon purchased; choose its family slot to equip.';}
  else if(a.type==='sell'){const q=p.owned[s][w][v];if(q<0)throw Error('Item not owned');if(w===0&&v===0)throw Error('The basic Pulse loaner cannot be sold');const value=Math.floor(weaponPrice(w,v)*.4)+q*30;p.credits+=value;p.owned[s][w][v]=-1;if(p.loadouts[s][w]===v){p.loadouts[s][w]=w===0?0:null;message=w===0?'Sold; the primary reverted to its basic loaner.':'Sold; slot '+(w+1)+' is now EMPTY.';}else message='Sold for '+value+' credits.';}
  else if(a.type==='equip'){if(p.owned[s][w][v]<0)throw Error('Item not owned');p.loadouts[s][w]=v;message='Compatible weapon equipped.';}
  else if(a.type==='craft'){const r=recipe(p,s,w,v);if(!r)throw Error('Already maximum quality');if(level(p)<r.level)throw Error('Requires pilot level '+r.level);spend(p,r);p.owned[s][w][v]=r.quality;message='Forged quality '+r.quality+'; equip it in slot '+(w+1)+'.';}
  else if(a.type==='buyArmour'||a.type==='forgeArmour'){const id=a.id;if(!integer(id,3)||id===0||p.armourOwned[id])throw Error('Invalid or already owned armour');const required=a.type==='forgeArmour'&&id===3?10:id===3?10:id===2?3:1;if(level(p)<required)throw Error('Requires pilot level '+required);spend(p,a.type==='buyArmour'?{credits:armourPrices[id],parts:0,cores:0}:{credits:[0,120,240,400][id],parts:[0,16,30,40][id],cores:id===3?1:0});p.armourOwned[id]=true;message='Armour acquired; equip it when ready.';}
  else if(a.type==='equipArmour'){if(!integer(a.id,3)||!p.armourOwned[a.id])throw Error('Armour not owned');p.armour=a.id;message='Armour equipped.';}
  else if(a.type==='sellArmour'){if(!integer(a.id,3)||a.id===0||!p.armourOwned[a.id])throw Error('Armour cannot be sold');p.credits+=armourSale(a.id);p.armourOwned[a.id]=false;if(p.armour===a.id)p.armour=0;message='Armour sold; standard plating is fitted if needed.';}
  else if(a.type==='ship'){if(!integer(a.id,2))throw Error('Invalid ship');p.selectedShip=a.id;}
  else if(a.type==='skin'){if(!integer(a.id,2)||!p.skins[a.id])throw Error('Skin not unlocked');p.skin=a.id;message='Cosmetic equipped; stats are unchanged.';}
  else if(a.type==='begin'){if(!integer(a.mission,6)||a.mission<1)throw Error('Invalid mission');p.runSerial++;p.run={id:p.runSerial,mission:a.mission,lastKills:a.kills??0,lastParts:a.parts??0,killsPaid:0,partsPaid:0};}
  else if(a.type==='progress'){if(!p.run)throw Error('No active sortie');if(!integer(a.kills,1e7)||!integer(a.parts,1e7)||!integer(a.mission,6)||a.mission<1)throw Error('Invalid progress');const r=p.run;if(a.mission!==r.mission){r.mission=a.mission;r.killsPaid=r.partsPaid=0;}const kills=Math.min(Math.max(0,a.kills-r.lastKills),200-r.killsPaid),parts=Math.min(Math.max(0,a.parts-r.lastParts),200-r.partsPaid);r.lastKills=a.kills;r.lastParts=a.parts;r.killsPaid+=kills;r.partsPaid+=parts;p.xp+=kills*4;p.credits+=kills*2;p.parts+=parts;}
  else if(a.type==='collect'){if(!integer(s,2)||!integer(w,8)||!integer(v,2))throw Error('Invalid collected variant');if(p.owned[s][w][v]<0)p.owned[s][w][v]=0;}
  else if(a.type==='encounter'){
   if(!p.run||typeof a.key!=='string'||a.key.length>80||!['mini','giant','complete'].includes(a.kind)||!integer(a.mission,6)||a.mission<1)throw Error('Invalid encounter');const key=p.run.id+':'+a.key;if(p.rewarded.includes(key))return 'Reward already recorded.';p.rewarded.push(key);if(p.rewarded.length>256)p.rewarded.shift();
   const giant=a.kind==='giant';p.credits+=a.kind==='complete'?100:giant?120+a.mission*20:40+a.mission*8;p.parts+=a.kind==='complete'?5:giant?8:3;p.xp+=a.kind==='complete'?80:giant?100:30;message='Encounter salvage secured.';
   if(a.kind!=='complete'){
    const roll=random(p,10000)*(giant?1:2),lv=level(p),ship=p.selectedShip;
    if(roll<1000){p.parts+=4;message+=' Bonus component cache: +4 parts (chance drop).';}
    else if(roll<1600&&lv>=3){const slot=random(p,9),variant=1+random(p,2),quality=lv===CAP?2:1;if(p.owned[ship][slot][variant]<quality){p.owned[ship][slot][variant]=quality;message+=' Rare weapon: slot '+(slot+1)+' variant '+(variant+1)+' quality '+quality+'.';}else{p.parts+=6;message+=' Duplicate rare weapon salvaged into 6 parts.';}}
    else if(roll<2000&&lv>=6){const id=lv===CAP?3:2;if(!p.armourOwned[id]){p.armourOwned[id]=true;message+=' Rare armour unlocked.';}else{p.parts+=8;message+=' Duplicate armour salvaged into 8 parts.';}}
    else if(roll<2400&&lv===CAP){p.cores++;message+=' Rare reactor core +1.';}
    else if(roll<2600&&lv===CAP){const id=1+random(p,2);if(!p.skins[id]){p.skins[id]=true;message+=' Cosmetic ship skin unlocked (no stat bonus).';}else{p.parts+=5;message+=' Duplicate cosmetic salvaged into 5 parts.';}}
   }
  }
  else throw Error('Unknown pilot action');
  p.credits=Math.min(p.credits,1e7);p.parts=Math.min(p.parts,1e6);p.cores=Math.min(p.cores,1e4);p.xp=Math.min(p.xp,1e8);return message;
 }
 function newSeed(){if(globalThis.crypto?.getRandomValues){const n=new Uint32Array(1);globalThis.crypto.getRandomValues(n);return n[0];}return (Date.now()^Math.floor(Math.random()*0xffffffff))>>>0;}
 function create(storage){let state=fresh(),damaged=null,migrated=false;const current=storage.getItem(KEY),previous=current===null?storage.getItem(PREVIOUS_KEY):null,legacy=current===null&&previous===null?storage.getItem(LEGACY_KEY):null,raw=current??previous??legacy,source=previous!==null?'v2':'v1';if(raw===null)state.rng=newSeed();if(raw!==null){try{state=validate(JSON.parse(raw));if(current===null){storage.setItem(KEY+'-migration-'+source,raw);storage.setItem(KEY,JSON.stringify(state));migrated=true;}}catch{damaged=raw;}}
  function backup(){const old=storage.getItem(KEY);if(old===null)return;let list=[];try{list=JSON.parse(storage.getItem(KEY+'-backups')||'[]');if(!Array.isArray(list))list=[];}catch{}list.push({at:new Date().toISOString(),data:old});storage.setItem(KEY+'-backups',JSON.stringify(list.slice(-3)));}
  function commit(next,makeBackup){validate(next);next.revision=state.revision+1;if(makeBackup)backup();storage.setItem(KEY,JSON.stringify(next));state=next;}
  return {get state(){return clone(state);},get damaged(){return damaged;},get migrated(){return migrated;},get level(){return level(state);},
   act(action){if(damaged!==null)return {ok:false,message:'Pilot save needs recovery; original data was retained. Export it or import a valid save.'};try{const next=clone(state),message=apply(next,action);commit(next,!['progress','collect'].includes(action.type));return {ok:true,message};}catch(e){return {ok:false,message:e.message};}},
   export(){return damaged===null?JSON.stringify(state,null,2):damaged;},
   import(text){try{const next=validate(JSON.parse(text));backup();storage.setItem(KEY,JSON.stringify(next));state=clone(next);damaged=null;return {ok:true,message:'Pilot imported. Previous save retained in backups.'};}catch(e){return {ok:false,message:'Import rejected: '+e.message};}},
   resetWithBackup(){try{backup();const next=fresh();next.rng=newSeed();storage.setItem(KEY,JSON.stringify(next));state=next;damaged=null;return {ok:true,message:'Fresh pilot created; previous raw save retained in backups.'};}catch(e){return {ok:false,message:e.message};}}
  };
 }
 const api={KEY,PREVIOUS_KEY,LEGACY_KEY,CAP,variantCount:variantCount84,premium:premium84,fresh,validate,level,weaponPrice,armourPrices,armourStats,armourSale,recipe,random,apply,create};if(typeof module==='object'&&module.exports)module.exports=api;else root.PilotStore=api;
})(typeof window==='object'?window:globalThis);
