#define HELLO 50

typedef struct TOKEN{} TOKEN;
int state;

int isdigit(char c) {
    int num = c - '0';
    if(num >= 0 && num <= 9) return 1;
    else return 0;

}

TOKEN get_token() {
    TOKEN mytoken;
    char c;
    while(1) {
        switch(state) {
            case 1: c = getCharacter();
                    if(c == '.') state = 2;
                    else if(isdigit(c)) state = 1;
                    else state = 9;
                    break;
                
            case 2: c = getCharacter();
                    if(isdigit(c)) state = 3;
                    else state = 10;
                    break;

            case 3: c = getCharacter();
                    if(isdigit(c)) state = 4;
                    else exit(0);  // Error
                    break;

            case 4: c = getCharacter();
                    if (c == 'E') state = 5;
                    else state = 11;
                    break;

            case 5: c = getCharacter();
                    if (c == '+' || c == '-') state = 6;
                    else if (isdigit(c)) state = 7;
                    else exit(0);  // Error
                    break;

            case 6: c = getCharacter();
                    if (isdigit(c)) state = 7;
                    else exit(0);  // Error
                    break;

            case 7: c = getCharacter();
                    if (isdigit(c)) state = 8;
                    else exit(0);
                    break;

            case 8: // TK_RNUM
                    mytoken.token == "TK_RNUM"
                    mytoken.value = get_num_string();
                    return mytoken;

            case 9: //TK_NUM    
                    retract(1); 
                    mytoken.token = "TK_NUM"
                    mytoken.value = get_num_string();
                    return mytoken;

            case 10: //TK_NUM
                    retract(2);
                    mytoken.token = "TK_NUM"
                    mytoken.value = get_num_string();
                    return mytoken;

            case 11: //TK_RNUM
                    retract(1);
                    mytoken.token = "TK_RNUM"
                    mytoken.value = get_num_string();
                    return mytoken;
        }
    }
}