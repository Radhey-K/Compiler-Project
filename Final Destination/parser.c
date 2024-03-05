/* ---------Group Information---------
Group Number: 15

Yash Mundada: 2021A7PS0001P
Aaryan Garg: 2021A7PS2222P
Dev Kulkarni: 2021A7PS2430P
Radhey Kanade: 2021A7PS2534P
Shardul Shingare: 2021A7PS2539P
Shantanu Ambekar: 2021A7PS2540P
--------------------------------------*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "lexer.h"
#include "lexerDef.h"
#include "symbol_table.h"
#include "parsetree.h"
#include "stack.h"
#include "parserDef.h"
#include "parser.h"

// Definitions
NODE rules[NUM_RULES];
FIRST* FIRST_T[NUM_TOKENS];
FIRST* FIRST_NT[NUM_NON_TERMINALS];
FIRST* FOLLOW_NT[NUM_NON_TERMINALS];
FIRST* FOLLOW_T[NUM_TOKENS];

int symEqual(symbol s1, symbol s2){
    if(s1.is_terminal != s2.is_terminal)return 0;
    if(s1.is_terminal){
        if(s1.t==s2.t)return 1;
    }else{
        if(s1.nt==s2.nt)return 1;
    }
    return 0;
}

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
}

void find_first_set(symbol sym){

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

    return head;
}

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
void parse_grammar_rule_for_table(NODE* rule_head, PNODE** predictive_table) {
    nonterminal rule_lhs = rule_head->sym.nt; // first node in rule is always the LHS NT
    NODE * rhs_cur = rule_head->next;
    if(rhs_cur->sym.is_terminal) {
        FIRST* cur_first;
        if(rhs_cur->sym.t == TK_EPS){
            cur_first = FOLLOW_NT[rule_lhs]; // NT --> TK_EPS in table (if rule is directly TK_EPS)
        }else{
            cur_first = FIRST_T[rhs_cur->sym.t];
        }
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
        if(first_head->sym.t != TK_EPS && predictive_table[cur_nt][first_head->sym.t].rule_rhs == NULL){
            predictive_table[cur_nt][first_head->sym.t].is_syn = 1;
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
    int count = 0;
    for(int i = 0; i < NUM_NON_TERMINALS; i++){
        for(int j = 0;  j < NUM_TOKENS; j++){
            // Syn should have already been populated for required tokens
            if(predictive_table[i][j].rule_rhs == NULL && predictive_table[i][j].is_syn == 0){
                predictive_table[i][j].is_error = 1;
                count += 1;
            }
        }
    }
    // printf("ERROR TOKENS : %d \n", count);
}

PNODE** generate_predictive_table(NODE * grammar_rules){
    PNODE ** predictive_table = create_predictive_table(); // Unpopulated
    for(int i = 0; i < NUM_RULES; i++){
        parse_grammar_rule_for_table((grammar_rules+i), predictive_table);
    }
    // Add SYN tokens
    populate_predictive_table_syn_tokens(predictive_table);

    // Set ERROR tokens
    populate_predictive_table_error_tokens(predictive_table);

    return predictive_table;
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

void push_list(NODE * rule_head, struct StackNode ** stack, Node * treeNode, int syn_eps){
    if(syn_eps){
        // Adding NT --> e if syn encountered
        symbol eps;
        eps.is_terminal = 1;
        eps.t = TK_EPS;
        add_child(NULL, treeNode, eps);
        return;
    }
    if(rule_head ==  NULL) {
        return;
    }else if(rule_head->sym.is_terminal && rule_head->sym.t == TK_EPS) { // Don't push to stack but write to tree
        // Popping NT off stack --> NT --> E
        // printf("POPPING FROM STACK WITH NO REPLACEMENT \n");
        add_child(NULL, treeNode, rule_head->sym);
        return;
    }
    push_list(rule_head->next, stack, treeNode, 0);
    // ADD child + push child pointer into stack along with the current sym
    Node * tree_child_pointer = add_child(NULL, treeNode, rule_head->sym);
    push(stack, rule_head->sym,tree_child_pointer);
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

void parser_main(char * filename){
    // parse_state = 0;
    // parse_lineNo = 1;
    // parse_filePointer = NULL;
    // parse_sharedvar = 0;
    // memset(parse_buffer, 0, 2 * BUFFER_SIZE);
    // parse_lexemeBegin = 0;
    // parse_forwardPtr = 0;
    stateInfo *s = (stateInfo *)malloc(sizeof(stateInfo));
    initialize(s);

    s->filePointer = fopen(filename, "r");
    if (s->filePointer == NULL) {
        printf("\033[0;31m");
        printf("Error: Source file not found\n");
        printf("\033[0m");
        return;
    }

    populateBuffer(0, s);

    char line[256];
    char lhs[256];
    int i = 0;

    FILE *fp = fopen("grammar.txt", "r");
    if (fp == NULL)
    {
        printf("\033[0;31m");
        printf("Error: File not found\n");
        printf("\033[0m");
        return;
    }

    while (fscanf(fp, "%[^\n]\n", line) != EOF)
    {
        char *token = strtok(line, " |");
        strcpy(lhs, token);

        rules[i].sym.nt = getNonTerminal(lhs);
        rules[i].sym.is_terminal = 0;

        token = strtok(NULL, " |");
       
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
            token = strtok(NULL, " |\r\n\t");
        }
        i++;
    }
    printf("Grammar rules parser\n");

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

    findFollowSet();

    PNODE** predictive_table = generate_predictive_table(rules);

    struct StackNode* stack = NULL;

    // TK_DOLLAR not in tree
    symbol sym1 = {.t = TK_DOLLAR, .is_terminal = 1};
    push(&stack, sym1, NULL);
    
    // Create root node in tree + push pointer to stack with symbol
    symbol sym2 = {.nt = program, .is_terminal = 0};
    ParseTree * ast = create_parse_tree_with_root(sym2);
    push(&stack, sym2, ast->root);
    
    int call_token=1;
    TOKEN curr;

    ST stable = create_symbol_table();
    populate_symbol_table(stable);

    while(s->buffer[s->forwardPtr] != EOF){
        if(call_token){
            curr = tokenizer(stable, s);
        }
        if(curr.name == TK_EOF) {
            break;
        }else if (strstr(curr.string, "pattern") != NULL) {
            char *substring = strstr(curr.string, "pattern");
            substring += strlen("pattern");
            printf("\033[0;31m");
            printf("Line no.%3d: Error : Unknown pattern <%s>\n", curr.lineNo, substring);
            printf("\033[0;m");
            call_token = 1;
        }
        else if (strstr(curr.string, "size") != NULL) {
            char *substring = strstr(curr.string, "size");
            substring += strlen("size");
            printf("\033[0;31m");
            printf("Line no.%3d: Error : Variable Identifier is longer than the prescribed length of 20 characters.\n", curr.lineNo);
            printf("\033[0;m");
            call_token = 1;
        }
        else if (strstr(curr.string, "symbol") != NULL) {
            char *substring = strstr(curr.string, "symbol");
            substring += strlen("symbol");
            printf("\033[0;31m");
            printf("Line no.%3d: Error : Unknown symbol <%s>\n", curr.lineNo, substring);
            printf("\033[0;m");
            call_token = 1;
        }
        else{
            // Token retrieved is valid --> can continue parsing
            struct StackNode * cur_top = top(stack);
            symbol sym = cur_top->data;
            if(curr.name == TK_COMMENT){
                continue;
            }
            if(sym.is_terminal==0 && predictive_table[sym.nt][curr.name].is_syn==0 && predictive_table[sym.nt][curr.name].is_error==0){
                pop(&stack);
                // printf("%s ==> ", parse_nonterminaltoString(sym.nt)); // Print the LHS of the rule
                // print_list(predictive_table[sym.nt][curr.name].rule_rhs);
                push_list(predictive_table[sym.nt][curr.name].rule_rhs, &stack, cur_top->treeNode, 0);
                free(cur_top);
                call_token=0;
                continue;
            }else if(sym.is_terminal==0 && predictive_table[sym.nt][curr.name].is_syn==1){
                // SYN
                printf("\033[0;31m");
                printf("Line no.%3d: (SYN) The token %s for lexeme %s does not match with expected nonterminal %s \n",curr.lineNo, tokenToString(curr.name), curr.string, nonterminaltoString(sym.nt));
                printf("\033[0;m");
                pop(&stack);
                push_list(NULL, &stack, cur_top->treeNode, 1);
                free(cur_top);
                call_token=0;
                continue;
            }else if(sym.is_terminal==0 && predictive_table[sym.nt][curr.name].is_error==1){
                // ERROR
                printf("\033[0;31m");
                printf("Line no.%3d: (ERROR) The token %s for lexeme %s does not match with expected nonterminal %s \n",curr.lineNo, tokenToString(curr.name), curr.string, nonterminaltoString(sym.nt));
                printf("\033[0;m");
                call_token=1;
                continue;
            }else if(sym.is_terminal==1){
                if(sym.t == curr.name){
                    pop(&stack);
                    free(cur_top);
                    call_token=1;
                    continue;
                }else{
                    printf("\033[0;31m");
                    printf("Line no.%3d: (TERMINAL NON MATCH) The token %s for lexeme %s does not match with expected token %s \n",curr.lineNo, tokenToString(curr.name), curr.string, tokenToString(sym.t));
                    printf("\033[0;m");
                    pop(&stack);
                    free(cur_top);
                    call_token = 0;
                    continue;
                }
            }
        }
    }

    printf("\n\n");
    print_inorder(ast->root);
}