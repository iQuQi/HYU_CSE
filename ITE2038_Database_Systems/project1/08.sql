SELECT AVG(level)
FROM Trainer AS T,CatchedPokemon AS C
WHERE C.owner_id=T.id AND T.name='Red';