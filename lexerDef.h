/*
BATCH NUMBER: 23
PRABODH AGARWAL 2012B1A7801P
DEEPANSHU SINGH 2012B3A7593P

lexerDef.h: defines the variables and data structures used in lexer.c

*/

#ifndef LEXERDEF
#define LEXERDEF

#define MAX_BUFFER_SIZE 512 // size of the buffer
#define MAX_LEXEME_SIZE 500 // larger than the allowed limits to accomodate large error messages

typedef enum {
    TK_ASSIGNOP,
    TK_COMMENT,
    TK_FIELDID,
    TK_ID,
    TK_NUM,
    TK_RNUM,
    TK_FUNID,
    TK_RECORDID,
    TK_WITH,
    TK_PARAMETERS,
    TK_END,
    TK_WHILE,
    TK_INT,
    TK_REAL,
    TK_TYPE,
    TK_MAIN,
    TK_GLOBAL,
    TK_PARAMETER,
    TK_LIST,
    TK_SQL,
    TK_SQR,
    TK_INPUT,
    TK_OUTPUT,
    TK_SEM,
    TK_COLON,
    TK_DOT,
    TK_COMMA,
    TK_ENDWHILE,
    TK_OP,
    TK_CL,
    TK_IF,
    TK_THEN,
    TK_ENDIF,
    TK_READ,
    TK_WRITE,
    TK_RETURN,
    TK_PLUS,
    TK_MINUS,
    TK_MUL,
    TK_DIV,
    TK_CALL,
    TK_RECORD,
    TK_ENDRECORD,
    TK_ELSE,
    TK_AND,
    TK_OR,
    TK_NOT,
    TK_LT,
    TK_LE,
    TK_EQ,
    TK_GT,
    TK_GE,
    TK_NE,
    TK_EOF,
    eps, // can be considered as a terminal
    TK_ERROR,
    $
}terminalId;

// Data Structure for token
typedef struct {

    char lexeme[MAX_LEXEME_SIZE];
    unsigned long long line_num;
    unsigned long long col;
    terminalId tokenClass;
} tokenInfo;

//Data structure for a row in Hash Table
struct row{
    char value[100];
    char token[50];
    struct row* next;
};

//Data structure for Hash Table
struct hashtable{
    int size;
    struct row* table;
};

typedef struct row row;
typedef struct hashtable hashtable;

#endif
