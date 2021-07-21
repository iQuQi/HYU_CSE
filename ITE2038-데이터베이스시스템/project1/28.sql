SELECT DISTINCT T.name,AVG(level)
FROM Trainer AS T,Pokemon AS P,CatchedPokemon AS C
WHERE T.id=C.owner_id AND P.id=C.pid AND type IN('Normal','Electric')
GROUP BY T.name
ORDER BY AVG(level);
