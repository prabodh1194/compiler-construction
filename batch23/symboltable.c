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

function_wise_identifier_hashtable* create_function_local_identifier_hashtable(int size){
	function_wise_identifier_hashtable* h = NULL;
	int i=0;
	if((h = (function_wise_identifier_hashtable*) malloc(sizeof(function_wise_identifier_hashtable))) == NULL)
		return NULL;

	if((h->table = (function_identifier_node **) malloc(sizeof(function_identifier_node *) * size)) == NULL)
		return NULL;
	bzero(h->table, sizeof(function_identifier_node *)*size);
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
    //newPair->name = (char *)malloc((strlen(name) + 1) * sizeof(char));
    //newPair->identifier.nameOfRecord = (char *)malloc((strlen(nameOfRecord) + 1) * sizeof(char));
    //strcpy(newPair->identifier.nameOfRecord, nameOfRecord);
    newPair->name = name;
    newPair->type = type;
    newPair->next = idlist;
    idlist = newPair;

    return newPair;
}

void add_identifier_to_identifierhashtable(identifier_hashtable *h, char *name, char *type){
	int index;
	index = hash_function(name, h->size);
	h->table[index] = addIdentifier(h->table[index], name, type);
	
}

void add_function_local_identifier_hashtable(function_wise_identifier_hashtable *h, char *fname, identifier_list *idlist){
	int index;
	index = hash_function(fname, h->size);
	if(h->table[index] == NULL){
		h->table[index] = (function_identifier_node *)malloc(sizeof(function_identifier_node));
        bzero(h->table[index],sizeof(function_identifier_node));
		h->table[index]->fname = fname;
		h->table[index]->id_hashtable = create_identifier_hashtable(h->size); //check size parameter
		add_identifier_to_identifierhashtable(h->table[index]->id_hashtable, idlist->name, idlist->type);
	}
	else{
		function_identifier_node *new_entry, *temp;
		temp = h->table[index];
		while(temp != NULL && strcmp(temp->fname,fname)!=0){
			temp = temp->next;
		}
		if(temp == NULL){
			new_entry = (function_identifier_node*) malloc(sizeof(function_identifier_node));
        	bzero(new_entry, sizeof(function_identifier_node));
			new_entry->fname=fname;
			new_entry->id_hashtable = create_identifier_hashtable(h->size); //check size parameter
			add_identifier_to_identifierhashtable(new_entry->id_hashtable, idlist->name, idlist->type);
			new_entry->next = h->table[index];
			h->table[index] = new_entry;
		}
		else
			add_identifier_to_identifierhashtable(temp->id_hashtable, idlist->name, idlist->type);
	}	
}

//Used to insert Keyword and its token in the Hash Table i.e. Populating the hashtable
int add_function(function_hashtable* h, char* fname, identifier_list* ip_list, int flag){
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
		return 1;
	}
	else{
		function_node *new_entry, *temp;
		new_entry = (function_node*) malloc(sizeof(function_node));
        bzero(new_entry, sizeof(function_node));
		new_entry->fname=fname;
		if (flag == TK_FUNID){
			temp = h->table[index];
			while(temp!=NULL && strcmp(temp->fname,fname)!=0)
				temp = temp->next;
			if(temp == NULL){
				new_entry->next = h->table[index];
				h->table[index] = new_entry;
				return 1;
			}
			else
				return -1;
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
		return 1;
	}
}

identifier_hashtable* get_function_identifier_hashtable(function_wise_identifier_hashtable* h, char *fname){
	int index;
	function_identifier_node *pos;
	index = hash_function(fname, h->size);
	pos = h->table[index];
	while(pos!=NULL){
		if(strcmp(fname,pos->fname) == 0)
			return pos->id_hashtable;
		pos = pos->next;
	}
	return NULL;
}

identifier_list* search_function_wise_identifier_hashtable(function_wise_identifier_hashtable* h, char *fname, char *iname){
	int index;
	identifier_hashtable *idh;
	identifier_list *idpos;
	idh = get_function_identifier_hashtable(h,fname);
	if(idh == NULL){
		printf("Function's identifier hashtable not found. Check code for error");
		return NULL;
	}
	else{
		index = hash_function(iname, idh->size);
		idpos = idh->table[index];
		while(idpos!=NULL){
			if(strcmp(iname,idpos->name) == 0)
				return idpos;
			idpos = idpos->next;
		}
		if(idpos == NULL){
			printf("Identifier not found in the identifier hash table of function %s",fname);
			return NULL;
		}
	}
}

function_node* search_function_hashtable(function_hashtable* h, char *fname){
	int index;
	function_node *pos;
	index = hash_function(fname, h->size);
	pos = h->table[index];
	while(pos!=NULL){
		if(strcmp(fname,pos->fname) == 0)
			return pos;
		pos = pos->next;
	}
	return NULL;	

}

identifier_list* get_input_parameter_list(function_hashtable* h, char *fname){
	function_node* flag;
	flag = search_function_hashtable(h,fname);
	if(flag != NULL)
		return flag->input_parameter_list;
	else
		return NULL;
}

identifier_list* get_output_parameter_list(function_hashtable* h, char *fname){
	function_node* flag;
	flag = search_function_hashtable(h,fname);
	if(flag != NULL)
		return flag->output_parameter_list;
	else
		return NULL;
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

void print_identifier_hashtable(identifier_hashtable *h){
	int i;
	identifier_list* current_pointer;
	for (i=0;i<h->size;i++){
		//printf("%d %s\n",i, h->table[i].value);
		current_pointer = h->table[i];
		printf("\t\t%d \n",i);
		while(current_pointer != NULL){
			printf("\t\tIdentifier Name: %s", current_pointer->name);
			printf("\t\tType: %s\n", current_pointer->type);
			current_pointer = current_pointer->next;
		}
	}
}

void print_function_wise_identifier_hashtable(function_wise_identifier_hashtable* h){
	int i;
	function_identifier_node* current_pointer;
	identifier_hashtable* current_identifier_hashtable;
	//printf("%d\n", h->size);
	for (i=0;i<h->size;i++){
		//printf("%d %s\n",i, h->table[i].value);
		current_pointer = h->table[i];
		printf("%d \n",i);
		while(current_pointer != NULL){
			printf("%s \nCurrent Function Scope: ",current_pointer->fname);
			current_identifier_hashtable = current_pointer->id_hashtable;
			print_identifier_hashtable(current_identifier_hashtable);
			current_pointer = current_pointer->next;
		}
	}
}