#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct y
{
    char a[13];
    struct y *next;
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
                if(i[0]=='e'&&i[1]=='1')
                {
                    if(c[k+2]='\0')
                        strcat(p[1]->a,i);
                }
                else
                {
                    strcat(p[1]->a,i);
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

int main(void)
{
    int i,l,j,k;
    char a[524], *b, *c;
    struct y *head;

    for(i=0;i<26;i++)
        for(j=0;j<10;j++)
        {
            ptr[i][j][0]=NULL;
            ptr[i][j][1]=(struct y *)malloc(sizeof(struct y));
            ptr[i][j][2]=(struct y *)malloc(sizeof(struct y));
        }

    while((l=scanf("%s",a))!=EOF)
        add(&ptr[a[0]-65][a[1]-48][0],(a+3));

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
                    if(c[0]=='e' && c[1]=='1') //epsilon
                    {
                        if(b[k+2]=='\0')
                            strcat(ptr[i][j][1]->a ,c );
                    }
                    else
                    {
                        strcat(ptr[i][j][1]->a ,c );
                        break;
                    }
                    k+=2;
                }
                head=head->next;
            }
            printf("%c%c:%s\n",(i+65),(j+48),ptr[i][j][1]->a);
        }
    }
    return 0;
}
