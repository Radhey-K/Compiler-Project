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
int isEmpty(struct StackNode *stack);
void push(struct StackNode **stack, symbol data);
symbol pop(struct StackNode **stack);
symbol top(struct StackNode *stack);

#endif
