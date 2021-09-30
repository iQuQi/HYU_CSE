 SELECT P.name,level,nickname
 FROM CatchedPokemon AS C,Trainer AS T,Gym AS G,Pokemon AS P
 WHERE C.owner_id=T.id AND T.id=G.leader_id AND P.id=C.pid 
 AND nickname LIKE 'A%'
 ORDER BY P.name DESC;