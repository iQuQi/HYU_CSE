SELECT T.name,AVG(level)
FROM Trainer AS T,CatchedPokemon AS C,Gym AS G
WHERE C.owner_id=T.id AND T.id=G.leader_id 
GROUP BY T.name
ORDER BY T.name;