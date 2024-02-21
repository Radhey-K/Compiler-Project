typedef struct TOKEN{} TOKEN;
int state;

int digit_2to7(char c){
    int num = c - '0';
    if(num >= 2 && num <= 7)return 1;
    else return 0;
} 

int lowercase(char c){
    int num = c - 'a';
    if(num >= 0 && num <= 25)return 1;
    else return 0;
}

// handling TK_ID and TK_FIELDID
TOKEN get_token(){
    TOKEN mytoken;
    char c;
    while(1){
        switch(state){
            case 12:    c = getCharacter();
                        if(digit_2to7(c)) state = 13;
                        else if(lowercase(c)) state = 16;
                        else state = 17;
                        break;
            case 13:    c = getCharacter();
                        if(c=='b' || c=='c' || c=='d') state = 13;
                        else if(digit_2to7(c)) state = 14;
                        else state = 15;
                        break;
            case 14:    c = getCharacter();
                        if(digit_2to7(c)) state = 14;
                        else state = 15;
                        break;
            case 15:    retract(1); //TK_ID
                        mytoken.token = search_token();
                        if(mytoken.token == IDENTIFIER) mytoken.value = get_id_string();
                        return mytoken;
            case 16:    c = getCharacter();
                        if(lowercase(c)) state = 16;
                        else state = 17;
                        break;
            case 17:    retract(1); //TK_FIELDID
                        mytoken.token = search_token();
                        if(mytoken.token == IDENTIFIER) mytoken.value = get_id_string();
                        return mytoken;           
        }
    }
}