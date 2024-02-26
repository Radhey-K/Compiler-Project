#include <stdio.h>
#include "stack.h"
// Creating stack using linked list of symbol type.
// Function to create a new stack node
struct StackNode *createNode(symbol data)
{
    struct StackNode *stackNode = (struct StackNode *)malloc(sizeof(struct StackNode));
    stackNode->data = data;
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
void push(struct StackNode **stack, symbol data)
{
    struct StackNode *stackNode = createNode(data);
    stackNode->next = *stack;
    *stack = stackNode;
    printf("Symbol pushed to stack\n");
}

// Function to pop a symbol from the stack. If empty, returning a default symbol with isterminal=-1 to indicate error
symbol pop(struct StackNode **stack)
{
    if (isEmpty(*stack))
    {
        printf("Stack is empty\n");
        // Return a default symbol if the stack is empty
        symbol default_symbol;
        default_symbol.is_terminal = -1;
        return default_symbol;
    }
    struct StackNode *temp = *stack;
    *stack = (*stack)->next;
    symbol popped_symbol = temp->data;
    free(temp);
    return popped_symbol;
}

// Function to see top of the top symbol of the stack. If empty, returning a default symbol with isterminal=-1 to indicate error
symbol top(struct StackNode *stack)
{
    if (isEmpty(stack))
    {
        printf("Stack is empty\n");
        // Return a default symbol if the stack is empty
        symbol default_symbol;
        default_symbol.is_terminal = -1;
        return default_symbol;
    }
    return stack->data;
}

// Demo function
// Outputting the token name as a integer, not as a string.
int main()
{
    struct StackNode *stack = NULL;
    symbol symbol1 = {.t = TK_ERROR, .is_terminal = 1};
    push(&stack, symbol1);
    printf("Top symbol: ");

    // Top() after popping
    symbol top_symbol = top(stack);
    if (top_symbol.is_terminal)
        printf("Terminal: tokName %d\n", top_symbol.t);
    else
        printf("Non-terminal: nonterminal %d\n", top_symbol.nt);

    symbol symbol2 = {.nt = booleanExpression, .is_terminal = 0};
    push(&stack, symbol2);

    // Top() again after pushing
    printf("Top symbol after pop: ");
    top_symbol = top(stack);
    if (top_symbol.is_terminal)
        printf("Terminal: tokName %d\n", top_symbol.t);
    else
        printf("Non-terminal: nonterminal %d\n", top_symbol.nt);

    symbol symbol3 = {.t = TK_NOT, .is_terminal = 1};
    push(&stack, symbol3);

    // Pop a symbol from the stack
    printf("Popped symbol: ");
    symbol popped_symbol = pop(&stack);
    if (popped_symbol.is_terminal)
        printf("Terminal: tokName %d\n", popped_symbol.t);
    else
        printf("Non-terminal: nonterminal %d\n", popped_symbol.nt);

    // Top() again after popping
    printf("Top symbol after pop: ");
    top_symbol = top(stack);
    if (top_symbol.is_terminal)
        printf("Terminal: tokName %d\n", top_symbol.t);
    else
        printf("Non-terminal: nonterminal %d\n", top_symbol.nt);

    return 0;
}
