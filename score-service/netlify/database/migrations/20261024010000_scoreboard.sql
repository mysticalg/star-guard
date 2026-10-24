CREATE TABLE sg_runs (
  id UUID PRIMARY KEY,
  starting_level INTEGER NOT NULL CHECK (starting_level BETWEEN 1 AND 10),
  started_at TIMESTAMPTZ NOT NULL DEFAULT NOW()
);
CREATE TABLE sg_scores (
  id UUID PRIMARY KEY,
  run_id UUID NOT NULL UNIQUE REFERENCES sg_runs(id),
  name VARCHAR(18) NOT NULL,
  score INTEGER NOT NULL CHECK (score BETWEEN 1 AND 99999999),
  level INTEGER NOT NULL CHECK (level BETWEEN 1 AND 10000),
  starting_level INTEGER NOT NULL CHECK (starting_level BETWEEN 1 AND 10),
  created_at TIMESTAMPTZ NOT NULL DEFAULT NOW()
);
CREATE INDEX sg_scores_ranking ON sg_scores (score DESC, level DESC, created_at ASC);
