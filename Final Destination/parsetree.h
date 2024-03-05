#ifndef PARSETREE_H
#define PARSETREE_H
#include "lexer.h"
#include <stdlib.h>
#include <string.h>

typedef struct Node
{
    symbol data;
    struct Node **children;
    int num_children;
    TOKEN token;
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

// Add Child: Return a pointer to a node
Node *add_child(ParseTree *tree, Node *parent, symbol child_value);

// Remove Child: Remove a specified child node from its parent in the parse tree
void remove_child(Node *parent, size_t index);

// Get Children: Retrieve the list of children nodes for a specified parent node
Node **get_children(Node *parent, size_t *num_children);

void print_parse_tree(Node *node, int depth);

void print_inorder(Node *node);

void add_lexeme_to_node(Node *node, TOKEN cur_token);

#endif // PARSETREE_H
