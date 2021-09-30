SELECT T.name,COUNT(*)
FROM Trainer AS T ,CatchedPokemon AS C,Gym AS G
WHERE  T.id=C.owner_id AND G.leader_id = T.id 
GROUP BY T.id
ORDER BY T.name;
