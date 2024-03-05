/* ---------Group Information---------
Group Number: 15

Yash Mundada: 2021A7PS0001P
Aaryan Garg: 2021A7PS2222P
Dev Kulkarni: 2021A7PS2430P
Radhey Kanade: 2021A7PS2534P
Shardul Shingare: 2021A7PS2539P
Shantanu Ambekar: 2021A7PS2540P
--------------------------------------*/

#include <stdio.h>
#include "stack.h"

struct StackNode *createNode(symbol data, Node * treeNode)
{
    struct StackNode *stackNode = (struct StackNode *)malloc(sizeof(struct StackNode));
    stackNode->data = data;
    stackNode->treeNode = treeNode;
    stackNode->next = NULL;
    return stackNode;
}

int isEmpty(struct StackNode *stack)
{
    return !stack;
}

void push(struct StackNode **stack, symbol data, Node * treeNode)
{
    struct StackNode *stackNode = createNode(data, treeNode);
    stackNode->next = *stack;
    *stack = stackNode;
}

struct StackNode * pop(struct StackNode **stack, TOKEN cur_token)
{
    if (isEmpty(*stack))
    {
        return NULL;
    }
    struct StackNode *temp = *stack;
    if (temp->data.is_terminal){
        add_lexeme_to_node(temp->treeNode, cur_token);
    }
    *stack = (*stack)->next;
    return temp;
}

struct StackNode * top(struct StackNode *stack)
{
    if (isEmpty(stack))
    {
        return NULL;
    }
    return stack;
}
