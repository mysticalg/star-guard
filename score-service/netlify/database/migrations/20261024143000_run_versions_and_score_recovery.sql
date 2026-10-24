ALTER TABLE sg_runs ADD COLUMN client_version INTEGER NOT NULL DEFAULT 1;

-- A retry or restored pending result must not occupy two leaderboard positions.
CREATE UNIQUE INDEX sg_scores_same_result ON sg_scores (name, score, level);

-- User-confirmed recovery: Palpatine, 850,075 points, finished in sector 03.
-- The screenshot's score was rejected by the legacy restart/session bug.
INSERT INTO sg_runs (id, starting_level, client_version)
SELECT 'dd636069-b852-4db6-b953-9e74aa74a5aa', 1, 2
WHERE NOT EXISTS (SELECT 1 FROM sg_scores WHERE name='Palpatine' AND score=850075 AND level=3)
ON CONFLICT (id) DO NOTHING;

INSERT INTO sg_scores (id, run_id, name, score, level, starting_level)
SELECT '28dad480-a3c2-412e-becb-050dc2cdc56f', id, 'Palpatine', 850075, 3, starting_level
FROM sg_runs WHERE id='dd636069-b852-4db6-b953-9e74aa74a5aa'
ON CONFLICT DO NOTHING;
