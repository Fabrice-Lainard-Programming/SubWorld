
 
 

(deftemplate enemy
 (slot id)
 (slot x)
 (slot y)
 (slot z)
 (slot distance))

(deftemplate situation_awareness
 (slot level) 
 (slot enemy_is_near (default 0)))

(deffacts startup "startup" 

    (situation_awareness (level strategical) (enemy_is_near 0))

    (enemy (id 1) (x 10) (y 10) (z 10) (distance 100))
    (enemy (id 2) (x 10) (y 10) (z 10) (distance 100))
            
)
 
(defrule enemy_approach 
 (enemy (id ?i) (distance ?d))
 =>
 (println ?i " : " ?d)
 
 )


(assert (situation_awareness (level strategical) (enemy_is_near 1))