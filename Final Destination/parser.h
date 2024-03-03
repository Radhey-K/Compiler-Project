#ifndef PARSER_H
#define PARSER_H
#include "parserDef.h"


void merge_list(FIRST* f1, FIRST* f2, int flag);
void find_first_set(symbol sym);
NODE* find_unique(NODE* root);
void print_list(NODE* root);
void copyFollowSet(FIRST* dest, FIRST* src);
int isEpsilonInFollow(NODE* head);
void removeEpsFromFollow(FIRST* followSet);
int compareFollowSetsForEquality(FIRST* f1, FIRST* f2);
void allocateMemoryAndInitializeToFollowSet();
void removeNonTerminalFromFollow(FIRST* followSet, int nonTerminal);
void removeProgramNonTerminalFromFollow();
void findFollowSet();
PNODE** create_predictive_table();
void push_rule_to_table(NODE* rule_head, FIRST* first_tokens, PNODE** predictive_table);
void parse_grammar_rule_for_table(NODE* rule_head, NODE** predictive_table);
void push_syn_for_nt(PNODE** predictive_table, FIRST * cur_first_follow, nonterminal cur_nt);
void populate_predictive_table_syn_tokens(PNODE** predictive_table);
void populate_predictive_table_error_tokens(PNODE ** predictive_table);
PNODE** generate_predictive_table(NODE * grammar_rules);
int getTerminal(const char* token);
int getNonTerminal(char* token);


#endif