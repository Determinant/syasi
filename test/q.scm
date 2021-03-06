(define (shl bits)
  (define len (vector-length bits))
  (define res (make-vector len))
  (define (copy i)
    (if (= i (- len 1))
      #t
      (and
        (vector-set! res i
                     (vector-ref bits (+ i 1)))
        (copy (+ i 1)))))
  (copy 0)
  (set! copy '())
  (vector-set! res (- len 1) #f)
  res)

(define (shr bits)
  (define len (vector-length bits))
  (define res (make-vector len))
  (define (copy i)
    (if (= i (- len 1))
      #t
      (and
        (vector-set! res (+ i 1)
                     (vector-ref bits i))
        (copy (+ i 1)))))
  (copy 0)
  (set! copy '())
  (vector-set! res 0 #f)
  res)

(define (empty-bits len) (make-vector len #f))
(define vs vector-set!)
(define vr vector-ref)
(define res 0)
(define (queen n)

  (define (search l m r step)
    (define (col-iter c)
      (if (= c n)
        #f
        (and
          (if (and (eq? (vr l c) #f)
                   (eq? (vr r c) #f)
                   (eq? (vr m c) #f))
            (and
              (vs l c #t)
              (vs m c #t)
              (vs r c #t)
              ((lambda () (search l m r (+ step 1)) #t))
              (vs l c #f)
              (vs m c #f)
              (vs r c #f))
            )
          (col-iter (+ c 1))
          )))
    (set! l (shl l))
    (set! r (shr r))
    (if (= step n)
      (set! res (+ res 1))
      (col-iter 0)))
  ;  (set! col-iter '()))

  (search (empty-bits n)
          (empty-bits n)
          (empty-bits n)
          0)
  res)

(display (queen 8))

(define shl '())
(define shr '())
(define empty-bits '())
(define res '())
(define queen '())
(set-gc-resolve-threshold! 0)   ; force cycle resolve
(display "\n")
(display (gc-status))
