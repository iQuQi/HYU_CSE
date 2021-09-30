SELECT P1.name
 FROM Evolution AS E,Pokemon AS P1,Pokemon AS P2
 WHERE E.before_id=P1.id AND E.after_id=P2.id
 AND P2.id<P1.id
 ORDER BY P1.name;