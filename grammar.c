#include "lexer.h"
#define NUM_RULES 95
#define NUM_TOKENS 61
#define NUM_NON_TERMINALS 53
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

typedef struct first{
    int has_epsilon; // 1 if epsilon is in first set.
    int is_filled; // 1 if first set is calculated already.
    NODE* head;
}FIRST;

NODE rules[NUM_RULES];
FIRST* FIRST_T[NUM_TOKENS];
FIRST* FIRST_NT[NUM_NON_TERMINALS];

int symEqual(symbol s1, symbol s2){
    if(s1.is_terminal != s2.is_terminal)return 0;
    if(s1.is_terminal){
        if(s1.t==s2.t)return 1;
    }else{
        if(s1.nt==s2.nt)return 1;
    }
    return 0;
}

//adds First(f2) to First(f1)
void merge_list(FIRST* f1, FIRST* f2, int flag){
    NODE* temp = f1->head;
    NODE* root = f2->head;
    if(temp!=NULL){
        while(temp->next!=NULL) temp = temp->next;
    }else{
        if(root==NULL)return;
        if (root->sym.is_terminal == 1 && root->sym.t == TK_EPS && flag == 0){
            root = root->next;
        }
        if (root == NULL) return;
        f1->head = (NODE*)malloc(sizeof(NODE));
        f1->head->sym = root->sym;
        f1->head->next = NULL;
        temp = f1->head;
        root = root->next;
    }

    while(root != NULL){
        if (root->sym.is_terminal == 1 && root->sym.t == TK_EPS && flag == 0){
            root = root->next;
        }
        if (root == NULL) return;
        temp->next = (NODE*)malloc(sizeof(NODE));
        temp->next->sym = root->sym;
        temp->next->next = NULL;
        temp = temp->next;
        root = root->next;
    }

    // if(f2->has_epsilon == 1){
    //     f1->has_epsilon = 1;
    // }
}

void find_first_set(symbol sym){

    // if(sym.is_terminal == 1){
    //     printf("%s ", tokenToString(sym.t));
    // }else{
    //     printf("%s ", nonterminaltoString(sym.nt));
    // }
    // // printf("\n");

    if(sym.is_terminal==1){
        if(FIRST_T[sym.t]!=NULL && FIRST_T[sym.t]->is_filled==1) return;
    }else{
        if(FIRST_NT[sym.nt]!=NULL && FIRST_NT[sym.nt]->is_filled==1) return;
    }

    FIRST* ptr = (FIRST*)malloc(sizeof(FIRST));
    ptr->head=NULL;
    //handling first of terminals.
    if(sym.is_terminal==1){
        ptr->head = (NODE*)malloc(sizeof(NODE));
        // ptr->head->sym = sym;
        ptr->head->sym.t = sym.t;
        ptr->head->sym.is_terminal=1;
        ptr->head->next = NULL;
        ptr->is_filled=1;
        if(sym.t == TK_EPS){
            ptr->has_epsilon=1;
        }else{
            ptr->has_epsilon=0;
        }
        FIRST_T[sym.t] = ptr;
        return;
    }

    for(int i=0; i<NUM_RULES; i++){
        if(symEqual(rules[i].sym, sym)){
            NODE* temp;
            temp = rules[i].next;
            while(temp!=NULL){
                find_first_set(temp->sym);
                FIRST* curr;
                if(temp->sym.is_terminal==1){
                    curr = FIRST_T[temp->sym.t];
                }else{
                    curr = FIRST_NT[temp->sym.nt];
                }
                merge_list(ptr, curr, 0);
                if(curr->has_epsilon==0)break;
                temp = temp->next;
            }
            if (temp == NULL){
                ptr->has_epsilon = 1;
                merge_list(ptr, FIRST_T[TK_EPS], 1);
            }
        }
    }

    ptr->is_filled=1;
    if(sym.is_terminal==1){
        FIRST_T[sym.t] = ptr;
    }else{
        FIRST_NT[sym.nt] = ptr;
    }
}



NODE* find_unique(NODE* root){ // use in follow set generation, not needed in first generation
    NODE* temp = root;
    if(temp==NULL)return NULL;
    int token_cnt[NUM_TOKENS];
    for(int i=0; i<NUM_TOKENS; i++){
        token_cnt[i]=0;
    }
    int non_terminal_cnt[NUM_NON_TERMINALS];
    for(int i=0; i<NUM_NON_TERMINALS; i++){
        non_terminal_cnt[i]=0;
    }
    while(temp!=NULL){
        if(temp->sym.is_terminal==1){
            token_cnt[temp->sym.t]++;
        }else{
            non_terminal_cnt[temp->sym.nt]++;
        }
        temp = temp->next;
    }
    NODE* head = (NODE*)malloc(sizeof(NODE));
    NODE* curr = head;
    for(int i=0; i<NUM_TOKENS; i++){
        if(token_cnt[i]>0){
            curr->next = (NODE*)malloc(sizeof(NODE));
            curr->next->sym.is_terminal=1;
            curr->next->sym.t=i;
            curr->next->next=NULL;
            curr=curr->next;
        }
    }
    for(int i=0; i<NUM_NON_TERMINALS; i++){
        if(non_terminal_cnt[i]>0){
            curr->next = (NODE*)malloc(sizeof(NODE));
            curr->next->sym.is_terminal=0;
            curr->next->sym.nt=i;
            curr->next->next=NULL;
            curr=curr->next;
        }
    }
    return head->next;
}

void print_list(NODE* root){
    while(root != NULL) {
        if(root->sym.is_terminal == 1){
            printf("%s ", tokenToString(root->sym.t));
        }else{
            printf("%s ", nonterminaltoString(root->sym.nt));
        }
        root = root->next;
    }
    printf("\n");
}




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
    else if (strcmp(token, "TK_EPS") == 0) {
        return TK_EPS;
    }
    else if (strcmp(token, "TK_ERROR") == 0) {
        return TK_ERROR;
    }
    else {
        return TK_UNKNOWN;
    }
}

int getNonTerminal(char* token) {
    if (strcmp(token, "<program>") == 0) {
        return program;
    } 
    else if (strcmp(token, "<otherFunctions>") == 0) {
        return otherFunctions;
    } 
    else if (strcmp(token, "<mainFunction>") == 0) {
        return mainFunction;
    }
    else if (strcmp(token, "<stmts>") == 0) {
        return stmts;
    }
    else if (strcmp(token, "<function>") == 0) {
        return function;
    }
    else if (strcmp(token, "<input_par>") == 0) {
        return input_par;
    }
    else if (strcmp(token, "<output_par>") == 0) {
        return output_par;
    }
    else if (strcmp(token, "<parameter_list>") == 0) {
        return parameter_list;
    }
    else if (strcmp(token, "<dataType>") == 0) {
        return dataType;
    }
    else if (strcmp(token, "<primitiveDatatype>") == 0) {
        return primitiveDatatype;
    }
    else if (strcmp(token, "<constructedDatatype>") == 0) {
        return constructedDatatype;
    }
    else if (strcmp(token, "<remaining_list>") == 0) {
        return remaining_list;
    }
    else if (strcmp(token, "<typeDefinitions>") == 0) {
        return typeDefinitions;
    }
    else if (strcmp(token, "<actualOrRedefined>") == 0) {
        return actualOrRedefined;
    }
    else if (strcmp(token, "<typeDefinition>") == 0) {
        return typeDefinition;
    }
    else if (strcmp(token, "<fieldDefinitions>") == 0) {
        return fieldDefinitions;
    }
    else if (strcmp(token, "<fieldDefinition>") == 0) {
        return fieldDefinition;
    }
    else if (strcmp(token, "<fieldType>") == 0) {
        return fieldType;
    }
    else if (strcmp(token, "<moreFields>") == 0) {
        return moreFields;
    }
    else if (strcmp(token, "<declarations>") == 0) {
        return declarations;
    }
    else if (strcmp(token, "<declaration>") == 0) {
        return declaration;
    }
    else if (strcmp(token, "<global_or_not>") == 0) {
        return global_or_not;
    }
    else if (strcmp(token, "<otherStmts>") == 0) {
        return otherStmts;
    }
    else if (strcmp(token, "<stmt>") == 0) {
        return stmt;
    }
    else if (strcmp(token, "<assignmentStmt>") == 0) {
        return assignmentStmt;
    }
    else if (strcmp(token, "<SingleOrRecId>") == 0) {
        return SingleOrRecId;
    }
    else if (strcmp(token, "<option_single_constructed>") == 0) {
        return option_single_constructed;
    }
    else if (strcmp(token, "<oneExpansion>") == 0) {
        return oneExpansion;
    }
    else if (strcmp(token, "<moreExpansions>") == 0) {
        return moreExpansions;
    }
    else if (strcmp(token, "<funCallStmt>") == 0) {
        return funCallStmt;
    }
    else if (strcmp(token, "<outputParameters>") == 0) {
        return outputParameters;
    }
    else if (strcmp(token, "<inputParameters>") == 0) {
        return inputParameters;
    }
    else if (strcmp(token, "<iterativeStmt>") == 0) {
        return iterativeStmt;
    }
    else if (strcmp(token, "<conditionalStmt>") == 0) {
        return conditionalStmt;
    }
    else if (strcmp(token, "<elsePart>") == 0) {
        return elsePart;
    }
    else if (strcmp(token, "<ioStmt>") == 0) {
        return ioStmt;
    }
    else if (strcmp(token, "<arithmeticExpression>") == 0) {
        return arithmeticExpression;
    }
    else if (strcmp(token, "<expPrime>") == 0) {
        return expPrime;
    }
    else if (strcmp(token, "<term>") == 0) {
        return term;
    }
    else if (strcmp(token, "<termPrime>") == 0) {
        return termPrime;
    }
    else if (strcmp(token, "<factor>") == 0) {
        return factor;
    }
    else if (strcmp(token, "<highPrecedenceOperator>") == 0) {
        return highPrecedenceOperator;
    }
    else if (strcmp(token, "<lowPrecedenceOperators>") == 0) {
        return lowPrecedenceOperators;
    }
    else if (strcmp(token, "<booleanExpression>") == 0) {
        return booleanExpression;
    }
    else if (strcmp(token, "<var>") == 0) {
        return var;
    }
    else if (strcmp(token, "<logicalOp>") == 0) {
        return logicalOp;
    }
    else if (strcmp(token, "<relationalOp>") == 0) {
        return relationalOp;
    }
    else if (strcmp(token, "<returnStmt>") == 0) {
        return returnStmt;
    }
    else if (strcmp(token, "<optionalReturn>") == 0) {
        return optionalReturn;
    }
    else if (strcmp(token, "<idList>") == 0) {
        return idList;
    }
    else if (strcmp(token, "<more_ids>") == 0) {
        return more_ids;
    }
    else if (strcmp(token, "<definetypestmt>") == 0) {
        return definetypestmt;
    }
    else if (strcmp(token, "<A>") == 0) {
        return A;
    }
    else {
        return -1; // Return -1 for unknown non-terminals
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

    // NODE *rules = malloc(sizeof(NODE) * NUM_RULES);

    char line[256];
    char lhs[256];
    int i = 0;
    while (fscanf(fp, "%[^\n]\n", line) != EOF)
    {
        char *token = strtok(line, " |");
        // printf("%s\n", token);
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
            // printf("%s\n", token);
            token = strtok(NULL, " |\r\n\t");
        }
        i++;
    }

    // computes first set for tokens
    for(int i=0; i<NUM_TOKENS; i++){
        symbol sym;
        sym.is_terminal=1;
        sym.t=i;
        find_first_set(sym);
    }

    // computes first set for non terminals
    for(int i=0; i<NUM_NON_TERMINALS; i++){
        symbol sym;
        sym.is_terminal=0;
        sym.nt=i;
        find_first_set(sym);
        // find_unique(FIRST_NT[i]->head);

        // if(sym.is_terminal == 1){
        //     printf("%s ", tokenToString(sym.t));
        // }else{
        //     printf("%s ", nonterminaltoString(sym.nt));
        // }
        // printf("\n");

    }
    for(int i=0; i<NUM_TOKENS; i++){
        print_list(FIRST_T[i]->head);
    }

    //printing follow sets
    for(int i=0; i<NUM_NON_TERMINALS; i++){
        printf("%s ===> ", nonterminaltoString(i));
        print_list(FIRST_NT[i]->head);
    }



    // printf("%d\n", rules[0].next->sym.nt);
    // printf("%d\n", rules[0].next->next->sym.nt);
    // printf("%d\n", rules[1].next->sym.nt);
    // printf("%d\n", rules[1].next->next->sym.nt);
    // printf("%d\n", rules[1].next->next->next->sym.nt);

    // for(int i = 0; i < NUM_RULES; i++) {
    //     printf("%s ===>", nonterminaltoString(rules[i].sym.nt));
    //     NODE* head = &rules[i];
    //     while(head->next) {
    //         if (head->next->sym.is_terminal) {
    //             printf(" %s", tokenToString(head->next->sym.t));
    //         } else {
    //             printf(" %s", nonterminaltoString(head->next->sym.nt));
    //         }
    //         head = head->next;
    //     }
    //     printf("\n");
    // }
}



