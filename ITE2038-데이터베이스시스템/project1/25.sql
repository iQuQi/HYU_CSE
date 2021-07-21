SELECT P.name
FROM Pokemon AS P
WHERE P.id IN(
  SELECT c.pid
  FROM  CatchedPokemon AS C,Trainer AS T
  WHERE C.owner_id =T.id AND hometown ='Sangnok City')
  AND P.id IN(
  SELECT c.pid
  FROM  CatchedPokemon AS C,Trainer AS T
  WHERE C.owner_id =T.id AND hometown ='Brown City')
  ;