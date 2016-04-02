#ifndef SYMBOLTABLEDEF
#define SYMBOLTABLEDEF

struct identifier_list{
	char *name;
	char *type;
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
	struct identifier_list** table;
};
typedef struct identifier_list identifier_list;
typedef struct identifier_hashtable identifier_hashtable;

#endif
