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
#include "parseDef.h"
#include "helper_functions.h"
#include "parser.h"

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
        fprintf(stderr,"error: line %llu:%llu %s\n",t->line_num,t->col,t->lexeme);
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
            fprintf(stderr, "error: line %llu:%llu The token %s for lexeme <%s> does not match. The expected token here is %s\n", t->line_num, t->col, tokenName(t->tokenClass), t->lexeme, tokenName(root->term.tokenClass));
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
            fprintf(stderr, "error: line %llu:%llu Found unexpected token %s for lexeme <%s>\n",t->line_num, t->col, tokenName(t->tokenClass), t->lexeme);
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
                node = root->children = (parseTree *)malloc(sizeof(parseTree)*nochildren);

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
