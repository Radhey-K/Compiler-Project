#ifndef LEXER_H
#define LEXER_H
#include "lexerDef.h"

const char* nonterminaltoString(nonterminal nt);
const char* tokenToString(tokName token);
void lexer_main();
void removeComments(char *filename);

#endif