/*
 * helper_functions.c: defines various utility functions as declared in utils.h
 */

#include <string.h>
#include "lexerDef.h"


int sanitizeError(char c) {
    /* returns true for characters which aren't easily displayed line newline, eof etc.
     * used when generating error messages
     */
    if(c == '\n' || c == '\r' || c == ' ' || c == '\v' || c == '\f' || c == '\t' || c == 26) // 26 is eof
        return 1;
    else
        return 0;
}

char *tokenName(int id) {
	// connverts enum to string
	if(id == TK_AND) return "TK_AND";
	if(id == TK_ASSIGNOP) return "TK_ASSIGNOP";
	if(id == TK_CALL) return "TK_CALL";
	if(id == TK_CL) return "TK_CL";
	if(id == TK_COLON) return "TK_COLON";
	if(id == TK_COMMA) return "TK_COMMA";
	if(id == TK_COMMENT) return "TK_COMMENT";
	if(id == TK_DIV) return "TK_DIV";
	if(id == TK_DOT) return "TK_DOT";
	if(id == TK_ELSE) return "TK_ELSE";
	if(id == TK_END) return "TK_END";
	if(id == TK_ENDIF) return "TK_ENDIF";
	if(id == TK_ENDWHILE) return "TK_ENDWHILE";
	if(id == TK_ENDRECORD) return "TK_ENDRECORD";
	if(id == TK_EOF) return "TK_EOF";
	if(id == eps) return "eps";
	if(id == TK_EQ) return "TK_EQ";
	if(id == TK_ERROR) return "TK_ERROR";
	if(id == TK_FIELDID) return "TK_FIELDID";
	if(id == TK_FUNID) return "TK_FUNID";
	if(id == TK_GE) return "TK_GE";
	if(id == TK_GLOBAL) return "TK_GLOBAL";
	if(id == TK_GT) return "TK_GT";
	if(id == TK_ID) return "TK_ID";
	if(id == TK_IF) return "TK_IF";
	if(id == TK_INPUT) return "TK_INPUT";
	if(id == TK_INT) return "TK_INT";
	if(id == TK_LE) return "TK_LE";
	if(id == TK_LIST) return "TK_LIST";
	if(id == TK_LT) return "TK_LT";
	if(id == TK_MAIN) return "TK_MAIN";
	if(id == TK_MINUS) return "TK_MINUS";
	if(id == TK_MUL) return "TK_MUL";
	if(id == TK_NE) return "TK_NE";
	if(id == TK_NOT) return "TK_NOT";
	if(id == TK_NUM) return "TK_NUM";
	if(id == TK_OP) return "TK_OP";
	if(id == TK_OR) return "TK_OR";
	if(id == TK_OUTPUT) return "TK_OUTPUT";
	if(id == TK_PARAMETER) return "TK_PARAMETER";
	if(id == TK_PARAMETERS) return "TK_PARAMETERS";
	if(id == TK_PLUS) return "TK_PLUS";
	if(id == TK_READ) return "TK_READ";
	if(id == TK_REAL) return "TK_REAL";
	if(id == TK_RECORD) return "TK_RECORD";
	if(id == TK_RECORDID) return "TK_RECORDID";
	if(id == TK_RETURN) return "TK_RETURN";
	if(id == TK_RNUM) return "TK_RNUM";
	if(id == TK_SEM) return "TK_SEM";
	if(id == TK_SQL) return "TK_SQL";
	if(id == TK_SQR) return "TK_SQR";
	if(id == TK_THEN) return "TK_THEN";
	if(id == TK_TYPE) return "TK_TYPE";
	if(id == TK_WITH) return "TK_WITH";	
	if(id == TK_WHILE) return "TK_WHILE";
	if(id == TK_WRITE) return "TK_WRITE";
	return "";
}


