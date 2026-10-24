const canvas=document.querySelector('#canvas'),startButton=document.querySelector('#start'),statusText=document.querySelector('#status');
const music=document.querySelector("#music"),musicVolume=document.querySelector("#music-volume");music.volume=.3;musicVolume.oninput=()=>{music.volume=+musicVolume.value/100;};
let ready=false,started=false,touchActive=false;
var Module={canvas,locateFile(path){return path+"?v=4";},noInitialRun:true,print:console.log,printErr:console.error,setStatus(text){if(!ready&&text)statusText.textContent=text;},onRuntimeInitialized(){ready=true;statusText.textContent='Ready for launch.';startButton.disabled=false;startButton.textContent='LAUNCH GAME â†’';},onAbort(){statusText.textContent='The game could not start. Please reload, or try a browser with WebGL enabled.';startButton.textContent='Reload page';startButton.disabled=false;startButton.onclick=()=>location.reload();}};
startButton.onclick=()=>{if(!ready||started)return;started=true;music.play().catch(()=>{});document.querySelector('#cover').hidden=true;Module.callMain([document.querySelector("#sector").value,document.querySelector("#resolution").value]);document.querySelector("#hud").hidden=false;document.querySelector('#sound').disabled=false;canvas.focus();};
document.querySelector('#full').onclick=()=>{let c=document.querySelector('#cabinet');if(document.fullscreenElement)document.exitFullscreen();else c.requestFullscreen?.().catch(()=>{});};
document.querySelector('#sound').onclick=e=>{if(!started)return;Module._web_mute();let off=e.target.textContent==='Sound on';e.target.textContent=off?'Sound off':'Sound on';canvas.focus();};
document.querySelectorAll('[data-command]').forEach(b=>b.onclick=()=>{if(started){Module._web_command(+b.dataset.command);canvas.focus();}});
document.querySelector('#weapon').onclick=()=>{if(started){Module._web_command(49+(Module._web_stat(2)+1)%8);canvas.focus();}};
function touchMove(e){if(!started||e.pointerType==='mouse')return;e.preventDefault();let r=canvas.getBoundingClientRect();Module._web_pointer((e.clientX-r.left)/r.width*320,(e.clientY-r.top)/r.height*(800/3));}
canvas.addEventListener('pointerdown',e=>{if(e.pointerType==='mouse')return;touchActive=true;canvas.setPointerCapture(e.pointerId);touchMove(e);Module._web_held(4);Module._web_command(32);});
canvas.addEventListener('pointermove',e=>{if(touchActive)touchMove(e);});
function touchEnd(){touchActive=false;if(started)Module._web_held(0);}
canvas.addEventListener('pointerup',touchEnd);canvas.addEventListener('pointercancel',touchEnd);
window.addEventListener('blur',touchEnd);
window.addEventListener('keydown',e=>{if(started&&!['INPUT','SELECT','BUTTON'].includes(e.target.tagName)&&['Space','ArrowUp','ArrowDown','ArrowLeft','ArrowRight','F3','F4'].includes(e.code))e.preventDefault();});
canvas.addEventListener('contextmenu',e=>e.preventDefault());

// Match game pause/focus state; duck the soundtrack during the thruster sweep.
setInterval(()=>{if(!started||typeof Module._web_stat!=="function")return;const off=!!Module._web_stat(18);music.muted=off;document.querySelector('#sound').textContent=off?'Sound off':'Sound on';const paused=!!Module._web_stat(6)||document.hidden||!document.hasFocus();if(paused){if(!music.paused)music.pause();}else if(music.paused&&!music.ended){music.play().catch(()=>{});}const target=+musicVolume.value/100*(Module._web_stat(16)?.45:1);music.volume+=(target-music.volume)*.25;},120);

const tracks=['audio/ovg-alien-8-follow-the-valves.mp3','audio/ovg-sabre-wulf-follow-the-orchid.mp3','audio/ovg-shinobi-school-run-file.mp3'];const trackSelect=document.querySelector('#music-track');
function chooseTrack(index){trackSelect.value=String(index);music.src=tracks[index];if(started&&!Module._web_stat(6))music.play().catch(()=>{});}
trackSelect.onchange=()=>chooseTrack(+trackSelect.value);
music.addEventListener('ended',()=>chooseTrack((+trackSelect.value+1)%tracks.length));

const sectorNames=['DEEP SPACE','ION NEBULA','ASTEROID BELT','HYPERLANE','SIEGE ARENA','ROCKY CAVERN','VERDANT SKIES','DUNE SEA','FROST RIDGE','MAGMA COAST'];
const weaponNames=['PULSE','SPREAD','RAPID','BEAM','HEATSEEKER','MULTILASER','RICOCHET','SPLIT LASER'];
const bossNames=['STAR DREADNOUGHT','ION ORACLE','IRON HARVESTER','RIFT LANCER','SIEGE CITADEL','CAVERN WARDEN','SKY MANTA','DUNE SCORPION','FROST CROWN','MAGMA HYDRA'];
setInterval(()=>{if(!started||typeof Module._web_stat!=='function')return;const stat=Module._web_stat;document.querySelector('#hud-score').textContent=stat(19).toLocaleString();document.querySelector('#hud-wave').textContent=String(stat(10)).padStart(2,'0');document.querySelector('#hud-lives').textContent=stat(20);document.querySelector('#hud-weapon').textContent=weaponNames[stat(2)];document.querySelector('#hud-barrier').style.width=Math.max(0,Math.min(100,stat(21)))+'%';document.querySelector('#hud-energy').style.width=stat(22)+'%';document.querySelector('#hud-sector').textContent=sectorNames[Math.max(0,stat(15))]+(stat(28)?" / SIDE FLIGHT":" / VERTICAL");document.querySelector('#hud-supplies').textContent=stat(3)+' NOVAS / '+stat(26)+' PODS';document.querySelector('#hud-fps').textContent=stat(27)+' FPS';let boss=stat(24);document.querySelector('#boss-hud').hidden=!boss;if(boss){document.querySelector('#boss-name').textContent=bossNames[boss-1];document.querySelector('#boss-health').style.width=stat(25)+'%';}const message=document.querySelector('#game-message');message.hidden=stat(1)!==2&&!stat(6);message.textContent=stat(1)===2?'GAME OVER · PRESS R TO RESTART':'PAUSED';document.querySelector('#transit-message').hidden=!stat(16);},100);

if(innerWidth<650)document.querySelector("#resolution").value="1920";
