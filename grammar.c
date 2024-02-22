#include "lexer.h"
#define NUM_RULES 5
#include <stdio.h>
#include <stdlib.h>

typedef enum
{
    E,
    E1,
    T,
    T1,
    F
} nonterminal;

typedef struct{
    union
    {
        tokName t;
        nonterminal nt;
    };
    int is_terminal;
} symbol;


typedef struct rhsnode{
    symbol sym;
    struct rhsnode* next;
}RHSNODE;


typedef struct lhsnode{
    int numProductions;
    symbol sym;
    RHSNODE** productionRule;
}LHSNODE;

LHSNODE rules[NUM_RULES];

int main(){
    rules[E].numProductions=1;
    rules[E].sym.nt=E;
    rules[E].sym.is_terminal=0;
    rules[E].productionRule=malloc(sizeof(RHSNODE*)*rules[E].numProductions);

    rules[E].productionRule[0]=malloc(sizeof(RHSNODE));
    rules[E].productionRule[0]->sym.nt=T;
    rules[E].productionRule[0]->sym.is_terminal=0;

    rules[E].productionRule[0]->next=malloc(sizeof(RHSNODE));
    rules[E].productionRule[0]->next->sym.nt=E1;
    rules[E].productionRule[0]->next->sym.is_terminal=0;

    rules[E].productionRule[0]->next->next=NULL;

}



