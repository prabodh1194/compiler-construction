#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
function_hashtable* create_function_hashtable(int size);
identifier_list *addIdentifier(identifier_list *idlist, char *name, char *type);
identifier_hashtable* create_identifier_hashtable(int size);
void print_function_hashtable(function_hashtable* h);
void add_function(function_hashtable* h, char* fname, identifier_list* ip_list, int flag);
#endif
