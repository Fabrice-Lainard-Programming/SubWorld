 


(deftemplate enemy
 (slot id)
 (slot x)
 (slot y)
 (slot z)
 (slot distance))

(deftemplate resource
 (slot id)
 (slot x)
 (slot y)
 (slot z)
 (slot distance))

(deftemplate team
 (slot id)
 (slot tactical-activity)
 (multislot units))

(deftemplate situation_awareness
 (slot level) 
 (slot strategy)
 (multislot team-response) 
 (slot enemy-is-near (default 0)))

(deffacts startup "startup" 

    (situation_awareness 
	(level strategical) 
	(strategy protection) 
	(friend-territory 0 0 0)	; point , radius
	(enemy-territory 0 0 0)		; point radius
	(enemy-is-near 0))

    (team (id 0) (tactical-activity waiting) (units 1 2 3))

    (enemy (id 1) (x 10) (y 10) (z 10) (distance 100))
    (enemy (id 2) (x 10) (y 10) (z 10) (distance 100))

    (resource (id 3) (x 10) (y 10) (z 10) (distance 1100))

    
)

 
  

(defrule adapt_strategy 
  ?sa<-(situation_awareness (level strategical)(strategy protection) (enemy-is-near 1) (team-response))
  ?team<- (team (id ?teamid) (tactical-activity waiting))
 =>
 (modify ?sa (team-response ?teamid))
 (modify ?team (tactical-activity protecting-zone))
 (printout t  " SA: need to determine tactical approach" crlf)
 )



 
(defrule enemy-approach 
  (enemy (id ?id) (distance ?d&:(< ?d 200)))
  ?f1<-(situation_awareness (level strategical)(enemy-is-near 0) )
 =>
 (modify ?f1 (enemy-is-near 1))
 (printout t  " SA: Enemy is near : " ?id " is at " ?d "m" crlf)
 )


 
(defrule compute-enemy-zone
  (enemy (id ?id) (x ?x) (y ?y) (z ?z) (distance ?d&:(< ?d 200)))
  ?f1<-(situation_awareness (level strategical)(enemy-is-near 0) (enemy-territory ?ex ?ey ?ez))
 =>
 (bind ?ed (distance ?x ?y ?ex ?ey))
 (modify ?f1 (enemy-is-near 1))
 (printout t  " SA: Enemy is near : " ?id " is at " ?d "m" crlf)
 )

 

(deffunction distance(?x1 ?y1 ?x2 ?y2)
	(bind ?dx (- ?x1 ?x2))
	(bind ?dy (- ?y1 ?y2))
  	(return (sqrt (+ (* ?dx ?dx) (* ?dy ?dy))))
)


(distance 1 1 20 1)

(stretch-zone 1 1 10 1 9)

(deffunction stretch-zone(?z1 ?z2 ?zr ?x ?y)
	(bind ?d  (distance ?z1 ?z2 ?x ?y))
 	(printout t  "dist: " ?d crlf)
	(if (< ?d ?zr ) 
		then
		(return ?zr)
		else 
		(return ?zr))
(return 0)
)
	

(deffunction stretch-zone(?z1 ?z2 ?zr ?x ?y)
	 
(return 0)
)

  (if (or (not (integerp ?a)) (< ?a 0)) then
        (printout t "Factorial Error!" crlf)
     else
        (if (= ?a 0) then
            1
         else
            (* ?a (factorial (- ?a 1))))))


(if <expression>
 then
 <action>*
 [else
 <action>*])

;//////////

(defrule calculate-distance
(point ?x1 ?y1)
(point ?x2 ?y2)
= >
(assert (distance (distance ?x1 ?y1 ?x2 ?y2)))
)
 