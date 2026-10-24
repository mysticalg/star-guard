const canvas=document.querySelector('#canvas'),startButton=document.querySelector('#start'),statusText=document.querySelector('#status');

const music=document.querySelector("#music"),musicVolume=document.querySelector("#music-volume");music.volume=.3;musicVolume.oninput=()=>{music.volume=+musicVolume.value/100;};

let ready=false,started=false,touchActive=false;

var Module={canvas,locateFile(path){return path+"?v=24";},noInitialRun:true,print:console.log,printErr:console.error,setStatus(text){if(!ready&&text)statusText.textContent=text;},onRuntimeInitialized(){ready=true;statusText.textContent='Ready for launch.';startButton.disabled=false;startButton.textContent='LAUNCH GAME  >';},onAbort(){statusText.textContent='The game could not start. Please reload, or try a browser with WebGL enabled.';startButton.textContent='Reload page';startButton.disabled=false;startButton.onclick=()=>location.reload();}};

startButton.onclick=()=>{if(!ready||started)return;started=true;HighScores.beginRun(+document.querySelector('#sector').value+1);music.play().catch(()=>{});document.querySelector('#cover').hidden=true;Module.callMain([document.querySelector("#sector").value,document.querySelector("#resolution").value]);document.querySelector("#hud").hidden=false;document.querySelector('#sound').disabled=false;canvas.focus();ShipVoice.start();};

document.querySelector('#full').onclick=()=>{let c=document.querySelector('#cabinet');if(document.fullscreenElement)document.exitFullscreen();else c.requestFullscreen?.().catch(()=>{});};

document.querySelector('#sound').onclick=e=>{if(!started)return;Module._web_mute();let off=e.target.textContent==='Sound on';e.target.textContent=off?'Sound off':'Sound on';canvas.focus();};

document.querySelectorAll('[data-command]').forEach(b=>b.onclick=()=>{if(started){Module._web_command(+b.dataset.command);canvas.focus();}});

document.querySelector('#weapon').onclick=()=>{if(started){Module._web_command(49+(Module._web_stat(2)+1)%9);canvas.focus();}};

function touchMove(e){if(!started||e.pointerType==='mouse')return;e.preventDefault();let r=canvas.getBoundingClientRect();Module._web_pointer((e.clientX-r.left)/r.width*320,(e.clientY-r.top)/r.height*(800/3));}

canvas.addEventListener('pointerdown',e=>{if(e.pointerType==='mouse')return;touchActive=true;canvas.setPointerCapture(e.pointerId);touchMove(e);Module._web_held(4);Module._web_command(32);});

canvas.addEventListener('pointermove',e=>{if(touchActive)touchMove(e);});

function touchEnd(){touchActive=false;if(started)Module._web_held(0);}

canvas.addEventListener('pointerup',touchEnd);canvas.addEventListener('pointercancel',touchEnd);

window.addEventListener('blur',touchEnd);

window.addEventListener('keydown',e=>{if(started&&!['INPUT','SELECT','BUTTON'].includes(e.target.tagName)&&['Space','ArrowUp','ArrowDown','ArrowLeft','ArrowRight','F3','F4'].includes(e.code))e.preventDefault();});

canvas.addEventListener('contextmenu',e=>e.preventDefault());



// Match game pause/focus state; duck the soundtrack during the thruster sweep.

setInterval(()=>{if(!started||typeof Module._web_stat!=="function")return;const off=!!Module._web_stat(18);music.muted=off;document.querySelector('#sound').textContent=off?'Sound off':'Sound on';const paused=!!Module._web_stat(6)||document.hidden||!document.hasFocus();if(paused){if(!music.paused)music.pause();}else if(music.paused&&!music.ended){music.play().catch(()=>{});}const target=+musicVolume.value/100*(ShipVoice.speaking?.3:Module._web_stat(65)?.12:Module._web_stat(16)?.45:1);music.volume+=(target-music.volume)*.25;},120);



const tracks=['audio/ovg-alien-8-follow-the-valves.mp3','audio/ovg-sabre-wulf-follow-the-orchid.mp3','audio/ovg-shinobi-school-run-file.mp3'];const trackSelect=document.querySelector('#music-track');

function chooseTrack(index){trackSelect.value=String(index);music.src=tracks[index];if(started&&!Module._web_stat(6))music.play().catch(()=>{});}

trackSelect.onchange=()=>chooseTrack(+trackSelect.value);

music.loop=true;let musicLevel=null;
function syncLevelMusic(level){if(level<0)return;if(musicLevel===null){musicLevel=level;return;}if(level!==musicLevel){musicLevel=level;chooseTrack((+trackSelect.value+1)%tracks.length);}}



const sectorNames=["DEEP SPACE", "ION NEBULA", "ASTEROID BELT", "HYPERLANE", "ASTEROID SURFACE", "ROCKY CAVERN", "VERDANT SKIES", "DUNE SEA", "FROST RIDGE", "FIRE STORM", "EARTH ORBIT", "GROUND ASSAULT", "ICE ASTEROIDS", "METAL ASTEROIDS", "HEAVY RADIATION", "DIAMOND ASTEROIDS", "URANIUM ASTEROIDS"];

function missionTime(ticks){const seconds=Math.floor(Math.max(0,ticks)/50);return String(Math.floor(seconds/60)).padStart(2,'0')+':'+String(seconds%60).padStart(2,'0');}
const weaponNames=['PULSE','SPREAD','RAPID','RAINBOW BEAM','HEATSEEKER','TRIPLE RAINBOW','RICOCHET','BOUNCING LASER','EMP STORM'];

const bossNames=['STAR DREADNOUGHT','ION ORACLE','IRON HARVESTER','RIFT LANCER','ASTEROID CITADEL','CAVERN WARDEN','SKY MANTA','DUNE SCORPION','FROST CROWN','MAGMA HYDRA','ORBITAL SENTINEL','GROUND LEVIATHAN','GLACIAL COLOSSUS','FERRO TITAN','ISOTOPE OVERMIND','DIAMOND SERAPH','URANIUM MONARCH'];

setInterval(()=>{if(!started||typeof Module._web_stat!=='function')return;const stat=Module._web_stat;ShipVoice.observe(stat);syncLevelMusic(stat(15));document.querySelector('#mission-timer').textContent='MISSION '+missionTime(stat(55))+' / SECTOR '+missionTime(stat(49))+' / WAVE '+String(stat(50)).padStart(2,'0')+'/12'+(stat(83)?' / EMP: WEAPONS OFFLINE '+(stat(83)/50).toFixed(1)+'s':stat(85)?' / HOSTILE EMP: '+(stat(85)/50).toFixed(1)+'s — DESTROY CHARGE':'');document.querySelector('#mission-timer').style.color=stat(83)?'#ff9fe8':stat(85)?'#80efff':'';HighScores.observeRun({mode:stat(1),score:stat(19),level:stat(10),tick:stat(0),phase:stat(15)});document.querySelector('#hud-score').textContent=stat(19).toLocaleString();document.querySelector('#hud-wave').textContent=String(stat(10)).padStart(2,'0');document.querySelector('#hud-lives').textContent=stat(20);document.querySelector('#hud-weapon').textContent=weaponNames[stat(2)]+" / LV "+stat(37)+(stat(62)?" / "+stat(61)+"/"+stat(62):" / MAX");document.querySelector('#hud-barrier').style.width=Math.max(0,Math.min(100,stat(21)))+'%';document.querySelector('#hud-energy').style.width=stat(22)+'%';document.querySelector('#hud-sector').textContent=sectorNames[Math.max(0,stat(15))]+(stat(28)?" / SIDE FLIGHT":" / VERTICAL");document.querySelector('#hud-supplies').textContent='SHIP MK '+stat(56)+' / '+stat(3)+' NOVAS / '+stat(26)+' PODS'+(stat(38)?' / SAT':'')+(stat(39)?' / BOMBER':'');document.querySelector('#hud-fps').textContent=stat(27)+' FPS';let boss=stat(24);document.querySelector('#boss-hud').hidden=!boss;if(boss){document.querySelector('#boss-name').textContent=bossNames[boss-1]+" / PHASE "+Math.max(1,stat(45))+(stat(52)>60&&stat(52)<=85?" / INVINCIBLE":stat(60)>0?" / SHIELD "+stat(60)+"%":" / EXPOSED");document.querySelector('#boss-health').style.width=stat(25)+'%';document.querySelector('#boss-shield').style.width=stat(60)+'%';document.querySelector('#boss-shield-status').textContent=stat(60)>0?'SHIELD '+stat(60)+'%':stat(63)>0?'RECHARGE '+(stat(63)/50).toFixed(1)+'s':'SHIELD DOWN';}const message=document.querySelector('#game-message');message.hidden=stat(1)!==2&&!stat(6);message.textContent=stat(1)===2?'GAME OVER · PRESS R TO RESTART':'PAUSED';const transit=document.querySelector('#transit-message');transit.hidden=!stat(16)&&!stat(33)&&!stat(34)&&!stat(41)&&!stat(42)&&!stat(46)&&!stat(48)&&!(stat(52)>0&&stat(52)<=60);transit.textContent=stat(52)>0&&stat(52)<=60?'BOSS LASER / CLEAR THE MARKED ZONE':stat(48)?((stat(49)>=240&&stat(49)<540?['','DENSE ASTEROID BARRAGE / KEEP FIRING','DENSE ICE BARRAGE / KEEP FIRING','DENSE FIRE BARRAGE / KEEP FIRING','ICE ASTEROID WALL / KEEP FIRING','METAL ASTEROID WALL / KEEP FIRING','ISOTOPE BARRAGE / KEEP FIRING','DIAMOND SHARD WALL / KEEP FIRING','URANIUM BARRAGE / KEEP FIRING']:['','ASTEROID STORM / BLAST A PATH','ICE STORM / SHATTER THE HAIL','FIRE STORM / INCOMING METEORS','ICE ASTEROIDS / SHATTER A PATH','METAL ASTEROIDS / HEAVY ARMOUR','HEAVY RADIATION / AVOID GREEN HALOS','DIAMOND ASTEROIDS / PRISM STORM','URANIUM ASTEROIDS / RADIOACTIVE ORE'])[stat(48)]):stat(46)?'WARNING / BOSS TRANSFORMING':stat(42)?'WARNING / REAR APPROACH':stat(41)?(stat(40)===11?'SALVAGE +'+stat(59)+' SCORE':stat(40)===10?'WEAPON EQUIPPED':stat(40)===12?(stat(62)?'WEAPON CHARGE '+stat(61)+'/'+stat(62):'MAX POWER / ENERGY RESTORED'):stat(40)===7?'WEAPON POWER UP':stat(40)===6?'OPTION ACQUIRED':stat(40)===8?'SATELLITE ACQUIRED':'BOMB DROPPER ACQUIRED'):stat(33)?'WARNING / BOSS APPROACHING':stat(34)?'SECTOR CLEAR':'SECTOR TRANSIT';transit.style.color=stat(33)?'#ffb48c':'#c1fff2';},100);



if(innerWidth<650)document.querySelector("#resolution").value="1920";


// Accumulate high-resolution wheel input; preserve queued steps between simulation ticks.
let weaponWheel=0,weaponWheelTarget=null,lastWeaponWheel=0;
canvas.addEventListener('wheel',event=>{if(!started)return;event.preventDefault();weaponWheel+=event.deltaY/(event.deltaMode===1?3:event.deltaMode===2?1:100);const steps=Math.trunc(weaponWheel);if(!steps)return;weaponWheel-=steps;const now=performance.now(),current=weaponWheelTarget!==null&&now-lastWeaponWheel<120?weaponWheelTarget:Module._web_stat(2);weaponWheelTarget=(current+steps%9+9)%9;lastWeaponWheel=now;Module._web_command(49+weaponWheelTarget);canvas.focus({preventScroll:true});},{passive:false});
canvas.addEventListener('keydown',event=>{if(/^[1-8]$/.test(event.key))weaponWheelTarget=null;});

document.querySelector('#voice-toggle').onclick=e=>{const on=ShipVoice.toggle();e.target.textContent=on?'Ship voice on':'Ship voice off';e.target.setAttribute('aria-pressed',String(on));};

const missionObjectives = ["Entering deep space. Clear the hostile formations, blast incoming debris, and defeat the sector command ship.", "Ion nebula ahead. Expect flanking attacks. Keep moving, clear the waves, then destroy the Oracle.", "Asteroid belt ahead. Blast a path through dense rock formations. Save energy for the Iron Harvester.", "Entering the hyperlane. High speed traffic and enemy interceptors ahead. Secure the route and defeat the Rift Lancer.", "Giant asteroid surface ahead. We are making a high speed approach. Clear the defences before entering the caverns.", "Entering rocky caverns. Choose your route through the forks. Walls are lethal. Defeat the Cavern Warden.", "Verdant skies ahead. Enemy aircraft are gathering over the forests. Clear the airspace and defeat the Sky Manta.", "Entering the dune sea. Watch for ambushes from behind. Break the enemy formations and destroy the Dune Scorpion.", "Frost ridge ahead. Shatter the ice storm and avoid the mountains. Destroy the Frost Crown to secure passage.", "Fire storm sector ahead. Blast the burning meteors and clear the hostile waves. Destroy the Magma Hydra.", "Earth orbit confirmed. Protect the planet below. Clear the incoming formations and destroy the Orbital Sentinel.", "Ground assault commencing. Stay above the surface. Destroy tanks and turrets, then eliminate the Ground Leviathan.", "Ice asteroids ahead. Shatter the frozen barrage and keep a clear flight path. Defeat the Glacial Colossus.", "Metal asteroid field ahead. These rocks have heavy armour. Focus your fire and eliminate the Ferro Titan.", "Heavy isotope radiation detected. Avoid the glowing cores and their radiation halos. Destroy the Isotope Overmind.", "Diamond asteroid field ahead. Shatter the crystal walls and conserve energy for the Diamond Seraph.", "Uranium asteroids detected. Avoid the radioactive ore. Keep your distance and destroy the Uranium Monarch."];
const briefing=document.createElement('section');briefing.id='mission-briefing';briefing.hidden=true;document.querySelector('#cabinet').append(briefing);
const debrief=document.createElement('section');debrief.id='mission-debrief';debrief.hidden=true;document.querySelector('#cabinet').append(debrief);
let victorySeen=0,stingContext;
function triumphSting(){try{stingContext ||= new (window.AudioContext||window.webkitAudioContext)();const ctx=stingContext;ctx.resume();const master=ctx.createGain();master.gain.value=.18;master.connect(ctx.destination);const notes=[261.63,329.63,392,523.25,659.25,783.99];notes.forEach((hz,i)=>{const start=ctx.currentTime+i*.15;for(const ratio of [1,1.005]){const osc=ctx.createOscillator(),gain=ctx.createGain();osc.type='triangle';osc.frequency.value=hz*ratio;gain.gain.setValueAtTime(0,start);gain.gain.linearRampToValueAtTime(.4,start+.02);gain.gain.exponentialRampToValueAtTime(.001,start+(i===5?1.7:.55));osc.connect(gain);gain.connect(master);osc.start(start);osc.stop(start+1.8);}});}catch{}}
setInterval(()=>{if(!started)return;const stat=Module._web_stat,intro=stat(64),victory=stat(65),phase=Math.max(0,stat(15));canvas.classList.toggle("hide-cursor",stat(1)===1&&!stat(6));canvas.classList.toggle("boss-shake",victory>0&&victory<85&&!stat(6));briefing.hidden=stat(1)!==1||!intro||!!stat(16)||!!stat(6);if(!briefing.hidden){briefing.innerHTML='<small>SHIP COMPUTER / MISSION '+String(stat(10)).padStart(2,'0')+'</small><h2>'+sectorNames[phase]+'</h2><p>'+missionObjectives[phase]+'</p><span>FREE FLIGHT · CONTACT IN '+Math.ceil(intro/50)+'s</span>';}
 debrief.hidden=victory<100||victory>275||!!stat(16)||!!stat(6);if(!debrief.hidden)debrief.innerHTML='<small>MISSION COMPLETE / FLIGHT REPORT</small><h2>'+sectorNames[phase]+' SECURED</h2><p>STAGE SCORE <b>'+stat(66).toLocaleString()+'</b><br>HOSTILES DESTROYED <b>'+stat(67)+'</b><br>SALVAGE RECOVERED <b>'+stat(68)+'</b><br>CLEARANCE BONUS <b>'+stat(69).toLocaleString()+'</b></p><span>NEXT MISSION / PREPARING DEPARTURE</span>';
 if(victory>=100&&victorySeen<100&&!stat(18))triumphSting();victorySeen=victory;if(stingContext){if(stat(6)||stat(18)||document.hidden)stingContext.suspend();else if(stingContext.state==='suspended')stingContext.resume();}
},100);

const continuePanel=document.createElement('section');continuePanel.id='continue-panel';continuePanel.hidden=true;continuePanel.innerHTML='<small>FLIGHT SYSTEM LOST / SIGNAL RECOVERY</small><h2>CONTINUE?</h2><div id="continue-number">9</div><p id="continue-remaining"></p><button id="use-continue">CONTINUE / ENTER</button><button id="finish-run">FINISH RUN</button>';document.querySelector('#cabinet').append(continuePanel);
document.querySelector('#use-continue').onclick=()=>{Module._web_command(13);canvas.focus();};document.querySelector('#finish-run').onclick=()=>{Module._web_command(110);canvas.focus();};
setInterval(()=>{if(!started)return;const stat=Module._web_stat;continuePanel.hidden=stat(1)!==3||!!stat(6);document.querySelector('#continue-number').textContent=Math.ceil(stat(75)/50);document.querySelector('#continue-remaining').textContent=stat(74)+' CONTINUES REMAINING';},80);

setInterval(()=>{if(!started)return;const stat=Module._web_stat,shield=stat(86),hull=stat(87),charged=stat(60)>0;document.querySelector('#boss-weakness').textContent=shield?(charged?'▶ ':'')+'SHIELD: '+weaponNames[shield-1]+' ['+shield+'] ×3 / '+(!charged?'▶ ':'')+'HULL: '+weaponNames[hull-1]+' ['+hull+'] ×2.5':'';},100);
