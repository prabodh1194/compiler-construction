/* codegen.c: implements the code generator
 *
 * Group 23 - Prabodh Agarwal (2012B1A7801P), Naresh Raghuraman (2010A7PS090P)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parserDef.h"
#include "lexerDef.h"
#include "helper_functions.h"
#include "symboltableDef.h"

// code for the read function
const char *exitProc = "Exit:\n\
                        mov eax, sys_exit\n\
                        xor ebx, ebx\n\
                        int 80H\n";

const char *readProc = "\
                        _read:\n\
                        push si\n\
                        mov ecx, inputbuf\n\
                        mov edx, buflen\n\
                        mov ebx, stdin\n\
                        mov eax, sys_read\n\
                        int 80h\n\
                        \n\
                        mov ebx, inputbuf\n\
                        mov ecx, eax\n\
                        sub ecx, 1\n\
                        mov eax, 0\n\
                        mov edx, 0\n\
                        mov bp, 10\n\
                        \n\
                        mov si, 0\n\
                        \n\
                        mov dl, [ebx]\n\
                        cmp dl, 45\n\
                        jne _startgetting\n\
                        mov si, 1\n\
                        dec cx\n\
                        jmp _getdigitfrombuf\n\
                        _startgetting:\n\
                        dec ebx\n\
                        _getdigitfrombuf:\n\
                        inc ebx\n\
                        mov dx,10\n\
                        mul dx\n\
                        mov dh, 0\n\
                        mov dl, [ebx]\n\
                        sub dx, 48\n\
                        add ax, dx\n\
                        \n\
                        loop _getdigitfrombuf\n\
                        \n\
                        cmp si, 1\n\
                        jne _readend\n\
                        not ax\n\
                        add ax, 1\n\
                        \n\
                        _readend:\n\
                        pop si\n\
                        ret\n";

// code for the write function
const char *writeProc = "\
                         _write:\n\
                         mov bx, ax\n\
                         and bx, 8000h\n\
                         cmp bx, 0\n\
                         je _writesignhandled\n\
                         not ax\n\
                         add ax, 1\n\
                         push ax\n\
                         mov ecx,minus\n\
                         mov edx,minuslen\n\
                         mov eax, sys_write\n\
                         mov ebx, stdout\n\
                         int 80h\n\
                         pop ax\n\
                         \n\
                         _writesignhandled:\n\
                         mov ecx, 0\n\
                         mov ebx, 0\n\
                         mov bx, 10\n\
                         mov esi, inputbuf\n\
                         add esi, buflen\n\
                         mov edx, 0\n\
                         _getdigitfromnum:\n\
                         div bx\n\
                         add dx, 48\n\
                         dec esi\n\
                         mov [esi], dl\n\
                         mov dx, 0\n\
                         inc cx\n\
                         \n\
                         cmp ax, bx\n\
                         jg _getdigitfromnum\n\
                         \n\
                         add ax, 48\n\
                         dec esi\n\
                         mov [esi], al\n\
                         inc cx\n\
                         \n\
                         _writedigit:\n\
                         mov edx,ecx\n\
                         mov ecx,esi\n\
                         mov eax,sys_write\n\
                         mov ebx,stdout\n\
                         int 80h\n\
                         ret\n";

int ifNo = 0;
char ifNoBuf[11];
int loopNo = 0;
char loopNoBuf[11];

void genCode(astree *p, FILE *out) {
    int i;
    if(!p->isTerminal) {
        switch(p->nonterm) {
            case program:
                fprintf(out, "\
                        sys_exit        equ     1\n\
                        sys_read        equ     3\n\
                        sys_write       equ     4\n\
                        stdin           equ     0\n\
                        stdout          equ     1\n\
                        stderr          equ     3\n\
                        \n\
                        SECTION         .data\n\
                        minus           db      \"-\",0\n\
                        minuslen       equ     $-minus\n\
                        \n\
                        SECTION         .bss\n\
                        inputbuf:       resb    6\n\
                        buflen          equ     $-inputbuf\n\
                        \n\
                        SECTION         .text\n\
                        global          _start\n\
                        \n\
                        %s\n\
                        \n\
                        %s\n\
                        \n\
                        %s\n", readProc, writeProc, exitProc);
                genCode(p->children+1, out);
                fprintf(out, "\njmp Exit\n");
                fprintf(out,"SECTION .bss\n");
                if(p->children[1].children[0].children[0].nonterm == declarations, out)
                    genCode(p->children[1].children[0].children, out);
                else if(p->children[1].children[0].children[1].nonterm == declarations, out)
                    genCode(p->children[1].children[0].children+1, out);
                break;

            case mainFunctions:
                //fprintf(out, "main proc\n\nmov ax, seg inputbuf\nmov ds, ax\n\n");
                genCode(p->children, out);
                break;

            case declarations:
                if(p->nochildren != 1) {
                    genCode(p->children, out);
                    genCode(p->children+1, out);
                }
                else
                    genCode(p->children, out);
                break;

            case declaration:
                if(p->children[1].isTerminal && p->children[1].term.tokenClass == TK_ID) 
                    fprintf(out, "%s: resw 1\n",p->children[1].term.lexeme);
                /*
                else {
                    function_hashtable *result, *iter;
                    result = findRecordDetails(recs,  p->children[1].children[0].children[1].term.lexeme, result);
                    for(iter = result; iter != NULL; iter = iter->next) {
                        genCode(&p->children[3], out);
                        fprintf(out, "_%s resw 1\n", iter->identifier.name);
                    }
                }
                */
                break;

            case stmts:
                fprintf(out, "_start:\n");
                for(i = 0; i < p->nochildren; i++)
                    if(p->children[i].nonterm == otherStmts)
                        break;
                genCode(p->children+i, out);
                //fprintf(out, "mov ah, 4ch\nmov al, 0\nint 21h\n\nmain endp\n\n");
                break;

            case otherStmts:
                if(p->nochildren != 1) {
                    // not eps
                    genCode(p->children, out);
                    genCode(p->children+1, out);
                }
                break;

            case stmt:
                genCode(p->children, out);
                break;

            case assignmentStmt:
                for (i = 2; i < p->nochildren; i++) 
                {
                    genCode(p->children+i, out);
                }
                fprintf(out, "mov [%s], ax\n\n",p->children[0].term.lexeme);
                break;

            case singleOrRecId:
                    fprintf(out, "mov %s_%s,ax\n",p->children[0].term.lexeme, p->children[1].term.lexeme);
                break;

            case conditionalStmt:
                snprintf(ifNoBuf, 10, "%d", ifNo);
                genCode(p->children+2, out);
                fprintf(out, "cmp ax, 0\n");
                fprintf(out, "je ELSE%s\n", ifNoBuf);
                genCode(p->children+5, out);
                genCode(p->children+6, out);
                fprintf(out, "jmp ENDIF%s\n", ifNoBuf);
                fprintf(out, "ELSE%s:\n", ifNoBuf);
                genCode(p->children+7, out);
                fprintf(out, "ENDIF%s:\n\n", ifNoBuf);
                ifNo++;
                break;

            case elseStmt:
                if(p->nochildren != 1) {
                    genCode(p->children+1, out);
                    genCode(p->children+2, out);
                }
                break;

            case ioStmt:
                if(p->children[0].term.tokenClass == TK_READ) {
                    fprintf(out, "call _read\n");
                    if(p->children[1].term.tokenClass == TK_ID)
                        fprintf(out,"mov [%s],ax\n",p->children[1].term.lexeme);
                    else
                        genCode(p->children+1, out);
                }
                else {
                    if(p->children[1].term.tokenClass == TK_ID)
                    {
                        genCode(p->children+1, out);
                        fprintf(out, "call _write\n\n");
                    }
                    else
                    {
                        fprintf(out, "call _write\n\n");
                    }
                }
                break;

            case iterativeStmt:
                snprintf(loopNoBuf, 10, "%d", loopNo);
                fprintf(out, "STARTLOOP%s:\n", loopNoBuf);
                genCode(p->children+2, out);
                fprintf(out, "cmp ax, 0\nje ENDLOOP%s\n", loopNoBuf);
                genCode(p->children+4, out);
                genCode(p->children+5, out);
                fprintf(out, "jmp STARTLOOP%s\nENDLOOP%s:\n\n", loopNoBuf, loopNoBuf);
                loopNo++;
                break;

            case allVar:
                if(p->nochildren == 1)
                    genCode(p->children+0, out);
                break;

            case var:
                genCode(p->children+0, out);
                break;

            case arithmeticExpression:
                genCode(p->children, out);
                if(p->children[1].nochildren != 1) {
                    fprintf(out, "push ax\n");
                    genCode(p->children+1, out);
                }
                break;

            case term:
                genCode(p->children, out);
                if(p->children[1].nochildren != 1) {
                    fprintf(out, "push ax\n");
                    genCode(p->children+1, out);
                    /*
                    fprintf(out, "mov bx, ax\npop ax\n");
                    if(p->children[1].children[0].children[0].term.tokenClass == TK_MUL)
                        fprintf(out, "mul bx\n");
                    else
                        fprintf(out, "mov dx, 0\ndiv bx\n");
                        */
                }
                break;

            case factor:
                if(p->nochildren != 1) 
                    genCode(p->children+1, out);
                else
                    genCode(p->children+0, out);
                break;

            case termPrime:
                if(p->nochildren == 2)
                {
                    fprintf(out,"mov bx, [%s]\n",p->children[1].term.lexeme);
                    fprintf(out,"pop ax\n");
                    if(p->children[0].term.tokenClass == TK_MUL)
                        fprintf(out,"mul bx\n");
                    else
                        fprintf(out,"div bx\n");
                }
                else if(p->nochildren == 3) {
                    genCode(p->children+1, out);
                    if(p->children[2].nochildren != 1) {
                        fprintf(out, "push ax\n");
                        genCode(p->children+2, out);
                        fprintf(out, "mov bx, ax\npop ax\n");
                        if(p->children[2].children[0].children[0].term.tokenClass == TK_MUL)
                            fprintf(out, "mul bx\n");
                        else
                            fprintf(out, "div bx\n");
                    }
                }
                break;

            case innerTerm:
                if(p->nochildren != 1) {
                    genCode(p->children+1, out);
                    if(p->children[2].nochildren != 1) {
                        if(p->nochildren == 3)
                        {
                            fprintf(out, "push ax\n");
                            genCode(p->children+2, out);
                        }
                        fprintf(out, "mov bx, ax\npop ax\n");
                        if(p->children[0].term.tokenClass == TK_PLUS)
                            fprintf(out, "add ax, bx\n");
                        else
                            fprintf(out, "sub ax, bx\n");
                    }
                }
                break;

            case all:
                if(p->nochildren == 1 || p->children[1].nochildren == 1) {
                    fprintf(out, "mov ax, ");
                    genCode(p->children, out);
                    fprintf(out, "\n");
                }
                break;

            case booleanExpression:
                if(p->children[0].term.tokenClass == TK_OP) {
                    genCode(p->children+1, out);
                    fprintf(out, "push ax\n");
                    genCode(p->children+5, out);
                    fprintf(out, "pop bx\n");
                    if(p->children[3].children[0].term.tokenClass == TK_AND)
                        fprintf(out, "and ax, bx\n\n");
                    else
                        fprintf(out, "or ax, bx\n\n");
                }
                else if(p->children[0].term.tokenClass == TK_NOT) {
                    genCode(p->children+1, out);
                    fprintf(out, "not ax\n\n");
                }
                else {
                    fprintf(out, "mov ax, ");
                    genCode(p->children, out);
                    fprintf(out, "\npush ax\nmov ax, ");
                    genCode(p->children+2, out);
                    fprintf(out, "\nmov bx, ax\npop ax\ncmp ax, bx\n");
                    fprintf(out, "pushf\npop ax\n");
                    switch(p->children[1].children[0].term.tokenClass) {
                        case TK_LT:
                            fprintf(out, "and ax, 0880h\nmov cl, 3\nshr ah, cl\n");
                            fprintf(out, "mov cl, 7\nshr al, cl\n");
                            fprintf(out, "xor al, ah\nmov ah, 0\n\n");
                            break;

                        case TK_LE:
                            fprintf(out, "mov bl, al\nand ax, 0880h\nmov cl, 3\nshr ah, cl\n");
                            fprintf(out, "mov cl, 7\nshr al, cl\n");
                            fprintf(out, "xor al, ah\nand bl, 40h\nmov cl, 6\nshr bl, cl\n");
                            fprintf(out, "or al, bl\nmov ah, 0\n\n");
                            break;

                        case TK_EQ:
                            fprintf(out, "and ax, 0040h\nmov cl, 6\nshr al, cl\n\n");
                            break;

                        case TK_GT:
                            fprintf(out, "mov bl, al\nand ax, 0880h\nmov cl, 3\nshr ah, cl\n");
                            fprintf(out, "mov cl, 7\nshr al, cl\n");
                            fprintf(out, "xor al, ah\nnot al\nand al, 01h\nnot bl\nand bl, 40h\n");
                            fprintf(out, "mov cl, 6\nshr bl, cl\n");
                            fprintf(out, "and al, bl\nmov ah, 0\n\n");
                            break;

                        case TK_GE:
                            fprintf(out, "and ax, 0880h\nmov cl, 3\nshr ah, cl\n");
                            fprintf(out, "mov cl, 7\nshr al, cl\n");
                            fprintf(out, "xor al, ah\nnot al\nand ax, 0001h\n\n");
                            break;

                        case TK_NE:
                            fprintf(out, "and ax, 0040h\nmov cl, 6\nshr al, cl\n");
                            fprintf(out, "not al\nand ax, 0001h\n\n");
                            break;
                    }
                }                        

            default:
                ;
        }
    }

    else
    {
        switch(p->term.tokenClass)
        {
            case TK_ID:
                fprintf(out,"mov ax,[%s]\n",p->term.lexeme);
                break;
            case TK_RNUM:
            case TK_NUM:
                fprintf(out,"mov ax,%s\n",p->term.lexeme);
                break;
        }
    }

}
