To run the program:-
    1) Run "make build"
    2) Run "./stage1exe {source_code} {output_file}"
        Eg:- ./stage1exe t6.txt parsetreeOutfile.txt
    3) Use the interface as required
    4) Run "make clean" to delete stage1exe executable



Error Recovery Heuristic used:-
    If predictive parse table has error, we get the next token in input,
    If predictive parse table has syn token, we pop the stack,
    We have also added semicolon,, ), ], and keywords like endwhile, endif, endrecord in the synchronization set,
    If the terminal on top of the stack does not match with input token, we pop the stack.
