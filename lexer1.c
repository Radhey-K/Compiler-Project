#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
// #include "helperFunc.c"
#include <ctype.h>
#include <time.h>

#define BUFFER_SIZE 1000

// State for dfa
int state = 0;

int lexemeBegin,forwardPtr;
int tokenSize=0;
int lineNo=1;

// Twin buffer
char buffer[2 * BUFFER_SIZE];
FILE *filePointer;

int isDigit(char c)
{
    return isdigit(c);
}

int isLetter(char c)
{
    return isalpha(c) != 0;
}

int isNumber(char c)
{
    return isalnum(c) != 0;
}

int isLowercase(char c)
{
    return islower(c) != 0;
}

int digit_2to7(char c){
    int num = c - '0';
    if(num >= 2 && num <= 7)return 1;
    else return 0;
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
        case TK_ERROR: return "TK_ERROR";

        default: return "Unknown Token";
    }
}




int populateBuffer(int whichBuffer){
    if(whichBuffer==0){
        fread(buffer, BUFFER_SIZE, 1, filePointer);
        // printf("%s \n", buffer);
    }
    else
    {
        fread(buffer + BUFFER_SIZE, BUFFER_SIZE, 1, filePointer);
        // printf("%s \n", buffer);
    }
    return 0;
}

char getCharacter()
{
    if (forwardPtr == BUFFER_SIZE)
    {
        populateBuffer(0);
    }
    else if (forwardPtr == 2 * BUFFER_SIZE)
    {
        populateBuffer(1);
        forwardPtr = 0;
    }
    char c = buffer[forwardPtr];
    // printf("%c",c);

    if (buffer[forwardPtr] == '\0')
    {
        return EOF;
    }

    if ((int)buffer[forwardPtr] == 10)
        return '\n';

    forwardPtr++;
    return c;
}

void retract(int noOfRetractions)
{
    forwardPtr -= noOfRetractions;
    if(forwardPtr < 0){
        forwardPtr += (2*BUFFER_SIZE);
    }
}

// Returns the actual token from when the accept state is reached with help of lexemeBegin and forwardPtr
char *tokenFromPtrs()
{
    int bufferSize = BUFFER_SIZE * 2;
    int length1, length2;

    if (forwardPtr < lexemeBegin)
    {
        length1 = bufferSize - lexemeBegin;
        length2 = forwardPtr;
    }
    else
    {
        length1 = forwardPtr - lexemeBegin;
        length2 = 0;
    }

    char *finalToken = (char *)malloc((length1 + length2 + 1) * sizeof(char));

    strncpy(finalToken, buffer + lexemeBegin, length1);
    finalToken[length1] = '\0';

    if (length2 > 0)
    {
        strncat(finalToken, buffer, length2);
    }
    lexemeBegin=forwardPtr;

    return finalToken;
}

int lexeme_length(){
    int bufferSize = BUFFER_SIZE * 2;
    int length1, length2;

    if (forwardPtr < lexemeBegin)
    {
        length1 = bufferSize - lexemeBegin;
        length2 = forwardPtr;
    }
    else
    {
        length1 = forwardPtr - lexemeBegin;
        length2 = 0;
    }

    return length1+length2;
}

// If there is any error send to error state
TOKEN tokenizer()
{
    char character;
    TOKEN token;
    token.integer = -1;
    token.realNum = -1;
    token.string = malloc(sizeof(char) * 20);
    // Need to initialize the lexemeBegin and forwardPtrs too
    while (true)
    {
        // printf("1");
        
        switch(state){
            case 0:
                character=getCharacter();
                if(0){}
            else if(isDigit(character))
                state = 1;
            else if(character=='b' || character=='c' || character=='d')
                state = 12;
            else if(isLowercase(character)&&character!='b' && character!='c' && character!='d')
                state = 16;
            else if(character == '<')
                state = 25;
            else if(character == '=')
                state = 32;
            else if(character == '>')
                state = 34;
            else if(character == '!')
                state = 37;
            else if(character == '@')
                state = 39;
            else if(character == '&')
                state = 42;
            else if (character == '_')
                state = 18;
            else if (character == '#')
                state = 22;
            else if (character == '[')
                state = 45;
            else if (character == ']')
                state = 46;
            else if (character == ',')
                state = 47;
            else if (character == ';')
                state = 48;
            else if (character == ':')
                state = 49;
            else if (character == '.')
                state = 50;
            else if (character == '(')
                state = 51;
            else if (character == ')')
                state = 52;
            else if (character == '+')
                state = 53;
            else if (character == '-')
                state = 54;
            else if (character == '*')
                state = 55;
            else if (character == '/')
                state = 56;
            else if (character == '~')
                state = 57;
            else if (character == '%')
                state = 61;

            else if (character == '\n')
                state = 62;
            else if (character == EOF)
            {
                token.name = TK_EOF;
                return token;
            }

            break;
        
        case 1: character = getCharacter();
                if(character == '.') state = 2;
                else if(isDigit(character)) state = 1;
                else state = 9;
                break;
            
        case 2: character = getCharacter();
                if(isDigit(character)) state = 3;
                else state = 10;
                break;

        case 3: character = getCharacter();
                if(isDigit(character)) state = 4;
                else exit(0);  // Error
                break;

        case 4: character = getCharacter();
                if (character == 'E') state = 5;
                else state = 11;
                break;

        case 5: character = getCharacter();
                if (character == '+' || character == '-') state = 6;
                else if (isDigit(character)) state = 7;
                else exit(0);  // Error
                break;

        case 6: character = getCharacter();
                if (isDigit(character)) state = 7;
                else exit(0);  // Error
                break;

        case 7: character = getCharacter();
                if (isDigit(character)) state = 8;
                else exit(0);
                break;

        case 8: // TK_RNUM
                token.name = TK_RNUM;
                strcpy(token.string, tokenFromPtrs());
                token.lineNo = lineNo;
                state = 0;
                lexemeBegin = forwardPtr;
                return token;
                break;

        case 9: //TK_NUM    
                retract(1); 
                token.name = TK_NUM;
                strcpy(token.string, tokenFromPtrs());
                token.integer = atoi(token.string);
                token.lineNo = lineNo;
                state = 0;
                lexemeBegin = forwardPtr;
                return token;
                break;

        case 10: //TK_NUM
                retract(2);
                token.name = TK_NUM;
                strcpy(token.string, tokenFromPtrs());
                token.integer = atoi(token.string);
                token.lineNo = lineNo;
                state = 0;
                lexemeBegin = forwardPtr;
                return token;
                break;

        case 11: //TK_RNUM
                retract(1);
                token.name = TK_RNUM;
                strcpy(token.string, tokenFromPtrs());
                token.realNum = atof(token.string);
                token.lineNo = lineNo;
                state = 0;
                lexemeBegin = forwardPtr;
                return token;
                break;

        case 12:    character = getCharacter();
                    if(digit_2to7(character)) state = 13;
                    else if(isLowercase(character)) state = 16;
                    else state = 17;
                    break;

        case 13:    character = getCharacter();
                    if(character=='b' || character=='c' || character=='d') state = 13;
                    else if(digit_2to7(character)) state = 14;
                    else state = 15;
                    break;

        case 14:    character = getCharacter();
                    if(digit_2to7(character)) state = 14;
                    else state = 15;
                    break;

        case 15:    retract(1); //TK_ID
                    if(lexeme_length() > MAX_VAR_ID_SIZE){
                        state = 60;
                        break;
                    }
                    token.name = TK_ID;
                    strcpy(token.string, tokenFromPtrs());
                    token.lineNo = lineNo;
                    state = 0;
                    lexemeBegin = forwardPtr;
                    return token;
                    break;

        case 16:    character = getCharacter();
                    if(isLowercase(character)) state = 16;
                    else state = 17;
                    break;

        case 17:    retract(1); //TK_FIELDID
                    token.name = TK_FIELDID;
                    strcpy(token.string, tokenFromPtrs());
                    token.lineNo = lineNo;
                    state = 0;
                    lexemeBegin = forwardPtr;
                    return token;
                    break;

        case 18:
            character = getCharacter();
            if (isLetter(character))
                state = 19;
            else
                state = 60;
            break;

        case 19:
            character = getCharacter();
            if (isLetter(character))
                state = 19;
            else if (isNumber(character))
                state = 20;
            else
                state = 21;
            break;

        case 20:
            character = getCharacter();
            if (isNumber(character))
                state = 20;
            else
                state = 21;
            break;

        case 21:
            retract(1);
            if(lexeme_length() > MAX_FUN_ID_SIZE){
                state = 60;
                break;
            }
            token.name = TK_FUNID;
            strcpy(token.string, tokenFromPtrs());
            token.lineNo = lineNo;
            state = 0;
            lexemeBegin = forwardPtr;
            return token;
            break;

        case 22:
            character = getCharacter();
            if (isLowercase(character))
                state = 23;
            else
                state = 60;
            break;

        case 23:
            character = getCharacter();
            if (isLowercase(character))
                state = 23;
            else
                state = 24;
            break;

        case 24:
            token.name = TK_RUID;
            strcpy(token.string, tokenFromPtrs());
            token.lineNo = lineNo;
            state = 0;
            lexemeBegin = forwardPtr;
            return token;
            break;

        case 25:
            character = getCharacter();
            if (character == '-')
                state = 26;
            else if (character == '=')
                state = 31;
            else
                state = 29;

        case 26:
            character = getCharacter();
            if (character == '-')
                state = 27;
            else
                state = 30;

        case 27:
            character = getCharacter();
            if (character == '-')
                state = 28;
            else
                state = 60;

        case 28:
            token.name = TK_ASSIGNOP;
            strcpy(token.string, tokenFromPtrs());
            token.lineNo = lineNo;
            state = 0;
            lexemeBegin = forwardPtr;
            return token;
            break;

        case 29:
            // retraction needed
            token.name = TK_LT;
            strcpy(token.string, tokenFromPtrs());
            token.lineNo = lineNo;
            state = 0;
            lexemeBegin = forwardPtr;
            return token;
            break;

        case 30:
            token.name = TK_LT;
            strcpy(token.string, tokenFromPtrs());
            token.lineNo = lineNo;
            state = 0;
            lexemeBegin = forwardPtr;
            return token;
            break;

        case 31:
            token.name = TK_LE;
            strcpy(token.string, tokenFromPtrs());
            token.lineNo = lineNo;
            state = 0;
            lexemeBegin = forwardPtr;
            return token;
            break;

        case 32:
            character = getCharacter();
            if (character == '=')
                state = 33;
            else
                state = 60;
            break;

        case 33:
            token.name = TK_EQ;
            strcpy(token.string, tokenFromPtrs());
            token.lineNo = lineNo;
            state = 0;
            lexemeBegin = forwardPtr;
            return token;
            break;

        case 34:
            character = getCharacter();
            if (character == '=')
                state = 35;
            else
                state = 36;

        case 35:
            token.name = TK_GE;
            strcpy(token.string, tokenFromPtrs());
            token.lineNo = lineNo;
            state = 0;
            lexemeBegin = forwardPtr;
            return token;
            break;

        case 36:
            // retraction needed
            token.name = TK_GT;
            strcpy(token.string, tokenFromPtrs());
            token.lineNo = lineNo;
            state = 0;
            lexemeBegin = forwardPtr;
            return token;
            break;

        case 37:
            character = getCharacter();
            if (character == '=')
                state = 38;
            else
                state = 60;

        case 38:
            token.name = TK_NE;
            strcpy(token.string, tokenFromPtrs());
            token.lineNo = lineNo;
            state = 0;
            lexemeBegin = forwardPtr;
            return token;
            break;

        case 39:
            character = getCharacter();
            if (character == '@')
                state = 40;
            else
                state = 60;

        case 40:
            character = getCharacter();
            if (character == '@')
                state = 41;
            else
                state = 60;

        case 41:
            token.name = TK_OR;
            strcpy(token.string, tokenFromPtrs());
            token.lineNo = lineNo;
            state = 0;
            lexemeBegin = forwardPtr;
            return token;
            break;

        case 42:
            character = getCharacter();
            if (character == '&')
                state = 43;
            else
                state = 60;

        case 43:
            character = getCharacter();
            if (character == '&')
                state = 44;
            else
                state = 60;

        case 44:
            token.name = TK_AND;
            strcpy(token.string, tokenFromPtrs());
            token.lineNo = lineNo;
            state = 0;
            lexemeBegin = forwardPtr;
            return token;
            break;

        case 45:
            token.name = TK_SQL;
            strcpy(token.string, "[");
            token.lineNo = lineNo;
            state = 0;
            lexemeBegin = forwardPtr;
            return token;
            break;

        case 46:
            token.name = TK_SQR;
            strcpy(token.string, "]");
            token.lineNo = lineNo;
            state = 0;
            lexemeBegin = forwardPtr;
            return token;
            break;

        case 47:
            token.name = TK_COMMA;
            strcpy(token.string, ",");
            token.lineNo = lineNo;
            state = 0;
            lexemeBegin = forwardPtr;
            return token;
            break;

        case 48:
            token.name = TK_SEM;
            strcpy(token.string, ";");
            token.lineNo = lineNo;
            state = 0;
            lexemeBegin = forwardPtr;
            return token;
            break;

        case 49:
            token.name = TK_COLON;
            strcpy(token.string, ":");
            token.lineNo = lineNo;
            state = 0;
            lexemeBegin = forwardPtr;
            return token;
            break;

        case 50:
            token.name = TK_DOT;
            strcpy(token.string, ".");
            token.lineNo = lineNo;
            state = 0;
            lexemeBegin = forwardPtr;
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
            token.name = TK_PLUS;
            strcpy(token.string, "+");
            token.lineNo = lineNo;
            state = 0;
            lexemeBegin = forwardPtr;
            return token;
            break;

        case 54:
            token.name = TK_MINUS;
            strcpy(token.string, "-");
            token.lineNo = lineNo;
            state = 0;
            lexemeBegin = forwardPtr;
            return token;
            break;

        case 55:
            token.name = TK_MUL;
            strcpy(token.string, "*");
            token.lineNo = lineNo;
            state = 0;
            lexemeBegin = forwardPtr;
            return token;
            break;

        case 56:
            token.name = TK_DIV;
            strcpy(token.string, "/");
            token.lineNo = lineNo;
            state = 0;
            lexemeBegin = forwardPtr;
            return token;
            break;

        case 57:
            token.name = TK_NOT;
            strcpy(token.string, "~");
            token.lineNo = lineNo;
            state = 0;
            lexemeBegin = forwardPtr;
            return token;
            break;

        case 58:
        case 59:

        // Error state. Need to change.
        case 60:
            retract(1);
            token.name = TK_ERROR;
            token.lineNo = lineNo;
            strcpy(token.string, tokenFromPtrs());
            state = 0;
            lexemeBegin = forwardPtr;
            return token;
            break;

        // Handles the comments
        case 61:
            // printf("3");
            character = getCharacter();
            // printf("%c\n",character);
            if (character == '\n')
            {
                // printf("%d",forwardPtr);
                state=62;
            }
            else if (character == EOF)
            {
                token.name = TK_EOF;
                return token;
            }
            else
                state = 61;
            break;

        // Increment the line number
        case 62:
            lineNo++;
            state = 0;
            forwardPtr++;
            lexemeBegin = forwardPtr;
            break;
        }
    }
}

int main()
{
    clock_t start_time, end_time;
    double total_CPU_time, total_CPU_time_in_seconds;
    start_time = clock();

    filePointer = fopen("test.txt", "r");
    populateBuffer(0);
    populateBuffer(1);
    lexemeBegin = 0;
    forwardPtr = 0;
    // printf("%s",tokenFromPtrs());
    // printf("%c",buffer[forwardPtr]);

    // while (buffer[forwardPtr] != EOF)
    // {
    //     TOKEN token = tokenizer();
    //     // if (token.integer != -1)
    //     //     printf("Line No. %d     Lexeme %d       Token %s\n", lineNo, token.integer, tokenToString(token.name));
    //     // else if (token.realNum != -1)
    //     //     printf("Line No. %d     Lexeme %f       Token %s\n", lineNo, token.realNum, tokenToString(token.name));
    //     // else
    //         printf("Line No. %d     Lexeme %s       Token %s\n", lineNo, token.string, tokenToString(token.name));

        if (token.name == TK_EOF)
            break;
    }

    end_time = clock();
    total_CPU_time = (double) (end_time - start_time);
    total_CPU_time_in_seconds = total_CPU_time / CLOCKS_PER_SEC;
    printf("Total CPU time: %f\n", total_CPU_time);
    printf("Total CPU time in seconds: %f\n", total_CPU_time_in_seconds);
}
