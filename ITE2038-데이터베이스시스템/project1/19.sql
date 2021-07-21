SELECT COUNT(DISTINCT P.type)
FROM Gym AS G,Trainer AS T ,CatchedPokemon AS C,Pokemon AS P
WHERE  T.id=C.owner_id AND T.id=G.leader_id AND C.pid=P.id
AND T.hometown = 'Sangnok City';