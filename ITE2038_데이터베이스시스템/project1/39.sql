SELECT DISTINCT T.name
FROM Trainer  T,CatchedPokemon AS C1
WHERE T.id = C1.owner_id  
AND C1.nickname <> ANY(
SELECT C2.nickname
FROM CatchedPokemon AS C2
WHERE C1.pid=C2.pid AND C1.owner_id = C2.owner_id
)
ORDER BY T.name;