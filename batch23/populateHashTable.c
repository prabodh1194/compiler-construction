#include <stdio.h>
#include <stdlib.h>
#include "lexerDef.h"
#include "parserDef.h"
#include "symboltableDef.h"
#include "symboltable.h"
#include "populateHashTable.h"
#include <string.h>

void populateGlobalRecords(parseTree *p, char *rname, int state)
{
    int i, nochildren = p->nochildren, offset;
    identifier_list *id;

    for (i = 0; i < nochildren; i++) 
    {
        offset = 4;
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
                        printf("error: %llu Variable %s is a global variable and cannot be declared again\n",p->children[i].term.line_num, p->children[i].term.lexeme);
                }
            }
        }

        if(p->nonterm == typeDefinition)
        {
            rname = p->children[1].term.lexeme;
            state = typeDefinition;
        }
        else if(p->nonterm == fieldDefinition && state == typeDefinition)
        {
            if(p->children[i].term.tokenClass == TK_FIELDID)
            {
                id = (identifier_list *)malloc(sizeof(identifier_list));
                id->name = p->children[i].term.lexeme;
                id->type = p->children[i-2].children[0].term.lexeme;
                id->next = NULL;
                if(p->children[i-2].children[0].term.tokenClass == TK_INT)
                    offset = 2;
                if(add_function_local_identifier_hashtable(record, rname, id, offset)==-1)
                    printf("error: %llu Field identifier %s declared multiple times\n",p->children[i].term.line_num, p->children[i].term.lexeme);

            }
            continue;
        }
        populateGlobalRecords(p->children+i, rname, state);
    }
}

void populateFunctionST(parseTree *p, char *fname, int state)
{
    int i, nochildren = p->nochildren, offset = 4;
    identifier_list *id;
    for(i=0;i<nochildren;i++)
    {
        offset = 4;
        if(p->term.tokenClass == TK_SEM)
            state = -1;

        if(p->nonterm == mainFunctions)
        {
            fname = (char *)malloc(sizeof(char)*6);
            strcpy(fname,"_main\0");
        }

        if(p->nonterm == function)
        {
            fname = p->children[0].term.lexeme;
            if(state!=function)
                if(add_function(funcs, fname, NULL, TK_FUNID)==-1)
                {
                    printf("error: %llu Function overloading not allowed\n",p->children[0].term.line_num);
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
            if(p->children[i].term.tokenClass == TK_ID)
            {
                id = (identifier_list *)malloc(sizeof(identifier_list));
                id->name = p->children[i].term.lexeme;
                if(p->children[i-1].children[0].nonterm == constructedDatatype)
                {
                    id->type = p->children[i-1].children[0].children[1].term.lexeme;
                    offset = get_record_size(record, id->type);
                }
                else
                {
                    id->type = p->children[i-1].children[0].children[0].term.lexeme;
                    if(p->children[i-1].children[0].children[0].term.tokenClass == TK_INT)
                        offset = 2;
                }
                id->next = NULL;
                add_function(funcs,fname,id,state);
                if(search_global_identifier(global, id->name)!=NULL)
                    printf("error: %llu Identifier %s declared global elsewhere\n",p->children[i].term.line_num, p->children[i].term.lexeme);
                else if(add_function_local_identifier_hashtable(local, fname, id, offset)==-1)
                    printf("error: %llu Identifier %s declared multiple times\n",p->children[i].term.line_num, p->children[i].term.lexeme);
            }
            else if(!(!p->children[i].isTerminal && p->children[i].nonterm == remaining_list))
                continue;
        }

        if(p->nonterm == declarations)
            state = declaration;
        if(p->nonterm == declaration && state!=-1)
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
                if(p->children[i+1].children[0].term.tokenClass==eps)
                {
                if(search_global_identifier(global, id->name)!=NULL)
                    printf("error: %llu Identifier %s declared global elsewhere\n",p->children[i].term.line_num, p->children[i].term.lexeme);
                else if(add_function_local_identifier_hashtable(local, fname, id, offset)==-1)
                    printf("error: %llu Identifier %s declared multiple times\n",p->children[i].term.line_num, p->children[i].term.lexeme);
                }
            }
            continue;
        }

        if(p->children[i].term.tokenClass == TK_FUNID)
        {
            if(search_function_hashtable(funcs, p->children[i].term.lexeme)==NULL)
            {
                printf("error: %llu Function %s not defined\n",p->children[i].term.line_num, p->children[i].term.lexeme);
                return;
            }
        }
        populateFunctionST(p->children+i, fname, state);
    }
}

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
                }
                else if(id!=NULL && id1!=NULL)
                {
                    printf("error: %llu %s declared in both global and local\n", p->children[i].term.line_num, p->children[i].term.lexeme);
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
