
/* ---------Group Information---------
Group Number: 15

Yash Mundada: 2021A7PS0001P
Aaryan Garg: 2021A7PS2222P
Dev Kulkarni: 2021A7PS2430P
Radhey Kanade: 2021A7PS2534P
Shardul Shingare: 2021A7PS2539P
Shantanu Ambekar: 2021A7PS2540P
--------------------------------------*/
#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#define LEXEME_MAX_CHARS 30
#define TABLE_SIZE 197
#include "lexer.h"
#include "lexerDef.h"

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

// FUNCTIONS

ST create_symbol_table();

int hash_lexeme(char *lexeme);

void table_insert(ST stable, char *lexeme, tokName tk_type);

ST_ELEMENT table_lookup(ST stable, char *lexeme);

void populate_symbol_table(ST stable);

#endif