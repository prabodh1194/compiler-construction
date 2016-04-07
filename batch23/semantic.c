#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parserDef.h"
#include "parser.h"
#include "helper_functions.h"
#include <string.h>

void checkType(parseTree *p, int state, char *id)
{
    int i, nochildren;
    nochildren = p->nochildren;

    for(i=0; i<nochildren; i++)
    {
        if(!p->children[i].isTerminal)
        {
            switch(p->children[i].nonterm)
            {
                case function:
                    state = function;
                    break;
                case input_par:
                    state = input_par;
                    break;
                case output_par:
                    state = output_par;
                    break;
                case returnStmt:
                    state = returnStmt;
                    list1 = outputparameterlist(id);
                    getParams(p, list2, 0);
                    compare(list1, list2);
                    break;
                case funCallStmt:
                    state = funCallStmt;
                    id = p->children[0].children[0].term.lexeme;
                    list1 = inputparameterlist(id);
                    getParams(p,list2, 2);
                    compare(list1, list2);
                    break;
                default:
                    break;
            }
        }
        else
        {
            switch(state)
            {
                case function:
                    lookupFUNID(state, p->term.lexeme);
                    strcpy(id, p->term.lexeme);
                    break;

                case input_par || output_par:
                    if(p->term.tokenClass)
                        lookupID(state, p->term.lexeme);
                    break;
            }
        }
        checkType(p->children+i, state, id);
    }
}

getParams(parseTree *p, Identifier *list, int start)
{
    int i;
    for(i=start;i<p->nochildren;i++) 
    {
        if(p->children[i].isTerminal)
        {
            if(p->children[i].term.tokenClass == TK_ID)
                addtolist(list, p->children[i].lexeme, p->children[i].tokenClass);
        }
        else
            getParams(p->children[i], list, 0);
    }
}
