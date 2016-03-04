#include <stdio.h>
#include "lexer.h"
#include "lexerDef.h"
#include "parseDef.h"
#include "helper_functions.h"
#include "parser.h"

int main(int argc, char **args)
{
    int i,j,choice,flag=0;

    FILE *fp = fopen(args[1],"r");
    FILE *outfile = fopen(args[2],"w");

    grammar g;
    table t;
    parseTree *tree;
    tokenInfo *to;

    to = (tokenInfo *)malloc(sizeof(tokenInfo));
    tree = (parseTree *)malloc(sizeof(parseTree));

    for(i=0;i<MAX_NON_TERMINALS;i++)
        for(j=0;j<MAX_TERMINALS;j++)
            t[i][j]=-1;

    h = hash_keywords();

    populate();
    getGrammar(&g);
    createParseTable(g,&t);

    do
    {
        printf("1. Print comment free code\n2. Print token list\n3. Syntactic analysis of code\n4. Print parse tree.\n5. Print FIRST and FOLLOW sets\n6. Exit\n(1/2/3/4/5/6)\t");
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
                if(!flag)
                {
                    getNextToken(fp,to);
                    tree->nonterm = program;
                    tree->isTerminal = 0;
                    tree->children = NULL;
                    parseInputSourceCode(fp,t,g,tree,to);
                    flag=1;
                    fclose(fp);
                }
                else
                    printf("Restart driver\n");
                break;
            case 4:
                if(!flag)
                {
                    printf("Please use option 3 before doing this\n");
                    break;
                }
                fprintf(outfile,"\nlexemeCurrentNode\tlineno\t\ttoken\t\tNodeSymbol\t\t    parentNodeSymbol\t isLeafNode\tvalueIfNumber");
                printParseTree(tree, outfile);
                printf("\n");
                fclose(outfile);
                break;
            case 5:
                printff(1);
                printff(2);
                break;
            default:
                printf("Wrong choice\n");
                break;
        }
    }
    while(choice!=6);

    return 0;
}
