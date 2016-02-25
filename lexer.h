
/*
 * lexer.h: contains prototype definitions of functions provided by the lexer
 * 
 */

#ifndef LEXER
#define LEXER

#include <stdio.h>
#include <stdlib.h>
#include "lexerDef.h"

void getStream(FILE *fp);
void printToken(tokenInfo *t);
void getNextToken(FILE *fp, tokenInfo *t);

#endif
