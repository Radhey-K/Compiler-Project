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

// Function to check if the stack is empty
int isEmpty(struct StackNode *stack)
{
    //  It returns 1 if the stack pointer is NULL, indicating that the stack is empty. Otherwise, it returns 0, indicating that the stack is not empty.
    return !stack;
}

// Function to push a symbol onto the stack
void push(struct StackNode **stack, symbol data, Node * treeNode)
{
    struct StackNode *stackNode = createNode(data, treeNode);
    stackNode->next = *stack;
    *stack = stackNode;
}

//If not empty return StructNode (we need both pointer + symbol for lookup) else return NULL pointer
struct StackNode * pop(struct StackNode **stack, TOKEN cur_token)
{
    if (isEmpty(*stack))
    {
        return NULL;
    }
    struct StackNode *temp = *stack;
    // Call update function for tree node
    if (temp->data.is_terminal){
        // Then pass cur_token to tree
        add_lexeme_to_node(temp->treeNode, cur_token);
    } // Else popping non-terminal from stack (no lexeme update needed)
    *stack = (*stack)->next;
    return temp;
}

// Function to see top of the top symbol of the stack. If empty, returning a default symbol with isterminal=-1 to indicate error
struct StackNode * top(struct StackNode *stack)
{
    if (isEmpty(stack))
    {
        return NULL;
    }
    return stack;
}
