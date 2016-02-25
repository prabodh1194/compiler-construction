	/*
	 * helper_functions.c: defines various utility functions as declared in utils.h
	 */
	
#include <string.h>
#include "lexerDef.h"
#include "parserDef.h"
	
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
	
int grammar_to_enum(char* grammar_token){
	if(strcmp(id, "a1") == 0) return TK_ASSIGNOP;
	if(strcmp(id, "a2") == 0) return TK_AND;
	if(strcmp(id, "c1") == 0) return TK_COMMA;
	if(strcmp(id, "c2") == 0) return TK_COLON;
	if(strcmp(id, "c3") == 0) return TK_CALL;
	if(strcmp(id, "c4") == 0) return TK_CL;
	if(strcmp(id, "d1") == 0) return TK_DOT;
	if(strcmp(id, "d2") == 0) return TK_DIV;
	if(strcmp(id, "e1") == 0) return TK_END;
	if(strcmp(id, "e2") == 0) return epsilon;
	if(strcmp(id, "e3") == 0) return TK_ENDWHILE;
	if(strcmp(id, "e4") == 0) return TK_ELSE;
	if(strcmp(id, "e5") == 0) return TK_ENDIF;
	if(strcmp(id, "e6") == 0) return TK_EQ;
	if(strcmp(id, "e7") == 0) return TK_ENDRECORD;
	if(strcmp(id, "f1") == 0) return TK_FUNID;
	if(strcmp(id, "f2") == 0) return TK_FIELDID;
	if(strcmp(id, "g1") == 0) return TK_GLOBAL;
	if(strcmp(id, "g2") == 0) return TK_GT;
	if(strcmp(id, "g3") == 0) return TK_GE;
	if(strcmp(id, "i1") == 0) return TK_INPUT;
	if(strcmp(id, "i2") == 0) return TK_ID;
	if(strcmp(id, "i3") == 0) return TK_INT;
	if(strcmp(id, "i4") == 0) return TK_IF;
	if(strcmp(id, "l1") == 0) return TK_LIST;
	if(strcmp(id, "l2") == 0) return TK_LT;
	if(strcmp(id, "l3") == 0) return TK_LE;
	if(strcmp(id, "m1") == 0) return TK_MAIN;
	if(strcmp(id, "m2") == 0) return TK_MUL;
	if(strcmp(id, "m3") == 0) return TK_MINUS;
	if(strcmp(id, "n1") == 0) return TK_NUM;
	if(strcmp(id, "n2") == 0) return TK_NOT;
	if(strcmp(id, "n3") == 0) return TK_NE;
	if(strcmp(id, "o1") == 0) return TK_OUTPUT;
	if(strcmp(id, "o2") == 0) return TK_OP;
	if(strcmp(id, "o3") == 0) return TK_OR;
	if(strcmp(id, "p1") == 0) return TK_PARAMETER;
	if(strcmp(id, "p2") == 0) return TK_PARAMETERS;
	if(strcmp(id, "p3") == 0) return TK_PLUS;
	if(strcmp(id, "r1") == 0) return TK_REAL;
	if(strcmp(id, "r2") == 0) return TK_RECORD;
	if(strcmp(id, "r3") == 0) return TK_RECORDID;
	if(strcmp(id, "r4") == 0) return TK_READ;
	if(strcmp(id, "r5") == 0) return TK_RNUM;
	if(strcmp(id, "r6") == 0) return TK_RETURN;
	if(strcmp(id, "s1") == 0) return TK_SEM;
	if(strcmp(id, "s2") == 0) return TK_SQL;
	if(strcmp(id, "s3") == 0) return TK_SQR;
	if(strcmp(id, "t1") == 0) return TK_TYPE;
	if(strcmp(id, "t2") == 0) return TK_THEN;
	if(strcmp(id, "w1") == 0) return TK_WITH;
	if(strcmp(id, "w2") == 0) return TK_WHILE;
	if(strcmp(id, "w3") == 0) return TK_WRITE;
	if(strcmp(id, "A4") == 0) return all;
	if(strcmp(id, "A3") == 0) return allVar;
	if(strcmp(id, "A2") == 0) return arithmeticExpression;
	if(strcmp(id, "A1") == 0) return assignmentStmt;
	if(strcmp(id, "B1") == 0) return booleanExpression;
	if(strcmp(id, "C2") == 0) return conditionalStmt;
	if(strcmp(id, "C1") == 0) return constructedDatatype;
	if(strcmp(id, "D1") == 0) return dataType;
	if(strcmp(id, "D3") == 0) return declaration;
	if(strcmp(id, "D2") == 0) return declarations;
	if(strcmp(id, "E1") == 0) return innerTerm;
	if(strcmp(id, "E2") == 0) return elseStmt;
	if(strcmp(id, "F5") == 0) return factor;
	if(strcmp(id, "F3") == 0) return fieldDefinition;
	if(strcmp(id, "F2") == 0) return fieldDefinitions;
	if(strcmp(id, "F4") == 0) return funCallStmt;
	if(strcmp(id, "F1") == 0) return function;
	if(strcmp(id, "G1") == 0) return global_or_not;
	if(strcmp(id, "H1") == 0) return mulDiv;
	if(strcmp(id, "I5") == 0) return idList;
	if(strcmp(id, "I1") == 0) return input_par;
	if(strcmp(id, "I4") == 0) return inputParameters;
	if(strcmp(id, "I3") == 0) return ioStmt;
	if(strcmp(id, "I2") == 0) return iterativeStmt;
	if(strcmp(id, "L2") == 0) return logicalOp;
	if(strcmp(id, "L1") == 0) return plusMinus;
	if(strcmp(id, "M1") == 0) return mainFunctions;
	if(strcmp(id, "M3") == 0) return more_ids;
	if(strcmp(id, "M2") == 0) return moreFields;
	if(strcmp(id, "O5") == 0) return operator;
	if(strcmp(id, "O6") == 0) return optionalReturn;
	if(strcmp(id, "O1") == 0) return otherFunctions;
	if(strcmp(id, "O3") == 0) return otherStmts;
	if(strcmp(id, "O2") == 0) return output_par;
	if(strcmp(id, "O4") == 0) return outputParameters;
	if(strcmp(id, "P2") == 0) return parameter_list;
	if(strcmp(id, "P3") == 0) return primitiveDatatype;
	if(strcmp(id, "P1") == 0) return program;
	if(strcmp(id, "R3") == 0) return relationalOp;
	if(strcmp(id, "R1") == 0) return remaining_list;
	if(strcmp(id, "R2") == 0) return returnStmt;
	if(strcmp(id, "S4") == 0) return singleOrRecId_;
	if(strcmp(id, "S3") == 0) return singleOrRecId;
	if(strcmp(id, "S2") == 0) return stmt;
	if(strcmp(id, "S1") == 0) return stmts;
	if(strcmp(id, "T5") == 0) return recAdjust;
	if(strcmp(id, "T3") == 0) return term;
	if(strcmp(id, "T4") == 0) return termPrime;
	if(strcmp(id, "T2") == 0) return typeDefinition;
	if(strcmp(id, "T1") == 0) return typeDefinitions;
	if(strcmp(id, "V1") == 0) return var;
	return -1;
}