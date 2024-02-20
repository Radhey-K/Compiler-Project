#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
// #include "helperFunc.c"
#include <ctype.h>

#define BUFFER_SIZE 50


// State for dfa
int state=0;

int lexemeBegin,forwardPtr;
int tokenSize=0;
int lineNo=0;

// Twin buffer
char buffer[2*BUFFER_SIZE];
FILE* filePointer;

int isDigit(char c){
    return isdigit(c) != 0;
}

int isLetter(char c){
    return isalpha(c) != 0;
}

int isNumber(char c){
    return isalnum(c) != 0;
}

int isLowercase(char c){
    return islower(c) != 0;
}

const char* tokenToString(tokName token) {
    switch(token) {
        case TK_ASSIGNOP: return "TK_ASSIGNOP";
        case TK_COMMENT: return "TK_COMMENT";
        case TK_FIELDID: return "TK_FIELDID";
        case TK_ID: return "TK_ID";
        case TK_NUM: return "TK_NUM";
        case TK_RNUM: return "TK_RNUM";
        case TK_FUNID: return "TK_FUNID";
        case TK_RUID: return "TK_RUID";
        case TK_WITH: return "TK_WITH";
        case TK_PARAMETERS: return "TK_PARAMETERS";
        case TK_END: return "TK_END";
        case TK_WHILE: return "TK_WHILE";
        case TK_UNION: return "TK_UNION";
        case TK_ENDUNION: return "TK_ENDUNION";
        case TK_DEFINETYPE: return "TK_DEFINETYPE";
        case TK_AS: return "TK_AS";
        case TK_TYPE: return "TK_TYPE";
        case TK_MAIN: return "TK_MAIN";
        case TK_GLOBAL: return "TK_GLOBAL";
        case TK_PARAMETER: return "TK_PARAMETER";
        case TK_LIST: return "TK_LIST";
        case TK_SQL: return "TK_SQL";
        case TK_SQR: return "TK_SQR";
        case TK_INPUT: return "TK_INPUT";
        case TK_OUTPUT: return "TK_OUTPUT";
        case TK_INT: return "TK_INT";
        case TK_REAL: return "TK_REAL";
        case TK_COMMA: return "TK_COMMA";
        case TK_SEM: return "TK_SEM";
        case TK_COLON: return "TK_COLON";
        case TK_DOT: return "TK_DOT";
        case TK_ENDWHILE: return "TK_ENDWHILE";
        case TK_OP: return "TK_OP";
        case TK_CL: return "TK_CL";
        case TK_IF: return "TK_IF";
        case TK_THEN: return "TK_THEN";
        case TK_ENDIF: return "TK_ENDIF";
        case TK_READ: return "TK_READ";
        case TK_WRITE: return "TK_WRITE";
        case TK_RETURN: return "TK_RETURN";
        case TK_PLUS: return "TK_PLUS";
        case TK_MINUS: return "TK_MINUS";
        case TK_MUL: return "TK_MUL";
        case TK_DIV: return "TK_DIV";
        case TK_CALL: return "TK_CALL";
        case TK_RECORD: return "TK_RECORD";
        case TK_ENDRECORD: return "TK_ENDRECORD";
        case TK_ELSE: return "TK_ELSE";
        case TK_AND: return "TK_AND";
        case TK_OR: return "TK_OR";
        case TK_NOT: return "TK_NOT";
        case TK_LT: return "TK_LT";
        case TK_LE: return "TK_LE";
        case TK_EQ: return "TK_EQ";
        case TK_GT: return "TK_GT";
        case TK_GE: return "TK_GE";
        case TK_NE: return "TK_NE";
        case TK_EOF: return "TK_EOF";
        default: return "Unknown Token";
    }
}





int populateBuffer(int whichBuffer){
    if(whichBuffer==0){
        fread(buffer, BUFFER_SIZE, 1, filePointer);
        // printf("%s \n", buffer);
    }
    else{
        fread(buffer+BUFFER_SIZE, BUFFER_SIZE, 1, filePointer);
        // printf("%s \n", buffer);
    }
    return 0;
}



char getCharacter(){
    if(forwardPtr==BUFFER_SIZE){
        populateBuffer(0);
    }
    else if(forwardPtr==2*BUFFER_SIZE){
        populateBuffer(1);
        forwardPtr=0;
    }
    char c=buffer[forwardPtr];
    // printf("%c",c);

    if (buffer[forwardPtr] == '\0') {
        return EOF;
    }


    forwardPtr++;
    return c;
}

void retract(int noOfRetractions){

}

// Returns the actual token from when the accept state is reached with help of lexemeBegin and forwardPtr
char* tokenFromPtrs() {
    int bufferSize = BUFFER_SIZE * 2;
    int length1, length2;

    if (forwardPtr < lexemeBegin) {
        length1 = bufferSize - lexemeBegin; 
        length2 = forwardPtr;                  
    } else {
        length1 = forwardPtr - lexemeBegin;    
        length2 = 0;                         
    }

    char* finalToken = (char*)malloc((length1 + length2 + 1) * sizeof(char));

    strncpy(finalToken, buffer + lexemeBegin, length1);
    finalToken[length1] = '\0'; 

    if (length2 > 0) {
        strncat(finalToken, buffer, length2);
    }

    return finalToken;
}





// If there is any error send to error state
TOKEN tokenizer(){
    char character;
    TOKEN token;
    token.string=malloc(sizeof(char)*20);
    // Need to initialize the lexemeBegin and forwardPtrs too
    while(true){
        // printf("1");

        switch(state){
            case 0:
                character=getCharacter();
                if(0){}

                else if (character == '_') state = 18;
                else if (character == '#') state = 22;

                else if (character == '[') state = 45;
                else if (character == ']') state = 46;
                else if (character == ',') state = 47;
                else if (character == ';') state = 48;
                else if (character == ':') state = 49;
                else if (character == '.') state = 50;
                else if (character == '(') state = 51;
                else if (character == ')') state = 52;
                else if (character == '+') state = 53;
                else if (character == '-') state = 54;
                else if (character == '*') state = 55;
                else if (character == '/') state = 56;
                else if (character == '~') state = 57;

                else if(character=='%') state=61;
                else if(character==EOF){
                    token.name=TK_EOF;
                    return token;
                }

                break;

            case 18:
                character=getCharacter();
                if(isLetter(character)) state=19;
                else state=60;
                break;


            case 19:
                character=getCharacter();
                if(isLetter(character)) state=19;
                else if(isNumber(character)) state=20;
                else state=21;
                break;

            case 20:
                character=getCharacter();
                if(isNumber(character)) state=20;
                else state=21;
                break;

            case 21:
                token.name=TK_FUNID;
                strcpy(token.string,tokenFromPtrs());
                token.lineNo=lineNo;
                state=0;
                lexemeBegin=forwardPtr;
                return token;
                break;

            case 22:
                character=getCharacter();
                if(isLowercase(character)) state=23;
                else state=60;
                break;

            case 23:
                character=getCharacter();
                if(isLowercase(character)) state=23;
                else state=24;
                break;

            case 24:
                printf("1");
                token.name=TK_RUID;
                strcpy(token.string,tokenFromPtrs());
                token.lineNo=lineNo;
                state=0;
                lexemeBegin=forwardPtr;
                return token;
                break;

            case 45:
                token.name=TK_SQL;
                strcpy(token.string,"[");
                token.lineNo=lineNo;
                state=0;
                lexemeBegin=forwardPtr;
                return token;
                break;


            case 46:
                token.name=TK_SQR;
                strcpy(token.string,"]");
                token.lineNo=lineNo;
                state=0;
                lexemeBegin=forwardPtr;
                return token;
                break;

            case 47:
                token.name=TK_COMMA;
                strcpy(token.string,",");
                token.lineNo=lineNo;
                state=0;
                lexemeBegin=forwardPtr;
                return token;
                break;

            case 48:
                token.name=TK_SEM;
                strcpy(token.string,";");
                token.lineNo=lineNo;
                state=0;
                lexemeBegin=forwardPtr;
                return token;
                break;


            case 49:
                token.name=TK_COLON;
                strcpy(token.string,":");
                token.lineNo=lineNo;
                state=0;
                lexemeBegin=forwardPtr;
                return token;
                break;


            case 50:
                token.name=TK_DOT;
                strcpy(token.string,".");
                token.lineNo=lineNo;
                state=0;
                lexemeBegin=forwardPtr;
                return token;
                break;


            case 51:
                token.name=TK_OP;
                strcpy(token.string,"(");
                token.lineNo=lineNo;
                state=0;
                lexemeBegin=forwardPtr;
                return token;
                break;



            case 52:
                token.name=TK_CL;
                strcpy(token.string,")");
                token.lineNo=lineNo;
                state=0;
                lexemeBegin=forwardPtr;
                return token;
                break;


            case 53:
                token.name=TK_PLUS;
                strcpy(token.string,"+");
                token.lineNo=lineNo;
                state=0;
                lexemeBegin=forwardPtr;
                return token;
                break;


            case 54:
                token.name=TK_MINUS;
                strcpy(token.string,"-");
                token.lineNo=lineNo;
                state=0;
                lexemeBegin=forwardPtr;
                return token;
                break;


            case 55:
                token.name=TK_MUL;
                strcpy(token.string,"*");
                token.lineNo=lineNo;
                state=0;
                lexemeBegin=forwardPtr;
                return token;
                break;


            case 56:
                token.name=TK_DIV;
                strcpy(token.string,"/");
                token.lineNo=lineNo;
                state=0;
                lexemeBegin=forwardPtr;
                return token;
                break;


            case 57:
                token.name=TK_NOT;
                strcpy(token.string,"~");
                token.lineNo=lineNo;
                state=0;
                lexemeBegin=forwardPtr;
                return token;
                break;

            case 58:
            case 59:

            // Error state. Need to change.
            case 60:
                token.name=TK_EOF;
                return token;
                break;

            // Handles the comments
            case 61:
                // printf("3");
                character=getCharacter();
                // printf("%c\n",character);
                if(character == '\n'){
                    // printf("%d",forwardPtr);
                    lineNo++;
                    state=0;
                }
                if(character==EOF){
                    token.name=TK_EOF;
                    return token;
                }
                else state=61;
                break;

            // Increment the line number 
            case 62:
                lineNo++;
                state=0;
                break;
        }

    }
}


int main(){
    filePointer = fopen("test.txt", "r");
    populateBuffer(0);
    populateBuffer(1);
    lexemeBegin=0;
    forwardPtr=0;
    // printf("%s",tokenFromPtrs());
    // printf("%c",buffer[forwardPtr]);


    while(buffer[forwardPtr]!=EOF){
        TOKEN token=tokenizer();
        printf("%s %d\n",tokenToString(token.name),lineNo);
        if(token.name==TK_EOF) break;
    }

}

