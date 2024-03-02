#include "lexer.h"
#define NUM_RULES 95
#define NUM_TOKENS 62
#define NUM_NON_TERMINALS 53
#define MAX_FOLLOW_SET_SIZE 100
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "stack.h"
#include "symbol_table.h"

// REMOVE LATER-START
#define TABLE_SIZE 197
#define LEXEME_MAX_CHARS 30

typedef enum {
    TK_UNKNOWN, // For symbol table entry init
    TK_ASSIGNOP,
    TK_COMMENT,
    TK_FIELDID,
    TK_ID,
    TK_NUM,
    TK_RNUM,
    TK_FUNID,
    TK_RUID,
    TK_WITH,
    TK_PARAMETERS,
    TK_END,
    TK_WHILE,
    TK_UNION,
    TK_ENDUNION,
    TK_DEFINETYPE,
    TK_AS,
    TK_TYPE,
    TK_MAIN,
    TK_GLOBAL,
    TK_PARAMETER,
    TK_LIST,
    TK_SQL,
    TK_SQR,
    TK_INPUT,
    TK_OUTPUT,
    TK_INT,
    TK_REAL,
    TK_COMMA,
    TK_SEM,
    TK_COLON,
    TK_DOT,
    TK_ENDWHILE,
    TK_OP,
    TK_CL,
    TK_IF,
    TK_THEN,
    TK_ENDIF,
    TK_READ,
    TK_WRITE,
    TK_RETURN,
    TK_PLUS,
    TK_MINUS,
    TK_MUL,
    TK_DIV,
    TK_CALL,
    TK_RECORD,
    TK_ENDRECORD,
    TK_ELSE,
    TK_AND,
    TK_OR,
    TK_NOT,
    TK_LT,
    TK_LE,
    TK_EQ,
    TK_GT,
    TK_GE,
    TK_NE,
    TK_EOF, // To detect that file has terminated
    TK_EPS,
    TK_ERROR,
    TK_DOLLAR
} tokName;

typedef enum
{
    program,
    otherFunctions,
    mainFunction,
    stmts,
    function,
    input_par,
    output_par,
    parameter_list,
    dataType,
    primitiveDatatype,
    constructedDatatype,
    remaining_list,
    typeDefinitions,
    actualOrRedefined,
    typeDefinition,
    fieldDefinitions,
    fieldDefinition,
    fieldType,
    moreFields,
    declarations,
    declaration,
    global_or_not,
    otherStmts,
    stmt,
    assignmentStmt,
    SingleOrRecId,
    option_single_constructed,
    oneExpansion,
    moreExpansions,
    funCallStmt,
    outputParameters,
    inputParameters,
    iterativeStmt,
    conditionalStmt,
    elsePart,
    ioStmt,
    arithmeticExpression,
    expPrime,
    term,
    termPrime,
    factor,
    highPrecedenceOperator,
    lowPrecedenceOperators,
    booleanExpression,
    var,
    logicalOp,
    relationalOp,
    returnStmt,
    optionalReturn,
    idList,
    more_ids,
    definetypestmt,
    A
} nonterminal;

typedef struct token {
    tokName name; //should be an enum
    int lineNo;
    char* string;
    int integer;
    float realNum;

} TOKEN;

const char* nonterminaltoString(nonterminal nt);
const char* tokenToString(tokName token);

const char* tokenToString(tokName token);
const char* nonterminaltoString(nonterminal nt);

typedef struct st_element * ST_ELEMENT;
typedef struct symbol_table st;
typedef struct symbol_table * ST;

// TABLE STRUCTS
struct st_element {
	char lexeme[LEXEME_MAX_CHARS];
	tokName tk_type;
    ST_ELEMENT next;
};

struct symbol_table {
    struct st_element table[TABLE_SIZE];
    int token_count;
};


// REMOVE LATER-END



// typedef struct{
//     union
//     {
//         tokName t;
//         nonterminal nt;
//     };
//     int is_terminal;
// } symbol;


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

typedef struct predictive_table_node{
    NODE *rule_rhs;
    int is_syn;
    int is_error;
} PNODE;

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
    NODE* temp=f1->head;
    NODE* temp1=f2->head;
    NODE* root=f2->head;

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
    if(temp==NULL) return NULL; // Add null check
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
    // for(int i=0; i<NUM_NON_TERMINALS; i++){
    //     if(non_terminal_cnt[i]>0){
    //         curr->next = (NODE*)malloc(sizeof(NODE));
    //         curr->next->sym.is_terminal=0;
    //         curr->next->sym.nt=i;
    //         curr->next->next=NULL;
    //         curr=curr->next;
    //     }
    // }
    return head;
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


// We dont need a different struct for follow as first has the same fields as follow. isEpsilon will always be 0
FIRST* FOLLOW_NT[NUM_NON_TERMINALS];
FIRST* FOLLOW_T[NUM_TOKENS];

// Copies the follow set from src to dest
void copyFollowSet(FIRST* dest, FIRST* src){
    dest->head = NULL;
    dest->is_filled = src->is_filled;
    dest->has_epsilon = src->has_epsilon;

    NODE* srcNode = src->head;
    NODE* destNode = NULL;

    while (srcNode != NULL) {
        NODE* newNode = (NODE*)malloc(sizeof(NODE));
        newNode->sym = srcNode->sym;
        newNode->next = NULL;

        if (dest->head == NULL) {
            dest->head = newNode;
            destNode = newNode;
        } else {
            destNode->next = newNode;
            destNode = destNode->next;
        }

        srcNode = srcNode->next;
    }
}

// This function is only applied to follow sets of non-terminals
int isEpsilonInFollow(NODE* head){
    NODE* node = head;
    while(node != NULL) {
        if(node->sym.is_terminal == 1 && node->sym.t == TK_EPS) {
            return 1;
        }
        node = node->next;
    }
    return 0;
}

// If the follow set have epsilon, this function removes it
void removeEpsFromFollow(FIRST* followSet){
    NODE* node = followSet->head;
    NODE* prev = NULL;

    while(node != NULL) {
        if(node->sym.is_terminal == 1 && node->sym.t == TK_EPS) {
            if(prev == NULL) {
                followSet->head = node->next;
            } else {
                prev->next = node->next;
            }
            return;
        }
        prev = node;
        node = node->next;
    }
}

// Checks if the two follow sets are equal
int compareFollowSetsForEquality(FIRST* f1, FIRST* f2) {
    int tokens[NUM_TOKENS] = {0}; // Array to mark tokens in f1
    int tokens2[NUM_TOKENS] = {0}; // Array to mark tokens in f2

    NODE* node1 = f1->head;
    NODE* node2 = f2->head;

    while (node1 != NULL) {
        if (node1->sym.is_terminal == 1) {
            tokens[node1->sym.t] = 1; // Mark token in f1
        }
        node1 = node1->next;
    }

    while (node2 != NULL) {
        if (node2->sym.is_terminal == 1) {
            tokens2[node2->sym.t] = 1; // Mark token in f2
        }
        node2 = node2->next;
    }

    // Check if the two arrays are equal
    for (int i = 0; i < NUM_TOKENS; i++) {
        if (tokens[i] != tokens2[i]) {
            return 1; // Arrays are not equal
        }
    }

    return 0; // Arrays are equal
}

// Allocate memory to the follow sets and initialize them to empty. Follow sets of program and mainFunction are initialized to $.
void allocateMemoryAndInitializeToFollowSet(){
    for(int i=0;i<NUM_NON_TERMINALS;i++){
        FOLLOW_NT[i]=(FIRST*)malloc(sizeof(FIRST));
        FOLLOW_NT[i]->head=NULL;
        FOLLOW_NT[i]->is_filled=0;
        FOLLOW_NT[i]->has_epsilon=0;
    }
    for(int i=0;i<NUM_TOKENS;i++){
        FOLLOW_T[i]=(FIRST*)malloc(sizeof(FIRST));
        FOLLOW_T[i]->head=NULL;
        FOLLOW_T[i]->is_filled=0;
        FOLLOW_T[i]->has_epsilon=0;
    }

    symbol s;
    s.is_terminal=0;
    s.t=TK_DOLLAR;

    FOLLOW_NT[program]->head=(NODE*)malloc(sizeof(NODE));
    FOLLOW_NT[program]->head->sym=s;
    FOLLOW_NT[program]->head->next=NULL;
    FOLLOW_NT[program]->is_filled=1;
    FOLLOW_NT[program]->has_epsilon=0;

    FOLLOW_NT[mainFunction]->head=(NODE*)malloc(sizeof(NODE));
    FOLLOW_NT[mainFunction]->head->sym=s;
    FOLLOW_NT[mainFunction]->head->next=NULL;
    FOLLOW_NT[mainFunction]->is_filled=1;
    FOLLOW_NT[mainFunction]->has_epsilon=0;

}

void removeNonTerminalFromFollow(FIRST* followSet, int nonTerminal) {
    NODE* node = followSet->head;
    NODE* prev = NULL;

    while (node != NULL) {
        if (node->sym.is_terminal == 0 && node->sym.nt == nonTerminal) {
            if (prev == NULL) {
                followSet->head = node->next;
            } else {
                prev->next = node->next;
            }
        }
        prev = node;
        node = node->next;
    }
    return;
}

void removeProgramNonTerminalFromFollow() {
    for (int i = 0; i < NUM_NON_TERMINALS; i++) {
        if (i != program) {
            removeNonTerminalFromFollow(FOLLOW_NT[i], program);
        }
    }
}

// Find the follow sets of the non-terminals in the grammar
void findFollowSet(){
    allocateMemoryAndInitializeToFollowSet();
    int isChanged=1;
    int lhs;
    int rhs_sym;
    NODE* rhs;

    while(isChanged==1){
        isChanged=0;
        for(int i=0;i<NUM_RULES;i++){
            
            lhs=rules[i].sym.nt;
            rhs=rules[i].next;
            NODE* temp=rhs;

            while(temp!=NULL){
                if(temp->sym.is_terminal==0 && temp->sym.nt!=mainFunction && temp->sym.nt!=program){
                    rhs_sym=temp->sym.nt;

        
                    FIRST* rhs_rule_set=(FIRST*)malloc(sizeof(FIRST));
                    
                    if(temp->next!=NULL && temp->next->sym.is_terminal==0)copyFollowSet(rhs_rule_set,FIRST_NT[temp->next->sym.nt]);
                    else if(temp->next!=NULL) copyFollowSet(rhs_rule_set,FIRST_T[temp->next->sym.t]);


                    FIRST* temp_follow=(FIRST*)malloc(sizeof(FIRST));
                    temp_follow->head=(NODE*)malloc(sizeof(NODE));
                    copyFollowSet(temp_follow,FOLLOW_NT[rhs_sym]);

                    int isEpsInRhs=0;

                    if(isEpsilonInFollow(rhs_rule_set->head) == 1){
                        isEpsInRhs=1;
                    }

                    removeEpsFromFollow(rhs_rule_set);

                    merge_list(FOLLOW_NT[rhs_sym],rhs_rule_set,0);
                    FOLLOW_NT[rhs_sym]->head=find_unique(FOLLOW_NT[rhs_sym]->head);

                    if( temp->next==NULL){
                        if(rhs_sym!=lhs)merge_list(FOLLOW_NT[rhs_sym],FOLLOW_NT[lhs],0);
                        FOLLOW_NT[rhs_sym]->head=find_unique(FOLLOW_NT[rhs_sym]->head);
                    }


                    else if(isEpsInRhs==1 ){
                        NODE* epsilonTraverse=temp;
                        int epsilonFlag=1;
                        while(epsilonFlag==1 && epsilonTraverse->next!=NULL){
                            epsilonTraverse=epsilonTraverse->next;
                            if(epsilonTraverse->sym.is_terminal==1){
                                merge_list(FOLLOW_NT[rhs_sym],FIRST_T[epsilonTraverse->sym.t],0);
                                epsilonFlag=0;
                            }else{
                                merge_list(FOLLOW_NT[rhs_sym],FIRST_NT[epsilonTraverse->sym.nt],0);
                                if(FIRST_NT[epsilonTraverse->sym.nt]->has_epsilon==0)epsilonFlag=0;
                            }
                        }

                        if(epsilonFlag==1){
                            merge_list(FOLLOW_NT[rhs_sym],FOLLOW_NT[lhs],0);
                        }
                        FOLLOW_NT[rhs_sym]->head=find_unique(FOLLOW_NT[rhs_sym]->head);

                    }

                    int check=compareFollowSetsForEquality(temp_follow,FOLLOW_NT[rhs_sym]);
                    if(check==1)isChanged=1;
                    
                    free(temp_follow);
                    free(rhs_rule_set);

                }
                temp = temp->next;
            }
        }
    }
    removeProgramNonTerminalFromFollow();

}

// Predictive Parse Table Functions -- Add NULL error checks
PNODE** create_predictive_table(){
    PNODE ** predictive_table = (PNODE **) malloc(NUM_NON_TERMINALS * sizeof(PNODE *));
    // Not initalizing pointers to structs (will be NULL if not required in final table)
    return predictive_table;
}

// Our table has one extra column corresponding to EPS token --> never write to eps even if first set contains eps --> should be error always
void push_rule_to_table(NODE* rule_head, FIRST* first_tokens, PNODE** predictive_table){
    nonterminal rule_lhs = rule_head->sym.nt;
    NODE * table_terminal_tokens = first_tokens->head;
    while(table_terminal_tokens != NULL){
        if(table_terminal_tokens->sym.t != TK_EPS){
            if(predictive_table[rule_lhs] == NULL){
                *(predictive_table + rule_lhs) = (PNODE *) malloc(NUM_TOKENS * sizeof(PNODE));
            }
            predictive_table[rule_lhs][table_terminal_tokens->sym.t].rule_rhs = (rule_head->next); // Only RHS of grammar rule
            predictive_table[rule_lhs][table_terminal_tokens->sym.t].is_error = 0;
            predictive_table[rule_lhs][table_terminal_tokens->sym.t].is_syn = 0;
        }
        table_terminal_tokens = table_terminal_tokens->next;
    }
    // All predictive_table[NT][T] which should raise errors are NULL (unitialized pointers)
}

// (Pass rules + i)
void parse_grammar_rule_for_table(NODE* rule_head, NODE** predictive_table) {
    nonterminal rule_lhs = rule_head->sym.nt; // first node in rule is always the LHS NT
    NODE * rhs_cur = rule_head->next;
    if(rhs_cur->sym.is_terminal) {
        FIRST* cur_first = FIRST_T[rhs_cur->sym.t];
        push_rule_to_table(rule_head, cur_first, predictive_table);
        return;
    }else{
        FIRST * cur_first = FIRST_NT[rhs_cur->sym.nt];
        push_rule_to_table(rule_head, cur_first, predictive_table);
        if(cur_first->has_epsilon){
            rhs_cur = rhs_cur->next;
            while(rhs_cur != NULL && cur_first->has_epsilon){
                if(rhs_cur->sym.is_terminal){
                    cur_first = FIRST_T[rhs_cur->sym.t];
                    push_rule_to_table(rule_head, cur_first, predictive_table);
                    return;
                }else{
                    cur_first = FIRST_NT[rhs_cur->sym.nt];
                    push_rule_to_table(rule_head, cur_first, predictive_table);
                    rhs_cur = rhs_cur->next;
                }
            }
            // Checking if the last term in RHS also contained epsilon in first set --> add rule to follow of original NT
            if (rhs_cur == NULL && cur_first->has_epsilon){
                // Ending RHS term must be a NT in this case
                cur_first = FOLLOW_NT[rule_head->sym.nt];
                push_rule_to_table(rule_head, cur_first, predictive_table);
            }
        }
    }
}

void push_syn_for_nt(PNODE** predictive_table, FIRST * cur_first_follow, nonterminal cur_nt){
    NODE * first_head = cur_first_follow->head;
    while(first_head != NULL){
        // skip if epsilon or rule already in place for this in table
        if(first_head->sym.t != TK_EPS && predictive_table[cur_nt][first_head->sym.t].rule_rhs != NULL){
            predictive_table[cur_nt][first_head->sym.t].is_syn = 1;
            predictive_table[cur_nt][first_head->sym.t].is_syn = 0;
        }
        first_head = first_head->next;
    }
}

void populate_predictive_table_syn_tokens(PNODE** predictive_table){
    // For all NT's if FIRST NT's node NULL or FOLLOW(NT) node NULL
    for(int i = 0 ; i < NUM_NON_TERMINALS; i++){
        FIRST* cur_first = FIRST_NT[i];
        FIRST* cur_follow = FOLLOW_NT[i];
        push_syn_for_nt(predictive_table, cur_first, i);
        push_syn_for_nt(predictive_table, cur_follow, i);
    }
}

void populate_predictive_table_error_tokens(PNODE ** predictive_table){
    for(int i = 0; i < NUM_NON_TERMINALS; i++){
        for(int j = 0;  j < NUM_TOKENS; j++){
            // Syn should have already been populated for required tokens
            if(predictive_table[i][j].rule_rhs != NULL && predictive_table[i][j].is_syn == 0){
                predictive_table[i][j].is_error = 1;
            }
        }
    }
}

PNODE** generate_predictive_table(NODE * grammar_rules){
    PNODE ** predictive_table = create_predictive_table(); // Unpopulated
    for(int i = 0; i < NUM_RULES; i++){
        parse_grammar_rule_for_table((grammar_rules+i), predictive_table);
    }
    // Add SYN tokens
    push_syn_tokens(predictive_table);

    // Set ERROR tokens
    populate_predictive_table_error_tokens(predictive_table);

    return predictive_table;
}

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
    }

    // for(int i=0; i<NUM_TOKENS; i++){
    //     print_list(FIRST_T[i]->head);
    // }
    // for(int i=0; i<NUM_NON_TERMINALS; i++){
    //     printf("%s ==>\t",nonterminaltoString(i));
    //     print_list(FIRST_NT[i]->head);
    // }

    // Find the follow sets of the non-terminals in the grammar
    findFollowSet();
    
    // Print the follow sets
    // for(int i=0; i<NUM_NON_TERMINALS; i++){
    //     printf("%s ==>\t",nonterminaltoString(i));
    //     print_list(FOLLOW_NT[i]->head);
    // }




    // printf("\n\n PREDICTIVE TABLE VALID ENTRIES : \n\n");
    // NODE ** predictive_table = generate_predictive_table(rules);

    // for(int i = 0; i < NUM_NON_TERMINALS; i++){
    //     for(int j = 0; j < NUM_TOKENS; j++){
    //         if((predictive_table[i][j].next) != NULL){
    //             nonterminal a = i;
    //             tokName b = j;
    //             printf("%s (on) %s : ", nonterminaltoString(a), tokenToString(b));
    //             print_list(predictive_table[i]+j);
    //         }else{
    //             // Rest are errors
    //             // printf("%d, %d : ERROR \n", i,j);
    //         }
    //     }
    // }


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

    struct StackNode* stack = NULL;
    symbol sym1 = {.t = TK_DOLLAR, .is_terminal = 1};
    push(&stack, sym1);
    symbol sym2 = {.nt = program, .is_terminal = 0};
    push(&stack, sym2);
    ST stable = create_symbol_table();
    populate_symbol_table(stable);
    
    int call_token=1;
    TOKEN curr;
    while(1){
        if(call_token){
            // curr = tokenizer(stable);
        }

    }
}



const char* tokenToString(tokName token) {
    switch(token) {
        case TK_ASSIGNOP: return "TK_ASSIGNOP";
        case TK_COMMENT: return "TK_COMMENT";
        case TK_FIELDID: return "TK_FIELDID";
        case TK_ID: return "TK_ID";
        case TK_NUM: return "TK_NUM";
        case TK_RNUM: return "TK_RNUM";
        case TK_FUNID: return "TK_FUNID";
        case TK_RUID: return "TK_RUID";
        case TK_WITH: return "TK_WITH";
        case TK_PARAMETERS: return "TK_PARAMETERS";
        case TK_END: return "TK_END";
        case TK_WHILE: return "TK_WHILE";
        case TK_UNION: return "TK_UNION";
        case TK_ENDUNION: return "TK_ENDUNION";
        case TK_DEFINETYPE: return "TK_DEFINETYPE";
        case TK_AS: return "TK_AS";
        case TK_TYPE: return "TK_TYPE";
        case TK_MAIN: return "TK_MAIN";
        case TK_GLOBAL: return "TK_GLOBAL";
        case TK_PARAMETER: return "TK_PARAMETER";
        case TK_LIST: return "TK_LIST";
        case TK_SQL: return "TK_SQL";
        case TK_SQR: return "TK_SQR";
        case TK_INPUT: return "TK_INPUT";
        case TK_OUTPUT: return "TK_OUTPUT";
        case TK_INT: return "TK_INT";
        case TK_REAL: return "TK_REAL";
        case TK_COMMA: return "TK_COMMA";
        case TK_SEM: return "TK_SEM";
        case TK_COLON: return "TK_COLON";
        case TK_DOT: return "TK_DOT";
        case TK_ENDWHILE: return "TK_ENDWHILE";
        case TK_OP: return "TK_OP";
        case TK_CL: return "TK_CL";
        case TK_IF: return "TK_IF";
        case TK_THEN: return "TK_THEN";
        case TK_ENDIF: return "TK_ENDIF";
        case TK_READ: return "TK_READ";
        case TK_WRITE: return "TK_WRITE";
        case TK_RETURN: return "TK_RETURN";
        case TK_PLUS: return "TK_PLUS";
        case TK_MINUS: return "TK_MINUS";
        case TK_MUL: return "TK_MUL";
        case TK_DIV: return "TK_DIV";
        case TK_CALL: return "TK_CALL";
        case TK_RECORD: return "TK_RECORD";
        case TK_ENDRECORD: return "TK_ENDRECORD";
        case TK_ELSE: return "TK_ELSE";
        case TK_AND: return "TK_AND";
        case TK_OR: return "TK_OR";
        case TK_NOT: return "TK_NOT";
        case TK_LT: return "TK_LT";
        case TK_LE: return "TK_LE";
        case TK_EQ: return "TK_EQ";
        case TK_GT: return "TK_GT";
        case TK_GE: return "TK_GE";
        case TK_NE: return "TK_NE";
        case TK_EOF: return "TK_EOF";
        case TK_ERROR: return "TK_ERROR";
        case TK_EPS: return "TK_EPS";
        case TK_DOLLAR: return "TK_DOLLAR";
        default: return "Unknown Token";
    }
}

const char* nonterminaltoString(nonterminal nt) {
    switch(nt) {
        case program: return "<program>";
        case otherFunctions: return "<otherFunctions>";
        case mainFunction: return "<mainFunction>";
        case stmts: return "<stmts>";
        case function: return "<function>";
        case input_par: return "<input_par>";
        case output_par: return "<output_par>";
        case parameter_list: return "<parameter_list>";
        case dataType: return "<dataType>";
        case primitiveDatatype: return "<primitiveDatatype>";
        case constructedDatatype: return "<constructedDatatype>";
        case remaining_list: return "<remaining_list>";
        case typeDefinitions: return "<typeDefinitions>";
        case actualOrRedefined: return "<actualOrRedefined>";
        case typeDefinition: return "<typeDefinition>";
        case fieldDefinitions: return "<fieldDefinitions>";
        case fieldDefinition: return "<fieldDefinition>";
        case fieldType: return "<fieldType>";
        case moreFields: return "<moreFields>";
        case declarations: return "<declarations>";
        case declaration: return "<declaration>";
        case global_or_not: return "<global_or_not>";
        case otherStmts: return "<otherStmts>";
        case stmt: return "<stmt>";
        case assignmentStmt: return "<assignmentStmt>";
        case SingleOrRecId: return "<SingleOrRecId>";
        case option_single_constructed: return "<option_single_constructed>";
        case oneExpansion: return "<oneExpansion>";
        case moreExpansions: return "<moreExpansions>";
        case funCallStmt: return "<funCallStmt>";
        case outputParameters: return "<outputParameters>";
        case inputParameters: return "<inputParameters>";
        case iterativeStmt: return "<iterativeStmt>";
        case conditionalStmt: return "<conditionalStmt>";
        case elsePart: return "<elsePart>";
        case ioStmt: return "<ioStmt>";
        case arithmeticExpression: return "<arithmeticExpression>";
        case expPrime: return "<expPrime>";
        case term: return "<term>";
        case termPrime: return "<termPrime>";
        case factor: return "<factor>";
        case highPrecedenceOperator: return "<highPrecedenceOperator>";
        case lowPrecedenceOperators: return "<lowPrecedenceOperators>";
        case booleanExpression: return "<booleanExpression>";
        case var: return "<var>";
        case logicalOp: return "<logicalOp>";
        case relationalOp: return "<relationalOp>";
        case returnStmt: return "<returnStmt>";
        case optionalReturn: return "<optionalReturn>";
        case idList: return "<idList>";
        case more_ids: return "<more_ids>";
        case definetypestmt: return "<definetypestmt>";
        case A: return "<A>";
        case TK_DOLLAR : return "TK_DOLLAR";
        default: return "Unknown";
    }
}



void parsing(){

}




