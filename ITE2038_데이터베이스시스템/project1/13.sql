SELECT P.name,P.id
FROM CatchedPokemon AS C,Pokemon AS P,Trainer AS T
WHERE C.pid=P.id AND T.id=C.owner_id AND T.hometown = 'Sangnok City' 
ORDER BY P.id;