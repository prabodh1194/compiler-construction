#include <stdio.h>
#include <stdlib.h>
#include "lexerDef.h"
#include "parserDef.h"
#include "symboltableDef.h"
#include "symboltable.h"

void populateFunctionST(parseTree *p, function_hashtable *funcs, char *fname, int state)
{
    int i, nochildren = p->nochildren;
    identifier_list *id;
    for(i=0;i<nochildren;i++)
    {
        if(p->term.tokenClass == TK_SEM)
            state = -1;

        if(p->nonterm == function)
        {
            fname = p->children[0].term.lexeme;
            if(state!=function)
                add_function(funcs, fname, NULL, TK_FUNID);
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
                    id->type = p->children[i-1].children[0].children[1].term.lexeme;
                else
                    id->type = p->children[i-1].children[0].children[0].term.lexeme;
                id->next = NULL;
                add_function(funcs,fname,id,state);
            }
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
                if(p->children[i-1].children[0].nonterm == constructedDatatype)
                    id->type = p->children[i-1].children[0].children[1].term.lexeme;
                else
                    id->type = p->children[i-1].children[0].children[0].term.lexeme;
                id->next = NULL;
                if(p->children[i+1].children[0].term.tokenClass==eps)
                {
                    //local
                }
                else
                {
                    //global
                }
            }
            continue;
        }

        if(p->nonterm == typeDefinition)
        {
            fname = p->children[1].term.lexeme;
            state = TK_RECORD;
        }
        else if(p->nonterm == fieldDefinition && state!=-1)
        {
            if(p->children[i].term.tokenClass == TK_FIELDID)
            {
            }
        }
        populateFunctionST(p->children+i, funcs, fname, state);
    }
}
