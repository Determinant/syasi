(+)
(-)
(*)
(/)
(+ 0)
(- 0)
(* 0)
(/ 0)
(<)
(>)
(=)
(< 1)
(> 1)
(= 1)
(+ 0 . 0)
(+ . 0)
(- 0 . 0)
(- . 0)
(< 0 . 0)
(< . 0)

(+ 0 'a)
(- 0 'a)
(* 0 'a)
(/ 0 'a)
(< #f)
(> #f)
(= #f)

(exact?)
(exact? 'a)
(exact? 1 2)
(exact? . 0)
(exact? 0 . 0)

(inexact?)
(inexact? 'b)
(inexact? 1 2)
(inexact? . 0)
(inexact? 0 . 0)

(not)
(not 1 2)
(not 1)
(not #f)
(not '())
(not . 0)
(not 0 . 0)

(boolean?)
(boolean? 1 2)
(boolean? 1)
(boolean? #t)
(boolean? . 0)
(boolean? 0 . 0)

(pair?)
(pair? 1 2)
(pair? '())
(pair? (cons 1 2))
(pair? '(3 . 4))
(pair? 3)
(pair? . 0)
(pair? 0 . 0)

(cons)
(cons 1)
(cons 1 2 3)
(cons 'a '())
(cons . 0)
(cons 0 . 0)


(define t (cons 'a '()))

(car)
(car 1)
(car 1 2)
(car '())
(car t)
(car . 0)
(car 0 . 0)

(cdr)
(cdr 1)
(cdr 1 2)
(cdr '())
(cdr t)
(cdr . 0)
(cdr 0 . 0)

(set-car!)
(set-car! 1)
(set-car! 1 2)
(set-car! t '())
t
(set-car! . 0)
(set-car! 0 . 0)

(set-cdr!)
(set-cdr! 1)
(set-cdr! 1 2)
(set-cdr! t 'a)
t
(set-cdr! . 0)
(set-cdr! 0 . 0)

(null?)
(null? 1 2)
(null? 1)
(null? '())
(null? #f)
(null? . 0)
(null? 0 . 0)

(list?)
(list? 1 2)
(list? '())
(list? t)
(set-cdr! t '())
t
(list? t)
(list? . 0)
(list? 0 . 0)

(list)
(list 1)
(list 1 2)
(list . 0)
(list 0 . 0)

(display)
(display 1 2)
(display . 0)
(display 0 . 0)
(display t)