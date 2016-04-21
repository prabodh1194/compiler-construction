/*
BATCH NUMBER: 23
PRABODH AGARWAL 2012B1A7801P
DEEPANSHU SINGH 2012B3A7593P

*/

#ifndef SYMBOLTABLEDEF
#define SYMBOLTABLEDEF

//Data structure to store an Identifier
struct identifier_list{
	char *name;
	char *type;
    int offset;
	struct identifier_list* next;
};

//Data Structure to store a Function in Function HashTable
struct function_node{
    char *fname;
    struct identifier_list *input_parameter_list;
    struct identifier_list *output_parameter_list;
    struct function_node *next;
};

//Data structure for Function HashTable
struct function_hashtable{
    int size;
    struct function_node** table;
};

//Data structure for Identifier HashTable
struct identifier_hashtable{
	int size;
    int offset;
	struct identifier_list** table;
};

//Data Structure to store the address of Function's Identifier HashTable and other details (First Level Hashing)
struct function_identifier_node{
    char *fname;
    int offset;
    int size;
    struct identifier_hashtable* id_hashtable;
    struct function_identifier_node *next;
};

//Data Structure for Function Wise Identifier HashTable
struct function_wise_identifier_hashtable{
    int size;
    struct function_identifier_node** table;
};

typedef struct function_node function_node;
typedef struct function_hashtable function_hashtable;
typedef struct identifier_list identifier_list;
typedef struct identifier_hashtable identifier_hashtable;
typedef struct function_identifier_node function_identifier_node;
typedef struct function_wise_identifier_hashtable function_wise_identifier_hashtable;

#endif
