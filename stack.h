#ifndef STACK_H
#define STACK_H
#include "lexer.h"
#include "parsetree.h"
#include <stdio.h>
#include <stdlib.h>

// Define your symbol struct
// typedef struct
// {
//     union
//     {
//         tokName t;
//         nonterminal nt;
//     };
//     int is_terminal;
// } symbol;

// Define a structure for a stack node
struct StackNode
{
    symbol data;
    struct StackNode *next;
    Node * treeNode;
};

// Function prototypes
struct StackNode *createNode(symbol data, Node * treeNode);
int isEmpty(struct StackNode *stack);
void push(struct StackNode **stack, symbol data, Node * treeNode);
struct StackNode * pop(struct StackNode **stack);
struct StackNode * top(struct StackNode *stack);

#endif
