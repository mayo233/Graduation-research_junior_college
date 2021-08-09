#include <stdio.h>
#include <stdlib.h>
#include "senserver.h"

ProcList *plist = NULL;

ProcList *newProcList(pid_t id) {
	ProcList *p = (ProcList*)calloc(sizeof(ProcList),1);
	p->pid = id;
	return p;
}

void addProcList(ProcList *p) {
    if(plist == NULL) {
        plist = p;
        return;
    }
    ProcList *q = plist;
    while(q->next != NULL) q = q->next;
    q->next = p;
}

ProcList *searchProcList(pid_t id) {
	if(plist == NULL) return NULL;
    
    ProcList *q;
    for(q = plist; q != NULL; q = q->next) {
        if(q->pid == id) return q;
    }
    return NULL;
}

ProcList *removeProcList(pid_t id) {
    if(plist == NULL) return NULL;
    ProcList *p,*q;
    if(plist->pid == id) {
        p = plist;
        plist = plist->next;
        return p;
    }
    for(q = plist; q->next != NULL; q = q->next) {
        if(q->next->pid == id) {
            p = q->next;
            q->next = q->next->next;
            return p;
        }
    }
    return NULL;
}
