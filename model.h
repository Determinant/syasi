#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <list>
#include <map>
#include <vector>

using std::list;
using std::string;
using std::map;
using std::vector;

// the range of unsigned char is enough for these types
typedef unsigned char ClassType;  
typedef unsigned char NumLvl;      

const int CLS_RET_ADDR = 1 << 0;
const int CLS_EVAL_OBJ = 1 << 1;
const int CLS_PAR_BRA = 1 << 2;

const int CLS_SIM_OBJ = 1 << 0;
const int CLS_CONS_OBJ = 1 << 1;
const int CLS_SYM_OBJ = 1 << 2;
const int CLS_OPT_OBJ = 1 << 3;
const int CLS_NUM_OBJ = 1 << 4;
const int CLS_BOOL_OBJ = 1 << 5;
const int CLS_CHAR_OBJ = 1 << 6;
const int CLS_STR_OBJ = 1 << 7;
const int CLS_VECT_OBJ = 1 << 8;


#define TO_PAIR(ptr) \
    (static_cast<Pair*>(ptr))

/** @class FrameObj
 * Objects that can be held in the evaluation stack
 */
class FrameObj {
    protected:
        /**
         * Report the type of the FrameObj, which can avoid the use of
         * dynamic_cast to improve efficiency. See the constructor for detail
         */
        ClassType ftype;   
    public:
        /**
         * Construct an EvalObj
         * @param ftype the type of the FrameObj (CLS_EVAL_OBJ for an EvalObj,
         * CLS_RET_ADDR for a return address)
         */
        FrameObj(ClassType ftype);
        virtual ~FrameObj() {}
        /**
         * Tell whether the object is a return address, according to ftype
         * @return true for yes
         */
        bool is_ret_addr();
        /**
         * Tell whether the object is a bracket, according to ftype
         * @return true for yes
         */
        bool is_parse_bracket();
};


class Pair;
/** @class EvalObj
 * Objects that represents a value in evaluation
 */
class EvalObj : public FrameObj {
    protected:
        /**
         * Report the type of the EvalObj, which can avoid the use of
         * dynamic_cast to improve efficiency. See the constructor for detail
         */
        int otype;
    public:
        /**
         * Construct an EvalObj
         * @param otype the type of the EvalObj (CLS_CONS_OBJ for a
         * construction, CLS_SIM_OBJ for a simple object), which defaults to
         * CLS_SIM_OBJ
         */
        EvalObj(int otype = CLS_SIM_OBJ);
        /** Check if the object is a simple object (instead of a call
         * invocation) 
         * @return true if the object is not a construction (Pair)
         * */
        bool is_simple_obj();
        /** Check if the object is a symobl */
        bool is_sym_obj();
        /** Check if the object is an operator */
        bool is_opt_obj();
        /** Check if the object is a Pair */
        bool is_cons_obj();
        /** Check if the object is a number */
        bool is_num_obj();
        /** Check if the object is a boolean */
        bool is_bool_obj();
        ClassType get_otype();
        virtual void prepare(Pair *pc);
        /** Any EvalObj has its external representation */
        virtual string ext_repr();
        /** Always true for all EvalObjs except BoolObj */
        virtual bool is_true();         
};

/** @class Pair
 * Pair construct, which can be used to represent a list, or further
 * more, a syntax tree
 * (car . cdr) in Scheme
 */
class Pair : public EvalObj {
    public:
        EvalObj *car;                   /**< car (as in Scheme) */
        EvalObj *cdr;                      /**< cdr (as in Scheme) */
        bool skip;                      /**< Wether to skip the current branch */
        Pair* next;                     /**< The next branch in effect */

        Pair(EvalObj *car, EvalObj *cdr);  /**< Create a Pair (car . cdr) */
};

/** @class EmptyList
 * The empty list (special situation of a list)
 */
class EmptyList: public Pair {
    public:
        EmptyList();
        string ext_repr();
};

/** @class RetAddr
 * Tracking the caller's Pair pointer
 */
class RetAddr : public FrameObj {
    public:
        Pair* addr;                      /**< The return address  */
        /** Constructs a return address object which refers to the node addr in
         * the AST */
        RetAddr(Pair *addr);
};

class ReprConstructor : public FrameObj {
    virtual EvalObj *next();
};

class ExtRepr : public FrameObj {
    public:
    string repr;
};

/** @class ParseBracket
 * To indiate a left bracket when parsing, used in the parse_stack
 */
class ParseBracket : public FrameObj {
    public:
        unsigned char btype;            /**< The type of the bracket */
        /** Construct a ParseBracket object */
        ParseBracket(unsigned char btype);
};

/** @class UnspecObj
 * The "unspecified" value returned by some builtin procedures
 */
class UnspecObj: public EvalObj {
    public:
        UnspecObj();
        string ext_repr();
};

/** @class SymObj
 * Symbols
 */
class SymObj: public EvalObj {
    public:
        string val;

        SymObj(const string &);
#ifdef DEBUG
        string _debug_repr();
#endif
        string ext_repr();
};

// Everything is cons
typedef Pair ArgList;
class Environment;
class Continuation;

/** @class OptObj
 * "Operators" in general sense
 */
class OptObj: public EvalObj {
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
        virtual Pair *call(ArgList *args, Environment * &envt, 
                            Continuation * &cont, FrameObj ** &top_ptr) = 0;
};

/** @class ProcObj
 * User-defined procedures
 */
class ProcObj: public OptObj {
    public:
        /** The procedure body, a list of expressions to be evaluated */
        Pair *body;        
        /** The arguments: <list> | var1 ... | var1 var2 ... . varn */
        EvalObj *params;
        /** Pointer to the environment */
        Environment *envt;

        /** Conctructs a ProcObj */
        ProcObj(Pair *body, Environment *envt, EvalObj *params);
        Pair *call(ArgList *args, Environment * &envt,
                    Continuation * &cont, FrameObj ** &top_ptr);
        string ext_repr();
};

/** @class SpecialOptObj
 * Special builtin syntax (`if`, `define`, `lambda`, etc.)
 */
class SpecialOptObj: public OptObj {
    protected:
        string name;
    public:
        SpecialOptObj(string name);
};

typedef EvalObj* (*BuiltinProc)(ArgList *, const string &);
/** @class BuiltinProcObj
 * Wrapping class for builtin procedures (arithmetic operators, etc.)
 */
class BuiltinProcObj: public OptObj {
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
        Pair *call(ArgList *args, Environment * &envt,
                    Continuation * &cont, FrameObj ** &top_ptr);
        string ext_repr();
};

/** @class BoolObj
 * Booleans
 */
class BoolObj: public EvalObj {
    public:
        bool val;                       /**< true for \#t, false for \#f */ 
        BoolObj(bool);                  /**< Converts a C bool value to a BoolObj*/
        bool is_true();                 /**< Override EvalObj `is_true()` */
        string ext_repr();
        /** Try to construct an BoolObj object 
         * @return NULL if failed
         */
        static BoolObj *from_string(string repr);
};

/** @class NumObj
 * The top level abstract of numbers
 */

class NumObj: public EvalObj {
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
        virtual bool lt(NumObj *r) = 0;
        virtual bool gt(NumObj *r) = 0;
        virtual bool eq(NumObj *r) = 0;
};

/** @class StrObj
 * String support
 */
class StrObj: public EvalObj {
    public:
        string str;
        
        /** Construct a string object */
        StrObj(string str);
        /** Try to construct an StrObj object 
         * @return NULL if failed
         */
        static StrObj *from_string(string repr);
        string ext_repr();
};

/** @class CharObj
 * Character type support
 */
class CharObj: public EvalObj {
    public:
        char ch;
        
        /** Construct a string object */
        CharObj(char ch);
        /** Try to construct an CharObj object 
         * @return NULL if failed
         */
        static CharObj *from_string(string repr);
        string ext_repr();
};


typedef vector<EvalObj*> EvalObjVec;
/**
 * @class VecObj
 * Vector support (currently a wrapper of STL vector)
 */
class VecObj: public EvalObj {
    private:
        EvalObjVec vec;
    public:
        /** Construct a vector object */
        VecObj();
        /** Resize the vector */
        void resize(int new_size);
        /** Add a new element to the rear */
        void push_back(EvalObj *new_elem);
};

typedef map<string, EvalObj*> Str2EvalObj;
/** @class Environment
 * The environment of current evaluation, i.e. the local variable binding 
 */
class Environment {
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
};

/** @class Continuation
 * Save the registers and necessary information when a user-defined call is
 * being made (Behave like a stack frame in C). When the call has accomplished,
 * the system will restore all the registers according to the continuation.
 */
class Continuation {
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
};

#endif
