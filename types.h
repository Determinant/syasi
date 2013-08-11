#ifndef TYPES_H
#define TYPES_H

#include "model.h"
#include <string>
#include <list>
#include <map>
#include <vector>
#include <set>
#include <gmpxx.h>

using std::list;
using std::string;
using std::map;
using std::vector;
using std::set;

const int CLS_OPT_OBJ = 1 << 3;
const int CLS_PROM_OBJ = 1 << 9;

const int CLS_SYM_OBJ = 1 << 2;
const int CLS_NUM_OBJ = 1 << 4;
const int CLS_BOOL_OBJ = 1 << 5;
const int CLS_CHAR_OBJ = 1 << 6;
const int CLS_STR_OBJ = 1 << 7;
const int CLS_VECT_OBJ = 1 << 8;

static const int NUM_LVL_COMP = 0;
static const int NUM_LVL_REAL = 1;
static const int NUM_LVL_RAT = 2;
static const int NUM_LVL_INT = 3;

typedef set<EvalObj*> EvalObjAddrHash;
typedef vector<EvalObj*> EvalObjVec;
typedef map<string, EvalObj*> Str2EvalObj;
typedef EvalObj* (*BuiltinProc)(Pair *, const string &);

class PairReprCons;
/** @class Pair
 * Pair construct, which can be used to represent a list, or further
 * more, a syntax tree
 * (car . cdr) in Scheme
 */
class Pair : public EvalObj {/*{{{*/
    public:
        EvalObj *car;                   /**< car (as in Scheme) */
        EvalObj *cdr;                      /**< cdr (as in Scheme) */
        Pair* next;                     /**< The next branch in effect */

        Pair(EvalObj *car, EvalObj *cdr);  /**< Create a Pair (car . cdr) */
        ReprCons *get_repr_cons();
};/*}}}*/

/** @class EmptyList
 * The empty list (special situation of a list)
 */
class EmptyList: public Pair {/*{{{*/
    public:
        EmptyList();
        ReprCons *get_repr_cons();
};/*}}}*/

/** @class RetAddr
 * Tracking the caller's Pair pointer
 */
class RetAddr : public FrameObj {/*{{{*/
    public:
        Pair* addr;                      /**< The return address  */
        /** Constructs a return address object which refers to the node addr in
         * the AST */
        RetAddr(Pair *addr);
};/*}}}*/

class ReprCons {/*{{{*/
    public:
        EvalObj *ori;
        bool done;
        string repr;
        ReprCons(bool done, EvalObj *ori = NULL);
        virtual EvalObj *next(const string &prev) = 0;
};/*}}}*/

class ReprStr : public ReprCons {/*{{{*/
    public:
        ReprStr(string repr);
        EvalObj *next(const string &prev);
};/*}}}*/

class PairReprCons : public ReprCons {/*{{{*/
    private:
        int state;
        EvalObj *ptr;
    public:
        PairReprCons(Pair *ptr, EvalObj *ori);
        EvalObj *next(const string &prev);
};/*}}}*/

class VecObj;
class VectReprCons : public ReprCons {/*{{{*/
    private:
        VecObj *ptr;
        size_t idx;
    public:
        VectReprCons(VecObj *ptr, EvalObj *ori);
        EvalObj *next(const string &prev);
};/*}}}*/

/** @class ParseBracket
 * To indiate a left bracket when parsing, used in the parse_stack
 */
class ParseBracket : public FrameObj {/*{{{*/
    public:
        unsigned char btype;            /**< The type of the bracket */
        /** Construct a ParseBracket object */
        ParseBracket(unsigned char btype);
};/*}}}*/

/** @class UnspecObj
 * The "unspecified" value returned by some builtin procedures
 */
class UnspecObj: public EvalObj {/*{{{*/
    public:
        UnspecObj();
        ReprCons *get_repr_cons();
};/*}}}*/

/** @class SymObj
 * Symbols
 */
class SymObj: public EvalObj {/*{{{*/
    public:
        string val;

        SymObj(const string &);
        ReprCons *get_repr_cons();
};/*}}}*/

// Everything is cons
class Environment;
class Continuation;

/** @class OptObj
 * "Operators" in general sense
 */
class OptObj: public EvalObj {/*{{{*/
    public:
        OptObj();
        /**
         * The function is called when an operation is needed.
         * @param args The argument list (the first one is the opt itself)
         * @param envt The current environment (may be modified)
         * @param cont The current continuation (may be modified)
         * @param top_ptr Pointing to the top of the stack (may be modified)
         * @return New value for pc register
         */
        virtual Pair *call(Pair *args, Environment * &envt,
                            Continuation * &cont, FrameObj ** &top_ptr) = 0;
};/*}}}*/

/** @class ProcObj
 * User-defined procedures
 */
class ProcObj: public OptObj {/*{{{*/
    public:
        /** The procedure body, a list of expressions to be evaluated */
        Pair *body;
        /** The arguments: <list> | var1 ... | var1 var2 ... . varn */
        EvalObj *params;
        /** Pointer to the environment */
        Environment *envt;

        /** Conctructs a ProcObj */
        ProcObj(Pair *body, Environment *envt, EvalObj *params);
        Pair *call(Pair *args, Environment * &envt,
                    Continuation * &cont, FrameObj ** &top_ptr);
        ReprCons *get_repr_cons();
};/*}}}*/

/** @class SpecialOptObj
 * Special builtin syntax (`if`, `define`, `lambda`, etc.)
 */
class SpecialOptObj: public OptObj {/*{{{*/
    protected:
        string name;
    public:
        SpecialOptObj(string name);
};/*}}}*/

/** @class BuiltinProcObj
 * Wrapping class for builtin procedures (arithmetic operators, etc.)
 */
class BuiltinProcObj: public OptObj {/*{{{*/
    private:
        /** The function that tackle the inputs in effect */
        BuiltinProc handler;
        string name;
    public:
        /**
         * Make a BuiltinProcObj which invokes proc when called
         * @param proc the actual handler
         * @param name the name of this built-in procedure
         */
        BuiltinProcObj(BuiltinProc proc, string name);
        Pair *call(Pair *args, Environment * &envt,
                    Continuation * &cont, FrameObj ** &top_ptr);
        ReprCons *get_repr_cons();
};/*}}}*/

/** @class BoolObj
 * Booleans
 */
class BoolObj: public EvalObj {/*{{{*/
    public:
        bool val;                       /**< true for \#t, false for \#f */
        BoolObj(bool);                  /**< Converts a C bool value to a BoolObj*/
        bool is_true();                 /**< Override EvalObj `is_true()` */
        ReprCons *get_repr_cons();
        /** Try to construct an BoolObj object
         * @return NULL if failed
         */
        static BoolObj *from_string(string repr);
};/*}}}*/

/** @class NumObj
 * The top level abstract of numbers
 */

class NumObj: public EvalObj {/*{{{*/
    protected:
        /** True if the number is of exact value */
        bool exactness;
    public:
        /** The level of the specific number. The smaller the level
         * is, the more generic that number is.
         */
        NumLvl level;

        /**
         * Construct a general Numeric object
         */
        NumObj(NumLvl level, bool _exactness);
        bool is_exact();
        virtual NumObj *convert(NumObj *r) = 0;
        virtual NumObj *add(NumObj *r) = 0;
        virtual NumObj *sub(NumObj *r) = 0;
        virtual NumObj *mul(NumObj *r) = 0;
        virtual NumObj *div(NumObj *r) = 0;
        virtual NumObj *abs();

        virtual bool lt(NumObj *r);
        virtual bool gt(NumObj *r);
        virtual bool le(NumObj *r);
        virtual bool ge(NumObj *r);
        virtual bool eq(NumObj *r) = 0;
};/*}}}*/

/** @class StrObj
 * String support
 */
class StrObj: public EvalObj {/*{{{*/
    public:
        string str;

        /** Construct a string object */
        StrObj(string str);
        /** Try to construct an StrObj object
         * @return NULL if failed
         */
        static StrObj *from_string(string repr);
        bool lt(StrObj *r);
        bool gt(StrObj *r);
        bool le(StrObj *r);
        bool ge(StrObj *r);
        bool eq(StrObj *r);
        ReprCons *get_repr_cons();
};/*}}}*/

/** @class CharObj
 * Character type support
 */
class CharObj: public EvalObj {/*{{{*/
    public:
        char ch;

        /** Construct a string object */
        CharObj(char ch);
        /** Try to construct an CharObj object
         * @return NULL if failed
         */
        static CharObj *from_string(string repr);
        ReprCons *get_repr_cons();
};/*}}}*/


/**
 * @class VecObj
 * Vector support (currently a wrapper of STL vector)
 */
class VecObj: public EvalObj {/*{{{*/
    public:
        EvalObjVec vec;
        /** Construct a vector object */
        VecObj();
        size_t get_size();
        EvalObj *get_obj(int idx);
        /** Resize the vector */
        void resize(int new_size);
        /** Add a new element to the rear */
        void push_back(EvalObj *new_elem);
        ReprCons *get_repr_cons();
};/*}}}*/

/**
 * @class PromObj
 * Promise support (partial)
 */
class PromObj: public EvalObj {/*{{{*/
    private:
        Pair *entry;
        EvalObj *mem;
    public:
        PromObj(EvalObj *exp);
        Pair *get_entry();
        EvalObj *get_mem();
        void feed_mem(EvalObj *res);
        ReprCons *get_repr_cons();
};/*}}}*/

/** @class Environment
 * The environment of current evaluation, i.e. the local variable binding
 */
class Environment {/*{{{*/
    private:
        Environment *prev_envt; /**< Pointer to the upper-level environment */
        Str2EvalObj binding;    /**< Store all pairs of identifier and its
                                  corresponding obj */
    public:
        /** Create an runtime environment
         * @param prev_envt the outer environment
         */
        Environment(Environment *prev_envt);
        /** Add a binding entry which binds sym_obj to eval_obj
         * @param def true to force the assignment
         * @return when def is set to false, this return value is true iff. the
         * assignment carried out successfully
         */
        bool add_binding(SymObj *sym_obj, EvalObj *eval_obj, bool def = true);
        /** Extract the corresponding EvalObj if obj is a SymObj, or just
         * simply return obj as it is
         * @param obj the object as request
         * */
        EvalObj *get_obj(EvalObj *obj);
};/*}}}*/

/** @class Continuation
 * Save the registers and necessary information when a user-defined call is
 * being made (Behave like a stack frame in C). When the call has accomplished,
 * the system will restore all the registers according to the continuation.
 */
class Continuation {/*{{{*/
    public:
        /** Linking the previous continuation on the chain */
        Continuation *prev_cont;
        Environment *envt;  /**< The saved envt */
        Pair *pc;           /**< The saved pc */
        /** Pointing to the current expression that is being evaluated.
         * When its value goes to empty_list, the call is accomplished.
         */
        Pair *proc_body;

        /** Create a continuation */
        Continuation(Environment *envt, Pair *pc, Continuation *prev_cont,
                Pair *proc_body);
};/*}}}*/

/** @class InexactNumObj
 * Inexact number implementation (using doubles)
 */
class InexactNumObj: public NumObj {/*{{{*/
    public:
        InexactNumObj(NumLvl level);
};/*}}}*/

/** @class CompNumObj
 * Complex numbers
 */
class CompNumObj: public InexactNumObj {/*{{{*/
    public:
        double real, imag;

        /** Construct a complex number */
        CompNumObj(double _real, double _imag);
        /** Try to construct an CompNumObj object
         * @return NULL if failed
         */
        static CompNumObj *from_string(string repr);
        /** Convert to a complex number from other numeric types */
        CompNumObj *convert(NumObj* obj);

        NumObj *add(NumObj *r);
        NumObj *sub(NumObj *r);
        NumObj *mul(NumObj *r);
        NumObj *div(NumObj *r);
        bool eq(NumObj *r);
        ReprCons *get_repr_cons();
};/*}}}*/

/** @class RealNumObj
 * Real numbers
 */
class RealNumObj: public InexactNumObj {/*{{{*/
    public:
        double real;
        /** Construct a real number */
        RealNumObj(double _real);
        /** Try to construct an RealNumObj object
         * @return NULL if failed
         */
        static RealNumObj *from_string(string repr);
        /** Convert to a real number from other numeric types */
        RealNumObj *convert(NumObj* obj);

        NumObj *add(NumObj *r);
        NumObj *sub(NumObj *r);
        NumObj *mul(NumObj *r);
        NumObj *div(NumObj *r);
        NumObj *abs();
        bool lt(NumObj *r);
        bool gt(NumObj *r);
        bool le(NumObj *r);
        bool ge(NumObj *r);
        bool eq(NumObj *r);
        ReprCons *get_repr_cons();

};/*}}}*/


/** @class ExactNumObj
 * Exact number implementation (using gmp)
 */
class ExactNumObj: public NumObj {/*{{{*/
    public:
        ExactNumObj(NumLvl level);
};/*}}}*/

/** @class RatNumObj
 * Rational numbers
 */
class RatNumObj: public ExactNumObj {/*{{{*/
    public:
#ifndef GMP_SUPPORT
        int a, b;
        /** Construct a rational number */
        RatNumObj(int _a, int _b);
#else
        mpq_class val;
        RatNumObj(mpq_class val);
#endif
        /** Try to construct an RatNumObj object
         * @return NULL if failed
         */
        static RatNumObj *from_string(string repr);
        /** Convert to a Rational number from other numeric types */
        RatNumObj *convert(NumObj* obj);

        NumObj *add(NumObj *r);
        NumObj *sub(NumObj *r);
        NumObj *mul(NumObj *r);
        NumObj *div(NumObj *r);
        NumObj *abs();
        bool lt(NumObj *r);
        bool gt(NumObj *r);
        bool le(NumObj *r);
        bool ge(NumObj *r);
        bool eq(NumObj *r);
        ReprCons *get_repr_cons();
};/*}}}*/

/** @class IntNumObj
 * Integers
 */
class IntNumObj: public ExactNumObj {/*{{{*/
    public:
#ifndef GMP_SUPPORT
        int val;
        /** Construct a integer */
        IntNumObj(int val);
        int get_i();
#else
        mpz_class val;
        /** Construct a integer */
        IntNumObj(mpz_class val);
        int get_i();
#endif
        /** Try to construct an IntNumObj object
         * @return NULL if failed
         */
        static IntNumObj *from_string(string repr);
        /** Convert to a integer from other numeric types */
        IntNumObj *convert(NumObj* obj);

        NumObj *add(NumObj *r);
        NumObj *sub(NumObj *r);
        NumObj *mul(NumObj *r);
        NumObj *div(NumObj *r);
        NumObj *abs();
        NumObj *mod(NumObj *r);
        NumObj *rem(NumObj *r);
        NumObj *quo(NumObj *r);
        NumObj *gcd(NumObj *r);
        NumObj *lcm(NumObj *r);
        bool lt(NumObj *r);
        bool gt(NumObj *r);
        bool le(NumObj *r);
        bool ge(NumObj *r);
        bool eq(NumObj *r);
        ReprCons *get_repr_cons();
};/*}}}*/

bool is_zero(double);
#endif