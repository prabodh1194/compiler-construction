/*
BATCH NUMBER: 23
PRABODH AGARWAL 2012B1A7801P
DEEPANSHU SINGH 2012B3A7593P

*/

#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
function_hashtable* create_function_hashtable(int size);
identifier_hashtable* create_identifier_hashtable(int size);
function_wise_identifier_hashtable* create_function_local_identifier_hashtable(int size);
identifier_list *create_identifier_list(char *name,char *type);
identifier_list *add_element_to_list(identifier_list *idlist, char *name, char *type);
identifier_list *addIdentifier(identifier_list *idlist, char *name, char *type, int offset);
int add_identifier_to_identifierhashtable(identifier_hashtable *h, char *name, char *type, int offset);
int add_function_local_identifier_hashtable(function_wise_identifier_hashtable *h, char *fname, identifier_list *idlist, int offset);
int add_function(function_hashtable* h, char* fname, identifier_list* ip_list, int flag);
int get_record_size(function_wise_identifier_hashtable* h, char *fname);
identifier_list* search_global_identifier(identifier_hashtable* h, char *name);
identifier_list *get_record_fields(function_wise_identifier_hashtable* h, char *rname);
identifier_hashtable* get_function_identifier_hashtable(function_wise_identifier_hashtable* h, char *fname);
identifier_list* search_function_wise_identifier_hashtable(function_wise_identifier_hashtable* h, char *fname, char *iname);
function_node* search_function_hashtable(function_hashtable* h, char *fname);
identifier_list* get_input_parameter_list(function_hashtable* h, char *fname);
identifier_list* get_output_parameter_list(function_hashtable* h, char *fname);
void print_function_hashtable(function_hashtable* h);
void print_identifier_hashtable(identifier_hashtable *h, char *fname, function_wise_identifier_hashtable* record);
void print_list(identifier_list* list, char *type);
void print_function_wise_identifier_hashtable(function_wise_identifier_hashtable* h, function_wise_identifier_hashtable* record);
void compare_parameter_list_type(identifier_list* i1, identifier_list* i2, char* returnmsg);
#endif
