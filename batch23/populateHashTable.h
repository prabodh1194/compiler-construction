#ifndef POPULATEHASHTABLE_H
#define POPULATEHASHTABLE_H
function_hashtable *funcs;
function_wise_identifier_hashtable *local;
identifier_hashtable *global;
function_wise_identifier_hashtable *record;
void populateFunctionST(parseTree *p, char *fname, int state);
identifier_list * getParams(parseTree *p, identifier_list *list, char *, int start);
#endif
