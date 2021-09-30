SELECT type,COUNT(*)
FROM Pokemon AS P
GROUP BY P.type
ORDER BY COUNT(*),type ;