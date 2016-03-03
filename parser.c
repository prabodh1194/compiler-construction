#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parseDef.h"
#include "helper_functions.h"

#define tt (*t)

typedef int grammar[MAX_RULES+1][MAX_RULE_SIZE+2];
typedef int table[MAX_NON_TERMINALS][MAX_TERMINALS];
extern struct y *ptr[26][10][3];

extern hashtable *h;

char * getFirstSet(char *, char *, char *);
char * getFollowSet(char *);

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
        gg[ruleLine][0]=grammar_to_enum(tok);
        gg[ruleLine][1]=0;
        rule=2;
        //RHS
        //fscanf(fp,"%s",token);
        for(i=3;token[i]!='\0';i+=2)
        {
            tok[0]=token[i];
            tok[1]=token[i+1];
            gg[ruleLine][rule++]=grammar_to_enum(tok);
        }
        gg[ruleLine++][1]=rule-2;//count of elements on RHS
    }
    fclose(fp);
}

void createParseTable(grammar g, table *t)
{
    int i,j,grammarEnum;
    char *fi, *rulerhs, *rulelhs, *rhspart;

    rhspart= (char *)malloc(sizeof(char)*3);
    rulelhs = (char *)malloc(sizeof(char)*3);
    rulerhs = (char *)malloc(sizeof(char)*MAX_RULE_SIZE);
    fi = (char *)calloc(MAX_RULE_SIZE,sizeof(char));

    rulelhs[2]='\0';

    for(i=0;i<MAX_RULES;i++)
    {
        bzero(rulerhs,MAX_RULE_SIZE);
        bzero(rulelhs,3);
        bzero(rhspart,3);
        strcat(rulelhs,enum_to_grammar(g[i][0]));
        for(j=0;j<g[i][1];j++)
            strcat(rulerhs,enum_to_grammar(g[i][j+2]));
        getFirstSet(rulelhs,rulerhs,fi);

        if(fi!=NULL)
        {
            for(j=0;j<strlen(fi);j+=2)
            {
                rhspart[0]=fi[j];
                rhspart[1]=fi[j+1];
                grammarEnum = grammar_to_enum(rhspart);            
                grammarEnum-=grammarEnum>=NON_TERMINAL_OFFSET?NON_TERMINAL_OFFSET:0;
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

int parseInputSourceCode(FILE *sourceCodeFile, table tb, grammar g, parseTree *root, tokenInfo *t)
{
    int state, i,ruleNo, *rule, nochildren;
    parseTree *tree, *node;
    nontermid goTo;
    short err = 0; //error flag

    tree = NULL;

    if(t->tokenClass == TK_ERROR)
    {
        fprintf(stderr,"error: line %llu: %s\n",t->line_num,t->lexeme);
        getNextToken(sourceCodeFile, t);
        return -2;
    }

    if(root->isTerminal)
    {
        //match the lookahead
        if(root->term.tokenClass != t->tokenClass)
        {
            fprintf(stderr, "error: line %llu: The token %s for lexeme <%s> does not match. The expected token here is %s\n", t->line_num, tokenName(t->tokenClass), t->lexeme, tokenName(root->term.tokenClass));
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
        state = root->nonterm;
        state-=NON_TERMINAL_OFFSET;

        ruleNo = tb[state][t->tokenClass]-1; //array indexing

        //no such rule!
        if(ruleNo < 0)
        {
            fprintf(stderr, "error: line %llu: Found unexpected token %s for lexeme <%s>\n",t->line_num, tokenName(t->tokenClass), t->lexeme);
            return -1;
        }
        //found rule
        else
        {

            rule = g[ruleNo];
            nochildren = rule[1];

            root->nochildren = nochildren;
            if(nochildren>0)
                node = root->children = (parseTree *)malloc(sizeof(parseTree)*nochildren);

            for(i=0;i<nochildren;i++)
            {
                node[i].nochildren = 0;
                node[i].children=NULL;
                bzero(node[i].term.lexeme,MAX_LEXEME_SIZE);
                if(rule[i+2]>=NON_TERMINAL_OFFSET)
                {
                    node[i].isTerminal = 0;
                    node[i].nonterm = rule[i+2];
                }
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
                if(parseInputSourceCode(sourceCodeFile, tb, g, node+i, t)==-1)
                {
                    //error due to token mismatch. Panic error recovery mode
                    /*If error occurs inside a block, search for the end to that
                     * block
                     */
                    /*
                       fprintf(stderr,"Expected tokens:  ");
                       int j;
                       for(j = 0; j < MAX_TERMINALS; j++)
                       if(tb[node[i].nonterm - NON_TERMINAL_OFFSET][j]!=-1)
                       fprintf(stderr,"%s\t",tokenName(j));
                       fprintf(stderr,"\n");
                       */
                    if(node[i].nonterm == assignmentStmt || node[i].nonterm == iterativeStmt || node[i].nonterm == conditionalStmt || node[i].nonterm == ioStmt || node[i].nonterm == funCallStmt || node[i].nonterm == typeDefinition || node[i].nonterm == declaration || node[i].nonterm == returnStmt || node[i].nonterm == mainFunctions || node[i].nonterm == function)
                    {
                        err = -1;
                        break;
                    }
                    else
                        return -1;
                }
            }
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

void printParseTree(parseTree *p, FILE *outfile)
{
    int nochildren = p->nochildren, i;
    parseTree *t;

    if(p->nonterm<NON_TERMINAL_OFFSET)
        return;

    t = p->children;

    for (i = 0; i < nochildren; i++,t++) 
    {
        if(t->isTerminal)
            printf("\n%17s%12llu\t%15s\t\t\t\t\t%20s\t\tyes",t->term.lexeme,t->term.line_num,tokenName(t->term.tokenClass),tokenName(p->nonterm));
        else
        {
            printf("\n             ----\t ----\t%34s\t\t%20s\t\t no",tokenName(t->nonterm),tokenName(p->nonterm));
            printParseTree(t, outfile);
        }
    }
}

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
