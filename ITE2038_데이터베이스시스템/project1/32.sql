SELECT P1.name
FROM Pokemon AS P1
WHERE P1.id NOT IN(
  SELECT P2.id
  FROM CatchedPokemon AS C,Pokemon AS P2
  WHERE C.pid=P2.id)
ORDER BY P1.name;