SELECT T.name
FROM Trainer  T,CatchedPokemon AS C
WHERE T.id = C.owner_id
GROUP BY T.name
HAVING COUNT(*) >2
ORDER BY COUNT(*) DESC;          