/* ---------Group Information---------
Group Number: 15

Yash Mundada: 2021A7PS0001P
Aaryan Garg: 2021A7PS2222P
Dev Kulkarni: 2021A7PS2430P
Radhey Kanade: 2021A7PS2534P
Shardul Shingare: 2021A7PS2539P
Shantanu Ambekar: 2021A7PS2540P
--------------------------------------*/

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

typedef struct first{
    int has_epsilon;
    int is_filled;
    NODE* head;
}FIRSTFOLLOW;

typedef struct predictive_table_node{
    NODE *rule_rhs;
    int is_syn;
    int is_error;
} PNODE;

typedef struct st_element * ST_ELEMENT;
typedef struct symbol_table st;
typedef struct symbol_table * ST;

#endif