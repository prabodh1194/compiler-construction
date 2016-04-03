/*
   BATCH NUMBER: 23
   PRABODH AGARWAL 2012B1A7801P
   DEEPANSHU SINGH 2012B3A7593P

   driver.c:

*/

#include <stdio.h>
#include "lexer.h"
#include "lexerDef.h"
#include "parserDef.h"
#include "helper_functions.h"
#include "parser.h"
#include "symboltableDef.h"
#include "symboltable.h"
#include "populateHashTable.h"

extern short compilation; //compilation flag
int main(int argc, char **args)
{
    int i,j,choice,flag=0;

    FILE *fp = fopen(args[1],"r");
    FILE *outfile = fopen(args[2],"w");

    printf("\n(a) FIRST and FOLLOW set automated");
    printf("\n(b) Both lexical and syntax analysis modules implemented");
    printf("\n(c) Modules compile suxxefully");
    printf("\n(d) modules work with all testcases");
    printf("\n(e) Parse tree constructed\n");

        grammar g; //data structure to store grammar as a double dimensional array of respective enums
    table t; //parse table data structure, rows=NON TERMINAL cols=TERMINALS, cells=rule numbers indexing into grammar g
    parseTree *tree,*ast; //parsetree datastructure--this is the root of the tree
    tokenInfo *to; //get token from the code file in getNextToken()

    to = (tokenInfo *)malloc(sizeof(tokenInfo)); //stores tokens from the code
    tree = (parseTree *)malloc(sizeof(parseTree));
    ast = (parseTree *)malloc(sizeof(parseTree));

    function_hashtable *funcs = create_function_hashtable(31);
    function_wise_identifier_hashtable *local = create_function_local_identifier_hashtable(31);

    //init parsetable to default as no rules available
    for(i=0;i<MAX_NON_TERMINALS;i++)
        for(j=0;j<MAX_TERMINALS;j++)
            t[i][j]=-1;

    //make lookup table of keywords for the lexer
    h = hash_keywords();

    //get first and follow sets
    populate();
    //read grammar file and populate data structure
    getGrammar(&g);
    //makr the parsetable
    createParseTable(g,&t);

    do
    {
        printf("1. Print comment free code\n2. Print token list\n3. Syntactic analysis of code\n4. Print parse tree\n5. Print FIRST and FOLLOW sets\n6. Generate AST\n7. Populate ST\n8. Exit\n(1/2/3/4/5/6/7/8)\t");
        scanf("%d",&choice);
        switch(choice)
        {
            case 1:
                printf("Printing comment free code\n");
                printCode(args[1]);
                break;
            case 2:
                printf("Printing tokens\n");
                printTok(args[1]);
                break;
            case 3:
                //parse tree shall be created only once
                if(!flag)
                {
                    getNextToken(fp,to);
                    ast->nonterm = tree->nonterm = program;
                    ast->isTerminal = tree->isTerminal = 0;
                    ast->children = tree->children = NULL;
                    parseInputSourceCode(fp,t,g,tree,to);
                    flag=1;
                    fclose(fp);
                    if(compilation)
                        printf("\nCode compiled succesfully\n");
                }
                else
                    printf("Restart driver\n");
                break;
            case 4:
                //can't print parsetree before making one
                if(!flag)
                {
                    printf("Please use option 3 before doing this\n");
                    break;
                }
                fprintf(outfile,"\n   lexemeCurrentNode\t    lineno\t\t      token\t\t    valueIfNumber\t\t      NodeSymbol\t\t    parentNodeSymbol \t isLeafNode\t");
                printParseTree(tree, outfile);
                printf("\n");
                fclose(outfile);
                printf("\nUse gedit outfile.txt to view the parse tree\n");
                break;
            case 5:
                //print first set
                printff(1);
                //print follow sets
                printff(2);
                break;
            case 6:
                createAbstractSyntaxTree(tree, ast);
                break;
            case 7:
                populateFunctionST(ast,funcs, local,NULL,-1);
                print_function_hashtable(funcs);
                break;
            case 8:
                break;
            default:
                printf("Wrong choice\n");
                break;
        }
    }
    while(choice!=8);

    return 0;
}
