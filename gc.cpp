#include "gc.h"
#include "exc.h"
#include "consts.h"

#ifdef GC_DEBUG
#include <cstdio>
typedef unsigned long long ull;
#endif

static EvalObj *gcq[GC_QUEUE_SIZE];

GarbageCollector::GarbageCollector() {
    mapping.clear();
    pend_cnt = 0;
    pending_list = NULL;
    collecting = false;
}

GarbageCollector::PendingEntry::PendingEntry(
        EvalObj *_obj, PendingEntry *_next) : obj(_obj), next(_next) {}


void GarbageCollector::expose(EvalObj *ptr) {
    bool flag = mapping.count(ptr);
    if (flag)
    {
#ifdef GC_DEBUG
        fprintf(stderr, "GC: 0x%llx exposed. count = %lu \"%s\"\n", 
            (ull)ptr, mapping[ptr] - 1, ptr->ext_repr().c_str());
#endif
        if (!--mapping[ptr] && collecting)
        {
#ifdef GC_DEBUG
            fprintf(stderr, "GC: 0x%llx pending. \n", (ull)ptr);
#endif
            pending_list = new PendingEntry(ptr, pending_list);
        } 
    }
}

void GarbageCollector::force() {
    EvalObj **l = gcq, **r = l;
/*    for (PendingEntry *p = pending_list, *np; p; p = np)
    {
        np = p->next;
        *r++ = p->obj;
        delete p;
    }   // fetch the pending pointers in the list
    // clear the list
    pending_list = NULL; */
    for (EvalObj2Int::iterator it = mapping.begin(); 
            it != mapping.end(); it++)
        if (it->second == 0) *r++ = it->first;

    collecting = true;
#ifdef GC_DEBUG
    size_t cnt = 0;
    fprintf(stderr, "GC: Forcing the clear process...\n");
#endif
    for (; l != r; l++)
    {
#ifdef GC_DEBUG
        fprintf(stderr, "GC: destroying space 0x%llx. \n", (ull)*l);
        cnt++;
#endif
        delete *l;
        mapping.erase(*l);
        // maybe it's a complex structure, 
        // so that more pointers are reported
        for (PendingEntry *p = pending_list, *np; p; p = np)
        {
            np = p->next;
            *r++ = p->obj;
            if (r == gcq + GC_QUEUE_SIZE)
                throw NormalError(RUN_ERR_GC_OVERFLOW);
            delete p;
        }   
        pending_list = NULL;
    }
#ifdef GC_DEBUG
    fprintf(stderr, "GC: Forced clear, %lu objects are freed, "
            "%lu remains\n", cnt, mapping.size());
/*    for (EvalObj2Int::iterator it = mapping.begin();
            it != mapping.end(); it++)
        fprintf(stderr, "%llx => %lu\n", (ull)it->first, it->second);
        */
#endif
    collecting = false;
}

EvalObj *GarbageCollector::attach(EvalObj *ptr) {
    if (!ptr) return NULL;   // NULL pointer
    bool flag = mapping.count(ptr);
    if (flag) mapping[ptr]++;
    else mapping[ptr] = 1;
#ifdef GC_DEBUG
    fprintf(stderr, "GC: 0x%llx attached. count = %lu \"%s\"\n", 
            (ull)ptr, mapping[ptr], ptr->ext_repr().c_str());
#endif
    return ptr; // passing through
}

