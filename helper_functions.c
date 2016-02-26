	/*
	 * helper_functions.c: defines various utility functions as declared in utils.h
	 */
	
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "lexerDef.h"
#include "parseDef.h"

hashtable* create_hashtable(int size){
	hashtable* h = NULL;
	int i=0;
	if((h = (hashtable*) malloc(sizeof(hashtable))) == NULL)
		return NULL;

	if((h->table = (row*) malloc(sizeof(row) * size)) == NULL)
		return NULL;

	for(i=0;i<size;i++){
		strcpy(h->table[i].value,"null");
		h->table[i].next = NULL;
	}
	h->size = size;
	return h;
}

void fill_hashtable(hashtable* h, char* key1, char* key2){
	int index;
	index = hash_function(key1,h->size);
	//printf("%d %s\n", index, key1);
	if(strcmp(h->table[index].value,"")==0){
		//printf("Success\n");
		strcpy(h->table[index].value, key1);
		strcpy(h->table[index].token, key2);
	}
	else{
		//printf("In else");
		row *new_entry,*collision;
		new_entry = (row*) malloc(sizeof(row));
		strcpy(new_entry->value,key1);
		strcpy(new_entry->token,key2);
		new_entry->next = NULL;
		collision = &(h->table[index]);
		while(collision->next != NULL)
			collision = collision->next;
		collision->next = new_entry;
		//h->table[index].next = new_entry;
		
	}
}

void print_hashtable(hashtable* h){
	int i=0;
	row* current_pointer;
	//printf("%d\n", h->size);
	for (i=0;i<h->size;i++){
		//printf("%d %s\n",i, h->table[i].value);
		current_pointer = &(h->table[i]);
		while(current_pointer != NULL){
			printf("%d %s %s\n",i,current_pointer->value,current_pointer->token);
			current_pointer = current_pointer->next;
		}
	}
}
int hash_function(char* key, int size){
	int i=0,sum=0;
	while(key[i]!='\0')
		sum += (int)key[i++]-97;
	return sum%size;
}
int check_function(int key[],int size){
	int sum, i=0;
	char input[100];
	FILE* fp;
	fp = fopen("keyword.txt","r");
	while(!feof(fp)){
		fscanf(fp,"%s",input);
		sum = hash_function(input,size);
		key[i++]=sum;
		printf("%s %d\n",input,key[i-1]);
	}
}

hashtable* hash_keywords(){
	hashtable *h1;
	char input1[100];
	char input2[50];
	FILE* fp;
	h1 = create_hashtable(53);
	fp = fopen("keyword.txt","r");
	while(!feof(fp)){
		fscanf(fp,"%s %s",input1,input2);
		fill_hashtable(h1,input1,input2);
	}
	//print_hashtable(h);
	return h1;
}


int sanitizeError(char c) {
    /* returns true for characters which aren't easily displayed line newline, eof etc.
     * used when generating error messages
     */
    if(c == '\n' || c == '\r' || c == ' ' || c == '\v' || c == '\f' || c == '\t' || c == 26) // 26 is eof
        return 1;
    else
        return 0;
}

terminalId tokenClass(char* tokenName){
	if (strcmp(tokenName,"TK_CALL") ==0) return TK_CALL;
	if (strcmp(tokenName,"TK_ELSE") ==0) return TK_ELSE;
	if (strcmp(tokenName,"TK_END") ==0) return TK_END;
	if (strcmp(tokenName,"TK_ENDIF") ==0) return TK_ENDIF;
	if (strcmp(tokenName,"TK_ENDWHILE") ==0) return TK_ENDWHILE;
	if (strcmp(tokenName,"TK_ENDRECORD") ==0) return TK_ENDRECORD;
	if (strcmp(tokenName,"TK_GLOBAL") ==0) return TK_GLOBAL;
	if (strcmp(tokenName,"TK_IF") ==0) return TK_IF;
	if (strcmp(tokenName,"TK_INPUT") ==0) return TK_INPUT;
	if (strcmp(tokenName,"TK_INT") ==0) return TK_INT;
	if (strcmp(tokenName,"TK_OUTPUT") ==0) return TK_OUTPUT;
	if (strcmp(tokenName,"TK_PARAMETER") ==0) return TK_PARAMETER;
	if (strcmp(tokenName,"TK_PARAMETERS") ==0) return TK_PARAMETERS;
	if (strcmp(tokenName,"TK_READ") ==0) return TK_READ;
	if (strcmp(tokenName,"TK_REAL") ==0) return TK_REAL;
	if (strcmp(tokenName,"TK_RECORD") ==0) return TK_RECORD;
	if (strcmp(tokenName,"TK_RETURN") ==0) return TK_RETURN;
	if (strcmp(tokenName,"TK_THEN") ==0) return TK_THEN;
	if (strcmp(tokenName,"TK_TYPE") ==0) return TK_TYPE;
	if (strcmp(tokenName,"TK_WITH") ==0) return TK_WITH;
	if (strcmp(tokenName,"TK_WHILE") ==0) return TK_WHILE;
	if (strcmp(tokenName,"TK_WRITE") ==0) return TK_WRITE;
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
/*	
int grammar_to_enum(char* grammar_token){
	if(strcmp(grammar_token, "a1") == 0) return TK_ASSIGNOP;
	if(strcmp(grammar_token, "a2") == 0) return TK_AND;
	if(strcmp(grammar_token, "c1") == 0) return TK_COMMA;
	if(strcmp(grammar_token, "c2") == 0) return TK_COLON;
	if(strcmp(grammar_token, "c3") == 0) return TK_CALL;
	if(strcmp(grammar_token, "c4") == 0) return TK_CL;
	if(strcmp(grammar_token, "d1") == 0) return TK_DOT;
	if(strcmp(grammar_token, "d2") == 0) return TK_DIV;
	if(strcmp(grammar_token, "e1") == 0) return TK_END;
	if(strcmp(grammar_token, "e2") == 0) return epsilon;
	if(strcmp(grammar_token, "e3") == 0) return TK_ENDWHILE;
	if(strcmp(grammar_token, "e4") == 0) return TK_ELSE;
	if(strcmp(grammar_token, "e5") == 0) return TK_ENDIF;
	if(strcmp(grammar_token, "e6") == 0) return TK_EQ;
	if(strcmp(grammar_token, "e7") == 0) return TK_ENDRECORD;
	if(strcmp(grammar_token, "f1") == 0) return TK_FUNID;
	if(strcmp(grammar_token, "f2") == 0) return TK_FIELDID;
	if(strcmp(grammar_token, "g1") == 0) return TK_GLOBAL;
	if(strcmp(grammar_token, "g2") == 0) return TK_GT;
	if(strcmp(grammar_token, "g3") == 0) return TK_GE;
	if(strcmp(grammar_token, "i1") == 0) return TK_INPUT;
	if(strcmp(grammar_token, "i2") == 0) return TK_ID;
	if(strcmp(grammar_token, "i3") == 0) return TK_INT;
	if(strcmp(grammar_token, "i4") == 0) return TK_IF;
	if(strcmp(grammar_token, "l1") == 0) return TK_LIST;
	if(strcmp(grammar_token, "l2") == 0) return TK_LT;
	if(strcmp(grammar_token, "l3") == 0) return TK_LE;
	if(strcmp(grammar_token, "m1") == 0) return TK_MAIN;
	if(strcmp(grammar_token, "m2") == 0) return TK_MUL;
	if(strcmp(grammar_token, "m3") == 0) return TK_MINUS;
	if(strcmp(grammar_token, "n1") == 0) return TK_NUM;
	if(strcmp(grammar_token, "n2") == 0) return TK_NOT;
	if(strcmp(grammar_token, "n3") == 0) return TK_NE;
	if(strcmp(grammar_token, "o1") == 0) return TK_OUTPUT;
	if(strcmp(grammar_token, "o2") == 0) return TK_OP;
	if(strcmp(grammar_token, "o3") == 0) return TK_OR;
	if(strcmp(grammar_token, "p1") == 0) return TK_PARAMETER;
	if(strcmp(grammar_token, "p2") == 0) return TK_PARAMETERS;
	if(strcmp(grammar_token, "p3") == 0) return TK_PLUS;
	if(strcmp(grammar_token, "r1") == 0) return TK_REAL;
	if(strcmp(grammar_token, "r2") == 0) return TK_RECORD;
	if(strcmp(grammar_token, "r3") == 0) return TK_RECORDID;
	if(strcmp(grammar_token, "r4") == 0) return TK_READ;
	if(strcmp(grammar_token, "r5") == 0) return TK_RNUM;
	if(strcmp(grammar_token, "r6") == 0) return TK_RETURN;
	if(strcmp(grammar_token, "s1") == 0) return TK_SEM;
	if(strcmp(grammar_token, "s2") == 0) return TK_SQL;
	if(strcmp(grammar_token, "s3") == 0) return TK_SQR;
	if(strcmp(grammar_token, "t1") == 0) return TK_TYPE;
	if(strcmp(grammar_token, "t2") == 0) return TK_THEN;
	if(strcmp(grammar_token, "w1") == 0) return TK_WITH;
	if(strcmp(grammar_token, "w2") == 0) return TK_WHILE;
	if(strcmp(grammar_token, "w3") == 0) return TK_WRITE;
	if(strcmp(grammar_token, "A4") == 0) return all;
	if(strcmp(grammar_token, "A3") == 0) return allVar;
	if(strcmp(grammar_token, "A2") == 0) return arithmeticExpression;
	if(strcmp(grammar_token, "A1") == 0) return assignmentStmt;
	if(strcmp(grammar_token, "B1") == 0) return booleanExpression;
	if(strcmp(grammar_token, "C2") == 0) return conditionalStmt;
	if(strcmp(grammar_token, "C1") == 0) return constructedDatatype;
	if(strcmp(grammar_token, "D1") == 0) return dataType;
	if(strcmp(grammar_token, "D3") == 0) return declaration;
	if(strcmp(grammar_token, "D2") == 0) return declarations;
	if(strcmp(grammar_token, "E1") == 0) return innerTerm;
	if(strcmp(grammar_token, "E2") == 0) return elseStmt;
	if(strcmp(grammar_token, "F5") == 0) return factor;
	if(strcmp(grammar_token, "F3") == 0) return fieldDefinition;
	if(strcmp(grammar_token, "F2") == 0) return fieldDefinitions;
	if(strcmp(grammar_token, "F4") == 0) return funCallStmt;
	if(strcmp(grammar_token, "F1") == 0) return function;
	if(strcmp(grammar_token, "G1") == 0) return global_or_not;
	if(strcmp(grammar_token, "H1") == 0) return mulDiv;
	if(strcmp(grammar_token, "I5") == 0) return idList;
	if(strcmp(grammar_token, "I1") == 0) return input_par;
	if(strcmp(grammar_token, "I4") == 0) return inputParameters;
	if(strcmp(grammar_token, "I3") == 0) return ioStmt;
	if(strcmp(grammar_token, "I2") == 0) return iterativeStmt;
	if(strcmp(grammar_token, "L2") == 0) return logicalOp;
	if(strcmp(grammar_token, "L1") == 0) return plusMinus;
	if(strcmp(grammar_token, "M1") == 0) return mainFunctions;
	if(strcmp(grammar_token, "M3") == 0) return more_ids;
	if(strcmp(grammar_token, "M2") == 0) return moreFields;
	if(strcmp(grammar_token, "O5") == 0) return operator;
	if(strcmp(grammar_token, "O6") == 0) return optionalReturn;
	if(strcmp(grammar_token, "O1") == 0) return otherFunctions;
	if(strcmp(grammar_token, "O3") == 0) return otherStmts;
	if(strcmp(grammar_token, "O2") == 0) return output_par;
	if(strcmp(grammar_token, "O4") == 0) return outputParameters;
	if(strcmp(grammar_token, "P2") == 0) return parameter_list;
	if(strcmp(grammar_token, "P3") == 0) return primitiveDatatype;
	if(strcmp(grammar_token, "P1") == 0) return program;
	if(strcmp(grammar_token, "R3") == 0) return relationalOp;
	if(strcmp(grammar_token, "R1") == 0) return remaining_list;
	if(strcmp(grammar_token, "R2") == 0) return returnStmt;
	if(strcmp(grammar_token, "S4") == 0) return singleOrRecId_;
	if(strcmp(grammar_token, "S3") == 0) return singleOrRecId;
	if(strcmp(grammar_token, "S2") == 0) return stmt;
	if(strcmp(grammar_token, "S1") == 0) return stmts;
	if(strcmp(grammar_token, "T5") == 0) return recAdjust;
	if(strcmp(grammar_token, "T3") == 0) return term;
	if(strcmp(grammar_token, "T4") == 0) return termPrime;
	if(strcmp(grammar_token, "T2") == 0) return typeDefinition;
	if(strcmp(grammar_token, "T1") == 0) return typeDefinitions;
	if(strcmp(grammar_token, "V1") == 0) return var;
	return -1;
}*/