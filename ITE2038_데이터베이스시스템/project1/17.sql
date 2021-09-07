SELECT COUNT(DISTINCT pid)
FROM Pokemon AS P ,Trainer AS T ,CatchedPokemon AS C
WHERE  T.id=C.owner_id AND P.id=C.pid AND T.hometown = 'Sangnok City';