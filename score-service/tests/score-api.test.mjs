import { test } from 'node:test';
import assert from 'node:assert/strict';
import { createScoreHandler } from '../lib/score-api.mjs';

const origin='https://mysticalg.github.io';
function setup(){
  const runs=new Map(),scores=new Map();
  const repo={
    start:async(id,level)=>runs.set(id,{starting_level:level,started_at:new Date(0).toISOString()}),
    run:async id=>runs.get(id),
    save:async s=>{if(!scores.has(s.runId))scores.set(s.runId,{id:s.id,name:s.name,score:s.score,level:s.level,starting_level:s.startingLevel});return scores.get(s.runId);},
    top:async()=>[...scores.values()].sort((a,b)=>b.score-a.score).slice(0,20)
  };
  const handler=createScoreHandler(repo,()=>30000);
  const call=async(path,body,customOrigin=origin)=>handler(new Request('https://score.test/api/'+path,{method:body===undefined?'GET':'POST',headers:{Origin:customOrigin,'Content-Type':'application/json'},body:body===undefined?undefined:JSON.stringify(body)}));
  return {repo,handler,call,scores};
}
test('scores are shared across independent clients; duplicate retries do not add or replace scores',async()=>{
  const {repo,call,scores}=setup();
  const {runId}=await(await call('runs',{startingLevel:1})).json();
  const payload={runId,name:'ACE',score:12000,level:2};
  assert.equal((await call('scores',payload)).status,201);
  await call('scores',{...payload,score:99999});
  assert.equal(scores.size,1);
  const otherClient=createScoreHandler(repo,()=>30000);
  const board=await(await otherClient(new Request('https://score.test/api/scores'))).json();
  assert.equal(board.scores[0].name,'ACE');assert.equal(board.scores[0].score,12000);
});
test('invalid names, impossible scores, unknown runs and unsupported origins are rejected',async()=>{
  const {call,scores}=setup();const {runId}=await(await call('runs',{startingLevel:1})).json();
  for(const change of [{name:'<script>alert(1)'},{score:-1},{score:99999999},{level:9999},{runId:'00000000-0000-4000-8000-000000000000'}])assert.equal((await call('scores',{runId,name:'ACE',score:1000,level:1,...change})).status,400);
  assert.equal((await call('runs',{startingLevel:1},'https://unrelated.test')).status,403);assert.equal(scores.size,0);
});
test('preflight allows the game origin, and malformed bodies cannot write',async()=>{
  const {handler,scores}=setup();
  const preflight=await handler(new Request('https://score.test/api/scores',{method:'OPTIONS',headers:{Origin:origin}}));
  assert.equal(preflight.status,204);assert.equal(preflight.headers.get('Access-Control-Allow-Origin'),origin);
  for(const body of ['null','[]','{bad',JSON.stringify({name:'x'.repeat(3000)})]){
    const response=await handler(new Request('https://score.test/api/scores',{method:'POST',headers:{Origin:origin,'Content-Type':'application/json'},body}));assert.ok([400,413].includes(response.status));
  }assert.equal(scores.size,0);
});

test("all seventeen starting sectors are accepted, out-of-range sectors are rejected",async()=>{const {call}=setup();for(let startingLevel=1;startingLevel<=17;startingLevel++)assert.equal((await call("runs",{startingLevel})).status,201);for(const startingLevel of [0,18,-1])assert.equal((await call("runs",{startingLevel})).status,400);});
