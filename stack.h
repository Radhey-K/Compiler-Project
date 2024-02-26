#ifndef STACK_H
#define STACK_H
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>

// Define your symbol struct
typedef struct
{
    union
    {
        tokName t;
        nonterminal nt;
    };
    int is_terminal;
} symbol;

// Define a structure for a stack node
struct StackNode
{
    symbol data;
    struct StackNode *next;
};

// Function prototypes
struct StackNode *createNode(symbol data);
int isEmpty(struct StackNode *root);
void push(struct StackNode **root, symbol data);
symbol pop(struct StackNode **root);
symbol top(struct StackNode *root);

#endif
