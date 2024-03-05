/* ---------Group Information---------
Group Number: 15

Yash Mundada: 2021A7PS0001P
Aaryan Garg: 2021A7PS2222P
Dev Kulkarni: 2021A7PS2430P
Radhey Kanade: 2021A7PS2534P
Shardul Shingare: 2021A7PS2539P
Shantanu Ambekar: 2021A7PS2540P
--------------------------------------*/

#include "parsetree.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

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

// void print_inorder(Node *node, char *outname)
// {
//     FILE *outf = fopen(outname, "a");
//     if (node != NULL)
//     {
//         if (node->num_children > 0)
//         {
//             print_inorder(node->children[0], outname);
//         }
//         if (node->data.is_terminal)
//         {
//             // printf("Terminal: %s\n", tokenToString(node->data.t));
//             fprintf(outf, "Terminal: %s\n", tokenToString(node->data.t));
//             // if node->data.t == TK_EPS (no line or lexeme)
//             fprintf(outf, "Line No : %d\n", node->token.lineNo);
//         }
//         else
//         {
//             fprintf(outf, "Non-terminal: %s\n", nonterminaltoString(node->data.nt));
//         }
//         for (int i = 1; i < node->num_children; i++)
//         {
//             print_inorder(node->children[i], outname);
//         }
//     }
//     fclose(outf);
// }

void print_inorder(Node *node, Node *parent, char *outfile)
{
    FILE *outf = fopen(outfile, "a");
    if (node != NULL)
    {
        char *lexeme;
        int line_number;
        char *token_name;
        float value;
        const char *parent_symbol;
        const char *leaf;
        const char *non_terminal;

        if (node->num_children > 0)
        {
            print_inorder(node->children[0], node, outfile);
        }
        if (node->data.is_terminal)
        {
            // 1. Lexeme if leaf else “—-”
            lexeme = "testing";
            // 2. Line number
            line_number = (node->token.name == TK_EPS) ? -1 : node->token.lineNo;
            // 3. Token name
            token_name = tokenToString(node->token.name);

            // 4. Value of lexeme if int or real
            value = (node->token.integer != -1) ? node->token.integer : ((node->token.realNum != -1.0) ? node->token.realNum : 0.0);

            // 5. Parent (ROOT)
            parent_symbol = (parent == NULL) ? "ROOT" : (parent->data.is_terminal ? "Terminal" : nonterminaltoString(parent->data.nt));

            // 6. YES/NO
            leaf = (node->data.is_terminal) ? "yes" : "no";

            // 7. Non-terminal symbol if node not leaf
            non_terminal = (node->data.is_terminal) ? "---" : nonterminaltoString(node->data.nt);
        }
        else
        {
            // 1. Lexeme if leaf else “—-”
            lexeme = "testing";
            // 2. Line number
            line_number = (node->token.name == TK_EPS) ? -1 : node->token.lineNo;
            // 3. Token name
            token_name = tokenToString(node->token.name);

            // 4. Value of lexeme if int or real
            value = (node->token.integer != -1) ? node->token.integer : ((node->token.realNum != -1.0) ? node->token.realNum : 0.0);

            // 5. Parent (ROOT)
            parent_symbol = (parent == NULL) ? "ROOT" : (parent->data.is_terminal ? "Terminal" : nonterminaltoString(parent->data.nt));

            // 6. YES/NO
            leaf = (node->data.is_terminal) ? "yes" : "no";

            // 7. Non-terminal symbol if node not leaf
            non_terminal = (node->data.is_terminal) ? "---" : nonterminaltoString(node->data.nt);
        }

        fprintf(outf, "%-20s %-10d %-20s %-10.2f %-20s %-5s %-20s\n", lexeme, line_number, token_name, value, parent_symbol, leaf, non_terminal);

        for (int i = 1; i < node->num_children; i++)
        {
            print_inorder(node->children[i], node, outfile);
        }
    }
    fclose(outf);
}

void add_lexeme_to_node(Node *node, TOKEN cur_token){
    if(node != NULL){
        node->token = cur_token;
    }
}