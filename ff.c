#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct y
{
    char a[20];
    struct y *next;
    char eps;
    char follow;
};

struct y *ptr[26][10][3];

void add(struct y **l, char *in)
{
    struct y *node;
    node = (struct y *)malloc(sizeof(struct y));
    strcpy(node->a,in);
    node->next==NULL;

    node->next=*l;
    *l = node;
}

char * first(char a, char b)
{
    struct y **p= ptr[a-65][b-48];
    struct y* head;
    char *c, *res, *i;

    res = (char *)malloc(2*sizeof(char));

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
                }
                k+=2;
            }
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
    struct y **p, *head; char *c, *d, *res;
    d = (char *)malloc(sizeof(char)*3);
    res = (char *)malloc(sizeof(char)*10);
    d[0]=a; d[1]=b; d[2]='\0';

    if(ptr[a-65][b-48][2]->follow>=2)
        return ptr[a-65][b-48][2]->a;

    ptr[a-65][b-48][2]->follow=3; //follow calculation in progrss. Prevents cycles
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
                            strcat(res,first(c[2],c[3])); //rule 1
                            if((c[2]>=65&&c[2]<=90) && ptr[c[2]-65][c[3]-48][1]->eps==2) //rule 2 and terminal can't have eps in its first()
                                strcat(res,follow((i+65),(j+48)));
                        }
                        else //rule 3
                            strcat(res,follow((i+65),(j+48)));
                    }
                }
                head=head->next;
            }
        }
    }
    //ptr[a-65][b-48][2]->follow=1;
    return res;
}

int main(void)
{
    int i,l,j,k,flag=0;
    char a[524], *b, *c;
    struct y *head;

    for(i=0;i<26;i++)
        for(j=0;j<10;j++)
        {
            ptr[i][j][0]=NULL;
            ptr[i][j][1]=(struct y *)malloc(sizeof(struct y));
            ptr[i][j][2]=(struct y *)malloc(sizeof(struct y));
            ptr[i][j][1]->eps=1;
            ptr[i][j][1]->follow=1;
        }

    while((l=scanf("%s",a))!=EOF)
    {
        add(&ptr[a[0]-65][a[1]-48][0],(a+3));
        if(!flag)
        {
            flag=1;
            strcpy(ptr[a[0]-65][a[1]-48][2]->a,"$1\0");
        }
    }

    printf("\nFIRST SETS\n");
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
                printf("%c%c:%s\n",(i+65),(j+48),ptr[i][j][1]->a);
                continue;
            }
            while(head!=NULL)
            {
                k=0;
                b = head->a;
                while(1)
                {
                    if(b[k]=='\0')
                        break;
                    c=first(b[k],b[k+1]);
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
                    }
                    k+=2;
                }
                head=head->next;
            }
            printf("%c%c:%s\n",(i+65),(j+48),ptr[i][j][1]->a);
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
            if(p[0]!=NULL)
            {
                p[2]->follow=1;
                strcat(p[2]->a,follow((i+65),(j+48)));
                printf("%c%c:%s\n",(i+65),(j+48),p[2]->a);
                p[2]->follow=2;
            }
        }
    }
    return 0;
}
