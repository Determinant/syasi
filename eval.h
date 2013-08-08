#ifndef EVAL_H
#define EVAL_H
#include "model.h"

/** @class Evaluator 
 * A runtime platform of interpreting
 */
class Evaluator {
    private:
        Environment *envt;              /**< Store the current environment */
        void add_builtin_routines();    /**< Add builtin routines to the env */
    public:
        Evaluator();
        EvalObj *run_expr(Pair *prog);  /**< Interpret a program */
};

#endif
