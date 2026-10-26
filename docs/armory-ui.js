/* Menu-only presentation; transactions remain in PilotBridge/PilotStore. */
window.Armory=(()=>{
 const root='images/armory-v46/',ships=['VECTOR','JADE','BASTION'],roles=['Balanced','Energy','Armour'];
 let selected=0,dispose=()=>{},enginePromise,hangar;
 const reduced=matchMedia('(prefers-reduced-motion: reduce)');
 function image(name,alt=''){const i=new Image();i.src=root+name+'.webp';i.alt=alt;i.decoding='async';return i;}
 function btn(text,action){const b=document.createElement('button');b.type='button';b.textContent=text;b.onclick=action;return b;}
 variantIcon=m=>'<span class="painted-weapon" style="--gear-colour:'+m.colour+'"><img src="'+root+'weapon-'+m.slot+'.webp" alt=""><small>'+['F','FL','B'][m.variant]+'</small></span>';
 function refreshHangar(){if(!hangar)return;const s=+document.querySelector('#ship').value;[...hangar.children].forEach((b,i)=>{b.setAttribute('aria-pressed',String(i===s));b.querySelector('.ship-selected').textContent=i===s?'SELECTED':'SELECT SHIP';});}
 function mountHangar(){
  hangar=document.createElement('section');hangar.id='hangar';hangar.setAttribute('aria-label','Choose your ship');
  for(let s=0;s<3;s++){const b=btn('',()=>{const select=document.querySelector('#ship');select.value=s;select.dispatchEvent(new Event('change'));refreshHangar();});b.className='hangar-ship';b.setAttribute('aria-label',ships[s]+' / '+roles[s]);const art=document.createElement('div');art.className='hangar-art';art.style.backgroundImage='url('+root+'ship-'+s+'.webp)';art.append(image('ship-'+s));b.append(art);const title=document.createElement('h2');title.textContent=ships[s];const role=document.createElement('p');role.textContent=roles[s]+' fighter';const stats=document.createElement('div');stats.className='ship-specs';stats.innerHTML='<span>ARMOUR <b>'+[100,80,130][s]+'%</b></span><meter min="0" max="130" value="'+[100,80,130][s]+'"></meter><span>ENDURANCE <b>'+[100,120,85][s]+'%</b></span><meter min="0" max="130" value="'+[100,120,85][s]+'"></meter>';const chosen=document.createElement('strong');chosen.className='ship-selected';b.append(title,role,stats,chosen);hangar.append(b);}
  document.querySelector('#pilot-summary').after(hangar);
  preflightNav.querySelector('[data-flight=intro]').textContent='HANGAR';preflightNav.querySelector('[data-flight=ship]').textContent='LOADOUT';
  document.querySelector('#ship').parentElement.classList.add('legacy-ship-control');
  document.querySelector('#ship').addEventListener('change',refreshHangar);refreshHangar();
  drawLoadout(preflightLoadout,false);document.querySelector('#start').addEventListener('click',()=>dispose());show('intro');
 }
 function show(tab){dispose();if(hangar)hangar.hidden=tab!=='intro';document.querySelector('#cover').dataset.view=tab;refreshHangar();}
 function engine(){return enginePromise??=new Promise((resolve,reject)=>{const script=document.createElement('script');script.src='armory-range.js?v=50';script.onload=()=>StarGuardRange({locateFile:p=>p+'?v=50'}).then(resolve,reject);script.onerror=reject;document.head.append(script);});}
 async function preview(host,s,w,v,q,armour){
  dispose();let dead=false,raf=0,visible=false,playing=!reduced.matches,last=0,acc=0,ticks=0,range;
  const canvas=document.createElement('canvas');canvas.width=640;canvas.height=420;canvas.setAttribute('aria-label',WeaponVariants[s][w][v].name+' firing demonstration');host.append(canvas);
  const controls=document.createElement('div');controls.className='range-controls';const toggle=btn(playing?'PAUSE PREVIEW':'PLAY PREVIEW',()=>{playing=!playing;toggle.textContent=playing?'PAUSE PREVIEW':'PLAY PREVIEW';schedule();});const status=document.createElement('span');status.textContent='Loading firing range…';controls.append(toggle,status);host.append(controls);
  const note=document.createElement('p');note.textContent='Rank 1 • '+(q<0?'Standard quality sample':['Standard','Tuned','Rare','Exotic'][q]+' quality')+' • durable targets • silent 6-second loop';host.append(note);
  const ctx=canvas.getContext('2d'),ship=image('player-'+s),deck=image('range-deck'),target=image('range-target');for(const art of [ship,deck,target])art.onload=()=>{if(!dead)draw();};
  function draw(){ctx.fillStyle='#05111e';ctx.fillRect(0,0,640,420);if(deck.complete&&deck.naturalWidth)ctx.drawImage(deck,0,0,640,420);
   const X=x=>x*2,Y=y=>y*1.6;
   for(let i=0;i<3;i++)if(target.complete&&target.naturalWidth){const x=range?range._range_read(300+i,1)/256:130+i*30,y=range?range._range_read(300+i,2)/256:120-i*18;ctx.drawImage(target,X(x)-22,Y(y)-22,44,44);}
   if(ship.complete&&ship.naturalWidth)ctx.drawImage(ship,292,Y(210),56,70);
   if(!range)return;const read=range._range_read;
   function line(x,y,xx,yy,col,width){ctx.strokeStyle=col;ctx.lineWidth=width;ctx.lineCap='round';ctx.beginPath();ctx.moveTo(X(x),Y(y));ctx.lineTo(X(xx),Y(yy));ctx.stroke();}
   for(let n=0;n<56;n++)if(read(n,0)){const x=read(n,1)/256,y=read(n,2)/256,vx=read(n,3)/256,vy=read(n,4)/256,type=read(n,5);const col='#'+read(n,6).toString(16).padStart(6,'0'),tail=type===7?32/(Math.hypot(vx,vy)||1):.9;line(x,y,x-vx*tail,y-vy*tail,col,type===4?5:3);line(x,y,x-vx*tail,y-vy*tail,'#edfbff',1);}
   if(read(200,0))for(let lane=read(201,0)?-1:0;lane<=(read(201,0)?1:0);lane++){line(160+lane*6,213,read(203,lane),20,'#'+read(213,0).toString(16).padStart(6,'0')+'88',read(202,0)*4);line(160+lane*6,213,read(203,lane),20,'#fff1e9',2);}
   for(let n=100;n<196;n++)if(read(n,0)){const x=read(n,1)/256,y=read(n,2)/256,xx=read(n,3)/256,yy=read(n,4)/256;line(x,y,(x+xx)/2+Math.sin(ticks+n)*4,(y+yy)/2,'#8e9fff',2);line((x+xx)/2+Math.sin(ticks+n)*4,(y+yy)/2,xx,yy,'#e3eeff',2);}
   for(let n=400;n<464;n++)if(read(n,0)){const progress=read(n,4)/Math.max(1,read(n,5)),r=read(n,3)/100*progress;ctx.strokeStyle='rgba(255,186,103,'+(1-progress)+')';ctx.lineWidth=2;ctx.beginPath();ctx.ellipse(X(read(n,1)/256),Y(read(n,2)/256),r*2,r*1.6,0,0,Math.PI*2);ctx.stroke();}
   status.textContent='BANK '+read(205,0)+'%'+(read(206,0)?' / COOLING':'');canvas.dataset.tick=String(read(204,0));
  }
  function loop(t){raf=0;if(dead||!visible||!playing||document.hidden||!host.isConnected)return;acc+=last?Math.min(100,t-last):20;last=t;while(acc>=20){range._range_step();ticks++;acc-=20;}draw();raf=requestAnimationFrame(loop);}
  function schedule(){cancelAnimationFrame(raf);raf=0;last=0;if(range&&!dead&&visible&&playing&&!document.hidden&&host.isConnected)raf=requestAnimationFrame(loop);}
  const observer=new IntersectionObserver(entries=>{visible=entries[0].isIntersecting&&!!host.getClientRects().length;schedule();},{threshold:.1});observer.observe(canvas);
  const visibility=()=>schedule(),motion=()=>{if(reduced.matches){playing=false;toggle.textContent='PLAY PREVIEW';schedule();}};document.addEventListener('visibilitychange',visibility);reduced.addEventListener('change',motion);
  const mutation=new MutationObserver(()=>{if(!host.isConnected||host.closest('[hidden]')){visible=false;schedule();}});mutation.observe(document.querySelector('#cabinet'),{subtree:true,attributes:true,attributeFilter:['hidden'],childList:true});
  dispose=()=>{dead=true;cancelAnimationFrame(raf);observer.disconnect();mutation.disconnect();document.removeEventListener('visibilitychange',visibility);reduced.removeEventListener('change',motion);};draw();
  try{range=await engine();if(dead)return;range._range_init(s,w,v,Math.max(0,q),armour);range._range_step();draw();schedule();}catch{if(!dead)status.textContent='Preview unavailable. Equipment controls remain available.';}
 }
 function enhance(parent,tab,s,w,p,changeFamily){
  dispose();refreshHangar();parent.classList.add('armory-panel');const oldSummary=parent.firstElementChild;if(oldSummary?.textContent.startsWith('LV '))oldSummary.remove();const heading=document.createElement('h2');heading.className='armory-heading';heading.textContent='WEAPON SYSTEMS';parent.prepend(heading);if(parent.id==='refit-economy'){const resources=document.createElement('div');resources.className='armory-resources';resources.innerHTML=resourceSummary(p,PilotStore.level(p),'');heading.after(resources);}
  const families=document.createElement('nav');families.className='family-grid';families.setAttribute('aria-label','Weapon families');
  for(let i=0;i<9;i++){const b=btn(weaponNames[i],()=>changeFamily(i));b.prepend(image('weapon-'+i));b.setAttribute('aria-pressed',String(i===w));families.append(b);}
  const cards=parent.querySelector('.gear-cards');cards.before(families);cards.classList.add('weapon-cards');
  for(let v=0;v<3;v++){const card=cards.children[v];for(const para of card.querySelectorAll('p')){if(para.textContent===WeaponVariants[s][w][v].description)para.remove();else if(/NOT OWNED|EQUIPPED|^(Standard|Tuned|Rare|Exotic)$/.test(para.textContent))para.className='gear-ownership';}card.dataset.variant=String(v);card.style.setProperty('--gear-colour',WeaponVariants[s][w][v].colour);card.classList.toggle('inspected',v===selected);card.prepend(image('weapon-'+w));const b=btn((selected===v?'SELECTED / ':'')+'VIEW '+['FOCUS','FLOW','BREAKER'][v],()=>{selected=v;enhanceSelection();});b.className='inspect-weapon';b.setAttribute('aria-pressed',String(selected===v));card.prepend(b);}
  for(const [id,card] of [...parent.querySelectorAll('.gear-cards:not(.weapon-cards) .gear-card')].entries()){card.prepend(image('armour-'+id));card.dataset.armour=id;}
  const inspector=document.createElement('section');inspector.className='weapon-inspector';cards.after(inspector);
  function enhanceSelection(){dispose();[...cards.children].forEach((c,v)=>{c.classList.toggle('inspected',v===selected);const b=c.querySelector('.inspect-weapon');b.textContent=(v===selected?'SELECTED / ':'')+'VIEW '+['FOCUS','FLOW','BREAKER'][v];b.setAttribute('aria-pressed',String(v===selected));});inspector.replaceChildren();const meta=WeaponVariants[s][w][selected],h=document.createElement('h3');h.textContent=meta.name;inspector.append(h);const desc=document.createElement('p');desc.textContent=['Standard calibrated pattern.','Lighter impact and lower energy cost.','Heavier impact and higher energy cost.'][selected];inspector.append(desc);const fitted=p.loadouts[s][w],current=document.createElement('p');current.textContent='CURRENT SLOT '+(w+1)+': '+(fitted===null?'EMPTY':WeaponVariants[s][w][fitted].name);inspector.append(current);if(tab==='workshop'){const host=document.createElement('div');host.className='firing-range';inspector.append(host);preview(host,s,w,selected,p.owned[s][w][selected],p.armour);}else{const art=image('weapon-'+w);art.className='inspector-art';inspector.append(art);}}
  enhanceSelection();
 }
 function resourceSummary(p,lv,warning){return '<span>PILOT LV '+lv+'</span>'+[['credits',p.credits,'CREDITS'],['parts',p.parts,'PARTS'],['cores',p.cores,'RARE CORES']].map(([name,n,label])=>'<span><img src="'+root+name+'.webp" alt="">'+n+' '+label+'</span>').join('')+(warning?'<span>'+warning+'</span>':'');}
 function decoratePilot(parent,p){for(const b of parent.querySelectorAll('button'))if(/^(STANDARD|AURORA|COPPER) SKIN/.test(b.textContent)){b.classList.add('skin-choice');b.prepend(image('ship-'+p.selectedShip));}}
 return {stopPreview:()=>dispose(),decoratePilot,mountHangar,refreshHangar,show,enhance,resourceSummary};
})();
