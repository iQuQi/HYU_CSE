SELECT  T.name
FROM Evolution AS E1,CatchedPokemon AS C,Trainer AS T,Pokemon AS P
WHERE  E1.after_id <> ALL ( SELECT E2.before_id FROM Evolution AS E2 )
AND P.id=C.pid AND T.id=C.owner_id AND E1.after_id=P.id 
ORDER BY T.name;