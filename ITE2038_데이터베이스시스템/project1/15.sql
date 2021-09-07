SELECT T.id,COUNT(*) AS 'catchNum'
FROM Trainer AS T , CatchedPokemon AS C
WHERE T.id=C.owner_id 
GROUP BY T.id
HAVING COUNT(*) >= ALL (
  SELECT COUNT(*)
  FROM Trainer AS T , CatchedPokemon AS C
  WHERE T.id=C.owner_id 
  GROUP BY T.id
  )
ORDER BY T.id;