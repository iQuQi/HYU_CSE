SELECT P1.name
FROM Pokemon AS P1
WHERE P1.type IN (
  SELECT P2.type
  FROM Pokemon P2
  GROUP BY P2.type
  HAVING COUNT(*) >= (
  SELECT NEW.typenum
  FROM (SELECT P2.type,COUNT(*) AS 'typenum'
  FROM Pokemon AS P2
  GROUP BY P2.type)AS NEW
  GROUP BY NEW.typenum
  ORDER BY NEW.typenum DESC LIMIT 1,1)

 )
ORDER BY P1.name;
