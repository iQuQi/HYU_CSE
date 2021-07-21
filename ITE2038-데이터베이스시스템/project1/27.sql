SELECT T.name, MAX(level)
FROM CatchedPokemon AS C,Trainer AS T
WHERE T.id=C.owner_id 
GROUP BY T.id
HAVING COUNT(*)>=4
ORDER BY T.name;