#include "parsetree.h"
#include <stdio.h>
#include "parsetree.h"

// Helper function to create a new node
Node *create_node(symbol data)
{
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (new_node == NULL)
    {
        // Memory allocation failed
        return NULL;
    }
    new_node->data = data;
    new_node->parent = NULL;
    new_node->children = NULL;
    new_node->num_children = 0;
    new_node->capacity_children = 0;
    return new_node;
}

// Constructor: Initialize an empty parse tree
ParseTree *create_parse_tree()
{
    ParseTree *tree = (ParseTree *)malloc(sizeof(ParseTree));
    if (tree == NULL)
    {
        // Memory allocation failed
        return NULL;
    }
    tree->root = NULL;
    return tree;
}

// Constructor: Create a parse tree with a root node containing the given symbol
ParseTree *create_parse_tree_with_root(symbol root_value)
{
    ParseTree *tree = create_parse_tree();
    if (tree == NULL)
    {
        return NULL;
    }
    tree->root = create_node(root_value);
    if (tree->root == NULL)
    {
        // Memory allocation failed
        free(tree);
        return NULL;
    }
    return tree;
}

// Destructor: Destroy the parse tree and free memory
void destroy_parse_tree(ParseTree *tree)
{
    if (tree == NULL)
    {
        return;
    }
    // Free memory for each node using a post-order traversal
    // Freeing memory for children before parents
    // to avoid accessing already freed memory
    if (tree->root != NULL)
    {
        // Start recursive traversal from the root
        free(tree->root->children);
        free(tree->root);
    }
    free(tree);
}

// Add Child: Add a child node with the given symbol to a specified parent node in the parse tree
void add_child(ParseTree *tree, Node *parent, symbol child_value)
{
    if (tree == NULL || parent == NULL)
    {
        return;
    }
    if (parent->num_children >= parent->capacity_children)
    {
        // Increase the capacity of the children array
        size_t new_capacity = (parent->capacity_children == 0) ? 1 : parent->capacity_children * 2;
        Node **new_children = (Node **)realloc(parent->children, new_capacity * sizeof(Node *));
        if (new_children == NULL)
        {
            // Memory reallocation failed
            return;
        }
        parent->children = new_children;
        parent->capacity_children = new_capacity;
    }
    // Create a new child node
    Node *new_child = create_node(child_value);
    if (new_child == NULL)
    {
        // Memory allocation failed
        return;
    }
    // Set parent for the new child
    new_child->parent = parent;
    // Add the new child to the parent's children array
    parent->children[parent->num_children] = new_child;
    parent->num_children++;
}

// Remove Child: Remove a specified child node from its parent in the parse tree
void remove_child(Node *parent, size_t index)
{
    if (parent == NULL || index >= parent->num_children)
    {
        return;
    }
    // Free memory for the child node
    free(parent->children[index]);
    // Shift remaining children to fill the gap
    for (size_t i = index; i < parent->num_children - 1; i++)
    {
        parent->children[i] = parent->children[i + 1];
    }
    parent->num_children--;
}

// Get Parent: Retrieve the parent node of a specified child node
Node *get_parent(Node *child)
{
    if (child == NULL)
    {
        return NULL;
    }
    return child->parent;
}

// Get Children: Retrieve the list of children nodes for a specified parent node
Node **get_children(Node *parent, size_t *num_children)
{
    if (parent == NULL)
    {
        *num_children = 0;
        return NULL;
    }
    *num_children = parent->num_children;
    return parent->children;
}

#include <stdio.h>
#include "parsetree.h"

// Function to print the data of a node
void print_node_data(Node *node)
{
    if (node->data.type == TERMINAL)
    {
        printf("%d ", node->data.t);
    }
    else
    {
        printf("%d ", node->data.nt);
    }
}

// Function to recursively print the parse tree with lines connecting parent nodes to their children
void print_parse_tree_recursive(Node *node, int depth)
{
    if (node == NULL)
    {
        return;
    }
    // Print node data
    for (int i = 0; i < depth - 1; i++)
    {
        printf("  |");
    }
    if (depth > 0)
    {
        printf("  +");
    }
    print_node_data(node);
    printf("\n");
    // Recursively print children
    for (size_t i = 0; i < node->num_children; i++)
    {
        print_parse_tree_recursive(node->children[i], depth + 1);
    }
}

// Function to print the parse tree with lines connecting parent nodes to their children
void print_parse_tree(ParseTree *tree)
{
    if (tree == NULL || tree->root == NULL)
    {
        printf("Parse tree is empty.\n");
        return;
    }
    printf("Parse Tree:\n");
    print_parse_tree_recursive(tree->root, 0);
}

int main()
{
    // Create a parse tree with a root node
    ParseTree *tree = create_parse_tree_with_root((symbol){.t = 1, .type = TERMINAL});

    // Add children nodes to the root
    add_child(tree, tree->root, (symbol){.nt = 2, .type = NONTERMINAL});
    add_child(tree, tree->root, (symbol){.t = 3, .type = TERMINAL});

    // Add children nodes to the non-terminal node
    Node *non_terminal_node = tree->root->children[0];
    add_child(tree, non_terminal_node, (symbol){.t = 4, .type = TERMINAL});
    add_child(tree, non_terminal_node, (symbol){.t = 5, .type = TERMINAL});

    // Print the parse tree
    print_parse_tree(tree);

    // Cleanup
    destroy_parse_tree(tree);

    return 0;
}
