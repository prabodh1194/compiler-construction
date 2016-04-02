/*
BATCH NUMBER: 23
PRABODH AGARWAL 2012B1A7801P
DEEPANSHU SINGH 2012B3A7593P

*/

#ifndef helper_functions
#define helper_functions
#include "lexerDef.h"

int hash_function(char* , int );
hashtable* create_hashtable(int );
void fill_hashtable(hashtable* , char* , char* );
void print_hashtable(hashtable* );
int hash_function(char* , int );
int check_function(int ,int );
hashtable * hash_keywords();
terminalId tokenClass(char* tokenName);
int isUseful(int tokenClass);

void populate();//get first and follow sets
int sanitizeError(char);
char *tokenName(int);
int grammar_to_enum(char*);
char * enum_to_grammar(int);
void printff(int);

struct y
{
    char a[1000];
    struct y *next;
    char eps;
    char follow;
    char cyclic;
}; 
#endif
