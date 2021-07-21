SELECT type
FROM Evolution AS E1,Pokemon AS P1
WHERE E1.before_id=P1.id
GROUP BY type
HAVING COUNT(*)>=3
ORDER BY type DESC;