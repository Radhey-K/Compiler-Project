// Implement st_element struct
// Create a st_table of type st_elements
// element struct --> string(lexeme) + tkName --> enum value??
// dont need a is_present bool --> it'll just be null (insert can be based on wether the pointer is null or not)

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#define LEXEME_MAX_CHARS 30
#define TABLE_SIZE 197
#include "lexer.h"

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