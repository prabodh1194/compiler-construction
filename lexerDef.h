/*
 * lexerDef.h: defines the data structures and constants used by the lexer
  */

#ifndef LEXERDEF
#define LEXERDEF

#define MAX_BUFFER_SIZE 512
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
    eps, // eps is not a nonterminal - can be considered a terminal
    TK_ERROR
} terminalId;

// the tokenInfo structure - defines a token
typedef struct {

    char lexeme[MAX_LEXEME_SIZE];
    unsigned long long line_num;
    terminalId tokenClass;
} tokenInfo;

#endif
