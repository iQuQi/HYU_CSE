SELECT Ct.name , Cp.nickname
FROM Trainer AS T,CatchedPokemon AS Cp,City AS Ct
WHERE T.id = Cp.owner_id AND Ct.name=T.hometown 
AND Cp.level >= ALL (
  SELECT Cp2.level
  FROM CatchedPokemon AS Cp2,City AS Ct2,Trainer AS T2
  WHERE Ct.name=Ct2.name AND T2.hometown=Ct2.name AND Cp2.owner_id=T2.id
)
ORDER BY Ct.name;
