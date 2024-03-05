/* ---------Group Information---------
Group Number: 15

Yash Mundada: 2021A7PS0001P
Aaryan Garg: 2021A7PS2222P
Dev Kulkarni: 2021A7PS2430P
Radhey Kanade: 2021A7PS2534P
Shardul Shingare: 2021A7PS2539P
Shantanu Ambekar: 2021A7PS2540P
--------------------------------------*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "lexer.h"
#include "lexerDef.h"
#include "symbol_table.h"

//-----*GLOBAL VARIABLES*-----
int state = 0;
int lexemeBegin,forwardPtr;
int lineNo = 1;
char buffer[2 * BUFFER_SIZE];
FILE *filePointer;
int sharedVar = 0;
//-----*GLOBAL VARIABLES*-----

//-----*HELPER FUNCTIONS*-----
int isDigit(char c){
    return isdigit(c);
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

int digit_2to7(char c){
    int num = c - '0';
    if(num >= 2 && num <= 7)return 1;
    else return 0;
}
//-----*HELPER FUNCTIONS*-----

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
        case TK_EPS: return "TK_EPS";
        default: return "Unknown Token";
    }
}

const char* nonterminaltoString(nonterminal nt) {
    switch(nt) {
        case program: return "<program>";
        case otherFunctions: return "<otherFunctions>";
        case mainFunction: return "<mainFunction>";
        case stmts: return "<stmts>";
        case function: return "<function>";
        case input_par: return "<input_par>";
        case output_par: return "<output_par>";
        case parameter_list: return "<parameter_list>";
        case dataType: return "<dataType>";
        case primitiveDatatype: return "<primitiveDatatype>";
        case constructedDatatype: return "<constructedDatatype>";
        case remaining_list: return "<remaining_list>";
        case typeDefinitions: return "<typeDefinitions>";
        case actualOrRedefined: return "<actualOrRedefined>";
        case typeDefinition: return "<typeDefinition>";
        case fieldDefinitions: return "<fieldDefinitions>";
        case fieldDefinition: return "<fieldDefinition>";
        case fieldType: return "<fieldType>";
        case moreFields: return "<moreFields>";
        case declarations: return "<declarations>";
        case declaration: return "<declaration>";
        case global_or_not: return "<global_or_not>";
        case otherStmts: return "<otherStmts>";
        case stmt: return "<stmt>";
        case assignmentStmt: return "<assignmentStmt>";
        case SingleOrRecId: return "<SingleOrRecId>";
        case option_single_constructed: return "<option_single_constructed>";
        case oneExpansion: return "<oneExpansion>";
        case moreExpansions: return "<moreExpansions>";
        case funCallStmt: return "<funCallStmt>";
        case outputParameters: return "<outputParameters>";
        case inputParameters: return "<inputParameters>";
        case iterativeStmt: return "<iterativeStmt>";
        case conditionalStmt: return "<conditionalStmt>";
        case elsePart: return "<elsePart>";
        case ioStmt: return "<ioStmt>";
        case arithmeticExpression: return "<arithmeticExpression>";
        case expPrime: return "<expPrime>";
        case term: return "<term>";
        case termPrime: return "<termPrime>";
        case factor: return "<factor>";
        case highPrecedenceOperator: return "<highPrecedenceOperator>";
        case lowPrecedenceOperators: return "<lowPrecedenceOperators>";
        case booleanExpression: return "<booleanExpression>";
        case var: return "<var>";
        case logicalOp: return "<logicalOp>";
        case relationalOp: return "<relationalOp>";
        case returnStmt: return "<returnStmt>";
        case optionalReturn: return "<optionalReturn>";
        case idList: return "<idList>";
        case more_ids: return "<more_ids>";
        case definetypestmt: return "<definetypestmt>";
        case A: return "<A>";
        default: return "Unknown";
    }
}

void populateBuffer(int whichBuffer, stateInfo *s){
    if (whichBuffer == 0 && s->sharedVar == 0) {
        memset(s->buffer, 0, BUFFER_SIZE);
        fread(s->buffer, BUFFER_SIZE, 1, s->filePointer);
        s->sharedVar = 1;
    }
    else if (whichBuffer == 1 && s->sharedVar == 1) {
        memset(s->buffer + BUFFER_SIZE, 0, BUFFER_SIZE);
        fread(s->buffer + BUFFER_SIZE, BUFFER_SIZE, 1, s->filePointer);
        s->sharedVar = 0;
    }
}

char getCharacter(stateInfo *s){
    if (s->forwardPtr == BUFFER_SIZE){
        populateBuffer(1, s);
    }
    else if (s->forwardPtr == 2 * BUFFER_SIZE){
        populateBuffer(0, s);
        s->forwardPtr = 0;
    }

    char c = s->buffer[s->forwardPtr];
    if (c == '\0'){
        s->forwardPtr++;
        return EOF;
    }
    
    s->forwardPtr++;
    return c;
}

void retract(int noOfRetractions, stateInfo *s) {
    s->forwardPtr -= noOfRetractions;
    if(s->forwardPtr < 0){
        s->forwardPtr += (2*BUFFER_SIZE);
    }
}

char *tokenFromPtrs(stateInfo *s){
    int bufferSize = BUFFER_SIZE * 2;
    int length1, length2;
    if (s->forwardPtr < s->lexemeBegin){
        length1 = bufferSize - s->lexemeBegin;
        length2 = s->forwardPtr;
    }
    else{
        length1 = s->forwardPtr - s->lexemeBegin;
        length2 = 0;
    }

    char *finalToken = (char *)malloc((length1 + length2 + 1) * sizeof(char));
    strncpy(finalToken, s->buffer + s->lexemeBegin, length1);
    finalToken[length1] = '\0';
    if (length2 > 0){
        strncat(finalToken, s->buffer, length2);
    }
    s->lexemeBegin=s->forwardPtr;
    return finalToken;
}

int lexeme_length(stateInfo *s) {
    int bufferSize = BUFFER_SIZE * 2;
    int length1, length2;
    if (s->forwardPtr < s->lexemeBegin){
        length1 = bufferSize - s->lexemeBegin;
        length2 = s->forwardPtr;
    }
    else{
        length1 = s->forwardPtr - s->lexemeBegin;
        length2 = 0;
    }
    return length1+length2;
}

TOKEN tokenizer(ST stable, stateInfo *s){
    char character;
    TOKEN token;
    token.integer = -1;
    token.realNum = -1;
    token.string = (char*)malloc(sizeof(char) * 50);
    ST_ELEMENT ele;

    while (true){
        switch(s->state){
            case 0:
                character=getCharacter(s);

                if (0){}
                else if (character == ' ')
                    s->state = 58;
                else if (isDigit(character))
                    s->state = 1;
                else if (character == 'b' || character == 'c' || character == 'd')
                    s->state = 12;
                else if (isLowercase(character) && character != 'b' && character != 'c' && character != 'd')
                    s->state = 16;
                else if (character == '<')
                    s->state = 25;
                else if (character == '=')
                    s->state = 32;
                else if (character == '>')
                    s->state = 34;
                else if (character == '!')
                    s->state = 37;
                else if (character == '@')
                    s->state = 39;
                else if (character == '&')
                    s->state = 42;
                else if (character == '_')
                    s->state = 18;
                else if (character == '#')
                    s->state = 22;
                else if (character == '[')
                    s->state = 45;
                else if (character == ']')
                    s->state = 46;
                else if (character == ',')
                    s->state = 47;
                else if (character == ';')
                    s->state = 48;
                else if (character == ':')
                    s->state = 49;
                else if (character == '.')
                    s->state = 50;
                else if (character == '(')
                    s->state = 51;
                else if (character == ')')
                    s->state = 52;
                else if (character == '+')
                    s->state = 53;
                else if (character == '-')
                    s->state = 54;
                else if (character == '*')
                    s->state = 55;
                else if (character == '/')
                    s->state = 56;
                else if (character == '~')
                    s->state = 57;
                else if (character == '%')
                    s->state = 60;
                else if (character == '\n')
                    s->state = 61;
                else if (character == EOF){
                    token.name = TK_EOF;
                    return token;
                }
                else if (character == '\r')
                    s->state = 0;
                else if (character == '\t'){
                    s->lexemeBegin = s->forwardPtr;
                    s->state = 0;
                }
                else
                    s->state = 62;
                break;

            case 1:
                character = getCharacter(s);
                if (character == '.')
                    s->state = 2;
                else if (isDigit(character))
                    s->state = 1;
                else
                    s->state = 9;
                break;

            case 2:
                character = getCharacter(s);
                if (isDigit(character))
                    s->state = 3;
                else
                    s->state = 10;
                break;

            case 3:
                character = getCharacter(s);
                if (isDigit(character))
                    s->state = 4;
                else
                    s->state = 59;
                break;

            case 4:
                character = getCharacter(s);
                if (character == 'E')
                    s->state = 5;
                else
                    s->state = 11;
                break;

            case 5:
                character = getCharacter(s);
                if (character == '+' || character == '-')
                    s->state = 6;
                else if (isDigit(character))
                    s->state = 7;
                else
                    s->state = 59;
                break;

            case 6:
                character = getCharacter(s);
                if (isDigit(character))
                    s->state = 7;
                else
                    s->state = 59;
                break;

            case 7:
                character = getCharacter(s);
                if (isDigit(character))
                    s->state = 8;
                else
                    s->state = 59;
                break;

            case 8:
                token.name = TK_RNUM;
                strcpy(token.string, tokenFromPtrs(s));
                token.lineNo = s->lineNo;
                s->state = 0;
                s->lexemeBegin = s->forwardPtr;
                return token;
                break;

            case 9:
                retract(1, s);
                token.name = TK_NUM;
                strcpy(token.string, tokenFromPtrs(s));
                token.integer = atoi(token.string);
                token.lineNo = s->lineNo;
                s->state = 0;
                s->lexemeBegin = s->forwardPtr;
                return token;
                break;

            case 10:
                retract(2, s);
                token.name = TK_NUM;
                strcpy(token.string, tokenFromPtrs(s));
                token.integer = atoi(token.string);
                token.lineNo = s->lineNo;
                s->state = 0;
                s->lexemeBegin = s->forwardPtr;
                return token;
                break;

            case 11:
                retract(1, s);
                token.name = TK_RNUM;
                strcpy(token.string, tokenFromPtrs(s));
                token.realNum = atof(token.string);
                token.lineNo = s->lineNo;
                s->state = 0;
                s->lexemeBegin = s->forwardPtr;
                return token;
                break;

            case 12:
                character = getCharacter(s);
                if (digit_2to7(character))
                    s->state = 13;
                else if (isLowercase(character))
                    s->state = 16;
                else
                    s->state = 17;
                break;

            case 13:
                character = getCharacter(s);
                if (character == 'b' || character == 'c' || character == 'd')
                    s->state = 13;
                else if (digit_2to7(character))
                    s->state = 14;
                else
                    s->state = 15;
                break;

            case 14:
                character = getCharacter(s);
                if (digit_2to7(character))
                    s->state = 14;
                else
                    s->state = 15;
                break;

            case 15:
                retract(1, s);
                if (lexeme_length(s) > MAX_VAR_ID_SIZE){
                    s->state = 64;
                    break;
                }
                token.name = TK_ID;
                strcpy(token.string, tokenFromPtrs(s));
                token.lineNo = s->lineNo;
                s->state = 0;
                s->lexemeBegin = s->forwardPtr;
                ele = table_lookup(stable, token.string);
                if (ele == NULL){
                    table_insert(stable, token.string, token.name);
                }
                return token;
                break;

            case 16:
                character = getCharacter(s);
                if (isLowercase(character))
                    s->state = 16;
                else
                    s->state = 17;
                break;

            case 17:
                retract(1, s);
                token.name = TK_FIELDID;
                strcpy(token.string, tokenFromPtrs(s));
                token.lineNo = s->lineNo;
                s->state = 0;
                s->lexemeBegin = s->forwardPtr;
                ele = table_lookup(stable, token.string);
                if (ele == NULL){
                    table_insert(stable, token.string, token.name);
                }
                else{
                    token.name = ele->tk_type;
                }
                return token;
                break;

            case 18:
                character = getCharacter(s);
                if (isLetter(character))
                    s->state = 19;
                else
                    s->state = 59;
                break;

            case 19:
                character = getCharacter(s);
                if (isLetter(character))
                    s->state = 19;
                else if (isNumber(character))
                    s->state = 20;
                else
                    s->state = 21;
                break;

            case 20:
                character = getCharacter(s);
                if (isNumber(character))
                    s->state = 20;
                else
                    s->state = 21;
                break;

            case 21:
                retract(1, s);
                if (lexeme_length(s) > MAX_FUN_ID_SIZE){
                    s->state = 59;
                    break;
                }
                token.name = TK_FUNID;
                strcpy(token.string, tokenFromPtrs(s));
                token.lineNo = s->lineNo;
                s->state = 0;
                s->lexemeBegin = s->forwardPtr;
                ele = table_lookup(stable, token.string);
                if (ele == NULL){
                    table_insert(stable, token.string, token.name);
                }
                else{
                    token.name = ele->tk_type;
                }
                return token;
                break;

            case 22:
                character = getCharacter(s);
                if (isLowercase(character))
                    s->state = 23;
                else
                    s->state = 59;
                break;

            case 23:
                character = getCharacter(s);
                if (isLowercase(character))
                    s->state = 23;
                else
                    s->state = 24;
                break;

            case 24:
                retract(1, s);
                token.name = TK_RUID;
                strcpy(token.string, tokenFromPtrs(s));
                token.lineNo = s->lineNo;
                s->state = 0;
                s->lexemeBegin = s->forwardPtr;
                ele = table_lookup(stable, token.string);
                if (ele == NULL){
                    table_insert(stable, token.string, token.name);
                }
                return token;
                break;

            case 25:
                character = getCharacter(s);
                if (character == '-')
                    s->state = 26;
                else if (character == '=')
                    s->state = 31;
                else
                    s->state = 29;
                break;

            case 26:
                character = getCharacter(s);
                if (character == '-')
                    s->state = 27;
                else
                    s->state = 30;
                break;

            case 27:
                character = getCharacter(s);
                if (character == '-')
                    s->state = 28;
                else
                    s->state = 59;
                break;

            case 28:
                token.name = TK_ASSIGNOP;
                strcpy(token.string, tokenFromPtrs(s));
                token.lineNo = s->lineNo;
                s->state = 0;
                s->lexemeBegin = s->forwardPtr;
                return token;
                break;

            case 29:
                retract(1, s);
                token.name = TK_LT;
                strcpy(token.string, tokenFromPtrs(s));
                token.lineNo = s->lineNo;
                s->state = 0;
                s->lexemeBegin = s->forwardPtr;
                return token;
                break;

            case 30:
                retract(2, s);
                token.name = TK_LT;
                strcpy(token.string, tokenFromPtrs(s));
                token.lineNo = s->lineNo;
                s->state = 0;
                s->lexemeBegin = s->forwardPtr;
                return token;
                break;

            case 31:
                token.name = TK_LE;
                strcpy(token.string, tokenFromPtrs(s));
                token.lineNo = s->lineNo;
                s->state = 0;
                s->lexemeBegin = s->forwardPtr;
                return token;
                break;

            case 32:
                character = getCharacter(s);
                if (character == '=')
                    s->state = 33;
                else
                    s->state = 59;
                break;

            case 33:
                token.name = TK_EQ;
                strcpy(token.string, tokenFromPtrs(s));
                token.lineNo = s->lineNo;
                s->state = 0;
                s->lexemeBegin = s->forwardPtr;
                return token;
                break;

            case 34:
                character = getCharacter(s);
                if (character == '=')
                    s->state = 35;
                else
                    s->state = 36;
                break;

            case 35:
                token.name = TK_GE;
                strcpy(token.string, tokenFromPtrs(s));
                token.lineNo = s->lineNo;
                s->state = 0;
                s->lexemeBegin = s->forwardPtr;
                return token;
                break;

            case 36:
                retract(1, s);
                token.name = TK_GT;
                strcpy(token.string, tokenFromPtrs(s));
                token.lineNo = s->lineNo;
                s->state = 0;
                s->lexemeBegin = s->forwardPtr;
                return token;
                break;

            case 37:
                character = getCharacter(s);
                if (character == '=')
                    s->state = 38;
                else
                    s->state = 59;
                break;

            case 38:
                token.name = TK_NE;
                strcpy(token.string, tokenFromPtrs(s));
                token.lineNo = s->lineNo;
                s->state = 0;
                s->lexemeBegin = s->forwardPtr;
                return token;
                break;

            case 39:
                character = getCharacter(s);
                if (character == '@')
                    s->state = 40;
                else
                    s->state = 59;
                break;

            case 40:
                character = getCharacter(s);
                if (character == '@')
                    s->state = 41;
                else
                    s->state = 59;
                break;

            case 41:
                token.name = TK_OR;
                strcpy(token.string, tokenFromPtrs(s));
                token.lineNo = s->lineNo;
                s->state = 0;
                s->lexemeBegin = s->forwardPtr;
                return token;
                break;

            case 42:
                character = getCharacter(s);
                if (character == '&')
                    s->state = 43;
                else
                    s->state = 59;
                break;

            case 43:
                character = getCharacter(s);
                if (character == '&')
                    s->state = 44;
                else
                    s->state = 59;
                break;

            case 44:
                token.name = TK_AND;
                strcpy(token.string, tokenFromPtrs(s));
                token.lineNo = s->lineNo;
                s->state = 0;
                s->lexemeBegin = s->forwardPtr;
                return token;
                break;

            case 45:
                token.name = TK_SQL;
                strcpy(token.string, "[");
                token.lineNo = s->lineNo;
                s->state = 0;
                s->lexemeBegin = s->forwardPtr;
                return token;
                break;

            case 46:
                token.name = TK_SQR;
                strcpy(token.string, "]");
                token.lineNo = s->lineNo;
                s->state = 0;
                s->lexemeBegin = s->forwardPtr;
                return token;
                break;

            case 47:
                token.name = TK_COMMA;
                strcpy(token.string, ",");
                token.lineNo = s->lineNo;
                s->state = 0;
                s->lexemeBegin = s->forwardPtr;
                return token;
                break;

            case 48:
                token.name = TK_SEM;
                strcpy(token.string, ";");
                token.lineNo = s->lineNo;
                s->state = 0;
                s->lexemeBegin = s->forwardPtr;
                return token;
                break;

            case 49:
                token.name = TK_COLON;
                strcpy(token.string, ":");
                token.lineNo = s->lineNo;
                s->state = 0;
                s->lexemeBegin = s->forwardPtr;
                return token;
                break;

            case 50:
                token.name = TK_DOT;
                strcpy(token.string, ".");
                token.lineNo = s->lineNo;
                s->state = 0;
                s->lexemeBegin = s->forwardPtr;
                return token;
                break;

            case 51:
                token.name = TK_OP;
                strcpy(token.string, "(");
                token.lineNo = s->lineNo;
                s->state = 0;
                s->lexemeBegin = s->forwardPtr;
                return token;
                break;

            case 52:
                token.name = TK_CL;
                strcpy(token.string, ")");
                token.lineNo = s->lineNo;
                s->state = 0;
                s->lexemeBegin = s->forwardPtr;
                return token;
                break;

            case 53:
                token.name = TK_PLUS;
                strcpy(token.string, "+");
                token.lineNo = s->lineNo;
                s->state = 0;
                s->lexemeBegin = s->forwardPtr;
                return token;
                break;

            case 54:
                token.name = TK_MINUS;
                strcpy(token.string, "-");
                token.lineNo = s->lineNo;
                s->state = 0;
                s->lexemeBegin = s->forwardPtr;
                return token;
                break;

            case 55:
                token.name = TK_MUL;
                strcpy(token.string, "*");
                token.lineNo = s->lineNo;
                s->state = 0;
                s->lexemeBegin = s->forwardPtr;
                return token;
                break;

            case 56:
                token.name = TK_DIV;
                strcpy(token.string, "/");
                token.lineNo = s->lineNo;
                s->state = 0;
                s->lexemeBegin = s->forwardPtr;
                return token;
                break;

            case 57:
                token.name = TK_NOT;
                strcpy(token.string, "~");
                token.lineNo = s->lineNo;
                s->state = 0;
                s->lexemeBegin = s->forwardPtr;
                return token;
                break;

            case 58:
                s->lexemeBegin = s->forwardPtr;
                s->state = 0;
                break;

            case 59:
                retract(1, s);
                token.name = TK_ERROR;
                token.lineNo = s->lineNo;
                strcpy(token.string, "pattern");
                strcat(token.string, tokenFromPtrs(s));
                s->state = 0;
                s->lexemeBegin = s->forwardPtr;
                return token;
                break;
            
            case 60:
                character = getCharacter(s);
                if (character == '\n'){
                    s->state = 63;
                }
                else if (character == EOF){
                    token.name = TK_EOF;
                    token.lineNo = s->lineNo;
                    return token;
                }
                else
                    s->state = 60;
                break;

            case 61:
                s->lineNo++;
                s->state = 0;
                s->lexemeBegin = s->forwardPtr;
                break;

            case 62:
                token.name = TK_ERROR;
                token.lineNo = s->lineNo;
                strcpy(token.string, "symbol");
                strcat(token.string, tokenFromPtrs(s));
                s->state = 0;
                s->lexemeBegin = s->forwardPtr;
                return token;
                break;

            case 63:
                token.name = TK_COMMENT;
                strcpy(token.string, "%");
                token.lineNo = s->lineNo;
                s->lineNo++;
                s->state = 0;
                s->lexemeBegin = s->forwardPtr;
                return token;
                break;

            case 64:
                token.name = TK_ERROR;
                token.lineNo = s->lineNo;
                strcpy(token.string, "size");
                strcat(token.string, tokenFromPtrs(s));
                s->state = 0;
                s->lexemeBegin = s->forwardPtr;
                return token;
                break;    
        }
    }
}

void initialize(stateInfo *s) {
    s->state = 0;
    s->lineNo = 1;
    s->lexemeBegin = 0;
    s->forwardPtr = 0;
    s->filePointer = NULL;
    s->sharedVar = 0;
    memset(s->buffer, 0, 2 * BUFFER_SIZE);
}

void lexer_main(char *filename) {
    // s->state = 0;
    // s->lineNo = 1;
    // s->filePointer = NULL;
    // s->sharedVar = 0;
    // memset(s->buffer, 0, 2 * BUFFER_SIZE);
    // s->lexemeBegin = 0;
    // s->forwardPtr = 0;
    stateInfo *s = (stateInfo *)malloc(sizeof(stateInfo));
    initialize(s);

    s->filePointer = fopen(filename, "r");
    if (s->filePointer == NULL) {
        printf("File not found\n");
        return;
    }

    populateBuffer(0, s);

    ST stable = create_symbol_table();
    populate_symbol_table(stable);

    while (s->buffer[s->forwardPtr] != EOF){
        TOKEN token = tokenizer(stable, s);
        if (token.name == TK_EOF) {
            break;
        }

        else if (strstr(token.string, "pattern") != NULL){
            char *substring = strstr(token.string, "pattern");
            substring += strlen("pattern");
            printf("\033[0;31m");
            printf("Line no.%3d        Error :%25sUnknown pattern <%s>\n", token.lineNo, "", substring);
            printf("\033[0m");
        }
        
        else if (strstr(token.string, "size") != NULL){
            char *substring = strstr(token.string, "size");
            substring += strlen("size");
            printf("\033[0;31m");
            printf("Line no.%3d        Error : Variable Identifier is longer than the prescribed length of 20 characters.\n", token.lineNo);
            printf("\033[0m");
        }
        
        else if (strstr(token.string, "symbol") != NULL){
            char *substring = strstr(token.string, "symbol");
            substring += strlen("symbol");
            printf("\033[0;31m");
            printf("Line no.%3d        Error :%25sUnknown symbol <%s>\n", token.lineNo, "", substring);
            printf("\033[0m");
        }

        else if (token.integer != -1){
            printf("Line no.%3d        Lexeme:%23d\t        Token:%15s\n", token.lineNo, token.integer, tokenToString(token.name));

        }

        else if (token.realNum != -1){
            printf("Line no.%3d        Lexeme:%23.2f\t        Token:%15s\n", token.lineNo, token.realNum, tokenToString(token.name));
        }

        else{
            printf("Line No.%3d        Lexeme:%23s\t        Token:%15s\n", token.lineNo, token.string, tokenToString(token.name));
        }
    }
    printf("Symbol table entries (includes prepopulation 51) : %d \n", stable->token_count);
    fclose(s->filePointer);
    free(stable);
}

void removeComments(char *filename) {
    char buffer[256];
    memset(buffer, 0, 256 * sizeof(char));
    FILE *filePointer = fopen(filename, "r");
    if (filePointer == NULL){
        printf("File not found\n");
        return;
    }
    char *newfile = malloc(100 * sizeof(char));
    strcpy(newfile, "clean_");
    strcat(newfile, filename);
    FILE *tempFile = fopen(newfile, "w");

    while (fscanf(filePointer, "%[^\n]", buffer) != EOF){
        int i = 0;
        while (buffer[i] == ' ' || buffer[i] == '\t'){
            i++;
        }
        if (buffer[i] == '%'){
            fprintf(tempFile, "\n");
            printf("\n");
        }
        else{
            fprintf(tempFile, "%s\n", buffer);
            printf("%s\n", buffer);
        }
        fscanf(filePointer, "%c", buffer);
        memset(buffer, 0, 256 * sizeof(char));
    }
    printf("\n-----Comments removed and saved in file: %s-----\n", newfile);
    fflush(tempFile);
    fclose(filePointer);
}
