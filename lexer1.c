#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "helperFunc.c"


// State for dfa
int state;
int lexemeBegin,forwardPtr;
int tokenSize;



char getCharacter(){
    return 'a';
}

void retract(int noOfRetractions){

}

// Returns the actual token from when the accept state is reached from the lexemeBegin and forwardPtr
char* tokenFromPtrs(){
    return "a";
}

// If there is any error send to error state
TOKEN tokenizer(){
    int lineNo;
    char character;
    TOKEN token;
    state=0,tokenSize=0;
    // Need to initialize the lexemeBegin and forwardPtrs too
    while(true){

        switch(state){
            case 0:
                character=getCharacter();
                if(1){}

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
                strcpy(token.name,tokenFromPtrs());
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
                if(isLowercase(character)) state=23;
                else state=24;
                break;

            case 24:
                token.name=TK_RUID;
                strcpy(token.name,tokenFromPtrs());
                token.lineNo=lineNo;
                state=0;
                lexemeBegin=forwardPtr;
                return token;
                break;

            case 45:
                token.name=TK_SQL;
                strcpy(token.name,"[");
                token.lineNo=lineNo;
                state=0;
                lexemeBegin=forwardPtr;
                return token;
                break;


            case 46:
                token.name=TK_SQR;
                strcpy(token.name,"]");
                token.lineNo=lineNo;
                state=0;
                lexemeBegin=forwardPtr;
                return token;
                break;

            case 47:
                token.name=TK_COMMA;
                strcpy(token.name,",");
                token.lineNo=lineNo;
                state=0;
                lexemeBegin=forwardPtr;
                return token;
                break;

            case 48:
                token.name=TK_SEM;
                strcpy(token.name,";");
                token.lineNo=lineNo;
                state=0;
                lexemeBegin=forwardPtr;
                return token;
                break;


            case 49:
                token.name=TK_COLON;
                strcpy(token.name,":");
                token.lineNo=lineNo;
                state=0;
                lexemeBegin=forwardPtr;
                return token;
                break;


            case 50:
                token.name=TK_DOT;
                strcpy(token.name,".");
                token.lineNo=lineNo;
                state=0;
                lexemeBegin=forwardPtr;
                return token;
                break;


            case 51:
                token.name=TK_OP;
                strcpy(token.name,"(");
                token.lineNo=lineNo;
                state=0;
                lexemeBegin=forwardPtr;
                return token;
                break;



            case 52:
                token.name=TK_CL;
                strcpy(token.name,")");
                token.lineNo=lineNo;
                state=0;
                lexemeBegin=forwardPtr;
                return token;
                break;


            case 53:
                token.name=TK_PLUS;
                strcpy(token.name,"+");
                token.lineNo=lineNo;
                state=0;
                lexemeBegin=forwardPtr;
                return token;
                break;


            case 54:
                token.name=TK_MINUS;
                strcpy(token.name,"-");
                token.lineNo=lineNo;
                state=0;
                lexemeBegin=forwardPtr;
                return token;
                break;


            case 55:
                token.name=TK_MUL;
                strcpy(token.name,"*");
                token.lineNo=lineNo;
                state=0;
                lexemeBegin=forwardPtr;
                return token;
                break;


            case 56:
                token.name=TK_DIV;
                strcpy(token.name,"/");
                token.lineNo=lineNo;
                state=0;
                lexemeBegin=forwardPtr;
                return token;
                break;


            case 57:
                token.name=TK_NOT;
                strcpy(token.name,"~");
                token.lineNo=lineNo;
                state=0;
                lexemeBegin=forwardPtr;
                return token;
                break;

            case 58:
            case 59:

            // Error state
            case 60:

        }

    }
}