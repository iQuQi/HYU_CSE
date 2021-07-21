SELECT DISTINCT T.name
FROM CatchedPokemon AS C,Trainer AS T
WHERE C.owner_id=T.id AND C.level<=10
ORDER BY T.name ;