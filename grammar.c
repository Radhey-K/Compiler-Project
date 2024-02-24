#include "lexer.h"
#define NUM_RULES 5
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct{
    union
    {
        tokName t;
        nonterminal nt;
    };
    int is_terminal;
} symbol;


typedef struct node{
    symbol sym;
    struct node* next;
}NODE;


typedef struct lhsnode{
    int numProductions;
    symbol sym;
    NODE** productionRule;
}LHSNODE;

// LHSNODE rules[NUM_RULES];

int getTerminal(const char* token) {
    if (strcmp(token, "TK_UNKNOWN") == 0) {
        return TK_UNKNOWN;
    } 
    else if (strcmp(token, "TK_ASSIGNOP") == 0) {
        return TK_ASSIGNOP;
    } 
    else if (strcmp(token, "TK_COMMENT") == 0) {
        return TK_COMMENT;
    } 
    else if (strcmp(token, "TK_FIELDID") == 0) {
        return TK_FIELDID;
    } 
    else if (strcmp(token, "TK_ID") == 0) {
        return TK_ID;
    } 
    else if (strcmp(token, "TK_NUM") == 0) {
        return TK_NUM;
    } 
    else if (strcmp(token, "TK_RNUM") == 0) {
        return TK_RNUM;
    } 
    else if (strcmp(token, "TK_FUNID") == 0) {
        return TK_FUNID;
    } 
    else if (strcmp(token, "TK_RUID") == 0) {
        return TK_RUID;
    } 
    else if (strcmp(token, "TK_WITH") == 0) {
        return TK_WITH;
    } 
    else if (strcmp(token, "TK_PARAMETERS") == 0) {
        return TK_PARAMETERS;
    } 
    else if (strcmp(token, "TK_END") == 0) {
        return TK_END;
    } 
    else if (strcmp(token, "TK_WHILE") == 0) {
        return TK_WHILE;
    } 
    else if (strcmp(token, "TK_UNION") == 0) {
        return TK_UNION;
    } 
    else if (strcmp(token, "TK_ENDUNION") == 0) {
        return TK_ENDUNION;
    } 
    else if (strcmp(token, "TK_DEFINETYPE") == 0) {
        return TK_DEFINETYPE;
    } 
    else if (strcmp(token, "TK_AS") == 0) {
        return TK_AS;
    } 
    else if (strcmp(token, "TK_TYPE") == 0) {
        return TK_TYPE;
    } 
    else if (strcmp(token, "TK_MAIN") == 0) {
        return TK_MAIN;
    } 
    else if (strcmp(token, "TK_GLOBAL") == 0) {
        return TK_GLOBAL;
    } 
    else if (strcmp(token, "TK_PARAMETER") == 0) {
        return TK_PARAMETER;
    } 
    else if (strcmp(token, "TK_LIST") == 0) {
        return TK_LIST;
    } 
    else if (strcmp(token, "TK_SQL") == 0) {
        return TK_SQL;
    } 
    else if (strcmp(token, "TK_SQR") == 0) {
        return TK_SQR;
    } 
    else if (strcmp(token, "TK_INPUT") == 0) {
        return TK_INPUT;
    } 
    else if (strcmp(token, "TK_OUTPUT") == 0) {
        return TK_OUTPUT;
    } 
    else if (strcmp(token, "TK_INT") == 0) {
        return TK_INT;
    } 
    else if (strcmp(token, "TK_REAL") == 0) {
        return TK_REAL;
    } 
    else if (strcmp(token, "TK_COMMA") == 0) {
        return TK_COMMA;
    } 
    else if (strcmp(token, "TK_SEM") == 0) {
        return TK_SEM;
    } 
    else if (strcmp(token, "TK_COLON") == 0) {
        return TK_COLON;
    } 
    else if (strcmp(token, "TK_DOT") == 0) {
        return TK_DOT;
    } 
    else if (strcmp(token, "TK_ENDWHILE") == 0) {
        return TK_ENDWHILE;
    } 
    else if (strcmp(token, "TK_OP") == 0) {
        return TK_OP;
    } 
    else if (strcmp(token, "TK_CL") == 0) {
        return TK_CL;
    } 
    else if (strcmp(token, "TK_IF") == 0) {
        return TK_IF;
    } 
    else if (strcmp(token, "TK_THEN") == 0) {
        return TK_THEN;
    } 
    else if (strcmp(token, "TK_ENDIF") == 0) {
        return TK_ENDIF;
    } 
    else if (strcmp(token, "TK_READ") == 0) {
        return TK_READ;
    } 
    else if (strcmp(token, "TK_WRITE") == 0) {
        return TK_WRITE;
    } 
    else if (strcmp(token, "TK_RETURN") == 0) {
        return TK_RETURN;
    } 
    else if (strcmp(token, "TK_PLUS") == 0) {
        return TK_PLUS;
    } 
    else if (strcmp(token, "TK_MINUS") == 0) {
        return TK_MINUS;
    } 
    else if (strcmp(token, "TK_MUL") == 0) {
        return TK_MUL;
    } 
    else if (strcmp(token, "TK_DIV") == 0) {
        return TK_DIV;
    } 
    else if (strcmp(token, "TK_CALL") == 0) {
        return TK_CALL;
    } 
    else if (strcmp(token, "TK_RECORD") == 0) {
        return TK_RECORD;
    } 
    else if (strcmp(token, "TK_ENDRECORD") == 0) {
        return TK_ENDRECORD;
    } 
    else if (strcmp(token, "TK_ELSE") == 0) {
        return TK_ELSE;
    } 
    else if (strcmp(token, "TK_AND") == 0) {
        return TK_AND;
    } 
    else if (strcmp(token, "TK_OR") == 0) {
        return TK_OR;
    } 
    else if (strcmp(token, "TK_NOT") == 0) {
        return TK_NOT;
    } 
    else if (strcmp(token, "TK_LT") == 0) {
        return TK_LT;
    } 
    else if (strcmp(token, "TK_LE") == 0) {
        return TK_LE;
    } 
    else if (strcmp(token, "TK_EQ") == 0) {
        return TK_EQ;
    } 
    else if (strcmp(token, "TK_GT") == 0) {
        return TK_GT;
    } 
    else if (strcmp(token, "TK_GE") == 0) {
        return TK_GE;
    } 
    else if (strcmp(token, "TK_NE") == 0) {
        return TK_NE;
    } 
    else if (strcmp(token, "TK_EOF") == 0) {
        return TK_EOF;
    } 
    else if (strcmp(token, "TK_ERROR") == 0) {
        return TK_ERROR;
    }
    else {
        return TK_UNKNOWN;
    }
}


int getNonTerminal(char* token) {
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
    else if (strcmp(token, "<stmts>") == 0)
        return stmts;
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

    NODE *rules = malloc(sizeof(NODE) * NUM_RULES);

    char line[256];
    char lhs[256];
    int i = 0;
    while (fscanf(fp, "%[^\n]\n", line) != EOF)
    {
        char *token = strtok(line, " |");
        printf("%s\n", token);
        strcpy(lhs, token);
        // rules[i].numProductions++;
        // rules[getNonTerminal(lhs)].sym.nt = getNonTerminal(lhs);
        // rules[getNonTerminal(lhs)].sym.is_terminal = 0;
        // rules[getNonTerminal(lhs)].productionRule = malloc(sizeof(RHSNODE*) * 10);
        rules[i].sym.nt = getNonTerminal(lhs);
        rules[i].sym.is_terminal = 0;

        token = strtok(NULL, " |");
        // rules[getNonTerminal(lhs)].productionRule[rules[getNonTerminal(lhs)].numProductions - 1] = malloc(sizeof(RHSNODE));
        NODE *head = &rules[i];

        while(token) {
            if (strcmp(token, "===>")) {

                NODE* root = malloc(sizeof(NODE));
                if (getNonTerminal(token) != -1) {
                    root->sym.nt = getNonTerminal(token);
                    root->sym.is_terminal = 0;
                    root->next = NULL;
                    head->next = root;
                    head = head->next;
                } else {
                    root->sym.t = getTerminal(token);
                    root->sym.is_terminal = 1;
                    root->next = NULL;
                    head->next = root;
                    head = head->next;
                }
            }
            printf("%s\n", token);
            token = strtok(NULL, " |\r\n\t");
        }
        i++;
    }
    printf("%d\n", rules[0].next->sym.nt);
    printf("%d\n", rules[0].next->next->sym.nt);
    printf("%d\n", rules[1].next->sym.nt);
    printf("%d\n", rules[1].next->next->sym.nt);
    printf("%d\n", rules[1].next->next->next->sym.nt);
}



