/* Stable game-state keys prevent polling, camera blending, and pause from restarting songs. */
(function(root){
 function resolve(s){
  if(!s.started)return {key:'menu:'+s.menu,index:['ship','shop','workshop','pilot'].includes(s.menu)?7:s.menu==='map'?8:6,loop:true};
  if(s.refit)return {key:"menu:refit",index:7,loop:true};
  const m=Math.max(1,Math.min(6,s.mission)),n=s.section||0,side=s.side?1:0;
  if(s.won)return {key:'win:campaign',index:23,loop:false};
  if(s.victory)return {key:`win:${m}:${n}:${s.tier}`,index:23,loop:false};
  if(s.giant)return {key:`giant:${m}:${n}`,index:11+(m-1)%3,loop:true};
  if(s.mini)return {key:`mini:${m}:${n}:${s.mini}`,index:10,loop:true};
  if(s.intro&&n===0)return {key:`intro:${m}`,index:9,loop:true};
  const cave=[2,3,4,6,8,11,12,13].includes(s.kind);
  return {key:`flight:${m}:${n}:${side}:${s.kind}`,index:cave?14+(m+n+side)%3:n===0&&!side?m-1:17+(m+n+side*2)%6,loop:true};
 }
 function create(play){let current=null,unlocked=false,history=[];return {
  update(s){const next=resolve(s);if(current?.key===next.key)return false;if(current&&next.index===current.index&&next.loop&&next.key.startsWith('flight:'))next.index=next.index>=14&&next.index<=16?14+(next.index-13)%3:17+(next.index-16)%6;current=next;history.push({...next});if(history.length>80)history.shift();if(unlocked)play(next.index,next.loop);return true;},
  unlock(){if(unlocked)return;unlocked=true;if(current)play(current.index,current.loop);},
  manual(index){if(!unlocked)this.unlock();play(index,true);},
  get state(){return {unlocked,current:current&&{...current},history:history.map(x=>({...x}))};}
 };}
 const api={resolve,create};if(typeof module==='object'&&module.exports)module.exports=api;else root.StarMusic=api;
})(typeof window==='object'?window:globalThis);
