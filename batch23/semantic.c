/*
   BATCH NUMBER: 23
   PRABODH AGARWAL 2012B1A7801P
   DEEPANSHU SINGH 2012B3A7593P

   codegen.h

*/
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parserDef.h"
#include "helper_functions.h"
#include "parser.h"
#include "symboltableDef.h"
#include "symboltable.h"
#include "populateHashTable.h"

/*
 * These functions are used to construct an AST. At the same time, semantic
 * analysis and type checking is performed.
 * Part of semantic analysis which involves identifier declaration is
 * performed during symbol table population. Type checking and other semantic
 * analysis is performed here along with AST creation.
 *
 * Type checking is performed in following manner.
 * - A terminal ID or NUM/RNUM tells its type to its parent node.
 * - If this is a first occurence, the parent initializes it's type to this
 *   type.
 * - However, if the parent already has a type associated with it, then it
 *   compares the incomeing type with the one it has. In case of mismatch, error
 *   is indicated.
 *
 * Parse tree to AST conversion is done by removing those terminals who won't
 * convey a useful meaning. Also, those non-terminal nodes having one child are
 * removed and the parent of such node now becomes parent of this node's child.
 * This is in accordance with our semantic rules submission.
 */

//construct an AST by removing useless nodes from the parse tree

int astSize = 0; //count of nodes in AST
short semantic  = 1; //successful semantic
//used to maintain detect if identifiers defined in a while boolean expression
//get updated or not
identifier_list *whileList = NULL;
short whileState = 0, isUpdate = 0;

//semantic analysis and type checking have been performed during AST creation

/*
* char *name is used to maintain state of the program and is used for type
* checking and sumbol table look ups as it carries the name of the function, the AST/parse tree is
* currently in.
*/
void createAbstractSyntaxTree(parseTree *p, astree *ast, char *name)
{
    /*
     * Certain terminals are defined as useless as their purpose is already
     * captured in the parse tree. These terminals have been used to capture
     * precedence or to indicate line ends and so on and serve no purpose in asm
     * code generated. These terminal nodes are trimmed off to reduce the
     * abstract syntax tree. Also, certain intermediate non-terminal nodes,
     * those having singular child nodes are removed. In this manner, Abstract
     * Syntax Tree is compressed from the parse tree.
     */
    
    int i, j, usefulChildrenCount = 0; //useful children count keeps track of nodes that'll be useful to the AST
    terminalId type;
    identifier_list *id, *id1;
    char *typeS;
    char message[200]; //used to capture error messages

    //check for useless terminal nodes
    for(i = 0; i < p->nochildren; i++)
        // all nonterminals are useful
        if(!p->children[i].isTerminal || isUseful(p->children[i].term.tokenClass))
            usefulChildrenCount++;
    if(usefulChildrenCount == 0)
        return;

    ast->nochildren = usefulChildrenCount;
    ast->children = (astree *)malloc(ast->nochildren * sizeof(astree));
    bzero(ast->children,usefulChildrenCount*sizeof(astree));
    astSize+=usefulChildrenCount;

    i = 0, j = 0;
    while(i < p->nochildren) {
        if(p->children[i].isTerminal) {
            if(p->children[j].term.tokenClass == TK_MAIN)
                name = p->children[j].term.lexeme;

            if(isUseful(p->children[i].term.tokenClass)) {
                // copy ith node of p to jth node of ast
                ast->children[j].isTerminal = 1;
                ast->children[j].term.tokenClass = p->children[i].term.tokenClass;
                strcpy(ast->children[j].term.lexeme, p->children[i].term.lexeme);
                ast->children[j].term.line_num = p->children[i].term.line_num;

                if(ast->children[j].term.tokenClass == TK_FUNID && ast->nonterm == function)
                    name = ast->children[j].term.lexeme;
                /*
                 * The name of function the parse tree is currently in, all
                 * subsequent IDs are scoped in this function unless specified
                 * otherwise
                 */

                if((ast->children[j].term.tokenClass == TK_ID || ast->children[j].term.tokenClass == TK_FIELDID) && ast->nonterm!=parameter_list && ast->nonterm!=declaration && ast->nonterm!=funCallStmt && ast->nonterm!=fieldDefinition)
                {
                    /*
                     * Find out if TK_ID used in <stmt> has actually been
                     * declared in function scope or globally.
                     */
                    ast->line_num = ast->children[j].term.line_num;
                    if(ast->children[j].term.tokenClass == TK_ID)
                    {
                        id1 = search_global_identifier(global, ast->children[j].term.lexeme);
                        id = search_function_wise_identifier_hashtable(local, name, ast->children[j].term.lexeme);

                        if(id1!=NULL && id!=NULL)
                        {
                            printf("error: %llu %s declared in both global and local\n", ast->children[j].term.line_num, ast->children[j].term.lexeme);
                            semantic = 0;
                            return;
                        }
                        if(id == NULL)
                            id = id1;
                        //make a list of all ids in while boolean expression
                        if(whileState == 1 && id!=NULL)
                            whileList = addIdentifier(whileList, id->name, id->type, 0);
                        /* 
                         * for ids occuring on LHS of assignment stmts, check if
                         * they occur in while boolean expressions
                         */
                        if(i == 0 && !isUpdate && whileList!=NULL && p->nonterm==singleOrRecId && id!=NULL)
                        {
                            identifier_list *temp;
                            temp = whileList;
                            while(temp!=NULL)
                            {
                                if(strcmp(temp->name, id->name)==0)
                                {
                                    isUpdate = 1;
                                    break;
                                }
                                temp = temp->next;
                            }
                        }
                    }
                    else
                        id = search_function_wise_identifier_hashtable(record, name, ast->children[j].term.lexeme); /* Check if given field id actually exists*/

                    if(id == NULL)
                    {
                        printf("error: %llu Variable %s isn't declared\n", ast->children[j].term.line_num, ast->children[j].term.lexeme);
                        semantic = 0;
                        return;
                    }

                    typeS = id->type;
                    //capture name of current record type of given id, given field id is declared in
                    if(p->children[i].term.tokenClass!=TK_FIELDID && p->children[i+1].nochildren!=0 && p->children[i+1].children[0].term.tokenClass!= eps && p->nonterm!=idList)
                        name = typeS;
                    else
                    {
                        /*
                        * capture type of IDs and propogate to parent nodes with
                        * subsequent type checks with sibling nodes 
                        */
                        if(strcmp(typeS,"int")==0)
                            type = TK_INT;
                        else if(strcmp(typeS,"real")==0)
                            type = TK_REAL;
                        else if(typeS[0]=='#')
                            type = TK_RECORD;

                        if(ast->type == -1 || ast->type == type)
                            ast->type = type;
                        else
                            ast->type = TK_ERROR;
                    }
                }
                else if(ast->children[j].term.tokenClass == TK_NUM && ast->nonterm!=funCallStmt)
                {
                    /*
                     * capture type of IDs and propogate to parent nodes with
                     * subsequent type checks with sibling nodes 
                     */
                    ast->line_num = ast->children[j].term.line_num;
                    if(ast->type == -1 || ast->type == TK_INT)
                        ast->type = TK_INT;
                    else
                        ast->type == TK_ERROR;
                }
                else if(ast->children[j].term.tokenClass == TK_RNUM && ast->nonterm!=funCallStmt)
                {
                    /*
                     * capture type of IDs and propogate to parent nodes with
                     * subsequent type checks with sibling nodes 
                     */
                    ast->line_num = ast->children[j].term.line_num;
                    if(ast->type == -1 || ast->type == TK_REAL)
                        ast->type = TK_REAL;
                    else
                        ast->type == TK_ERROR;
                }
                else if(ast->children[j].term.tokenClass == TK_FUNID && ast->nonterm == funCallStmt)
                {
                    if(strcmp(ast->children[j].term.lexeme, name)==0)
                    {
                        printf("error: %llu %s is a recursive function call\n", ast->children[j].term.line_num, name);
                        semantic = 0;
                        return;
                    }
                    /*
                     * for function call stmts, check if input/output parameters
                     * match types and number of arguments by fetching from the
                     * symbol table and making a list of existing parameters
                     */
                    id = NULL;
                    //make a list of ids defined as output of funtion call
                    id = getParams(p->children, id, name, 0);
                    bzero(message, 200);
                    compare_parameter_list_type(id, get_output_parameter_list(funcs, ast->children[j].term.lexeme), message);
                    if(message[0]!='O')
                    {
                        if(message[0]=='N')
                            printf("error: %llu The number of output parameters at function call <%s> is incorrect\n",ast->children[j].term.line_num,ast->children[j].term.lexeme);
                        else
                            printf("error: %llu In output parameters %s\n",ast->children[j].term.line_num,message);
                        semantic = 0;
                        return;
                    }
                    id1 = NULL;
                    //make a list of ids defined as input of funtion call
                    id1 = getParams(p->children+5, id1, name, 0);
                    bzero(message, 200);
                    compare_parameter_list_type(id1, get_input_parameter_list(funcs, ast->children[j].term.lexeme), message);
                    if(message[0]!='O')
                    {
                        if(message[0]=='N')
                            printf("error: %llu The number of input parameters at function call <%s> is incorrect\n",ast->children[j].term.line_num,ast->children[j].term.lexeme);
                        else
                            printf("error: %llu In input parameters %s\n",ast->children[j].term.line_num,message);
                        semantic = 0;
                        return;
                    }
                }

                j++;
            }
        }
        else {
            //recursively construct the ast rooted here
            parseTree *temp = p->children;
            temp+=i;
            int nochildrenl;
            if(usefulChildrenCount == 1)
            {
                while(temp->nochildren == 1 && !temp->children[0].isTerminal)
                {
                    nochildrenl = temp->children[0].nochildren;
                    temp = temp->children[0].children;
                }
            }
            ast->children[j].nonterm = p->children[i].nonterm;
            ast->children[j].isTerminal = 0;
            ast->children[j].type = -1;

            if(ast->nonterm == returnStmt)
            {
                /*
                 * match if return type of function matches with the definition
                 */
                id = id1 = NULL;
                id1 = get_output_parameter_list(funcs, name);
                id = getParams(p, id, name, 0);
                bzero(message, 200);
                compare_parameter_list_type(id, id1, message);
                if(message[0]!='O')
                {
                    if(message[0]=='N')
                        printf("error: %llu The number of output parameters at return of function <%s> is incorrect\n",ast->children[j].term.line_num,name);
                    else
                        printf("error: %llu In output parameters %s\n",ast->children[0].term.line_num,message);
                    semantic = 0;
                    return;
                }
            }
            //maintain state to work on while statement semantics
            else if(ast->children[i].nonterm == iterativeStmt)
                whileState = 1;
            else if(whileState && ast->nonterm == stmt)
                whileState = 2;

            createAbstractSyntaxTree(temp, &ast->children[j], name);

            //after this part, the jth child node will carry its type info and
            //parent node will compare it with its current type or adopt its
            //type

            if(ast->children[i].nonterm == iterativeStmt && whileState==2)
            {
                if(!isUpdate)
                {
                    printf("error: %llu While statement is not getting updated\n",ast->children[i].line_num);
                    semantic = 0;
                }
                isUpdate = 0;
                whileList = NULL;
                whileState = 0;
            }
            //type checking by extracting the type of children nodes if parent
            //doesn't jnow the type, otherwise, parent node checks if all
            //children nodes have same type. In case, this ain't the situation,
            //the parent sets type as error. This node's type further propogates
            //to parent.
            if(ast->type == -1)
            {
                ast->type = ast->children[j].type;
                ast->line_num = ast->children[j].line_num;
            }
            else if(ast->type != ast->children[j].type && ast->children[j].type!=-1)
            {
                if(ast->nonterm == term && ast->type == TK_RECORD)
                    ast->type = TK_RECORD;
                else
                    ast->type = TK_ERROR;
                ast->line_num = ast->children[j].line_num;
            }
            else if(ast->nonterm == term && ast->type == TK_RECORD && ast->children[j].type!=-1)
            {
                if(ast->children[j].type == TK_RECORD)
                    ast->type = TK_ERROR;
                else
                    ast->type = TK_RECORD;
            }

            //display errorneous children types
            if(ast->type == TK_ERROR && (ast->nonterm == assignmentStmt || ast->nonterm == booleanExpression))
            {
                printf("error: %llu Type mismatch\n",ast->line_num);
                semantic = 0;
            }

            //compress AST by removing nodes having singular child
            if(ast->children[j].nochildren==0)
            {
                ast->nochildren-=1;
                j-=1;
                astSize-=1;
            }
            else if(ast->children[j].nochildren==1)
            {
                astree *temp = ast->children[j].children;
                if(temp[0].isTerminal)
                {
                    ast->children[j].isTerminal = 1;
                    ast->children[j].term.tokenClass = temp->term.tokenClass;
                    strcpy(ast->children[j].term.lexeme, temp->term.lexeme);
                    ast->children[j].term.line_num = temp->term.line_num;
                    ast->children[j].nochildren = 0;
                    ast->children[j].children = NULL;
                    free(temp);
                    astSize-=1;
                }
            }
            j++;
        }
        i++;
    }
}
