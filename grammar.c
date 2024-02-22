#include "lexer.h"
#define NUM_RULES 5
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum
{
    E,
    E1,
    T,
    T1,
    F,
    program,
    otherFunctions,
    mainFunction
} nonterminal;

typedef struct{
    union
    {
        tokName t;
        nonterminal nt;
    };
    int is_terminal;
} symbol;


typedef struct rhsnode{
    symbol sym;
    struct rhsnode* next;
}RHSNODE;


typedef struct lhsnode{
    int numProductions;
    symbol sym;
    RHSNODE** productionRule;
}LHSNODE;

LHSNODE rules[NUM_RULES];

int getEnumValue(const char* token) {
    if (strcmp(token, "E") == 0) {
        return E;
    } 
    else if (strcmp(token, "E1") == 0) {
        return E1;
    } 
    else if (strcmp(token, "T") == 0) {
        return T;
    } 
    else if (strcmp(token, "T1") == 0) {
        return T1;
    } 
    else if (strcmp(token, "F") == 0) {
        return F;
    } 
    else if (strcmp(token, "<program>") == 0) {
        return program;
    } 
    else if (strcmp(token, "<otherFunctions>") == 0) {
        return otherFunctions;
    } 
    else if (strcmp(token, "<mainFunction>") == 0) {
        return mainFunction;
    }
    else {
        return -1;
    }
}

int main(){
    // rules[E].numProductions=1;
    // rules[E].sym.nt=E;
    // rules[E].sym.is_terminal=0;
    // rules[E].productionRule=malloc(sizeof(RHSNODE*)*rules[E].numProductions);

    // rules[E].productionRule[0]=malloc(sizeof(RHSNODE));
    // rules[E].productionRule[0]->sym.nt=T;
    // rules[E].productionRule[0]->sym.is_terminal=0;

    // rules[E].productionRule[0]->next=malloc(sizeof(RHSNODE));
    // rules[E].productionRule[0]->next->sym.nt=E1;
    // rules[E].productionRule[0]->next->sym.is_terminal=0;

    // rules[E].productionRule[0]->next->next=NULL;

    FILE *fp = fopen("test_grammar.txt", "r");
    if (fp == NULL)
    {
        printf("Error: File not found\n");
        return 1;
    }

    char line[256];
    char lhs[256];
    while (fscanf(fp, "%[^\n]\n", line) != EOF)
    {
        char *token = strtok(line, " |");
        int i = 0;
        printf("%s\n", token);
        strcpy(lhs, token);
        rules[getEnumValue(lhs)].numProductions++;
        rules[getEnumValue(lhs)].sym.nt = getEnumValue(lhs);
        rules[getEnumValue(lhs)].sym.is_terminal = 0;

        token = strtok(NULL, " |");

        rules[getEnumValue(lhs)].productionRule[i] = malloc(sizeof(RHSNODE));
        RHSNODE *root = rules[getEnumValue(lhs)].productionRule[i];
        while(token) {
            if (strcmp(token, "===>")) {
                if (getEnumValue(token) != -1) {
                    
                }
            }
            printf("%s\n", token);
            token = strtok(NULL, " |");
        }
    }
}



