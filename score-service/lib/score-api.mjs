import { randomUUID } from 'node:crypto';

const origins = new Set(['https://mysticalg.github.io', 'https://star-guard-scores-mysticalg.netlify.app', 'http://127.0.0.1:8927']);
const uuid = /^[0-9a-f]{8}-[0-9a-f]{4}-4[0-9a-f]{3}-[89ab][0-9a-f]{3}-[0-9a-f]{12}$/i;
const integer = (x, min, max) => Number.isInteger(x) && x >= min && x <= max;

export function createScoreHandler(repo, now = Date.now) {
  return async request => {
    const origin = request.headers.get('origin');
    const headers = {'Content-Type':'application/json', 'Cache-Control':'no-store', 'Vary':'Origin'};
    if (origins.has(origin)) {
      headers['Access-Control-Allow-Origin'] = origin;
      headers['Access-Control-Allow-Methods'] = 'GET, POST, OPTIONS';
      headers['Access-Control-Allow-Headers'] = 'Content-Type';
    }
    const reply = (data, status = 200) => new Response(JSON.stringify(data), {status, headers});
    if (origin && !origins.has(origin)) return reply({error:'Origin not allowed.'}, 403);
    if (request.method === 'OPTIONS') return new Response(null, {status:204, headers});
    const path = new URL(request.url).pathname;
    try {
      if (request.method === 'GET' && path === '/api/scores') return reply({scores:await repo.top()});
      if (request.method !== 'POST') return reply({error:'Method not allowed.'}, 405);
      if (!origins.has(origin)) return reply({error:'Open the game to submit a score.'}, 403);
      if (!request.headers.get('content-type')?.startsWith('application/json')) return reply({error:'JSON required.'}, 415);
      if (Number(request.headers.get('content-length')) > 2048) return reply({error:'Request too large.'}, 413);
      const text = await request.text();
      if (text.length > 2048) return reply({error:'Request too large.'}, 413);
      let body;
      try { body = JSON.parse(text); } catch { return reply({error:'Invalid JSON.'}, 400); }
      if (!body || typeof body !== 'object' || Array.isArray(body)) return reply({error:'Invalid request.'}, 400);
      if (path === '/api/runs') {
        if (!integer(body.startingLevel, 1, 12)) return reply({error:'Invalid starting level.'}, 400);
        const id = randomUUID();
        await repo.start(id, body.startingLevel);
        return reply({runId:id}, 201);
      }
      if (path !== '/api/scores') return reply({error:'Not found.'}, 404);
      const name = typeof body.name === 'string' ? body.name.trim().replace(/\s+/g,' ') : '';
      if (!/^[\p{L}\p{N} _.'-]{1,18}$/u.test(name)) return reply({error:'Use 1–18 letters, numbers, spaces or simple punctuation for your pilot name.'}, 400);
      if (!uuid.test(body.runId || '') || !integer(body.score,1,99999999) || !integer(body.level,1,10000)) return reply({error:'Invalid score.'}, 400);
      const run = await repo.run(body.runId);
      if (!run) return reply({error:'This run was not registered. Start a new game.'}, 400);
      const seconds = Math.max(0,(now() - new Date(run.started_at).getTime())/1000);
      if (seconds > 172800 || body.score > 100000 + seconds*20000 || body.level < run.starting_level || body.level > run.starting_level + Math.ceil(seconds/8)+1) return reply({error:'Score does not match this run.'}, 400);
      const saved = await repo.save({id:randomUUID(), runId:body.runId, name, score:body.score, level:body.level, startingLevel:run.starting_level});
      return reply({saved, scores:await repo.top()}, 201);
    } catch (error) {
      console.error('Score service unavailable', error?.code || 'storage-error');
      return reply({error:'The score service is temporarily unavailable. Please retry.'}, 503);
    }
  };
}
