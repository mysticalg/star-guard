const canvas=document.querySelector('#canvas'),startButton=document.querySelector('#start'),statusText=document.querySelector('#status');
let ready=false,started=false,touchActive=false;
var Module={canvas,noInitialRun:true,print:console.log,printErr:console.error,setStatus(text){if(!ready&&text)statusText.textContent=text;},onRuntimeInitialized(){ready=true;statusText.textContent='Ready for launch.';startButton.disabled=false;startButton.textContent='LAUNCH GAME →';},onAbort(){statusText.textContent='The game could not start. Please reload, or try a browser with WebGL enabled.';startButton.textContent='Reload page';startButton.disabled=false;startButton.onclick=()=>location.reload();}};
startButton.onclick=()=>{if(!ready||started)return;started=true;document.querySelector('#cover').hidden=true;Module.callMain([]);document.querySelector('#sound').disabled=false;canvas.focus();};
document.querySelector('#full').onclick=()=>{let c=document.querySelector('#cabinet');if(document.fullscreenElement)document.exitFullscreen();else c.requestFullscreen?.().catch(()=>{});};
document.querySelector('#sound').onclick=e=>{if(!started)return;Module._web_mute();let off=e.target.textContent==='Sound on';e.target.textContent=off?'Sound off':'Sound on';canvas.focus();};
document.querySelectorAll('[data-command]').forEach(b=>b.onclick=()=>{if(started){Module._web_command(+b.dataset.command);canvas.focus();}});
document.querySelector('#weapon').onclick=()=>{if(started){Module._web_command(49+(Module._web_stat(2)+1)%7);canvas.focus();}};
function touchMove(e){if(!started||e.pointerType==='mouse')return;e.preventDefault();let r=canvas.getBoundingClientRect();Module._web_pointer((e.clientX-r.left)/r.width*320,(e.clientY-r.top)/r.height*(800/3));}
canvas.addEventListener('pointerdown',e=>{if(e.pointerType==='mouse')return;touchActive=true;canvas.setPointerCapture(e.pointerId);touchMove(e);Module._web_held(4);Module._web_command(32);});
canvas.addEventListener('pointermove',e=>{if(touchActive)touchMove(e);});
function touchEnd(){touchActive=false;if(started)Module._web_held(0);}
canvas.addEventListener('pointerup',touchEnd);canvas.addEventListener('pointercancel',touchEnd);
window.addEventListener('blur',touchEnd);
window.addEventListener('keydown',e=>{if(started&&['Space','ArrowUp','ArrowDown','ArrowLeft','ArrowRight','F3','F4'].includes(e.code))e.preventDefault();});
canvas.addEventListener('contextmenu',e=>e.preventDefault());
