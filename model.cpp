#include <cstdio>
#include "model.h"

FrameObj::FrameObj(ClassType _ftype) : ftype(_ftype) {}

EmptyList *empty_list = new EmptyList();

EmptyList::EmptyList() : Cons(NULL, NULL) {}

string EmptyList::ext_repr() { return string("()"); }

#ifdef DEBUG
string EmptyList::_debug_repr() { return ext_repr(); }
#endif

bool FrameObj::is_ret_addr() { 
    return ftype == CLS_RET_ADDR;
}

EvalObj::EvalObj(ClassType _otype) : FrameObj(CLS_EVAL_OBJ), otype(_otype) {}

void EvalObj::prepare(Cons *pc) {}

bool EvalObj::is_simple_obj() {
    return otype == CLS_SIM_OBJ;
}

#ifdef DEBUG
void EvalObj::_debug_print() {
    printf("mem: 0x%llX\n%s\n\n", (unsigned long long)this,
            _debug_repr().c_str());
}
#endif

bool EvalObj::is_true() {
    return true;
}

Cons::Cons(EvalObj *_car, Cons *_cdr) : 
    EvalObj(CLS_CONS_OBJ), car(_car), cdr(_cdr), skip(false), 
    next(cdr == empty_list ? NULL : cdr) {}

string Cons::ext_repr() { return string("#<Cons>"); }

#ifdef DEBUG
string Cons::_debug_repr() { return ext_repr(); }

void Cons::_debug_print() {
    printf("mem: 0x%llX (0x%llX . 0x%llX) | 0x%llX\n%s\n", 
            (unsigned long long)this,
            (unsigned long long)car,
            (unsigned long long)cdr,
            (unsigned long long)next,
    ("car: " + car -> ext_repr() + "\n" + \
     "cdr: " + cdr -> ext_repr() + "\n").c_str());
}
#endif

RetAddr::RetAddr(Cons *_addr) : FrameObj(CLS_RET_ADDR), addr(_addr) {}

#ifdef DEBUG
string RetAddr::_debug_repr() { return string("#<Return Address>"); }
#endif

UnspecObj::UnspecObj() : EvalObj() {}

string UnspecObj::ext_repr() { return string("#<Unspecified>"); }

#ifdef DEBUG
string UnspecObj::_debug_repr() { return ext_repr(); }
#endif

SymObj::SymObj(const string &str) : EvalObj(), val(str) {}

string SymObj::ext_repr() { return "#<Symbol: " + val + ">"; }

#ifdef DEBUG
string SymObj::_debug_repr() { return ext_repr(); }
#endif

OptObj::OptObj() : EvalObj() {}

ProcObj::ProcObj(ASTList *_body, 
                    Environment *_envt, 
                    SymbolList *_para_list) :
    OptObj(), body(_body), envt(_envt), para_list(_para_list) {}

Cons *ProcObj::call(ArgList *args, Environment * &_envt,
                    Continuation * &cont, FrameObj ** &top_ptr) {
    // Create a new continuation
    Cons *ret_addr = dynamic_cast<RetAddr*>(*top_ptr)->addr;
    Continuation *ncont = new Continuation(_envt, ret_addr, cont, body);
    cont = ncont;                   // Add to the cont chain
    _envt = new Environment(envt);   // Create local env and recall the closure
    // TODO: Compare the arguments to the parameters
    for (Cons *ptr = args->cdr, *ppar = para_list; 
            ptr != empty_list; ptr = ptr->cdr, ppar = ppar->cdr)
        _envt->add_binding(dynamic_cast<SymObj*>(ppar->car), ptr->car);
    *top_ptr++ = new RetAddr(NULL);   // Mark the entrance of a cont
    return body;                    // Move pc to the proc entry point
}

string ProcObj::ext_repr() { return string("#<Procedure>"); }

#ifdef DEBUG
string ProcObj::_debug_repr() { return ext_repr(); }
#endif

SpecialOptObj::SpecialOptObj() : OptObj() {}

NumberObj::NumberObj() : EvalObj() {}

BuiltinProcObj::BuiltinProcObj(BuiltinProc f, string _name) :
    OptObj(), handler(f), name(_name) {}

Cons *BuiltinProcObj::call(ArgList *args, Environment * &envt,
                                Continuation * &cont, FrameObj ** &top_ptr) {

    Cons *ret_addr = dynamic_cast<RetAddr*>(*top_ptr)->addr;
    *top_ptr++ = handler(args->cdr);
    return ret_addr->next;          // Move to the next instruction
}

string BuiltinProcObj::ext_repr() { 
    return "#<Builtin Procedure: " + name + ">";
}

#ifdef DEBUG
string BuiltinProcObj::_debug_repr() { return ext_repr(); }
#endif

Environment::Environment(Environment *_prev_envt) : prev_envt(_prev_envt) {}

void Environment::add_binding(SymObj *sym_obj, EvalObj *eval_obj) {
    binding[sym_obj->val] = eval_obj;
}

EvalObj *Environment::get_obj(EvalObj *obj) {
    SymObj *sym_obj = dynamic_cast<SymObj*>(obj);
    if (!sym_obj) return obj;       // Not a SymObj

    string name(sym_obj->val);
    for (Environment *ptr = this; ptr; ptr = ptr->prev_envt)
    {
        bool has_key = ptr->binding.count(name);
        if (has_key) return ptr->binding[name];
    }
    //TODO: exc key not found
}

bool Environment::has_obj(SymObj *sym_obj) {
    string name(sym_obj->val);
    for (Environment *ptr = this; ptr; ptr = ptr->prev_envt)
        if (ptr->binding.count(name))
            return true;
    return false;
}

Continuation::Continuation(Environment *_envt, Cons *_pc, 
                            Continuation *_prev_cont, 
                            ASTList *_proc_body) : 
        envt(_envt), pc(_pc), prev_cont(_prev_cont), 
        proc_body(_proc_body) {}