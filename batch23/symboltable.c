/*
BATCH NUMBER: 23
PRABODH AGARWAL 2012B1A7801P
DEEPANSHU SINGH 2012B3A7593P


symboltable.c: contains definintions of functions related to creation of symbol tables,
               insetion in symbol tables and lookups in symbol tables.

Descriptions of the following functions given before their definition in this file:

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
void print_identifier_hashtable(identifier_hashtable *h, char *fname);
void print_function_wise_identifier_hashtable(function_wise_identifier_hashtable* h);
void compare_parameter_list_type(identifier_list* i1, identifier_list* i2, char* returnmsg);
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexerDef.h"
#include "parserDef.h"
#include "symboltableDef.h"
#include "symboltable.h"
#include "helper_functions.h"


//Creates the symbol table for storing function name , its input parameter list and its output parameter list
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


//Creates the symbol table for storing identifier name , its type and offset
identifier_hashtable* create_identifier_hashtable(int size){
	identifier_hashtable* h = NULL;
	int i=0;
	if((h = (identifier_hashtable*) malloc(sizeof(identifier_hashtable))) == NULL)
		return NULL;

	if((h->table = (identifier_list **) malloc(sizeof(identifier_list *) * size)) == NULL)
		return NULL;
	bzero(h->table, sizeof(identifier_list *)*size);
	h->size = size;
	h->offset = 0;
	return h;
}

/*
 *Creates the local identifier symbol table which is hashtable of function names 
 *where each function name in turn points to a hash table of type identifier_hashtable 
 *containing the functions local identifiers in the form of a hash table
*/
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

/*
 *Creates a new identifier list
 *Note: Following variables are of type identifier_list:
 		---> input_parameter_list of a function
 		---> output_parameter_list of a function
 		---> identifier_hashtable stores identifiers in the 
 		     form of identifier_list at hashed location
*/
identifier_list *create_identifier_list(char *name,char *type){
	identifier_list *idlist;
	idlist = add_element_to_list(NULL,name,type);
	return idlist;
}

/*
 *Adds an element to identifier list (no offset entry required in the identifier list)
*/
identifier_list *add_element_to_list(identifier_list *idlist, char *name, char *type){
    identifier_list *newPair = (identifier_list *)malloc(sizeof(identifier_list));
    bzero(newPair, sizeof(identifier_list));
    newPair->name = name;
    newPair->type = type;
    newPair->next = idlist;
    idlist = newPair;
    return newPair;
}

/* 
 *inserts a new (name, type,offset) pair to the beginning of identifier list
 *and returns a pointer to the new list
*/
identifier_list *addIdentifier(identifier_list *idlist, char *name, char *type, int offset){ 
    identifier_list *newPair = (identifier_list *)malloc(sizeof(identifier_list));
    bzero(newPair, sizeof(identifier_list));
    newPair->name = name;
    newPair->type = type;
    newPair->offset = offset;
    newPair->next = idlist;
    idlist = newPair;
    return newPair;
}


/*
 * Wrapper function which ultimately inserts identifier in the identifier_hashtable at the hashed location
*/
int add_identifier_to_identifierhashtable(identifier_hashtable *h, char *name, char *type,int offset){
	int index;
	index = hash_function(name, h->size);
	identifier_list* temp;
	temp = h->table[index];
	while(temp!=NULL){
		if(strcmp(name,temp->name) == 0)
			return -1;
		temp = temp->next;
	}
	h->table[index] = addIdentifier(h->table[index], name, type, offset);
}

//Adds an identifier to function's local identifier symbol table
int add_function_local_identifier_hashtable(function_wise_identifier_hashtable *h, char *fname, identifier_list *idlist, int offset){
	int index,flag=0;
	index = hash_function(fname, h->size); //Hash value computed by hash function for the given function name
	/*
	  * The if condition will be executed when no function has been hashed at this location 
	  * i.e. no entry for this function in the function's local identifier symbol table
	*/
	if(h->table[index] == NULL){ 
		h->table[index] = (function_identifier_node *)malloc(sizeof(function_identifier_node));
        bzero(h->table[index],sizeof(function_identifier_node));
		h->table[index]->fname = fname;
		h->table[index]->id_hashtable = create_identifier_hashtable(h->size); //check size parameter
		h->table[index]->offset = 0;
		h->table[index]->size = 0 ;
		flag = add_identifier_to_identifierhashtable(h->table[index]->id_hashtable, idlist->name, idlist->type,h->table[index]->offset);
		if(flag == -1)
			return flag;
		h->table[index]->offset += offset;
		h->table[index]->size += offset;
		return flag;
	}
	/*
		The else condition will be executed when a function already 
		exists at the location specified by hash value i.e. index
	*/
	else{
		function_identifier_node *new_entry, *temp;
		temp = h->table[index]; // temp now points to the location specified by hash value i.e. index
		while(temp != NULL && strcmp(temp->fname,fname)!=0){ // searches for fname at the hashed location
			temp = temp->next; // moves to next function in the chain at the hashed location
		}
		/*
		   The if condition will be executed when the given fname does not exist at the hashed location but other 
		   functions exist at that location. So create a function_identifier_node which will store the function name,
		   its identifier_hashtable and a pointer to the next function_identifier_node in case of chaining.
		*/
		if(temp == NULL){ 
			new_entry = (function_identifier_node*) malloc(sizeof(function_identifier_node)); //creates new function_identifier_node
        	bzero(new_entry, sizeof(function_identifier_node));
			new_entry->fname = fname;
			//create idenitifer_hashtable for the above function_identifier_node
			new_entry->id_hashtable = create_identifier_hashtable(h->size); //check size parameter
			new_entry->size = 0;
			new_entry->offset = 0;
			//inserts the given identifier in the local identifier symbol table of the function
			flag = add_identifier_to_identifierhashtable(new_entry->id_hashtable, idlist->name, idlist->type, new_entry->offset);
			if (flag == -1)
				return flag;
			
			new_entry->offset += offset;
			new_entry->size += offset;
			//puts the newly created function_identifier_node at the head of the hashed location i.e. chaining
			new_entry->next = h->table[index];
			h->table[index] = new_entry;
			return flag;
		}
		/*
			The else condition will be executed when the given function already exists at the hashed location.
			So just add the identifier to that function's local identifier hash table.
		*/
		else{
			flag = add_identifier_to_identifierhashtable(temp->id_hashtable, idlist->name, idlist->type, temp->offset);
			if (flag == -1){
				return flag;
			}	
			temp->offset += offset;
			temp->size += offset;
			
			return flag;
		}
	}	
}

//Used to insert the function name and its input and output parameter list in Function Symbol Table
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
/*
 * The functions returns the size of a record from the Record HashTable
*/
int get_record_size(function_wise_identifier_hashtable* h, char *fname){
	int index;
	function_identifier_node *pos;
	index = hash_function(fname, h->size);
	pos = h->table[index];
	while(pos!=NULL){
		if(strcmp(fname,pos->fname) == 0)
			return pos->size; // returns the size of the record
		pos = pos->next;// in case of chaining move on to the next record hashed at same location
	}
	return -1;
}

// Function to search an identifier in the global identifier hashtable
identifier_list* search_global_identifier(identifier_hashtable* h, char *name){
	int index;
	identifier_list* temp;
	index = hash_function(name, h->size);
	temp = h->table[index];
	while(temp!=NULL){
		if(strcmp(temp->name,name)==0) //identifier found
			return temp; // returns the identifier
		temp = temp->next; // in case of chaining move on to the next identifier hashed at same location
	}
	return NULL;
}

//Functions returns the fields of a record in the form of a list
identifier_list *get_record_fields(function_wise_identifier_hashtable* h, char *rname){
	int index,i;
	identifier_hashtable *idh;// idh refers to the hashtable which stores the fields of record rname
	identifier_list *idpos,*temp;
	temp = (identifier_list*) malloc(sizeof(identifier_list));
	bzero(temp, sizeof(identifier_list));
	idh = get_function_identifier_hashtable(h,rname); //gets the identifier hashtable for the record rname
	if(idh == NULL){
		return NULL;
	}
	else{
		for(i=0;i<h->size;i++){ //iterate to search for all fields hashed in the hashtable
			idpos = idh->table[i]; 
			while(idpos!=NULL){
				temp = add_element_to_list(temp,idpos->name,idpos->type); //adding each field to the list to be returned
				idpos = idpos->next; //in case of chaining move on to next field at the same location
			}
		}
		return temp;
	}
}

//Function which returns the identifier_hashtable of a given function/record
identifier_hashtable* get_function_identifier_hashtable(function_wise_identifier_hashtable* h, char *fname){
	int index;
	function_identifier_node *pos;
	index = hash_function(fname, h->size);
	pos = h->table[index];
	while(pos!=NULL){
		if(strcmp(fname,pos->fname) == 0)
			return pos->id_hashtable; //returns the identifier_hashtable of function/record fname
		pos = pos->next;
	}
	return NULL;
}

//Function used to search the function's local identifier hashtable for a given identifier
identifier_list* search_function_wise_identifier_hashtable(function_wise_identifier_hashtable* h, char *fname, char *iname){
	int index;
	identifier_hashtable *idh;
	identifier_list *idpos;
	idh = get_function_identifier_hashtable(h,fname); //get the identifier hashtable for the function
	if(idh == NULL){
		return NULL; //identifier hashtable for given function fname does not exist
	}
	else{
		index = hash_function(iname, idh->size); //find the location of iname in the identifier hashtable
		idpos = idh->table[index];
		while(idpos!=NULL){
			if(strcmp(iname,idpos->name) == 0) //search for iname at the given location
				return idpos; //if found return the identifier
			idpos = idpos->next;
		}
		if(idpos == NULL){
			return NULL; //iname not found return null
		}
	}
}

//Function used to search the function hashtable for the given pararmeter fname
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

//Function returns the input parameter list of function fname (specified as parameter). If fname not found, returns NULL
identifier_list* get_input_parameter_list(function_hashtable* h, char *fname){
	function_node* flag;
	flag = search_function_hashtable(h,fname);
	if(flag != NULL)
		return flag->input_parameter_list;
	else
		return NULL;
}

//Function returns the output parameter list of function fname (specified as parameter). If fname not found, returns NULL
identifier_list* get_output_parameter_list(function_hashtable* h, char *fname){
	function_node* flag;
	flag = search_function_hashtable(h,fname);
	if(flag != NULL)
		return flag->output_parameter_list;
	else
		return NULL;
}

//Used to print the function hashtable i.e. function name, input parameter list, output parameter list.
void print_function_hashtable(function_hashtable* h){
	int i=0;
	function_node* current_pointer;
	identifier_list* current_id;
	for (i=0;i<h->size;i++){
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

void print_list(identifier_list* list, char *type){
	if(list == NULL)
		return ;
	else{
		print_list(list->next, type);
		if(list->type == NULL)
			return;
		else{
			strcat(type,list->type);
			strcat(type," x ");
			return ;
		}
	}
}
//Function to print the Local Identifier Table of the given function fname in the specified format
void print_identifier_hashtable(identifier_hashtable *h,char *fname, function_wise_identifier_hashtable* record_table){
	int i, offset=0;
	identifier_list* current_pointer,*record_fields;
	char type[10000];
	for (i=0;i<h->size;i++){
		current_pointer = h->table[i];
		if(fname == NULL){
			fname = (char *)malloc(sizeof(char)*25);
			strcpy(fname,"Global");
		}
		while(current_pointer != NULL){
			printf("%23s", current_pointer->name);
			if(current_pointer->type[0]=='#'){
				record_fields = get_record_fields(record_table,current_pointer->type);
				strcat(type,current_pointer->type);
				strcat(type," (");
				print_list(record_fields,type);
				type[strlen(type)-3]=')';
				type[strlen(type)-2]='\0';
				printf("%39s",type);
				strcpy(type,"");
			}
			else
				printf("%39s", current_pointer->type);
			printf("%21s", fname);
			if(strcmp(fname,"Global")==0)
				printf("              -\n");	
			else
				printf("%15d\n", current_pointer->offset);
			current_pointer = current_pointer->next;
		}
	}
}

//Wrapper function which helps print the local identifer table of all functions in the specified format
void print_function_wise_identifier_hashtable(function_wise_identifier_hashtable* h, function_wise_identifier_hashtable* record_table){
	int i;
	function_identifier_node* current_pointer;
	identifier_hashtable* current_identifier_hashtable;
	printf("                 Lexeme                                   Type                Scope         Offset \n");
	for (i=0;i<h->size;i++){//iterating through all functions' identifier tables and printing them one by one
		current_pointer = h->table[i];

		while(current_pointer != NULL){
			current_identifier_hashtable = current_pointer->id_hashtable;
			print_identifier_hashtable(current_identifier_hashtable, current_pointer->fname, record_table); //print the identifer table of fname
			current_pointer = current_pointer->next;// in case of chaining move on to the next function
		}
	}
}

/*
 *Helper function to compare two identifier lists in terms of the type and number of elements
 *Used in semantic analysis phase mainly to check the semantics of return statement
*/
void compare_parameter_list_type(identifier_list* i1, identifier_list* i2, char* returnmsg){
	identifier_list* temp1, *temp2;
	temp1 = i1;
	temp2 = i2;
	
	while(temp1 != NULL && temp2 != NULL && strcmp(temp1->type,temp2->type)==0){
		temp1 = temp1->next;
		temp2 = temp2->next;
	}
	if(temp1 == NULL && temp2 == NULL) //type of each element matches and the number of elements also match
		sprintf(returnmsg, "OK");
	else if(temp1 !=NULL && temp2 != NULL){ //Type of some element does not matchh with corresponding element's type in the other list
		if(strcmp(temp1->type,"err") == 0 || strcmp(temp2->type,"err") == 0)
			sprintf(returnmsg, "OK");
		else
			sprintf(returnmsg,"The type <%s> of variable <%s> returned does not match with the type <%s> of the formal output parameter <%s>",
			i1->type,i1->name,i2->type,i2->name);
	}
	else{ //Length of both lists are not same
		sprintf(returnmsg,"N");
	} 
}