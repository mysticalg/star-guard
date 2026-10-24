(() => {
  const api='https://star-guard-scores-mysticalg.netlify.app/api';
  const rows=document.querySelector('#score-rows'),status=document.querySelector('#score-status');
  const form=document.querySelector('#score-form'),name=document.querySelector('#pilot-name'),save=document.querySelector('#save-score');
  let runPromise=null,pending=null,lastMode=null,lastTick=null,saving=false;
  try{name.value=localStorage.getItem('sg-pilot-name')||'';pending=JSON.parse(sessionStorage.getItem('sg-pending-score')||'null');}catch{}
  async function request(path,body){
    const response=await fetch(api+path,{method:body?'POST':'GET',headers:body?{'Content-Type':'application/json'}:{},body:body?JSON.stringify(body):undefined,signal:AbortSignal.timeout(15000)});
    const data=await response.json();if(!response.ok)throw Error(data.error||'Score service unavailable.');return data;
  }
  function render(scores){
    rows.replaceChildren();
    if(!scores.length){const tr=rows.insertRow(),td=tr.insertCell();td.colSpan=5;td.textContent='No scores yet. Set the first high score.';return;}
    scores.forEach((s,i)=>{const tr=rows.insertRow();[String(i+1).padStart(2,'0'),s.name,Number(s.score).toLocaleString(),String(s.level).padStart(2,'0')].forEach(value=>{tr.insertCell().textContent=value;});const cell=tr.insertCell(),date=new Date(s.created_at);cell.style.whiteSpace='nowrap';if(Number.isFinite(date.getTime())){const time=document.createElement('time');time.dateTime=date.toISOString();time.textContent=date.toLocaleDateString('en-GB',{day:'2-digit',month:'short',year:'numeric',timeZone:'UTC'});time.title=date.toUTCString();cell.append(time);}else cell.textContent='—';tr.title='Started at sector '+s.starting_level;});
  }
  async function refresh(){
    status.textContent='Loading the world table…';
    try{const data=await request('/scores');render(data.scores);status.textContent='Shared online · top 20 runs';}
    catch{status.textContent='The online table is temporarily unavailable. Try Refresh.';}
  }
  function showPending(){
    form.hidden=!pending;if(!pending)return;
    document.querySelector('#pending-score').textContent=Number(pending.score).toLocaleString()+' points · sector '+String(pending.level).padStart(2,'0');
    save.disabled=false;save.textContent='SAVE SCORE';document.querySelector('#save-status').textContent='Enter your pilot name to join the shared table.';
  }
  function beginRun(startingLevel){
    runPromise=request('/runs',{startingLevel,clientVersion:2}).then(data=>data.runId).catch(()=>null);
    lastTick=null;lastMode=null;
  }
  async function finish(score,level,promise){
    if(score<=0)return;const runId=await promise;
    if(!runId){status.textContent='This run could not connect to the score service. Start a new game to register a score.';return;}
    if(pending&&pending.score>score)return;
    pending={runId,score,level};try{sessionStorage.setItem('sg-pending-score',JSON.stringify(pending));}catch{}
    showPending();form.scrollIntoView({behavior:'smooth',block:'nearest'});
  }
  function observeRun(state){
    if(lastTick!==null&&state.tick<lastTick&&state.mode!==2)beginRun(state.level);
    if(state.mode===2&&lastMode!==2)finish(state.score,state.level,runPromise);
    lastMode=state.mode;lastTick=state.tick;
  }
  form.addEventListener('submit',async event=>{
    event.preventDefault();if(!pending||saving)return;saving=true;save.disabled=true;
    const message=document.querySelector('#save-status');message.textContent='Saving to the world table…';
    try{
      const data=await request('/scores',{...pending,name:name.value.trim()});render(data.scores);
      try{localStorage.setItem('sg-pilot-name',name.value.trim());sessionStorage.removeItem('sg-pending-score');}catch{}
      pending=null;message.textContent='Score saved online. Good flying!';save.textContent='SAVED';status.textContent='Shared online · top 20 runs';
    }catch(error){message.textContent=error.message||'Could not save. Please retry.';save.disabled=false;}
    finally{saving=false;}
  });
  document.querySelector('#refresh-scores').addEventListener('click',refresh);
  window.HighScores={beginRun,observeRun};showPending();refresh();
})();
