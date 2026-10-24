-- Remove only the automated verification run; never player scores.
DELETE FROM sg_scores WHERE run_id = '856285eb-b9de-409f-b76d-eb9aaa1614de' AND name = 'QA FLIGHT';
DELETE FROM sg_runs WHERE id = '856285eb-b9de-409f-b76d-eb9aaa1614de' AND NOT EXISTS (SELECT 1 FROM sg_scores WHERE run_id = sg_runs.id);
