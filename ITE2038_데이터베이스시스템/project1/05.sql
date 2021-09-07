SELECT DISTINCT T.name
FROM Trainer AS T
WHERE T.id <> ALL (
  SELECT leader_id
  FROM GYM
)
ORDER BY T.name;