#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "helper_functions.h"

#define GRAMMAR_FILE "grammar.txt"

struct y *ptr[26][10][3];

void add(struct y **l, char *in)
{
    struct y *node;
    node = (struct y *)malloc(sizeof(struct y));
    bzero(node->a,200);
    strcpy(node->a,in);
    node->next==NULL;

    node->next=*l;
    *l = node;
}

char * first(char a, char b)
{
    struct y **p= ptr[a-65][b-48];
    struct y* head;
    char *c, *i;
    int epsflag = 0;

    char *res = (char *)calloc(3,sizeof(char));

    //terminals
    if(a<65 || a>90)
    {
        res[0]=a;
        res[1]=b;
        return res;
    }
    else if(strlen(p[1]->a)==0)
    {
        head = p[0];
        while(head!=NULL)
        {
            c = head->a;
            int k=0;
            epsflag=0;
            while(1)
            {
                if(c[k]=='\0')
                    break;
                i = first(c[k],c[k+1]);
                if(i[0]=='e'&&i[1]=='2')
                {
                    if(c[k+2]=='\0')
                    {
                        //append epsilon
                        p[1]->eps=2;
                    }
                }
                else
                {
                    strcat(p[1]->a,i);
                    if(c[k]>=65 && c[k]<=90&&ptr[c[k]-65][c[k+1]-48][1]->eps!=2) // for terminals
                        break;
                    else if(c[k]<65 || c[k]>90)
                        break;
                    epsflag+=2;
                }
                k+=2;
            }
            if(epsflag==strlen(c))
                p[1]->eps=2;
            head=head->next;
        }
        return p[1]->a;
    }
    else
        return p[1]->a;
}

char * follow(char a, char b)
{
    int i,j;
    struct y **p, *head; char *c, *d;
    d = (char *)malloc(sizeof(char)*3);
    char *res = (char *)malloc(sizeof(char)*1000);
    bzero(res,1000);
    d[0]=a; d[1]=b; d[2]='\0';

    if(ptr[a-65][b-48][2]->follow==2 || ptr[a-65][b-48][2]->cyclic==2)
        return ptr[a-65][b-48][2]->a;

    ptr[a-65][b-48][2]->cyclic=2;

    for (i = 0; i < 26; i++)
    {
        for (j = 0; j < 10; j++) 
        {
            p = ptr[i][j];
            head=p[0];
            while(head!=NULL)
            {
                c = strstr(head->a,d);
                if(c!=NULL)
                {
                    int pos = strlen(head->a)-strlen(c); //position of pointer
                    if(c[0]-65!=i || c[1]-48!=j)
                    {
                        if(c[2]!='\0') //rules 1 & 2
                        {
                            int e=0;
                            while(1)
                            {
                                if(e>=strlen(c+2))
                                    break;
                                strcat(res,first(c[e+2],c[e+3])); //rule 1
                                if(c[e+2]<65 || c[e+2]>90 || ptr[c[e+2]-65][c[e+3]-48][1]->eps!=2)
                                    break;
                                e+=2;
                            }
                            if(e == strlen(c+2) && e>0 && strlen(c+2)>0) //rule 2
                            {
                                if(ptr[i][j][2]->cyclic==1)
                                {
                                    if(c[0]-65!=i || c[1]-48!=j)
                                        strcat(res,follow((i+65),(j+48)));
                                    //strcat(res,ptr[i][j][2]->a);
                                    ptr[i][j][2]->cyclic=1;
                                }
                            }
                        }
                        else //rule 3
                        {
                            if(ptr[i][j][2]->cyclic==1)
                            {
                                strcat(res,follow((i+65),(j+48)));
                                //strcat(res,ptr[i][j][2]->a);
                                ptr[i][j][2]->cyclic=1;
                            }
                        }
                    }
                }
                head=head->next;
            }
        }
    }
    strcat(res,ptr[a-65][b-48][2]->a);
    return res;
}

void calc()
{
    int i,l,j,k,flag=0;
    char a[524], *b, *c;
    struct y *head;

    FILE *fp = fopen(GRAMMAR_FILE,"r");

    for(i=0;i<26;i++)
        for(j=0;j<10;j++)
        {
            ptr[i][j][0]=NULL;
            ptr[i][j][1]=(struct y *)calloc(1,sizeof(struct y));
            ptr[i][j][2]=(struct y *)calloc(1,sizeof(struct y));
            ptr[i][j][1]->eps=1;
            ptr[i][j][2]->follow=1;
            ptr[i][j][2]->cyclic=1;
            bzero(ptr[i][j][1]->a,1000);
            bzero(ptr[i][j][2]->a,1000);
        }

    while((l=fscanf(fp,"%s",a))!=EOF)
    {
        add(&ptr[a[0]-65][a[1]-48][0],(a+3));
        if(!flag)
        {
            flag=1;
            strcpy(ptr[a[0]-65][a[1]-48][2]->a,"$1\0");
        }
    }

    printf("\nFIRST SETS\n");
    int epsflag = 0;
    //find first set
    for(i=0;i<26;i++)
    {
        for(j=0;j<10;j++)
        {
            head = ptr[i][j][0];
            if(head==NULL)
                continue;
            if(strlen(ptr[i][j][1]->a)!=0)
            {
                printf("%c%c:%s\t%d\n",(i+65),(j+48),ptr[i][j][1]->a,ptr[i][j][1]->eps);
                continue;
            }
            while(head!=NULL)
            {
                k=0;
                epsflag=0;
                b = head->a;
                while(1)
                {
                    if(b[k]=='\0')
                        break; c=first(b[k],b[k+1]);
                    if(c[0]=='e' && c[1]=='2') //epsilon
                    {
                        if(b[k+2]=='\0')
                            ptr[i][j][1]->eps = 2 ;
                    }
                    else
                    {
                        strcat(ptr[i][j][1]->a ,c );
                        if(b[k]>=65&&b[k]<=90&&ptr[b[k]-65][b[k+1]-48][1]->eps!=2)
                            break;
                        else if(b[k]<65 || b[k]>90)
                            break;
                        epsflag+=2;
                    }
                    k+=2;
                }
                if(epsflag==strlen(b))
                {
                    ptr[i][j][1]->eps=2;
                }
                head=head->next;
            }
            printf("%c%c:%s\t%d\n",(i+65),(j+48),ptr[i][j][1]->a,ptr[i][j][1]->eps);
        }
    }

    printf("\nFOLLOW SETS\n");
    struct y **p;
    //find follow set
    for (i = 0; i < 26; i++) 
    {
        for (j = 0; j < 10; j++) 
        {
            p = ptr[i][j];
            if(p[0]!=NULL && p[2]->follow!=2)
            {
                p[2]->follow=1;
                char *c = follow((i+65),(j+48));
                strcat(p[2]->a,c);
                p[2]->follow=2;
                p[2]->cyclic=1;
            }
            if(p[2]->follow==2)
                printf("%c%c:%s\n",(i+65),(j+48),p[2]->a);
        }
    }
    return;
    fclose(fp);
}

void populate()
{
    FILE *fi, *foll;
    int i,j;

    char *a = (char *)malloc(sizeof(char)*40);

    fi = fopen("first","r");
    foll = fopen("foll","r");

    for(i=0;i<50;i++)
    {
        bzero(a,40);
        fscanf(fi,"%s",a);
        ptr[a[0]-65][a[1]-48][1]=(struct y *)malloc(sizeof(struct y));
        ptr[a[0]-65][a[1]-48][1]->eps=1;
        strcpy(ptr[a[0]-65][a[1]-48][1]->a,(a+3));
        bzero(a,30);
        fscanf(foll,"%s",a);
        ptr[a[0]-65][a[1]-48][2]=(struct y *)malloc(sizeof(struct y));
        strcpy(ptr[a[0]-65][a[1]-48][2]->a,(a+3));
    }
    bzero(a,30);
    fscanf(fi,"%s",a);
    for(i=0;i<strlen(a);i+=2)
        ptr[a[i]-65][a[i+1]-48][1]->eps=2;
    free(a);
}

/*
int main(void)
{
    calc();
    return 0;
}
*/
