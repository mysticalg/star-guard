import {test} from 'node:test';
import assert from 'node:assert/strict';
import vm from 'node:vm';
import {readFile} from 'node:fs/promises';

test('restart registers the mission number and preserves a failed score for retry',async()=>{
  const nodes=new Map(),stored=new Map(),calls=[];let fail=true;
  const node=id=>{if(!nodes.has(id))nodes.set(id,{value:'Palpatine',textContent:'',hidden:true,disabled:false,replaceChildren(){},insertRow(){return {insertCell(){return {};}};},scrollIntoView(){},addEventListener(type,fn){this[type]=fn;}});return nodes.get(id);};
  const storage={getItem:k=>stored.get(k)||null,setItem:(k,v)=>stored.set(k,v),removeItem:k=>stored.delete(k)};
  const sandbox={window:{},document:{querySelector:node},localStorage:storage,sessionStorage:storage,AbortSignal,Date,Number,JSON,console,
    fetch:async(url,options={})=>{const payload=options.body&&JSON.parse(options.body);calls.push({url,payload});if(url.endsWith('/runs'))return {ok:true,json:async()=>({runId:'123e4567-e89b-42d3-a456-426614174000'})};if(payload&&fail)return {ok:false,json:async()=>({error:'Temporary connection error'})};return {ok:true,json:async()=>({scores:[]})};}};
  vm.runInNewContext(await readFile(new URL('../../docs/highscores.js',import.meta.url),'utf8'),sandbox);
  const api=sandbox.window.HighScores;api.beginRun(1);api.observeRun({tick:200,mode:1,level:1,phase:10});api.observeRun({tick:1,mode:1,level:1,phase:10});
  await new Promise(resolve=>setImmediate(resolve));const starts=calls.filter(c=>c.url.endsWith('/runs'));assert.equal(starts.length,2);assert.deepEqual({...starts[1].payload},{startingLevel:1,clientVersion:2});
  api.observeRun({tick:15000,mode:2,level:3,phase:2,score:850075});await new Promise(resolve=>setImmediate(resolve));
  assert.equal(node('#score-form').hidden,false);await node('#score-form').submit({preventDefault(){}});assert.equal(JSON.parse(stored.get('sg-pending-score')).score,850075);assert.equal(node('#save-score').disabled,false);
  fail=false;await node('#score-form').submit({preventDefault(){}});assert.equal(stored.has('sg-pending-score'),false);assert.equal(node('#save-status').textContent,'Score saved online. Good flying!');
});
