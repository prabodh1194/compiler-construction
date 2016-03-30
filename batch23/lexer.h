/*
BATCH NUMBER: 23
PRABODH AGARWAL 2012B1A7801P
DEEPANSHU SINGH 2012B3A7593P

lexer.h: function prototypes for the functions used in lexer.c

*/

#ifndef LEXER
#define LEXER

#include <stdio.h>
#include <stdlib.h>
#include "lexerDef.h"

char getStream(FILE *fp);
void getNextToken(FILE *fp, tokenInfo *t);
void printTok(char *file);
void printCode(char *file);


#endif
