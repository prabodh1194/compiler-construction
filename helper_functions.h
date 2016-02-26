
#ifndef helper_functions
#define helper_functions


#include "lexerDef.h"

void calc();//get first and follow sets
int sanitizeError(char);
char *tokenName(int);
int grammar_to_enum(char*);
char * enum_to_grammar(int);

struct y
{
    char a[20];
    struct y *next;
    char eps;
    char follow;
}; 
#endif
