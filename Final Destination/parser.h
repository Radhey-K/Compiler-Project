/* ---------Group Information---------
Group Number: 15

Yash Mundada: 2021A7PS0001P
Aaryan Garg: 2021A7PS2222P
Dev Kulkarni: 2021A7PS2430P
Radhey Kanade: 2021A7PS2534P
Shardul Shingare: 2021A7PS2539P
Shantanu Ambekar: 2021A7PS2540P
--------------------------------------*/

#ifndef PARSER_H
#define PARSER_H
#include "parserDef.h"

void parser_main(char * filename, char *outfile);
void merge_list(FIRSTFOLLOW* f1, FIRSTFOLLOW* f2, int flag);
void find_first_set(symbol sym);
NODE* find_unique(NODE* root);
void print_list(NODE* root);
void copyFollowSet(FIRSTFOLLOW* dest, FIRSTFOLLOW* src);
int isEpsilonInFollow(NODE* head);
void removeEpsFromFollow(FIRSTFOLLOW* followSet);
int compareFollowSetsForEquality(FIRSTFOLLOW* f1, FIRSTFOLLOW* f2);
void allocateMemoryAndInitializeToFollowSet();
void removeNonTerminalFromFollow(FIRSTFOLLOW* followSet, int nonTerminal);
void removeProgramNonTerminalFromFollow();
void findFollowSet();
PNODE** create_predictive_table();
void push_rule_to_table(NODE* rule_head, FIRSTFOLLOW* first_tokens, PNODE** predictive_table);
void parse_grammar_rule_for_table(NODE* rule_head, PNODE** predictive_table);
void push_syn_for_nt(PNODE** predictive_table, FIRSTFOLLOW * cur_first_follow, nonterminal cur_nt);
void populate_predictive_table_syn_tokens(PNODE** predictive_table);
void populate_predictive_table_error_tokens(PNODE ** predictive_table);
PNODE** generate_predictive_table(NODE * grammar_rules);
int getTerminal(const char* token);
int getNonTerminal(char* token);


#endif