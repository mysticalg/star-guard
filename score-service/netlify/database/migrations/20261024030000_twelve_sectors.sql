ALTER TABLE sg_runs DROP CONSTRAINT sg_runs_starting_level_check;
ALTER TABLE sg_runs ADD CONSTRAINT sg_runs_starting_level_check CHECK (starting_level BETWEEN 1 AND 12);
ALTER TABLE sg_scores DROP CONSTRAINT sg_scores_starting_level_check;
ALTER TABLE sg_scores ADD CONSTRAINT sg_scores_starting_level_check CHECK (starting_level BETWEEN 1 AND 12);
