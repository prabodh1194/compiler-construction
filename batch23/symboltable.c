#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symboltableDef.h"
#include "symboltable.h"

function_hashtable* create_function_hashtable(int size){
	function_hashtable* h = NULL;
	int i=0;
	if((h = (function_hashtable*) malloc(sizeof(function_hashtable))) == NULL)
		return NULL;

	if((h->table = (function_node*) malloc(sizeof(function_node) * size)) == NULL)
		return NULL;
	bzero(h->table, sizeof(function_node)*size);
	h->size = size;
	return h;
}

identifier_hashtable* create_identifier_hashtable(int size){
	identifier_hashtable* h = NULL;
	int i=0;
	if((h = (identifier_hashtable*) malloc(sizeof(identifier_hashtable))) == NULL)
		return NULL;

	if((h->table = (identifer_list*) malloc(sizeof(identifier_list) * size)) == NULL)
		return NULL;
	bzero(h->table, sizeof(function_node)*size);
	h->size = size;
	return h;
}

identifier_list *create_identifier_list(char *name,int type){
	identifier_list *idlist;
	idlist = addIdentifier(NULL,name,type);
	return idlist;
}
identifier_list *addIdentifier(identifier_list *idlist, char *name, int type){ //, char *nameOfRecord) {
    /* appends a new (name, type) pair to the end of idlist
     * and returns a pointer to the new list
     */

    identifier_list *newPair = (identifier_list *)malloc(sizeof(identifier_list));
    newPair->name = (char *)malloc((strlen(name) + 1) * sizeof(char));
    //newPair->identifier.nameOfRecord = (char *)malloc((strlen(nameOfRecord) + 1) * sizeof(char));
    //strcpy(newPair->identifier.nameOfRecord, nameOfRecord);
    strcpy(newPair->identifier.name, name);
    newPair->type = type;
    while(idlist->next!= NULL)
    	idlist = idlist->next;
    idlist = newPair;
    newPair->next = NULL;

    return newPair;
}
//Used to insert Keyword and its token in the Hash Table i.e. Populating the hashtable
void fill_function_hashtable(function_hashtable* h, char* fname, input_parameter_list* ip_list, output_parameter_list *op_list){
	int index;
	index = hash_function(fname,h->size);
	if(h->table[index].fname == NULL){ //check this condition
		strcpy(h->table[index].fname, fname);
		h->table[index].input_parameter_list = ip_list;
		h->table[index].output_parameter_list = op_list;
	}
	else{
		row *new_entry,*collision;
		new_entry = (row*) malloc(sizeof(row));
		strcpy(new_entry->fname,fname);
		new_entry->input_parameter_list = ip_list;
		new_entry->output_parameter_list = op_list;
		new_entry->next = NULL;
		collision = &(h->table[index]);
		while(collision->next != NULL)
			collision = collision->next;
		collision->next = new_entry;
	}
}

void search_function_hashtable(function_hashtable* h, char *fname){
	int index;
	function_hashtable *pos;
	index = hash_function(fname, h->size);
	pos = h->table[index];
	while(pos!=NULL){
		if(strcmp(fname,pos->fname) == 0)
			return 1;
		pos = pos->next;
	}
	return 0;	

}

//Used to print the Hash Table
void print_hashtable(hashtable* h){
	int i=0;
	row* current_pointer;
	//printf("%d\n", h->size);
	for (i=0;i<h->size;i++){
		//printf("%d %s\n",i, h->table[i].value);
		current_pointer = &(h->table[i]);
		while(current_pointer != NULL){
			printf("%d %s %s\n",i,current_pointer->value,current_pointer->token);
			current_pointer = current_pointer->next;
		}
	}
}