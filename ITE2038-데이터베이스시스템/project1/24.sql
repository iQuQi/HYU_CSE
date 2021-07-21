SELECT Ct.name ,AVG(level)
FROM CatchedPokemon AS Cp,Trainer AS T,City AS Ct
WHERE Cp.owner_id=T.id AND T.hometown = Ct.name 
GROUP BY hometown
ORDER BY AVG(level);