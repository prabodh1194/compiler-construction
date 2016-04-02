#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexerDef.h"
#include "parserDef.h"
#include "symboltableDef.h"
#include "symboltable.h"
#include "helper_functions.h"

function_hashtable* create_function_hashtable(int size){
	function_hashtable* h = NULL;
	int i=0;
	if((h = (function_hashtable*) malloc(sizeof(function_hashtable))) == NULL)
		return NULL;

	if((h->table = (function_node **) malloc(sizeof(function_node *) * size)) == NULL)
		return NULL;
	bzero(h->table, sizeof(function_node *)*size);
	h->size = size;
	return h;
}

identifier_hashtable* create_identifier_hashtable(int size){
	identifier_hashtable* h = NULL;
	int i=0;
	if((h = (identifier_hashtable*) malloc(sizeof(identifier_hashtable))) == NULL)
		return NULL;

	if((h->table = (identifier_list **) malloc(sizeof(identifier_list *) * size)) == NULL)
		return NULL;
	bzero(h->table, sizeof(identifier_list *)*size);
	h->size = size;
	return h;
}

identifier_list *create_identifier_list(char *name,char *type){
	identifier_list *idlist;
	idlist = addIdentifier(NULL,name,type);
	return idlist;
}
identifier_list *addIdentifier(identifier_list *idlist, char *name, char *type){ //, char *nameOfRecord) {
    /* appends a new (name, type) pair to the end of idlist
     * and returns a pointer to the new list
     */

    identifier_list *newPair = (identifier_list *)malloc(sizeof(identifier_list));
    newPair->name = (char *)malloc((strlen(name) + 1) * sizeof(char));
    //newPair->identifier.nameOfRecord = (char *)malloc((strlen(nameOfRecord) + 1) * sizeof(char));
    //strcpy(newPair->identifier.nameOfRecord, nameOfRecord);
    strcpy(newPair->name, name);
    newPair->type = type;
    newPair->next = idlist;
    idlist = newPair;

    return newPair;
}

void add_identifier_to_hashtable(identifier_hashtable *h, char *name, char *type){
	int index;
	index = hash_function(identifier_list->name, h->size);
	if(h->table[index] == NULL){
		h->table[index] = (identifier_list *)malloc(sizeof(identifier_list));
        bzero(h->table[index],sizeof(identifier_list));
		h->table[index] = addIdentifier(h->table[index], name, type);
	}
	else{
		h->table[index] = addIdentifier(h->table[index], name, type);	
	}
}
//Used to insert Keyword and its token in the Hash Table i.e. Populating the hashtable
void add_function(function_hashtable* h, char* fname, identifier_list* ip_list, int flag){
	int index;
	index = hash_function(fname,h->size);
	if(h->table[index] == NULL){ //check this condition
        h->table[index] = (function_node *)malloc(sizeof(function_node));
        bzero(h->table[index],sizeof(function_node));
		h->table[index]->fname = fname;
		if(flag == input_par)
			h->table[index]->input_parameter_list = ip_list; 
		else if(flag == output_par)
			h->table[index]->output_parameter_list = ip_list;
	}
	else{
		function_node *new_entry, *temp;
		new_entry = (function_node*) malloc(sizeof(function_node));
        bzero(new_entry, sizeof(function_node));
		new_entry->fname=fname;
		if (flag == TK_FUNID){
			new_entry->next = h->table[index];
			h->table[index] = new_entry;
		}
		else if (flag == input_par){
			temp = h->table[index];
			while(temp!=NULL && strcmp(temp->fname,fname)!=0)
				temp = temp->next;
			if(temp == NULL)
				printf("Wrong function name specified as parameter");
			else{
				ip_list->next = temp->input_parameter_list;
				temp->input_parameter_list = ip_list;
			}

		}
		else if(flag == output_par){
			temp = h->table[index];
			while(temp!= NULL && strcmp(temp->fname,fname)!=0)
				temp = temp->next;
			if(temp == NULL)
				printf("Wrong function name specified as parameter");
			else{
				ip_list->next = temp->output_parameter_list;
				temp->output_parameter_list = ip_list;
			}
		}
	}
}

void search_function_hashtable(function_hashtable* h, char *fname){
	int index;
	function_node *pos;
	index = hash_function(fname, h->size);
	pos = h->table[index];
	while(pos!=NULL){
		if(strcmp(fname,pos->fname) == 0)
			return;
		pos = pos->next;
	}
	return;	

}

//Used to print the Hash Table
void print_function_hashtable(function_hashtable* h){
	int i=0;
	function_node* current_pointer;
	identifier_list* current_id;
	//printf("%d\n", h->size);
	for (i=0;i<h->size;i++){
		//printf("%d %s\n",i, h->table[i].value);
		current_pointer = h->table[i];
		printf("%d \n",i);
		while(current_pointer != NULL){
			printf("%s \nInput Paramters: ",current_pointer->fname);
			current_id = current_pointer->input_parameter_list;
			while(current_id != NULL){
				printf("%s %s ", current_id->name, current_id->type);
				current_id = current_id->next;
			}
			printf("\nOutput Parameters: ");
			current_id = current_pointer->output_parameter_list;
			while(current_id != NULL){
				printf("%s %s ", current_id->name, current_id->type);
				current_id = current_id->next;
			}
			printf("\n");
			current_pointer = current_pointer->next;
		}
	}
}
