/*
   BATCH NUMBER: 23
   PRABODH AGARWAL 2012B1A7801P
   DEEPANSHU SINGH 2012B3A7593P

   populateHashTable.c

*/
#include <stdio.h>
#include <stdlib.h>
#include "lexerDef.h"
#include "parserDef.h"
#include "symboltableDef.h"
#include "symboltable.h"
#include "populateHashTable.h"
#include <string.h>

/*
 * there exist four symbol tables in total.
 * for local identifiers
 * for global identifiers
 * for maintaining record definitions
 * for maintaining input/output parameters of functions
 */

extern short semantic;
//called to populate global identifiers symbol table the record data structure
//information in respective symbol tables
void populateGlobalRecords(parseTree *p, char *rname, int state)
{
    int i, nochildren = p->nochildren, offset;
    identifier_list *id;

    for (i = 0; i < nochildren; i++) 
    {
        offset = 4;
        //populate hashtable for global identifier declarations
        if(p->nonterm == declarations)
            state = declarations;
        if(p->nonterm == declaration && state == declarations)
        {
            if(p->children[i].term.tokenClass == TK_ID)
            {
                id = (identifier_list *)malloc(sizeof(identifier_list));
                id->name = p->children[i].term.lexeme;
                if(p->children[i-2].children[0].nonterm == constructedDatatype)
                {
                    id->type = p->children[i-2].children[0].children[1].term.lexeme;
                    offset = get_record_size(record, id->type);
                }
                else
                {
                    id->type = p->children[i-2].children[0].children[0].term.lexeme;
                    if(p->children[i-2].children[0].children[0].term.tokenClass == TK_INT)
                        offset = 2;
                }
                id->next = NULL;
                if(p->children[i+1].children[0].term.tokenClass!=eps)
                {
                    if(add_identifier_to_identifierhashtable(global, id->name, id->type, global->offset)==-1)
                    {
                        printf("error: %llu Variable %s is a global variable and cannot be declared again\n",p->children[i].term.line_num, p->children[i].term.lexeme);
                        semantic = 0;
                    }
                }
            }
        }

        //fill record defintions' hashtable
        if(p->nonterm == typeDefinition)
        {
            rname = p->children[1].term.lexeme;
            state = typeDefinition;
        }
        else if(p->nonterm == fieldDefinition && state == typeDefinition)
        {
            //fieldDefinition = TK_TYPE primitiveDatatype TK_COLON TK_FIELDID TK_SEM
            if(p->children[i].term.tokenClass == TK_FIELDID)
            {
                id = (identifier_list *)malloc(sizeof(identifier_list));
                id->name = p->children[i].term.lexeme;
                id->type = p->children[i-2].children[0].term.lexeme;
                id->next = NULL;
                if(p->children[i-2].children[0].term.tokenClass == TK_INT)
                    offset = 2;
                if(add_function_local_identifier_hashtable(record, rname, id, offset)==-1)
                {
                    printf("error: %llu Field identifier %s declared multiple times\n",p->children[i].term.line_num, p->children[i].term.lexeme);
                    semantic = 0;
                }
            }
            continue;
        }
        populateGlobalRecords(p->children+i, rname, state);
    }
}

/*
 * this function is used to populate symbol table with identifiers in function
 * scope and also maintains information of function parameters
 * fname is used to maintain name of current function the parsetree is in. this
 * is used to maintain scope information for identifier declaration. To maintain
 * local scope, every function has its own hashtable and local identifiers hash
 * to this table. These individual function tables are in turn hashed to one
 * common structure. A document has been submitted to detail how we have
 * constructed our symbol tables. A separate symbol table exists to hold info
 * regarding input and output parameters of the functions. This is used for type
 * checking in function call statements.
 */
void populateFunctionST(parseTree *p, char *fname, int state)
{
    int i, nochildren = p->nochildren, offset = 4;
    identifier_list *id;
    for(i=0;i<nochildren;i++)
    {
        offset = 4; //offset for real identifier declarations
        if(p->term.tokenClass == TK_SEM)
            state = -1;

        if(p->nonterm == mainFunctions)
        {
            fname = (char *)malloc(sizeof(char)*6);
            strcpy(fname,"_main\0");
        }

        if(p->nonterm == function)
        {
            //capture function name
            fname = p->children[0].term.lexeme;
            if(state!=function)
                if(add_function(funcs, fname, NULL, TK_FUNID)==-1)
                {
                    //initialize symbol table for the function
                    printf("error: %llu Function overloading not allowed\n",p->children[0].term.line_num);
                    semantic = 0;
                    return;
                }
            state = function;
        }
        else if(p->nonterm == input_par)
            state = input_par;
        else if(p->nonterm == output_par)
            state = output_par;
        else if(p->nonterm == parameter_list && state!=-1)
        {
            //populate symbol table with input and output parameter information
            //and ensure given parameter hasn't been declared as global
            if(p->children[i].term.tokenClass == TK_ID)
            {
                id = (identifier_list *)malloc(sizeof(identifier_list));
                id->name = p->children[i].term.lexeme;
                //record type identifier
                if(p->children[i-1].children[0].nonterm == constructedDatatype)
                {
                    id->type = p->children[i-1].children[0].children[1].term.lexeme;
                    offset = get_record_size(record, id->type);
                }
                //non-record type identifier
                else
                {
                    id->type = p->children[i-1].children[0].children[0].term.lexeme;
                    if(p->children[i-1].children[0].children[0].term.tokenClass == TK_INT)
                        offset = 2;
                }
                id->next = NULL;
                add_function(funcs,fname,id,state);
                if(search_global_identifier(global, id->name)!=NULL)
                {
                    printf("error: %llu Identifier %s declared global elsewhere\n",p->children[i].term.line_num, p->children[i].term.lexeme);
                    semantic = 0;
                }
                else if(add_function_local_identifier_hashtable(local, fname, id, offset)==-1)
                {
                    printf("error: %llu Identifier %s declared multiple times\n",p->children[i].term.line_num, p->children[i].term.lexeme);
                    semantic = 0;
                }
            }
            else if(!(!p->children[i].isTerminal && p->children[i].nonterm == remaining_list))
                continue;
        }

        if(p->nonterm == declarations)
            state = declaration;
        if(p->nonterm == declaration && state!=-1)
        {
            //handle local identifier declarations and ensure multiple
            //declarations don't happen and don't collide with global declarations
            if(p->children[i].term.tokenClass == TK_ID)
            {
                id = (identifier_list *)malloc(sizeof(identifier_list));
                id->name = p->children[i].term.lexeme;
                if(p->children[i-2].children[0].nonterm == constructedDatatype)
                {
                    id->type = p->children[i-2].children[0].children[1].term.lexeme;
                    offset = get_record_size(record, id->type);
                }
                else
                {
                    id->type = p->children[i-2].children[0].children[0].term.lexeme;
                    if(p->children[i-2].children[0].children[0].term.tokenClass == TK_INT)
                        offset = 2;
                }
                id->next = NULL;
                if(p->children[i+1].children[0].term.tokenClass==eps)
                {
                    if(search_global_identifier(global, id->name)!=NULL)
                    {
                        printf("error: %llu Identifier %s declared global elsewhere\n",p->children[i].term.line_num, p->children[i].term.lexeme);
                        semantic = 0;
                    }
                    else if(add_function_local_identifier_hashtable(local, fname, id, offset)==-1)
                    {
                        printf("error: %llu Identifier %s declared multiple times\n",p->children[i].term.line_num, p->children[i].term.lexeme);
                        semantic = 0;
                    }
                }
            }
            continue;
        }

        if(p->children[i].term.tokenClass == TK_FUNID)
        {
            if(search_function_hashtable(funcs, p->children[i].term.lexeme)==NULL)
            {
                printf("error: %llu Function %s not defined\n",p->children[i].term.line_num, p->children[i].term.lexeme);
                semantic = 0;
                return;
            }
        }
        populateFunctionST(p->children+i, fname, state);
    }
}

//this function is used repeatedly during function call typechecking to populate
//a list with input or output parameters in function call statements. The
//returned list is matched with that from the symbol table for type checking
//purposes
identifier_list * getParams(parseTree *p, identifier_list *list, char *func, int start)
{
    int i;
    char *type;
    identifier_list *id, *id1;

    for(i=start;i<p->nochildren;i++) 
    {
        if(p->children[i].isTerminal)
        {
            if(p->children[i].term.tokenClass == TK_ID)
            {
                id = search_function_wise_identifier_hashtable(local, func, p->children[i].term.lexeme);
                id1 = search_global_identifier(global, p->children[i].term.lexeme);
                if(id==NULL && id1 == NULL)
                {
                    printf("error: %llu Variable %s isn't declared\n",p->children[i].term.line_num, p->children[i].term.lexeme);
                    type = (char *)malloc(sizeof(char)*4);
                    sprintf(type, "err");
                    semantic = 0;
                }
                else if(id!=NULL && id1!=NULL)
                {
                    printf("error: %llu %s declared in both global and local\n", p->children[i].term.line_num, p->children[i].term.lexeme);
                    semantic = 0;
                }
                else if(id == NULL)
                    id = id1;

                if(id!=NULL)
                    type = id->type;
                list = add_element_to_list(list, p->children[i].term.lexeme, type);
            }
        }
        else
            list = getParams(p->children+i, list, func, 0);
    }
    return list;
}
