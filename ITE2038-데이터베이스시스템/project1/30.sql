SELECT  P1.id, P1.name,P2.name,P3.name
FROM Evolution AS E1,Evolution AS E2,Pokemon AS P1,Pokemon AS P2,Pokemon AS P3
WHERE E1.before_id=P1.id  AND E1.after_id=P2.id  AND E2.after_id=P3.id  
AND E1.after_id=E2.before_id
ORDER BY P1.id;