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
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>

ParseTree *create_parse_tree()
{
    ParseTree *tree = (ParseTree *)malloc(sizeof(ParseTree));
    if (tree != NULL)
    {
        tree->root = NULL;
    }
    return tree;
}

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
            free(tree);
            return NULL;
        }
    }
    return tree;
}

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
        return NULL;
    }

    parent->children = (Node **)realloc(parent->children, (parent->num_children + 1) * sizeof(Node *));
    if (parent->children == NULL)
    {
        return NULL;
    }

    for (size_t i = parent->num_children; i > 0; i--)
    {
        parent->children[i] = parent->children[i - 1];
    }

    Node *new_child = (Node *)malloc(sizeof(Node));
    if (new_child == NULL)
    {
        return NULL;
    }

    new_child->data = child_value;
    new_child->children = NULL;
    new_child->num_children = 0;

    parent->children[0] = new_child;
    parent->num_children++;

    return new_child;
}

void remove_child(Node *parent, size_t index)
{
    if (parent == NULL || index >= parent->num_children)
    {
        return;
    }

    destroy_node(parent->children[index]);

    for (size_t i = index; i < parent->num_children - 1; i++)
    {
        parent->children[i] = parent->children[i + 1];
    }

    parent->num_children--;

    parent->children = (Node **)realloc(parent->children, parent->num_children * sizeof(Node *));
}

Node **get_children(Node *parent, size_t *num_children)
{
    if (parent != NULL)
    {
        *num_children = parent->num_children;
        return parent->children;
    }
    return NULL;
}

void print_parse_tree(Node *node, int depth)
{
    if (node != NULL)
    {
        for (int i = 0; i < depth; i++)
        {
            printf("  ");
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

void print_inorder(Node *node, Node *parent, FILE *outfile)
{
    if (node != NULL)
    {
        char *lexeme = malloc(50 * sizeof(char));
        int line_number;
        const char* token_name;
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
            line_number = (node->token.name == TK_EPS) ? -1 : node->token.lineNo;
            token_name = (node->data.is_terminal) ? tokenToString(node->data.t) : nonterminaltoString(node->data.nt);
            value = (node->token.integer != -1) ? node->token.integer : ((node->token.realNum != -1.0) ? node->token.realNum : 0.0);
            parent_symbol = (parent == NULL) ? "ROOT" : (parent->data.is_terminal ? "Terminal" : nonterminaltoString(parent->data.nt));
            leaf = (node->data.is_terminal) ? "yes" : "no";
            non_terminal = (node->data.is_terminal) ? "---" : nonterminaltoString(node->data.nt);

            if (node->token.integer != -1)
                fprintf(outfile, "%-30d %-5d %-30s %-10.2f %-30s %-8s %-30s\n", node->token.integer, line_number, token_name, value, parent_symbol, leaf, non_terminal);
            else if (node->token.realNum != -1)
                fprintf(outfile, "%-30f %-5d %-30s %-10.2f %-30s %-8s %-30s\n", node->token.realNum, line_number, token_name, value, parent_symbol, leaf, non_terminal);
            else
                fprintf(outfile, "%-30s %-5d %-30s %-10.2f %-30s %-8s %-30s\n", node->token.string, line_number, token_name, value, parent_symbol, leaf, non_terminal);
        }
        else
        {
            lexeme = "---";
            line_number = -1;
            token_name = (node->data.is_terminal) ? tokenToString(node->data.t) : nonterminaltoString(node->data.nt);
            value = (node->token.integer != -1) ? node->token.integer : ((node->token.realNum != -1.0) ? node->token.realNum : 0.0);
            parent_symbol = (parent == NULL) ? "ROOT" : (parent->data.is_terminal ? "Terminal" : nonterminaltoString(parent->data.nt));
            leaf = (node->data.is_terminal) ? "yes" : "no";
            non_terminal = (node->data.is_terminal) ? "---" : nonterminaltoString(node->data.nt);

            fprintf(outfile, "%-30s %-5d %-30s %-10.2f %-30s %-8s %-30s\n", lexeme, line_number, token_name, value, parent_symbol, leaf, non_terminal);
        }
        fflush(outfile);

        for (int i = 1; i < node->num_children; i++)
        {
            print_inorder(node->children[i], node, outfile);
        }
    }
}

void add_lexeme_to_node(Node *node, TOKEN cur_token){
    if(node != NULL){
        node->token = cur_token;
    }
}