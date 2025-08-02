#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define LETTER 0
#define DIGIT 1
#define UNKNOWN 99

#define INT_LIT 10
#define IDENT 11
#define KEYWORD 12
#define ASSIGN_OP 20
#define ADD_OP 21
#define SUB_OP 22
#define MULT_OP 23
#define DIV_OP 24
#define LEFT_PAREN 25
#define RIGHT_PAREN 26
#define END_OF_FILE -1

int charClass;
char lexeme[100];
char nextChar;
int lexLen;
int nextToken;
FILE* in_fp;

void addChar() {
    if (lexLen <= 98) {
        lexeme[lexLen++] = nextChar;
        lexeme[lexLen] = '\0';
    }
}

void getChar() {
    int c = getc(in_fp);
    if (c != EOF) {
        nextChar = (char)c;
        if (isalpha(nextChar))
            charClass = LETTER;
        else if (isdigit(nextChar))
            charClass = DIGIT;
        else
            charClass = UNKNOWN;
    }
    else {
        charClass = END_OF_FILE;
        nextChar = '\0';
    }
}

void skipLineComment() {
    while (nextChar != '\n' && charClass != END_OF_FILE) {
        getChar();
    }
}

int isKeyword(const char* s) {
    return strcmp(s, "int") == 0 || strcmp(s, "float") == 0 ||
        strcmp(s, "if") == 0 || strcmp(s, "else") == 0 ||
        strcmp(s, "return") == 0;
}

void getNonBlank() {
    while (isspace(nextChar))
        getChar();
}

int lookup(char ch) {
    switch (ch) {
    case '(':
        addChar();
        nextToken = LEFT_PAREN;
        break;
    case ')':
        addChar();
        nextToken = RIGHT_PAREN;
        break;
    case '+':
        addChar();
        nextToken = ADD_OP;
        break;
    case '-':
        addChar();
        nextToken = SUB_OP;
        break;
    case '*':
        addChar();
        nextToken = MULT_OP;
        break;
    case '/':
        addChar();
        nextToken = DIV_OP;
        break;
    case '=':
        addChar();
        nextToken = ASSIGN_OP;
        break;
    default:
        addChar();
        nextToken = UNKNOWN;
        break;
    }
    return nextToken;
}

int lex() {
    lexLen = 0;
    getNonBlank();

    if (nextChar == '/') {
        int saved = ftell(in_fp);
        addChar();
        getChar();
        if (nextChar == '/') {
            skipLineComment();
            getNonBlank();
        }
        else {
            fseek(in_fp, saved, SEEK_SET);
            getChar();
        }
    }

    switch (charClass) {
    case LETTER:
        addChar();
        getChar();
        while (charClass == LETTER || charClass == DIGIT) {
            addChar();
            getChar();
        }
        if (isKeyword(lexeme))
            nextToken = KEYWORD;
        else
            nextToken = IDENT;
        break;
    case DIGIT:
        addChar();
        getChar();
        while (charClass == DIGIT) {
            addChar();
            getChar();
        }
        nextToken = INT_LIT;
        break;
    case UNKNOWN:
        lookup(nextChar);
        getChar();
        break;
    case END_OF_FILE:
        nextToken = END_OF_FILE;
        strcpy(lexeme, "EOF");
        break;
    default:
        nextToken = END_OF_FILE;
        strcpy(lexeme, "EOF");
        break;
    }

    printf("Next token is: %d, Next lexeme is %s\n", nextToken, lexeme);
    return nextToken;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <source-file>\n", argv[0]);
        return 1;
    }

    in_fp = fopen(argv[1], "r");
    if (in_fp == NULL) {
        printf("ERROR - cannot open %s\n", argv[1]);
        return 1;
    }

    getChar();
    printf("Lexical Analysis Output:\n");
    do {
        lex();
    } while (nextToken != END_OF_FILE);

    fclose(in_fp);
    return 0;
}