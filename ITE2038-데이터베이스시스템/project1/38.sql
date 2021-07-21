SELECT DISTINCT P.name
FROM Evolution AS E1,Pokemon AS P
WHERE  E1.after_id <> ALL ( SELECT E2.before_id FROM Evolution AS E2 )
AND E1.after_id=P.id 
ORDER BY P.name;
