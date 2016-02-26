/*
 * helper_functions.c: defines various utility functions as declared in utils.h
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

char * enum_to_grammar(int id)
{
    if(id == TK_ASSIGNOP) return "a1";
    if(id == TK_AND) return "a2";
    if(id == TK_COMMA) return "c1";
    if(id == TK_COLON) return "c2";
    if(id == TK_CALL) return "c3";
    if(id == TK_CL) return "c4";
    if(id == TK_DOT) return "d1";
    if(id == TK_DIV) return "d2";
    if(id == TK_END) return "e1";
    if(id == eps) return "e2";
    if(id == TK_ENDWHILE) return "e3";
    if(id == TK_ELSE) return "e4";
    if(id == TK_ENDIF) return "e5";
    if(id == TK_EQ) return "e6";
    if(id == TK_ENDRECORD) return "e7";
    if(id == TK_FUNID) return "f1";
    if(id == TK_FIELDID) return "f2";
    if(id == TK_GLOBAL) return "g1";
    if(id == TK_GT) return "g2";
    if(id == TK_GE) return "g3";
    if(id == TK_INPUT) return "i1";
    if(id == TK_ID) return "i2";
    if(id == TK_INT) return "i3";
    if(id == TK_IF) return "i4";
    if(id == TK_LIST) return "l1";
    if(id == TK_LT) return "l2";
    if(id == TK_LE) return "l3";
    if(id == TK_MAIN) return "m1";
    if(id == TK_MUL) return "m2";
    if(id == TK_MINUS) return "m3";
    if(id == TK_NUM) return "n1";
    if(id == TK_NOT) return "n2";
    if(id == TK_NE) return "n3";
    if(id == TK_OUTPUT) return "o1";
    if(id == TK_OP) return "o2";
    if(id == TK_OR) return "o3";
    if(id == TK_PARAMETER) return "p1";
    if(id == TK_PARAMETERS) return "p2";
    if(id == TK_PLUS) return "p3";
    if(id == TK_REAL) return "r1";
    if(id == TK_RECORD) return "r2";
    if(id == TK_RECORDID) return "r3";
    if(id == TK_READ) return "r4";
    if(id == TK_RNUM) return "r5";
    if(id == TK_RETURN) return "r6";
    if(id == TK_SEM) return "s1";
    if(id == TK_SQL) return "s2";
    if(id == TK_SQR) return "s3";
    if(id == TK_TYPE) return "t1";
    if(id == TK_THEN) return "t2";
    if(id == TK_WITH) return "w1";
    if(id == TK_WHILE) return "w2";
    if(id == TK_WRITE) return "w3";
    if(id == all) return "A4";
    if(id == allVar) return "A3";
    if(id == arithmeticExpression) return "A2";
    if(id == assignmentStmt) return "A1";
    if(id == booleanExpression) return "B1";
    if(id == conditionalStmt) return "C2";
    if(id == constructedDatatype) return "C1";
    if(id == dataType) return "D1";
    if(id == declaration) return "D3";
    if(id == declarations) return "D2";
    if(id == innerTerm) return "E1";
    if(id == elseStmt) return "E2";
    if(id == factor) return "F5";
    if(id == fieldDefinition) return "F3";
    if(id == fieldDefinitions) return "F2";
    if(id == funCallStmt) return "F4";
    if(id == function) return "F1";
    if(id == global_or_not) return "G1";
    if(id == mulDiv) return "H1";
    if(id == idList) return "I5";
    if(id == input_par) return "I1";
    if(id == inputParameters) return "I4";
    if(id == ioStmt) return "I3";
    if(id == iterativeStmt) return "I2";
    if(id == logicalOp) return "L2";
    if(id == plusMinus) return "L1";
    if(id == mainFunctions) return "M1";
    if(id == more_ids) return "M3";
    if(id == moreFields) return "M2";
    if(id == operator) return "O5";
    if(id == optionalReturn) return "O6";
    if(id == otherFunctions) return "O1";
    if(id == otherStmts) return "O3";
    if(id == output_par) return "O2";
    if(id == outputParameters) return "O4";
    if(id == parameter_list) return "P2";
    if(id == primitiveDatatype) return "P3";
    if(id == program) return "P1";
    if(id == relationalOp) return "R3";
    if(id == remaining_list) return "R1";
    if(id == returnStmt) return "R2";
    if(id == singleOrRecId_) return "S4";
    if(id == singleOrRecId) return "S3";
    if(id == stmt) return "S2";
    if(id == stmts) return "S1";
    if(id == recAdjust) return "T5";
    if(id == term) return "T3";
    if(id == termPrime) return "T4";
    if(id == typeDefinition) return "T2";
    if(id == typeDefinitions) return "T1";
    if(id == var) return "V1";
    if(id == $) return "$1";
    else return '\0';
}
