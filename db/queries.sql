SELECT * FROM readings ORDER BY id DESC LIMIT 10;
SELECT sensor, MIN(value) AS min_value, AVG(value) AS avg_value, MAX(value) AS max_value FROM readings GROUP BY sensor;
SELECT * FROM events ORDER BY t_s, id;
