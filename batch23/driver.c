/*
   BATCH NUMBER: 23
   PRABODH AGARWAL 2012B1A7801P
   DEEPANSHU SINGH 2012B3A7593P

   driver.h

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
#include "codegen.h"

extern int parseSize;
extern int astSize;
extern short syntactic; //syntactic flag
extern short semantic;  //semantic flag
int main(int argc, char **args)
{

    if(argc < 3)
    {
        printf("Usage:: ./toycompiler <source code> <asm out file>\n\n");
        return -1;
    }

    int i,j,choice,flag=0;
    short flags[] = {0,0,0,0,0,0,0};

    FILE *fp = fopen(args[1],"r");
    FILE *outfile = stdout;

    printf("LEVEL 4: AST/ Symbol table/ Type checking/ Semantic analysis/ Code generation\n");

    grammar g; //data structure to store grammar as a double dimensional array of respective enums
    table t; //parse table data structure, rows=NON TERMINAL cols=TERMINALS, cells=rule numbers indexing into grammar g
    parseTree *tree; //parsetree datastructure--this is the root of the tree
    astree *ast;
    tokenInfo *to; //get token from the code file in getNextToken()

    to = (tokenInfo *)malloc(sizeof(tokenInfo)); //stores tokens from the code
    tree = (parseTree *)malloc(sizeof(parseTree));
    ast = (astree *)malloc(sizeof(astree));

    funcs = create_function_hashtable(31);
    local = create_function_local_identifier_hashtable(31);
    record = create_function_local_identifier_hashtable(31);
    global = create_identifier_hashtable(31);

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
        printf("1. Print token list\n2. Syntactic analysis of code\n3. Print AST\n4. Memory usage\n5. Print symbol table\n6. Compile\n7. Generate ASM code\n8. Exit\n(1/2/3/4/5/6/7/8)\t");
        scanf("%d",&choice);
        switch(choice)
        {
            case 1:
                if(flags[1-1])
                {
                    printf("Token list can be printed only once\n");
                    //break;
                }
                printf("Printing tokens\n");
                printTok(args[1]);
                flags[1-1] = 1;
                break;
            case 2:
                if(!flags[2-1])
                {
                    printf("Compile code first by pressing 6\n");
                    break;
                }
                fprintf(outfile, "\n   lexemeCurrentNode\t    lineno\t\t      token\t\t    valueIfNumber\t\t      NodeSymbol\t\t    parentNodeSymbol \t isLeafNode\t");
                printParseTree(tree, outfile);
                printf("\n");
                //printf("\nUse gedit outfile.txt to view the parse tree\n");
                break;
            case 3:
                if(!flags[3-1])
                {
                    if(flags[2-1] && syntactic)
                    {
                        printf("The code contains syntactic errors. Please fix them\n");
                        break;
                    }
                    printf("Compile code first by pressing 6\n");
                    break;
                }
                fprintf(outfile, "\n   lexemeCurrentNode\t    lineno\t\t      token\t\t    valueIfNumber\t\t      NodeSymbol\t\t    parentNodeSymbol \t isLeafNode\t");
                printasTree(ast, outfile);
                printf("\n");
                break;
            case 4:
                if(!flags[4-1])
                {
                    if(flags[2-1] || flags[3-1])
                    {
                        printf("Code contains errors. Please fix them first\n");
                        break;
                    }
                    printf("Compile code first by pressing 6\n");
                    break;
                }
                float ps = parseSize*sizeof(parseTree);
                float as = astSize*sizeof(astree);
                printf("Parse tree: Number of nodes = %d\tAllocated Memory = %lf\n", parseSize, ps);
                printf("Abstract syntax tree: Number of nodes = %d\tAllocated Memory = %lf\n", astSize, as);
                printf("Compression percentage = %lf\n",(ps-as)*100/ps);
                break;
            case 5:
                if(!flags[5-1])
                {
                    if(flags[2-1])
                    {
                        printf("Code contains syntactic errors\n");
                        break;
                    }
                    printf("Compile code first by pressing 6\n");
                    break;
                }
                //print_function_hashtable(funcs);
                printf("Assumptions:\nThe symbol table has values after hashing, hence the display order of fields in record might be jumbled compared to the order in source code.\n");
                print_function_wise_identifier_hashtable(local, record);
                //print_function_wise_identifier_hashtable(record);
                print_identifier_hashtable(global, NULL, record);
                break;
            case 6:
                if(flags[6-1])
                {
                    printf("Code already compiled. Restart driver.\n");
                    break;
                }
                getNextToken(fp,to);
                ast->nonterm = tree->nonterm = program;
                ast->isTerminal = tree->isTerminal = 0;
                ast->children = NULL; 
                tree->children = NULL;
                parseInputSourceCode(fp,t,g,tree,to);
                flags[2-1] = 1;
                flags[6-1] = 1;
                fclose(fp);
                if(!syntactic)
                    break;
                printf("Code successfully analyzed for syntactic errors\n");
                populateGlobalRecords(tree, NULL, -1);
                flags[5-1] = 1;
                populateFunctionST(tree, NULL,-1);
                createAbstractSyntaxTree(tree, ast, NULL);
                flags[4-1] = flags[3-1] = 1;
                if(syntactic && semantic)
                {
                    printf("Code compiled successfully\n");
                    flags[7-1] =  1;
                }
                break;
            case 7:
                if(flags[7-1])
                {
                    createAsm(ast, NULL, fopen(args[2],"w"));
                    printf("ASM file generated successfully.\nIt has been assumed that input file contains only main and performs only integer operations.\nInput of real numbers will lead to SEGFAULT during out file execution. Use following options to execute the file.\n\nnasm -f elf %s\nld -m elf_i386 -o a.out %s\b\b\bo  \n\n",args[2], args[2]);
                }
                else if(flags[2-1] && !flags[3-1])
                    printf("Syntactic errors exist in code\n");
                else if(flags[2-1] && flags[3-1] && !flags[7-1])
                    printf("Semantic errors exist in code\n");
                else
                    printf("Compile code first by pressing 6\n");
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
