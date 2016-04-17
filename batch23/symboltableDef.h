#ifndef SYMBOLTABLEDEF
#define SYMBOLTABLEDEF

struct identifier_list{
	char *name;
	char *type;
    int offset;
	struct identifier_list* next;
};

struct function_node{
    char *fname;
    struct identifier_list *input_parameter_list;
    struct identifier_list *output_parameter_list;
    struct function_node *next;
};

//Data structure for Function Hash Table
struct function_hashtable{
    int size;
    struct function_node** table;
};

typedef struct function_node function_node;
typedef struct function_hashtable function_hashtable;

//Data structure for storing Identifiers
struct identifier_hashtable{
	int size;
    int offset;
	struct identifier_list** table;
};

struct function_identifier_node{
    char *fname;
    int offset;
    int size;
    struct identifier_hashtable* id_hashtable;
    struct function_identifier_node *next;
};
struct function_wise_identifier_hashtable{
    int size;
    struct function_identifier_node** table;
};

typedef struct identifier_list identifier_list;
typedef struct identifier_hashtable identifier_hashtable;
typedef struct function_identifier_node function_identifier_node;
typedef struct function_wise_identifier_hashtable function_wise_identifier_hashtable;

#endif
