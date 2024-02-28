#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <time.h>

void populate_follow_sets() {
  bool is_changed = true;
  int lhs;
  int rhs_sym;
  rhsnode_ptr rhs_ptr;

  set_add_elem(follow_set[MAINPROGRAM], DOLLAR);

  while (is_changed == true) // traverse until convergence
  {
	is_changed = false;
	for (int i = 0; i < NUM_OF_RULES; i++) {
	  lhs = grammar[i].lhs;
	  rhs_ptr = grammar[i].head;
	  rhsnode_ptr temp = rhs_ptr;
      
	  while (temp != NULL) // traverse till end of the rule
	  {
		if ((temp->sym).is_terminal == false) {
		  rhs_sym = ((temp->sym).nt);
		  set rhs_rule_set = get_rule_first_set(temp->next);
		  set tmp_follow = (ull *)malloc(sizeof(ull) * SET_SIZE);

		  if (tmp_follow == NULL) {
			perror("Follow set memory allocation failed\n");
			exit(1);
		  }

		  for (int j = 0; j < SET_SIZE; j++) {
			tmp_follow[j] = follow_set[rhs_sym][j];
		  }

		  bool eps_in_rhs = false;

		  if (set_find_elem(rhs_rule_set, EPSILON) ==
			  true) // eps present in this rule
		  {
			eps_in_rhs = true;
		  }

		  set_remove_elem(rhs_rule_set, EPSILON);

		  set_union(follow_set[rhs_sym], follow_set[rhs_sym], rhs_rule_set);

		  if ((eps_in_rhs == true) || (temp->next == NULL)) {
			set_union(follow_set[rhs_sym], follow_set[rhs_sym],
					  follow_set[lhs]);
		  }

		  for (int j = 0; j < SET_SIZE; j++) {
			if (follow_set[rhs_sym][j] != tmp_follow[j]) {
			  is_changed = true;
			}
		  }
		  free(tmp_follow);
		}
		temp = temp->next;
	  } // end of rule linked list traversal while loop
	}   // end of for - grammar traversal
  }     // end of while - infinite loop until convergence
}