SELECT C.nickname
FROM CatchedPokemon AS C
WHERE C.level>=50 AND C.owner_id >=6
ORDER BY C.nickname;