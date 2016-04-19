/*
   BATCH NUMBER: 23
   PRABODH AGARWAL 2012B1A7801P
   DEEPANSHU SINGH 2012B3A7593P

   parser.c:

   void getGrammar(grammar *g);
   void createParseTable(grammar g, table *t);
   char* getFirstSet(char *lhs, char *rhs, char *set);
   int parseInputSourceCode(FILE *sourceCodeFile, table tb, grammar g, parseTree *root, tokenInfo *t);
   void printParseTree(parseTree *p, FILE *outfile);
   void printParseTable(table t);
   char * getFirstSet(char *, char *, char *);
   char * getFollowSet(char *);


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
 * read the grammar file and populate the data structure with respective enum
 * values. 1st entry of row is the lhs of rule, 2nd entry is a count entry of number of elements
 * on the rhs, 3rd entry onwards, the rhs of the grammar is stored. The values
 * are stored as respective enums that can be obtained from lexerdef.h for
 * terminals and parsrdef for non-terminals.
 * The grammar.txt file stores rules in a form of 2 characters per rule, e.g. --
 * <assignmentStmt> = a1
 * <arithmetixExpression> = a2 and so on, hence read as 2 characters at once
 */

short compilation = 1; //successful compilation

void printExpectedTokens(nontermid state, table tb)
{
    if(state==0)
        return;
    fprintf(stderr,"Expected tokens:  ");
    int j;
    for(j = 0; j < MAX_TERMINALS; j++)
        if(tb[state - NON_TERMINAL_OFFSET][j]!=-1)
            fprintf(stderr,"%s\t",tokenName(j));
    fprintf(stderr,"\n");
}

void getGrammar(grammar *g)
{
    FILE *fp;
    char token[20],tok[3];
    fp = fopen("grammar.txt","r");
    int rule,ruleLine = 0,i;
    tok[2]='\0';

    while(!feof(fp))
    {
        bzero(token,21);
        fscanf(fp,"%s",token); //LHS
        tok[0]=token[0];
        tok[1]=token[1];
        //retrieve enum value for given LHS token
        gg[ruleLine][0]=grammar_to_enum(tok);
        gg[ruleLine][1]=0;
        rule=2;
        //RHS
        //fscanf(fp,"%s",token);
        for(i=3;token[i]!='\0';i+=2)
        {
            tok[0]=token[i];
            tok[1]=token[i+1];
            //retrieve enum value for given RHS token
            gg[ruleLine][rule++]=grammar_to_enum(tok);
        }
        gg[ruleLine++][1]=rule-2;//count of elements on RHS
    }
    fclose(fp);
}

/*
 * Read the grammar and populate parse table. Given table entry, e.g.:
 * table[enum of some nonterminal][enum of some terminal] = valid rule number in
 * grammar g data struture or -1 in case no rule exists
 */
void createParseTable(grammar g, table *t)
{
    int i,j,grammarEnum;
    char *fi, *rulerhs, *rulelhs, *rhspart;

    rhspart= (char *)malloc(sizeof(char)*3);
    rulelhs = (char *)malloc(sizeof(char)*3);
    rulerhs = (char *)malloc(sizeof(char)*MAX_RULE_SIZE);
    fi = (char *)calloc(MAX_RULE_SIZE,sizeof(char));

    rulelhs[2]='\0';

    //i indexes to rules in order as given in grammar.txt
    for(i=0;i<MAX_RULES;i++)
    {
        bzero(rulerhs,MAX_RULE_SIZE);
        bzero(rulelhs,3);
        bzero(rhspart,3);
        strcat(rulelhs,enum_to_grammar(g[i][0]));
        for(j=0;j<g[i][1];j++)
            strcat(rulerhs,enum_to_grammar(g[i][j+2]));
        //for rule A->x; for all t belonging to FIRST(x), table[A,t] = rule.
        getFirstSet(rulelhs,rulerhs,fi);

        if(fi!=NULL)
        {
            for(j=0;j<strlen(fi);j+=2)
            {
                rhspart[0]=fi[j];
                rhspart[1]=fi[j+1];
                grammarEnum = grammar_to_enum(rhspart);            
                grammarEnum-=grammarEnum>=NON_TERMINAL_OFFSET?NON_TERMINAL_OFFSET:0;
                //tt is a macro
                tt[g[i][0]-NON_TERMINAL_OFFSET][grammarEnum]=i+1;
            }
        }
    }
}

char* getFirstSet(char *lhs, char *rhs, char *set)
{
    int i,epsflag=0;
    struct y **p;

    //read the rule string char by char and get first sets.
    bzero(set,20);
    for(i=0;i<strlen(rhs);i+=2)
    {
        if(rhs[i]<65||rhs[i]>90)
        {
            //terminal
            if(rhs[i]!='e'||rhs[i+1]!='2') //check for not epsilon
                sprintf(set,"%s%c%c",set,rhs[i],rhs[i+1]);
            else
                epsflag=1;
            break;
        }
        else
        {
            p = ptr[rhs[i]-65][rhs[i+1]-48];
            strcat(set,p[1]->a);
            if(p[1]->eps!=2) //A-->BC; if eps belongs to first(B) then append firct(C) else don't
                break;
        }
        epsflag=1;//first(BC) contains eps, hence we need to add follow of A as well
    }
    if(epsflag)
        strcat(set,ptr[lhs[0]-65][lhs[1]-48][2]->a); //add follow(A) from A-->BC
    return set;
}

/*
 * Make a parse tree by reading the source code file. The program's start state
 * is at "program". As the lookahead tokens are recieved by getNextToken(), the parsetable
 * is looked up using current state and lookahead and the rule number is retrieved, using these rules, parse table is created
 * with the rules on right hand side forming sibling nodes at level of the tree
 * and rules on lhs forming the parent node.
 */
int parseInputSourceCode(FILE *sourceCodeFile, table tb, grammar g, parseTree *root, tokenInfo *t)
{
    int state, i,ruleNo, *rule, nochildren;
    parseTree *tree, *node;
    nontermid goTo;
    short err = 0; //error flag

    tree = NULL;

    //in case errorneous token is recieved, notify appropriately and get next token
    if(t->tokenClass == TK_ERROR)
    {
        fprintf(stderr,"error: line %llu:%ld %s\n",t->line_num,t->col,t->lexeme);
        printExpectedTokens(root->nonterm, tb);
        getNextToken(sourceCodeFile, t);
        compilation = 0;
        return -2;
    }

    //if state points at terminal, match with lookahead to determine correctness.
    if(root->isTerminal)
    {
        //match the lookahead
        //case of lookahead mismatch
        if(root->term.tokenClass != t->tokenClass)
        {
            fprintf(stderr, "error: line %llu:%ld The token %s for lexeme <%s> does not match. The expected token here is %s\n", t->line_num, t->col, tokenName(t->tokenClass), t->lexeme, tokenName(root->term.tokenClass));
            compilation = 0;
            return -2;
        }
        else
        {
            //matching lookaheads, advance token
            strcpy(root->term.lexeme,t->lexeme);
            root->term.line_num=t->line_num;
            //printf("\n%s\t%llu\t%s\t\tyes",root->term.lexeme,root->term.line_num,enum_to_grammar(root->term.tokenClass));
            getNextToken(sourceCodeFile, t);
            return 0;
        }
    }
    else
    {
        //get current state of parser
        state = root->nonterm;
        state-=NON_TERMINAL_OFFSET;

        //retrieve rule number based on current state and the lookahead
        ruleNo = tb[state][t->tokenClass]-1; //array indexing

        //no such rule!
        //error happened because no rule exists for parser in current state with
        //given lookup
        if(ruleNo < 0)
        {
            fprintf(stderr, "error: line %llu:%ld Found unexpected token %s for lexeme <%s>\n",t->line_num, t->col, tokenName(t->tokenClass), t->lexeme);
            printExpectedTokens(root->nonterm, tb);
            compilation = 0;
            return -1;
        }
        //found rule
        else
        {

            /*
             * the rule is of the form:
             * lhs count(terms in rhs) rules on rhs
             */
            rule = g[ruleNo];
            //number of terms on rhs
            nochildren = rule[1];

            root->nochildren = nochildren;
            if(nochildren>0)
            {
                node = root->children = (parseTree *)malloc(sizeof(parseTree)*nochildren);
                bzero(root->children, sizeof(parseTree)*nochildren);
            }

            for(i=0;i<nochildren;i++)
            {
                //initialize tree node
                node[i].nochildren = 0;
                node[i].children=NULL;
                bzero(node[i].term.lexeme,MAX_LEXEME_SIZE);
                //set node for non-terminal
                if(rule[i+2]>=NON_TERMINAL_OFFSET)
                {
                    node[i].isTerminal = 0;
                    node[i].nonterm = rule[i+2];
                }
                //set node for terminal
                else
                {
                    node[i].isTerminal = 1;
                    node[i].term.tokenClass = rule[i+2];
                    if(rule[2]==eps)
                    {
                        strcpy(node[i].term.lexeme,"eps");
                        node[i].term.line_num = t->line_num;
                        node[i].term.tokenClass = eps;
                        continue;
                    }

                }
                //printf("\n----\t----\t%s\t\t%s\tno",enum_to_grammar(node[i].nonterm),enum_to_grammar(node[i].nonterm));
                //go to next state using the lookahead
                if(parseInputSourceCode(sourceCodeFile, tb, g, node+i, t)==-1)
                {
                    //error due to token mismatch or wrong state for given lookahead.
                    /*
                     * Error recovery technique mentioned below, succesfully
                     * detects errors. For the given grammar, Panic
                     * Error Recovery Mode missed a lot of syntax errors and
                     * hence can be deemed weaker than the technique described
                     * below.
                     */

                    /*If error occurs inside a block, search for the end to that
                     * block. A block can be defined as a set of tokens which
                     * are complete. e.g.: A semi-colon terminated assignment
                     * statement is a complete block, any error found in this
                     * block will be successfully recovered by getting out of
                     * the assignment which can be done by consuming tokens
                     * ubtil a semicolon (;) is found. Similarly, errors in
                     * conditional blocks can be recovered from by consuming all
                     * tokens until <endif> is found and iterative stmt is
                     * recovered by consuming tokens until <endwhile> is found
                     */


                    //unfold the tree to go to the start state of a given block
                    //in which error is found and then consume tokens until
                    //corresponding end state is found
                    if(node[i].nonterm == mainFunctions || node[i].nonterm == declaration || node[i].nonterm == assignmentStmt || node[i].nonterm == iterativeStmt || node[i].nonterm == funCallStmt || node[i].nonterm == conditionalStmt || node[i].nonterm == ioStmt || node[i].nonterm == typeDefinition || node[i].nonterm == returnStmt || node[i].nonterm == function)
                    {
                        err = -1;
                        break;
                    }
                    else
                        compilation = 0;
                    return -1;
                }
            }
            //goTo contains the endstate of the block where an error is found,
            //tokens are consumed until such endstate is not reached
            if(err==-1)
            {
                if(node[i].nonterm == conditionalStmt)
                    goTo = TK_ENDIF;
                else if(node[i].nonterm == iterativeStmt)
                    goTo = TK_ENDWHILE;
                else if(node[i].nonterm == function) 
                    goTo = TK_END;
                else
                    goTo = TK_SEM;

                while(t->tokenClass != goTo)
                {
                    getNextToken(sourceCodeFile, t);
                    if(t->tokenClass == TK_ERROR)
                        fprintf(stderr,"error: line %llu: %s\n",t->line_num,t->lexeme);
                    else if (t->tokenClass == TK_EOF)
                        return 1;
                }
                getNextToken(sourceCodeFile, t);
            }
        }
    }
    return 1;
}

//construct an AST by removing useless nodes from the parse tree

identifier_list *whileList = NULL;
short whileState = 0, isUpdate = 0;

void createAbstractSyntaxTree(parseTree *p, astree *ast, char *name)
{
    /* recursively constructs the abstract syntax tree
     * by removing "useless" nodes from the parse tree
     * the ast must be initialized to the program node
     * every call constructs a subtree of the ast
     * rooted at the current nonterminal
     */
    int i, j, usefulChildrenCount = 0;
    terminalId type;
    identifier_list *id, *id1;
    char *typeS;
    char message[200];

    for(i = 0; i < p->nochildren; i++)
        // all nonterminals are useful
        if(!p->children[i].isTerminal || isUseful(p->children[i].term.tokenClass))
            usefulChildrenCount++;
    if(usefulChildrenCount == 0)
        return;

    ast->nochildren = usefulChildrenCount;
    ast->children = (astree *)malloc(ast->nochildren * sizeof(astree));
    bzero(ast->children,usefulChildrenCount*sizeof(astree));

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

                if((ast->children[j].term.tokenClass == TK_ID || ast->children[j].term.tokenClass == TK_FIELDID) && ast->nonterm!=parameter_list && ast->nonterm!=declaration && ast->nonterm!=funCallStmt && ast->nonterm!=fieldDefinition)
                {
                    ast->line_num = ast->children[j].term.line_num;
                    if(ast->children[j].term.tokenClass == TK_ID)
                    {
                        id1 = search_global_identifier(global, ast->children[j].term.lexeme);
                        id = search_function_wise_identifier_hashtable(local, name, ast->children[j].term.lexeme);

                        if(id1!=NULL && id!=NULL)
                        {
                            printf("error: %llu %s declared in both global and local\n", ast->children[j].term.line_num, ast->children[j].term.lexeme);
                            return;
                        }
                        if(id == NULL)
                            id = id1;
                        if(whileState == 1 && id!=NULL)
                            whileList = addIdentifier(whileList, id->name, id->type, 0);
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
                        id = search_function_wise_identifier_hashtable(record, name, ast->children[j].term.lexeme);

                    if(id == NULL)
                    {
                        printf("error: %llu %s doesn't exist\n", ast->children[j].term.line_num, ast->children[j].term.lexeme);
                        return;
                    }

                    typeS = id->type;
                    if(p->children[i].term.tokenClass!=TK_FIELDID && p->children[i+1].nochildren!=0 && p->children[i+1].children[0].term.tokenClass!= eps && p->nonterm!=idList)
                        name = typeS;
                    else
                    {
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
                    ast->line_num = ast->children[j].term.line_num;
                    if(ast->type == -1 || ast->type == TK_INT)
                        ast->type = TK_INT;
                    else
                        ast->type == TK_ERROR;
                }
                else if(ast->children[j].term.tokenClass == TK_RNUM && ast->nonterm!=funCallStmt)
                {
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
                        return;
                    }
                    id = NULL;
                    id = getParams(p->children, id, name, 0);
                    bzero(message, 200);
                    compare_parameter_list_type(id, get_output_parameter_list(funcs, ast->children[j].term.lexeme), message);
                    if(message[0]!='O')
                    {
                        if(message[0]=='N')
                            printf("error: %llu Mismatch in number of output parameters\n",ast->children[j].term.line_num);
                        else
                            printf("error: %llu In output parameters %s\n",ast->children[j].term.line_num,message);
                        return;
                    }
                    id1 = NULL;
                    id1 = getParams(p->children+5, id1, name, 0);
                    bzero(message, 200);
                    compare_parameter_list_type(id1, get_input_parameter_list(funcs, ast->children[j].term.lexeme), message);
                    if(message[0]!='O')
                    {
                        if(message[0]=='N')
                            printf("error: %llu Mismatch in number of input parameters\n",ast->children[j].term.line_num);
                        else
                            printf("error: %llu In input parameters %s\n",ast->children[j].term.line_num,message);
                        return;
                    }
                }

                j++;
            }
        }
        else {
            // recursively construct the ast rooted here
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
                id = id1 = NULL;
                id1 = get_output_parameter_list(funcs, name);
                id = getParams(p, id, name, 0);
                bzero(message, 200);
                compare_parameter_list_type(id, id1, message);
                if(message[0]!='O')
                {
                    if(message[0]=='N')
                        printf("error: %llu Mismatch in number of output parameters\n",ast->children[0].term.line_num);
                    else
                        printf("error: %llu In output parameters %s\n",ast->children[0].term.line_num,message);
                    return;
                }
            }
            else if(ast->children[i].nonterm == iterativeStmt)
                whileState = 1;
            else if(whileState && ast->nonterm == stmt)
                whileState = 2;

            createAbstractSyntaxTree(temp, &ast->children[j], name);

            if(ast->children[i].nonterm == iterativeStmt && whileState==2)
            {
                if(!isUpdate)
                    printf("error: While statement is not getting updated\n");
                isUpdate = 0;
                whileList = NULL;
                whileState = 0;
            }
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

            if(ast->type == TK_ERROR && (ast->nonterm == assignmentStmt || ast->nonterm == booleanExpression))
                printf("error: %llu Type mismatch\n",ast->line_num);

            if(ast->children[j].nochildren==0)
            {
                ast->nochildren-=1;
                j-=1;
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
                }
            }
            j++;
        }
        i++;
    }
}


//print contents od parsetable
void printParseTable(table t)
{
    int i,j;
    for(i=0;i<MAX_NON_TERMINALS;i++)
    {
        for(j=0;j<MAX_TERMINALS;j++)
            if(t[i][j]>0)
                printf("For %s-->%s rule is %d\n",enum_to_grammar(i+NON_TERMINAL_OFFSET),enum_to_grammar(j),t[i][j]);
    }
}

//print parse tree in depth first order
void printParseTree(parseTree *p, FILE *outfile)
{
    int nochildren = p->nochildren, i;
    parseTree *t;

    if(p->nonterm<NON_TERMINAL_OFFSET)
        return;

    t = p->children;

    for (i = 0; i < nochildren; i++,t++) 
    {
        char n[100];
        bzero(n,100);
        if(t->term.tokenClass == TK_NUM || t->term.tokenClass == TK_RNUM)
            strcpy(n , t->term.lexeme);
        else
            strcpy(n , "----");
        if(t->isTerminal)
            fprintf(outfile,"\n%20s%12llu\t%15s\t\t%16s\t\t\t\t\t            %20s\t\tyes",t->term.lexeme,t->term.line_num,tokenName(t->term.tokenClass),n,tokenName(p->nonterm));
        else
        {
            fprintf(outfile,"\n                ----\t     ----              ----%21s%24s\t\t%20s\t\t no",n,tokenName(t->nonterm),tokenName(p->nonterm));
            printParseTree(t, outfile);
        }
    }
}

void printasTree(astree *p, FILE *outfile)
{
    int nochildren = p->nochildren, i;
    astree *t;

    if(p->nonterm<NON_TERMINAL_OFFSET)
        return;

    t = p->children;

    for (i = 0; i < nochildren; i++,t++) 
    {
        char n[100];
        bzero(n,100);
        if(t->term.tokenClass == TK_NUM || t->term.tokenClass == TK_RNUM)
            strcpy(n , t->term.lexeme);
        else
            strcpy(n , "----");
        if(t->isTerminal)
            fprintf(outfile,"\n%20s%12llu\t%15s\t\t%16s\t\t\t\t\t            %20s\t\tyes\t",t->term.lexeme,t->term.line_num,tokenName(t->term.tokenClass),n,tokenName(p->nonterm));
        else
        {
            fprintf(outfile,"\n                ----\t     ----              ----%21s%24s\t\t%20s\t\t no\t%10s",n,tokenName(t->nonterm),tokenName(p->nonterm),tokenName(t->type));
            printasTree(t, outfile);
        }
    }
}

/*
   int main(int argc, char **args)
   {
   int i,j;

   FILE *fp = fopen(args[1],"r");
   FILE *outfile = fopen("outfile.txt","w");

   grammar g;
   table t;
   parseTree *tree;
   tokenInfo *to;

   to = (tokenInfo *)malloc(sizeof(tokenInfo));
   tree = (parseTree *)malloc(sizeof(parseTree));

   tree->nonterm = program;
   tree->isTerminal = 0;
   tree->children = NULL;
   for(i=0;i<MAX_NON_TERMINALS;i++)
   for(j=0;j<MAX_TERMINALS;j++)
   t[i][j]=-1;

   h = hash_keywords();

   getNextToken(fp,to);
   populate();
   getGrammar(&g);
   createParseTable(g,&t);
   printParseTable(t);
   parseInputSourceCode(fp,t,g,tree,to);
   printf("\nlexemeCurrentNode\tlineno\t\ttoken\t\tNodeSymbol\t\t    parentNodeSymbol\t isLeafNode\tvalueIfNumber");
   printParseTree(tree, outfile);
   printf("\n");
   fclose(fp);
   fclose(outfile);
   return 0;
   }
   */
