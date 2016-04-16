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
                        mov eax, 0\n\
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
                        dec bx\n\
                        _getdigitfrombuf:\n\
                        inc bx\n\
                        mov dx, 10\n\
                        mul dx\n\
                        mov dh, 0\n\
                        mov dl, [bx]\n\
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
                        ret\n\
                        _read endp";

// code for the write function
const char *writeProc = "\
                         _write proc\n\
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
                         mov ebx, 10\n\
                         mov bx, 10\n\
                         mov esi, inputbuf\n\
                         mov edx, 0\n\
                         _getdigitfromnum:\n\
                         div bx\n\
                         add ax, 48\n\
                         mov [esi], al\n\
                         inc esi\n\
                         inc cx\n\
                         mov ax, dx\n\
                         \n\
                         cmp ax, bx\n\
                         jg _getdigitfromnum\n\
                         \n\
                         mov ah, 02h\n\
                         \n\
                         _writedigit:\n\
                         mov edx,ecx\n\
                         mov ecx,inpbuffer\n\
                         mov eax,sys_write\n\
                         mov ebx,stdout\n\
                         int 80h\n\
                         ret\n\
                         _write endp";

int ifNo = 0;
char ifNoBuf[11];
int loopNo = 0;
char loopNoBuf[11];

void genCode(parseTree *p, FILE *out) {
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
                        inpbuffer       resb    6\n\
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
                genCode(p->children[1].children[1].children+1, out);
                fprintf(out, "\njmp Exit");
                break;

            case mainFunctions:
                //fprintf(out, "main proc\n\nmov ax, seg inputbuf\nmov ds, ax\n\n");
                genCode(p->children+1, out);
                break;

            case declarations:
                if(p->nochildren != 1) {
                    fprintf(out,"SECTION .bss\n");
                    genCode(p->children, out);
                    genCode(p->children+1, out);
                }
                break;

            case declaration:
                if(p->children[0].isTerminal && p->children[0].term.tokenClass == TK_ID) {
                    fprintf(out, "%s resw 1\n",p->children[0].term.lexeme);
                }
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
                genCode(p->children+2, out);
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
                genCode(p->children+2, out);
                fprintf(out, "mov ");
                genCode(p->children, out);
                fprintf(out, ", ax\n\n");
                break;

            case singleOrRecId:
                genCode(p->children, out);
                if(p->children[1].nochildren != 1) {
                    fprintf(out, "_");
                    genCode(p->children[1].children+1, out);
                }
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
                    fprintf(out, "mov ");
                    genCode(p->children+2, out);
                    fprintf(out, ", ax\n\n");
                }
                else {
                    genCode(&p->children[2], out);
                    fprintf(out, "\ncall _write\n\n");
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
                    fprintf(out, "mov bx, ax\npop ax\n");
                    if(p->children[1].children[0].children[0].term.tokenClass == TK_PLUS)
                        fprintf(out, "add ax, bx\n\n");
                    else
                        fprintf(out, "sub ax, bx\n\n");
                }
                break;

            case term:
                genCode(p->children, out);
                if(p->children[1].nochildren != 1) {
                    fprintf(out, "push ax\n");
                    genCode(p->children+1, out);
                    fprintf(out, "mov bx, ax\npop ax\n");
                    if(p->children[1].children[0].children[0].term.tokenClass == TK_MUL)
                        fprintf(out, "mul bx\n");
                    else
                        fprintf(out, "mov dx, 0\ndiv bx\n");
                }
                break;

            case factor:
                if(p->nochildren != 1) 
                    genCode(p->children+1, out);
                else
                    genCode(p->children+0, out);
                break;

            case termPrime:
                if(p->nochildren != 1) {
                    genCode(p->children+1, out);
                    if(p->children[2].nochildren != 1) {
                        fprintf(out, "push ax\n");
                        genCode(p->children+2, out);
                        fprintf(out, "mov bx, ax\npop ax\n");
                        if(p->children[2].children[0].children[0].term.tokenClass == TK_MUL)
                            fprintf(out, "mul ax, bx\n");
                        else
                            fprintf(out, "div ax, bx\n");
                    }
                }
                break;

            case innerTerm:
                if(p->nochildren != 1) {
                    genCode(p->children+1, out);
                    if(p->children[2].nochildren != 1) {
                        fprintf(out, "push ax\n");
                        genCode(p->children+2, out);
                        fprintf(out, "mov bx, ax\npop ax\n");
                        if(p->children[2].children[0].children[0].term.tokenClass == TK_PLUS)
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
        fprintf(out, "%s", p->term.lexeme);

}
