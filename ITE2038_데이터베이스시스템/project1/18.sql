SELECT AVG(level)
FROM Gym AS G,Trainer AS T ,CatchedPokemon AS C
WHERE  T.id=C.owner_id AND T.id=G.leader_id ;
