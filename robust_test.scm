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

(length)
(length 1 2)
(length '( 1 . 2))
(length '())
(length '( 1 2 3 ))

(append)
(append '())
(append '(1 2) 3)
(append '(1 2) '(3 4) 5)
(append '(1 2) 3 '(1))
(append '() '() '() '(1 2) 3)

(display)
(display 1 2)
(display . 0)
(display 0 . 0)
(display t)

(define)
(define x)
(define 1)
(define x x)
(define x 1 2)
(define x . 1)
(define x 1 . 2)
(define ())
(define (f))
(define (f . ) 1)
(define () 3)

(lambda)
(lambda ())
(lambda 1)
(lambda () '(1 2 3))
(lambda () 1 2 3)
(lambda #() 1)

(define src
  '(define g (lambda (x) (if (= x 5) 0 ((lambda () (display x) (g (+ x 1))))))))
src
(eval src)
(eval '(g 0))

(define f (lambda (x) (+ x x))) ;; test comments
((lambda (x y) (f 3)) 1 2)      ;; first-class procedure
; this is a single line comment
; another line
(define f (lambda (x) (lambda (y) (+ x y))))

(f 1)   ; #<procedure>

((f 1) 2) ; 3

(define g (lambda (x) (define y 2) (+ x y)))
(g 3)

((lambda () (display 2) (display 1) (+ 1 2)))
(define g (lambda (x) (if (= x 5) 0 ((lambda () (display x) (g (+ x 1)))))))
(g 0)

(define g (lambda (x)
            (if (= x 5)
              0
              ((lambda ()
                 (display x)
                 (g (+ x 1)))))))
(g 0)

(define square (lambda (x) (* x x)))

(square 2)

(define (f x y)
  ((lambda (a b)
	 (+ (* x (square a))
		(* y b)
		(* a b)))
   (+ 1 (* x y ))
   (- 1 y)))

(f 1 2)

((lambda (x + y) (+ x y)) 4 / 2) ; a classic trick

(if #t 1 ())
(if #f 1 ()) ; Error
; "Test double quotes in comments"
(display " Test double quotes outside the comments ; ;; ; ; ")

(equal? #(1 2 '()) #(1 2 '(1)))
(equal? #(1 2 '(1)) #(1 2 '(1)))
