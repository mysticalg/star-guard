import { getDatabase } from '@netlify/database';
import { createScoreHandler } from '../../lib/score-api.mjs';

export default async (request: Request) => {
  const db = getDatabase();
  const repo = {
    top: () => db.sql`SELECT id, name, score, level, starting_level, created_at FROM sg_scores ORDER BY score DESC, level DESC, created_at ASC LIMIT 20`,
    start: (id: string, level: number) => db.sql`INSERT INTO sg_runs (id, starting_level) VALUES (${id}, ${level})`,
    run: async (id: string) => (await db.sql`SELECT started_at, starting_level FROM sg_runs WHERE id=${id}`)[0],
    save: async (s: {id:string,runId:string,name:string,score:number,level:number,startingLevel:number}) => {
      await db.sql`INSERT INTO sg_scores (id,run_id,name,score,level,starting_level) VALUES (${s.id},${s.runId},${s.name},${s.score},${s.level},${s.startingLevel}) ON CONFLICT (run_id) DO NOTHING`;
      return (await db.sql`SELECT id,name,score,level,starting_level,created_at FROM sg_scores WHERE run_id=${s.runId}`)[0];
    }
  };
  return createScoreHandler(repo)(request);
};
export const config = {
  path:['/api/scores','/api/runs'],
  rateLimit:{action:'rate_limit',aggregateBy:['ip','domain'],windowSize:60,windowLimit:60}
};
