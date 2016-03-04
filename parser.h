/*
BATCH NUMBER: 23
PRABODH AGARWAL 2012B1A7801P
DEEPANSHU SINGH 2012B3A7593P

parser.h: function prototypes for the functions used in parser.c


*/


#ifndef PARSER
#define PARSER
#define tt (*t)
typedef int grammar[MAX_RULES+1][MAX_RULE_SIZE+2];
typedef int table[MAX_NON_TERMINALS][MAX_TERMINALS];
extern struct y *ptr[26][10][3];

extern hashtable *h;

char * getFirstSet(char *, char *, char *);
char * getFollowSet(char *);
void printParseTree(parseTree *p, FILE *outfile);
void printParseTable(table t);
int parseInputSourceCode(FILE *sourceCodeFile, table tb, grammar g, parseTree *root, tokenInfo *t);
char* getFirstSet(char *lhs, char *rhs, char *set);
void createParseTable(grammar g, table *t);
void getGrammar(grammar *g);
#endif
