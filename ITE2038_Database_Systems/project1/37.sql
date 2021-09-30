SELECT T.name,SUM(level)
FROM CatchedPokemon AS C,Pokemon AS P,Trainer AS T
WHERE  C.pid=P.id AND T.id=C.owner_id 
GROUP BY T.name
HAVING SUM(level)>=ALL(
  SELECT SUM(level)
  FROM CatchedPokemon AS C,Pokemon AS P,Trainer AS T
  WHERE  C.pid=P.id AND T.id=C.owner_id 
  GROUP BY T.name) 
ORDER BY T.name;