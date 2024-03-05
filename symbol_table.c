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
#include "symbol_table.h"
#include "lexer.h"

ST create_symbol_table() {
    ST symbol_table = (ST) malloc(sizeof(st));
    if (symbol_table == NULL) {
        printf("Symbol table creation failed");
    }
    symbol_table->token_count = 0;
    return symbol_table;
}

ST_ELEMENT create_st_element(){
    ST_ELEMENT st_ele = (ST_ELEMENT) malloc(sizeof(struct st_element));
    if (st_ele == NULL) {
        printf("Unable to create new symbol table element");
        return NULL;
    }
    st_ele->next = NULL;
    st_ele->tk_type = TK_UNKNOWN;
    return st_ele;
}

int hash_lexeme(char *lexeme) {
    unsigned long hash = 5381;
    int c;

    while ((c = *lexeme++))
        hash = ((hash << 5) + hash) + c; 

    return hash % TABLE_SIZE;
}

void table_insert(ST stable, char *lexeme, tokName tk_type){
    if (strlen(lexeme) > MAX_FUN_ID_SIZE) {
        printf("Insertion error - max lexeme length exceeded");
        return;
    }
    int insert_index = hash_lexeme(lexeme);
    if (stable == NULL) {
        printf("Symbol table not initialized");
        return;
    }

    ST_ELEMENT head = (stable->table + insert_index);
    while(head->next != NULL){
        head = head->next;
    }
    ST_ELEMENT new_ele = create_st_element();
    strcpy(new_ele->lexeme, lexeme);
    new_ele->tk_type = tk_type;
    head->next = new_ele;
    new_ele->next = NULL;
    stable->token_count += 1;
    return;
}

ST_ELEMENT table_lookup(ST stable, char *lexeme) {
    int insert_index = hash_lexeme(lexeme);
    if (stable == NULL) {
        printf("Symbol table not initialized");
        return NULL;
    }
    ST_ELEMENT head = (stable->table + insert_index);
    while(head) {
        if (strcmp(lexeme, head->lexeme) == 0 ){
            return head;
        }
        head = head->next;
    }
    return NULL;
}

int fetch_table_count(ST stable){
    if (stable == NULL) {
        printf("Symbol table not initialized");
        return -1;
    }
    return stable->token_count;
}

void populate_symbol_table(ST stable){
    table_insert(stable, "<---", TK_ASSIGNOP);
    table_insert(stable, "%", TK_COMMENT);
    table_insert(stable, "with", TK_WITH);
    table_insert(stable, "parameters", TK_PARAMETERS);
    table_insert(stable, "end", TK_END);
    table_insert(stable, "while", TK_WHILE);
    table_insert(stable, "union", TK_UNION);
    table_insert(stable, "endunion", TK_ENDUNION);
    table_insert(stable, "definetype", TK_DEFINETYPE);
    table_insert(stable, "as", TK_AS);
    table_insert(stable, "type", TK_TYPE);
    table_insert(stable, "_main", TK_MAIN);
    table_insert(stable, "global", TK_GLOBAL);
    table_insert(stable, "parameter", TK_PARAMETER);
    table_insert(stable, "list", TK_LIST);
    table_insert(stable, "[", TK_SQL);
    table_insert(stable, "]", TK_SQR);
    table_insert(stable, "input", TK_INPUT);
    table_insert(stable, "output", TK_OUTPUT);
    table_insert(stable, "int", TK_INT);
    table_insert(stable, "real", TK_REAL);
    table_insert(stable, ",", TK_COMMA);
    table_insert(stable, ";", TK_SEM);
    table_insert(stable, ":", TK_COLON);
    table_insert(stable, ".", TK_DOT);
    table_insert(stable, "endwhile", TK_ENDWHILE);
    table_insert(stable, "(", TK_OP);
    table_insert(stable, ")", TK_CL);
    table_insert(stable, "if", TK_IF);
    table_insert(stable, "then", TK_THEN);
    table_insert(stable, "endif", TK_ENDIF);
    table_insert(stable, "read", TK_READ);
    table_insert(stable, "write", TK_WRITE);
    table_insert(stable, "return", TK_RETURN);
    table_insert(stable, "+", TK_PLUS);
    table_insert(stable, "-", TK_MINUS);
    table_insert(stable, "*", TK_MUL);
    table_insert(stable, "/", TK_DIV);
    table_insert(stable, "call", TK_CALL);
    table_insert(stable, "record", TK_RECORD);
    table_insert(stable, "endrecord", TK_ENDRECORD);
    table_insert(stable, "else", TK_ELSE);
    table_insert(stable, "&&&", TK_AND);
    table_insert(stable, "@@@", TK_OR);
    table_insert(stable, "~", TK_NOT);
    table_insert(stable, "<", TK_LT);
    table_insert(stable, "<=", TK_LE);
    table_insert(stable, "==", TK_EQ);
    table_insert(stable, ">", TK_GT);
    table_insert(stable, ">=", TK_GE);
    table_insert(stable, "!=", TK_NE);
}