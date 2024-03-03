#include "parsetree.h"
#include <stdio.h>

// Constructor: Initialize an empty parse tree
ParseTree *create_parse_tree()
{
    ParseTree *tree = (ParseTree *)malloc(sizeof(ParseTree));
    if (tree != NULL)
    {
        tree->root = NULL;
    }
    return tree;
}

// Constructor: Create a parse tree with a root node containing the given symbol
ParseTree *create_parse_tree_with_root(symbol root_value)
{
    ParseTree *tree = create_parse_tree();
    if (tree != NULL)
    {
        tree->root = (Node *)malloc(sizeof(Node));
        if (tree->root != NULL)
        {
            tree->root->data = root_value;
            tree->root->children = NULL;
            tree->root->num_children = 0;
        }
        else
        {
            // Cleanup if memory allocation fails
            free(tree);
            return NULL;
        }
    }
    return tree;
}

// Recursively destroy nodes in the parse tree
void destroy_node(Node *node)
{
    if (node != NULL)
    {
        for (size_t i = 0; i < node->num_children; i++)
        {
            destroy_node(node->children[i]);
        }
        free(node->children);
        free(node);
    }
}

// Destructor: Destroy the parse tree and free memory
void destroy_parse_tree(ParseTree *tree)
{
    if (tree != NULL)
    {
        destroy_node(tree->root);
        free(tree);
    }
}

Node *add_child(ParseTree *tree, Node *parent, symbol child_value)
{
    if (parent == NULL)
    {
        return NULL; // Cannot add child to NULL parent
    }

    // Reallocate memory to expand the children array
    parent->children = (Node **)realloc(parent->children, (parent->num_children + 1) * sizeof(Node *));
    if (parent->children == NULL)
    {
        return NULL; // Memory reallocation failed
    }

    // Shift existing children to the right to make space for the new child
    for (size_t i = parent->num_children; i > 0; i--)
    {
        parent->children[i] = parent->children[i - 1];
    }

    // Allocate memory for the new child node
    Node *new_child = (Node *)malloc(sizeof(Node));
    if (new_child == NULL)
    {
        return NULL; // Memory allocation failed
    }

    // Initialize the new child node
    new_child->data = child_value;
    new_child->children = NULL;
    new_child->num_children = 0;

    // Add the new child to the beginning of the parent's children array
    parent->children[0] = new_child;
    parent->num_children++;

    return new_child;
}

// Remove Child: Remove a specified child node from its parent in the parse tree
void remove_child(Node *parent, size_t index)
{
    if (parent == NULL || index >= parent->num_children)
    {
        return; // Invalid parent or index
    }

    // Free the memory occupied by the child node to be removed
    destroy_node(parent->children[index]);

    // Shift remaining children to fill the gap
    for (size_t i = index; i < parent->num_children - 1; i++)
    {
        parent->children[i] = parent->children[i + 1];
    }

    // Reduce the number of children
    parent->num_children--;

    // Reallocate memory to resize the children array
    parent->children = (Node **)realloc(parent->children, parent->num_children * sizeof(Node *));
}

// Get Children: Retrieve the list of children nodes for a specified parent node
Node **get_children(Node *parent, size_t *num_children)
{
    if (parent != NULL)
    {
        *num_children = parent->num_children;
        return parent->children;
    }
    return NULL;
}

// Helper function to print the parse tree structure recursively
void print_parse_tree(Node *node, int depth)
{
    if (node != NULL)
    {
        for (int i = 0; i < depth; i++)
        {
            printf("  "); // Print indentation for better visualization
        }
        if (node->data.is_terminal)
        {
            printf("Terminal: %d\n", node->data.t);
        }
        else
        {
            printf("Non-terminal: %d\n", node->data.nt);
        }
        for (size_t i = 0; i < node->num_children; i++)
        {
            print_parse_tree(node->children[i], depth + 1);
        }
    }
}

// Driver code
int main()
{
    // Create a parse tree with root symbol "Program"
    ParseTree *tree = create_parse_tree_with_root((symbol){.t = TK_AND, .is_terminal = 0});

    // Add children to the root node
    Node *stmt1 = add_child(tree, tree->root, (symbol){.t = TK_ASSIGNOP, .is_terminal = 1});
    Node *stmt2 = add_child(tree, tree->root, (symbol){.t = TK_CALL, .is_terminal = 1});

    // Add children to stmt1 node
    add_child(tree, stmt1, (symbol){.t = TK_CL, .is_terminal = 1});
    add_child(tree, stmt1, (symbol){.t = TK_COLON, .is_terminal = 1});

    // Add children to stmt2 node
    add_child(tree, stmt2, (symbol){.t = TK_COMMENT, .is_terminal = 1});
    add_child(tree, stmt2, (symbol){.t = TK_DEFINETYPE, .is_terminal = 1});

    // Print the parse tree structure
    print_parse_tree(tree->root, 0);

    // Destroy the parse tree
    destroy_parse_tree(tree);

    return 0;
}
