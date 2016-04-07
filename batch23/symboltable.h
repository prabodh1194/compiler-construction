#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
function_hashtable* create_function_hashtable(int size);
identifier_list *addIdentifier(identifier_list *idlist, char *name, char *type);
identifier_hashtable* create_identifier_hashtable(int size);
function_wise_identifier_hashtable* create_function_local_identifier_hashtable(int size);
void print_function_hashtable(function_hashtable* h);
void add_identifier_to_hashtable(identifier_hashtable *h, char *name, char *type);
void add_identifier_to_identifierhashtable(identifier_hashtable *h, char *name, char *type);
void add_function_local_identifier_hashtable(function_wise_identifier_hashtable *h, char *fname, identifier_list *idlist);
int add_function(function_hashtable* h, char* fname, identifier_list* ip_list, int flag);
void search_function_hashtable(function_hashtable* h, char *fname);
void print_function_hashtable(function_hashtable* h);
void print_identifier_hashtable(identifier_hashtable *h);
void print_function_wise_identifier_hashtable(function_wise_identifier_hashtable* h);
#endif
