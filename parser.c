#include <stdio.h>
#include <stdlib.h>

#define gg (*g)
#define MAX_RULES 90
#define MAX_RULE_SIZE 8
#define MAX_TERMINALS 52
#define MAX_NON_TERMINALS 54
typedef int grammar[MAX_RULES][MAX_RULE_SIZE+2];
typedef int table[MAX_TERMINALS][MAX_NON_TERMINALS];

int grammar_to_enum(char *);

void getGrammar(grammar *g)
{
    FILE *fp;
    char token[20];
    fp = fopen("gram1","r");
    int rule,ruleLine = 0;

    while(!feof(fp))
    {
        fscanf(fp,"%s",token);
        gg[ruleLine][0] = grammar_to_enum(token);
        gg[ruleLine][1] = 0;
        rule=2;
        while(1)
        {
            fscanf(fp,"%s",token);
            if(token[0]='\0')
                break;
            gg[ruleLine][rule++]=grammar_to_enum(token);
        }
        gg[ruleLine++][1]=rule-3;
    }
}

void createParseTable(grammar g, table t)
{
    int rule,ruleLine,i;

    for(i=0;i<ruleLine;i++)
    {


    char *f = getFirstSet();

}

int main(void)
{
    grammar g;
    getGrammar(&g);
    return 0;
}
