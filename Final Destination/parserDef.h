#ifndef PARSER_DEF_H
#define PARSER_DEF_H

#define NUM_RULES 95
#define NUM_TOKENS 62
#define NUM_NON_TERMINALS 53
#define MAX_FOLLOW_SET_SIZE 100
#define TABLE_SIZE 197
#define LEXEME_MAX_CHARS 30

#include "stack.h"
#include "symbol_table.h"
#include "lexerDef.h"

typedef struct node{
    symbol sym;
    struct node* next;
}NODE;

typedef struct lhsnode{
    int numProductions;
    symbol sym;
    NODE** productionRule;
}LHSNODE;

typedef struct first{
    int has_epsilon;
    int is_filled;
    NODE* head;
}FIRST;

typedef struct predictive_table_node{
    NODE *rule_rhs;
    int is_syn;
    int is_error;
} PNODE;

typedef struct st_element * ST_ELEMENT;
typedef struct symbol_table st;
typedef struct symbol_table * ST;

#endif