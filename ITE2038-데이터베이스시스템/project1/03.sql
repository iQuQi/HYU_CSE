SELECT AVG(level) 
FROM Trainer AS T,CatchedPokemon AS C,Pokemon AS P 
WHERE T.hometown = 'Sangnok City' AND T.id=C.owner_id
  AND C.pid=P.id AND P.type = 'Electric';