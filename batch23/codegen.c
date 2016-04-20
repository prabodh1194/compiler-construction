/* codegen.c: implements the code generator
 *
 * Group 23 - Prabodh Agarwal (2012B1A7801P), Prabodh Agarwal (2012B1A7801P)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parserDef.h"
#include "lexerDef.h"
#include "helper_functions.h"
#include "symboltableDef.h"
#include "symboltable.h"
#include "populateHashTable.h"

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
                        jne _getstring\n\
                        mov si, 1\n\
                        dec cx\n\
                        jmp _getdigitfromstr\n\
                        _getstring:\n\
                        dec ebx\n\
                        _getdigitfromstr:\n\
                        inc ebx\n\
                        mov dx,10\n\
                        mul dx\n\
                        mov dh, 0\n\
                        mov dl, [ebx]\n\
                        sub dx, 48\n\
                        add ax, dx\n\
                        \n\
                        loop _getdigitfromstr\n\
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
                         je _writebuf\n\
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
                         _writebuf:\n\
                         mov ecx, 0\n\
                         mov ebx, 0\n\
                         mov bx, 10\n\
                         mov esi, inputbuf\n\
                         add esi, buflen\n\
                         mov edx, 0\n\
                         _digitfromnum:\n\
                         div bx\n\
                         add dx, 48\n\
                         dec esi\n\
                         mov [esi], dl\n\
                         mov dx, 0\n\
                         inc cx\n\
                         \n\
                         cmp ax, bx\n\
                         jge _digitfromnum\n\
                         \n\
                         add ax, 48\n\
                         dec esi\n\
                         mov [esi], al\n\
                         inc cx\n\
                         \n\
                         mov edx,ecx\n\
                         mov ecx,esi\n\
                         mov eax,sys_write\n\
                         mov ebx,stdout\n\
                         int 80h\n\
                         \n\
                         mov eax, sys_write\n\
                         mov ebx, stdout\n\
                         mov ecx, newl\n\
                         mov edx, newlen\n\
                         int 80h\n\
                         ret\n";

int ifNo = 0, loopNo = 0, boolNo = 0;

void genCode(astree *p, char *field, FILE *out) {
    int i;
    identifier_list *fields, *id;
    char *type;
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
                        minuslen        equ     $-minus\n\
                        newl            db      10\n\
                        newlen          equ     $-newl\n\
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

                for(i = 0; i < p->nochildren; i++)
                    if(p->children[i].nonterm == mainFunctions)
                        break;

                genCode(p->children+i, field, out);
                fprintf(out, "\njmp Exit\n");
                fprintf(out,"SECTION .bss\n");
                if(p->children[i].children[0].children[0].nonterm == declarations)
                    genCode(p->children[i].children[0].children, field, out);
                else if(p->children[i].children[0].children[1].nonterm == declarations)
                    genCode(p->children[i].children[0].children+1, field, out);
                break;

            case mainFunctions:
                //fprintf(out, "main proc\n\nmov ax, seg inputbuf\nmov ds, ax\n\n");
                genCode(p->children, field, out);
                break;

            case declarations:
                if(p->nochildren != 1) {
                    genCode(p->children, field, out);
                    genCode(p->children+1, field, out);
                }
                else
                    genCode(p->children, field, out);
                break;

            case declaration:
                if(p->children[0].isTerminal && p->children[1].term.tokenClass == TK_ID) 
                    fprintf(out, "%s resw 1\n",p->children[1].term.lexeme);
                else {
                    fields = get_record_fields(record, p->children[0].children[0].children[1].term.lexeme);
                    for(; fields!=NULL; fields = fields->next)
                    {
                        if(fields->name == NULL)
                            continue;
                        fprintf(out, "%s_%s resw 1\n", p->children[1].term.lexeme, fields->name);
                    }
                }
                break;

            case stmts:
                fprintf(out, "_start:\n");
                for(i = 0; i < p->nochildren; i++)
                    if(p->children[i].nonterm == otherStmts)
                        break;
                genCode(p->children+i, field, out);
                //fprintf(out, "mov ah, 4ch\nmov al, 0\nint 21h\n\nmain endp\n\n");
                break;

            case otherStmts:
                for (i = 0; i < p->nochildren; i++) 
                {
                    genCode(p->children+i, field, out);                    
                }
                break;

            case stmt:
                genCode(p->children, field, out);
                break;

            case assignmentStmt:
                if(!p->children[0].isTerminal || p->type == TK_INT || p->type == TK_REAL)
                {
                    genCode(p->children+2, field, out);
                    if(!p->children[0].isTerminal && p->children[0].nonterm == singleOrRecId)
                        genCode(p->children, field, out);
                    else
                        fprintf(out, "mov [%s], ax\n\n",p->children[0].term.lexeme);
                }
                else
                {
                    id = search_function_wise_identifier_hashtable(local, "_main\0", p->children[0].term.lexeme);
                    if(id == NULL)
                        id = search_global_identifier(global, p->children[0].term.lexeme);
                    type = id->type;
                    fields = get_record_fields(record, type);
                    for(;fields!=NULL;fields=fields->next)
                    {
                        if(fields->name == NULL)
                            continue;
                        genCode(p->children+2, fields->name, out);
                        fprintf(out, "mov [%s_%s], ax\n\n",p->children[0].term.lexeme, fields->name);
                    }
                }
                break;

            case singleOrRecId:
                fprintf(out, "mov [%s_%s],ax\n",p->children[0].term.lexeme, p->children[1].term.lexeme);
                break;

            case conditionalStmt:
                genCode(p->children, field, out);
                fprintf(out, "cmp ax, 0\n");
                fprintf(out, "je ELSE%d\n", ifNo);
                genCode(p->children+2, field, out);

                if(p->children[3].nonterm == otherStmts)
                    genCode(p->children+3, field, out);

                fprintf(out, "jmp ENDIF%d\n", ifNo);
                fprintf(out, "ELSE%d:\n", ifNo);

                if(p->children[3].nonterm == elseStmt)
                    genCode(p->children+3, field, out);
                else if(p->children[4].nonterm == elseStmt)
                    genCode(p->children+4, field, out);

                fprintf(out, "ENDIF%d:\n\n", ifNo);
                ifNo++;
                break;

            case elseStmt:
                if(p->nochildren != 1) {
                    genCode(p->children+1, field, out);
                    genCode(p->children+2, field, out);
                }
                break;

            case ioStmt:
                if(p->children[0].term.tokenClass == TK_READ) {
                    fprintf(out, "call _read\n");
                    if(p->children[1].term.tokenClass == TK_ID)
                        fprintf(out,"mov [%s],ax\n",p->children[1].term.lexeme);
                    else
                        genCode(p->children+1, field, out);
                }
                else {
                    type = id->type;
                    if(p->type == TK_INT || p->type == TK_REAL)
                    {
                        genCode(p->children+1, field, out);
                        fprintf(out, "call _write\n\n");
                    }
                    else
                    {
                        id = search_function_wise_identifier_hashtable(local, "_main\0", p->children[1].term.lexeme);
                        if(id == NULL)
                            id = search_global_identifier(global, p->children[1].term.lexeme);
                        type = id->type;
                        identifier_list *fields = get_record_fields(record, type);
                        for(; fields != NULL; fields = fields->next)
                        {
                            if(fields->name == NULL)
                                continue;
                            fprintf(out, "mov ax,[%s_%s]\n",p->children[1].term.lexeme, fields->name);
                            fprintf(out, "call _write\n\n");
                        }
                    }
                }
                break;

            case iterativeStmt:
                fprintf(out, "STARTLOOP%d:\n", loopNo);
                genCode(p->children, field, out);
                fprintf(out, "cmp ax, 0\nje ENDLOOP%d\n", loopNo);
                genCode(p->children+1, field, out);
                genCode(p->children+2, field, out);
                fprintf(out, "jmp STARTLOOP%d\nENDLOOP%d:\n\n", loopNo, loopNo);
                loopNo++;
                break;

            case allVar:
                if(p->nochildren == 1)
                    genCode(p->children, field, out);
                break;

            case var:
                genCode(p->children, field, out);
                break;

            case arithmeticExpression:
                genCode(p->children, field, out);
                if(p->children[1].nochildren != 1) {
                    fprintf(out, "push ax\n");
                    genCode(p->children+1, field, out);
                }
                break;

            case term:
                genCode(p->children, field, out);
                if(p->children[1].nochildren != 1) {
                    fprintf(out, "push ax\n");
                    genCode(p->children+1, field, out);
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
                    genCode(p->children+1, field, out);
                else
                    genCode(p->children+0, field, out);
                break;

            case termPrime:
                if(p->nochildren == 2)
                {
                    if(p->children[1].term.tokenClass == TK_ID)
                        fprintf(out,"mov bx, [%s]\n",p->children[1].term.lexeme);
                    else
                        fprintf(out,"mov bx, %s\n",p->children[1].term.lexeme);
                    fprintf(out,"pop ax\n");
                    if(p->children[0].term.tokenClass == TK_MUL)
                        fprintf(out,"mul bx\n");
                    else
                        fprintf(out,"div bx\n");
                }
                else if(p->nochildren == 3) {
                    genCode(p->children+1, field, out);
                    if(p->children[2].nochildren != 1) {
                        fprintf(out, "push ax\n");
                        genCode(p->children+2, field, out);
                        fprintf(out, "mov bx, ax\npop ax\n");
                        if(p->children[2].children[0].children[0].term.tokenClass == TK_MUL)
                            fprintf(out, "mul bx\n");
                        else
                            fprintf(out, "div bx\n");
                    }
                }
                break;

            case innerTerm:
                if(p->nochildren >= 2) {
                    genCode(p->children+1, field, out);
                    if(p->children[2].nochildren != 1) {
                        if(p->nochildren == 3)
                        {
                            fprintf(out, "push ax\n");
                            genCode(p->children+2, field, out);
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
                    genCode(p->children, field, out);
                    fprintf(out, "\n");
                }
                break;

            case booleanExpression:
                if(p->children[0].nonterm == booleanExpression) {
                    genCode(p->children, field, out);
                    fprintf(out, "push ax\n");
                    genCode(p->children+2, field, out);
                    fprintf(out, "pop bx\n");
                    if(p->children[1].term.tokenClass == TK_AND)
                        fprintf(out, "and ax, bx\n\n");
                    else
                        fprintf(out, "or ax, bx\n\n");
                }
                else if(p->children[0].term.tokenClass == TK_NOT) {
                    genCode(p->children+1, field, out);
                    fprintf(out, "not ax\n\n");
                }
                else {
                    genCode(p->children, field, out);
                    fprintf(out, "\npush ax\n");
                    genCode(p->children+2, field, out);
                    fprintf(out, "\nmov bx, ax\npop ax\ncmp ax, bx\n");
                    //fprintf(out, "pushf\npop eax\n");
                    switch(p->children[1].term.tokenClass) {
                        case TK_LT:
                            fprintf(out, "jl true%d\n",boolNo);
                            break;

                        case TK_LE:
                            fprintf(out, "jle true%d\n",boolNo);
                            break;

                        case TK_EQ:
                            fprintf(out, "je true%d\n",boolNo);
                            break;

                        case TK_GT:
                            fprintf(out, "jg true%d\n",boolNo);
                            break;

                        case TK_GE:
                            fprintf(out, "jge true%d\n",boolNo);
                            break;

                        case TK_NE:
                            fprintf(out, "jne true%d\n",boolNo);
                            break;
                    }
                    fprintf(out, "false%d:\n\
                            mov ax, 0\n\
                            jmp next%d\n\
                            true%d:\n\
                            mov ax, 1\n\
                            next%d:\n", boolNo, boolNo, boolNo, boolNo);
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
                fprintf(out,"mov ax,[%s",p->term.lexeme);
                id = search_function_wise_identifier_hashtable(local, "_main", p->term.lexeme);
                if(id == NULL)
                    id = search_global_identifier(global, p->term.lexeme);
                type = id->type;
                if(strcmp(type,"int")==0 || strcmp(type, "real")==0)
                    fprintf(out,"]\n");
                else
                    fprintf(out,"_%s]\n",field);

                break;
            case TK_RNUM:
            case TK_NUM:
                fprintf(out,"mov ax,%s\n",p->term.lexeme);
                break;
        }
    }

}
