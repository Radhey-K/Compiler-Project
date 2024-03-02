#ifndef PARSETREE_H
#define PARSETREE_H
#include "lexer.h"
#include <stdlib.h>
#include <string.h>

typedef enum
{
    TERMINAL,
    NONTERMINAL
} NodeType;

typedef struct
{
    union
    {
        tokName t;
        nonterminal nt;
    };
    NodeType type;
} symbol;

typedef struct Node
{
    symbol data;
    struct Node *parent;
    struct Node **children;
    size_t num_children;
    size_t capacity_children; // Capacity of children array
} Node;

typedef struct ParseTree
{
    Node *root;
} ParseTree;

// Function prototypes

// Constructor: Initialize an empty parse tree
ParseTree *create_parse_tree();

// Constructor: Create a parse tree with a root node containing the given symbol
ParseTree *create_parse_tree_with_root(symbol root_value);

// Destructor: Destroy the parse tree and free memory
void destroy_parse_tree(ParseTree *tree);

// Add Child: Add a child node with the given symbol to a specified parent node in the parse tree
void add_child(ParseTree *tree, Node *parent, symbol child_value);

// Remove Child: Remove a specified child node from its parent in the parse tree
void remove_child(Node *parent, size_t index);

// Get Parent: Retrieve the parent node of a specified child node
Node *get_parent(Node *child);

// Get Children: Retrieve the list of children nodes for a specified parent node
Node **get_children(Node *parent, size_t *num_children);

#endif // PARSETREE_H
