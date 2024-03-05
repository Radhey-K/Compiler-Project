/* ---------Group Information---------
Group Number: 15

Yash Mundada: 2021A7PS0001P
Aaryan Garg: 2021A7PS2222P
Dev Kulkarni: 2021A7PS2430P
Radhey Kanade: 2021A7PS2534P
Shardul Shingare: 2021A7PS2539P
Shantanu Ambekar: 2021A7PS2540P
--------------------------------------*/

#ifndef LEXER_H
#define LEXER_H
#include "lexerDef.h"
#include "symbol_table.h"
#include "lexer.h"

typedef struct symbol_table * ST;

const char* nonterminaltoString(nonterminal nt);
const char* tokenToString(tokName token);
void lexer_main(char *filename);
void removeComments(char *filename);
TOKEN tokenizer(ST stable, stateInfo *s);
void initialize(stateInfo *s);
void populateBuffer(int whichbuffer, stateInfo *s);

#endif