#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "helper_functions.h"
#include "lexerDef.h"


char buffer[MAX_BUFFER_SIZE];
unsigned long line_num = 1;
int pos_in_buffer;
int num_char_read = -1;
hashtable *h;

char getStream(FILE *fp){

	if(pos_in_buffer == MAX_BUFFER_SIZE || num_char_read == -1) {
        bzero(buffer,MAX_BUFFER_SIZE);
        num_char_read = fread(buffer, sizeof(char), (size_t)MAX_BUFFER_SIZE, fp);
        pos_in_buffer = 1;
        if(num_char_read == 0)
            return 3; // EOF will be represented by 3
        else
            return buffer[0];
    }

    if(num_char_read == 0) // EOF will be represented by 3
        return 3;
    else
        return buffer[pos_in_buffer++];

}

void getNextToken(FILE *fp, tokenInfo* t){
    bzero(t->lexeme, MAX_LEXEME_SIZE);
	char c;
    int pos_in_lexeme=0;
	int hash_value, s = 0;
	row* current_pointer;
    while (1) {
		c = getStream(fp);
		t->lexeme[pos_in_lexeme++] = c;
        //printf("%s\n",t->lexeme);
		t->line_num = line_num;
        //printf("s= %d line_num = %lu c= %c pos_in_buffer=%d\n",s,line_num,c,pos_in_buffer);
		switch(s){
			case 0:
				switch(c){
					case 'b':
					case 'd':
						s = 1; //done
						break;	 
					case 'c':
                        s = 39; //done
                        break;
                    case '_':
                        s = 43; //done
                        break;
                    case 'a':
                    case 'e':
                    case 'f':
                    case 'g':
                    case 'h':
                    case 'i':
                    case 'j':
                    case 'k':
                    case 'l':
                    case 'm':
                    case 'n':
                    case 'o':
                    case 'p':
                    case 'q':
                    case 'r':
                    case 's':
                    case 't':
                    case 'u':
                    case 'v':
                    case 'w':
                    case 'x':
                    case 'y':
                    case 'z':
                        s = 189; //done
                        break;
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        s = 220;
                        break;
                    case '#':
                        s = 223; // done
                        break;
                    case '%':
                    	s = 204; //done
                    	//t->tokenClass = TK_COMMENT;
                    	break;
                    case '[': // TK_SQL found
                    	s = 205; //FINAL STATE
                        t->tokenClass = TK_SQL;
                        return;
                    case ']': // TK_SQR found
                    	s = 206; //FINAL STATE
                        t->tokenClass = TK_SQR;
                        return;
                    case ';': // TK_SEM found
                    	s = 207; //FINAL STATE
                        t->tokenClass = TK_SEM;
                        return;
                    case ':': // TK_COLON found
                    	s = 208; //FINAL STATE
                        t->tokenClass = TK_COLON;
                        return;
                    case '.': // TK_DOT found
                    	s = 209; //FINAL STATE
                        t->tokenClass = TK_DOT;
                        return;
                    case '(': // TK_OP found
                    	s = 210; //FINAL STATE
                        t->tokenClass = TK_OP;
                        return;
                    case ')': // TK_CL found
                    	s = 211; //FINAL STATE
                        t->tokenClass = TK_CL;
                        return;
                    case '+': // TK_PLUS found
                    	s = 212; //FINAL STATE
                        t->tokenClass = TK_PLUS;
                        return;
                    case '-': // TK_MINUS found
                    	s = 213; //FINAL STATE
                        t->tokenClass = TK_MINUS;
                        return;
                    case '*': // TK_MUL found
                    	s = 214; //FINAL STATE
                        t->tokenClass = TK_MUL;
                        return;
                    case '/': // TK_DIV found
                    	s = 215; //FINAL STATE
                        t->tokenClass = TK_DIV;
                        return;
                    case '~':
                    	s = 216; //FINAL STATE
                    	t->tokenClass = TK_NOT;
                    	return;
                    case ',': // TK_COMMA found
                    	s = 21; //FINAL STATE
                        t->tokenClass = TK_COMMA;
                        return;
                    case '&':
                        s = 217; //done
                        break;
                    case '@':
                        s = 201; //done
                        break;
                    case '<':
                    	s = 190; //done
                    	break;
                    case '=':
                        s = 195; //done
                        break;
                    case '>':
                        s = 197; //done
                        break;
                    case '!':
                        s = 13; //done
                        break;
                    case '\n':
                        line_num++;
                    case ' ':
                    case '\t':
                    case '\r':
                    case '\f':
                    case '\v':
                    case 0:
                        s = 225;
                        pos_in_lexeme--;
                        break;
                    case 3: // EOF
                        t->tokenClass = TK_EOF;
                        return;
                    default: // Unknown character found
                        t->tokenClass = TK_ERROR;
                        //printf("%d", (int)c);
                        sprintf(t->lexeme, "Unknown character: %c", c); // error messages also beign put in tokenInfo.lexeme: be wary of MAX_LEXEME_SIZE
                        t->line_num = line_num;
                        return;
				}//end switch inner for case 0  of switch_outer
				break;
			
			case 1:
				switch(c){
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
						s = 2; //done
						break;
					case 'a':
                    case 'b':
                    case 'c':
                    case 'd':
                    case 'e':
                    case 'f':
                    case 'g':
                    case 'h':
                    case 'i':
                    case 'j':
                    case 'k':
                    case 'l':
                    case 'm':
                    case 'n':
                    case 'o':
                    case 'p':
                    case 'q':
                    case 'r':
                    case 's':
                    case 't':
                    case 'u':
                    case 'v':
                    case 'w':
                    case 'x':
                    case 'y':
                    case 'z':
                        s = 189; //done
                        break;
					default: // TK_FIELDID found
                        pos_in_buffer--; // the current character is part of the next token
                        t->tokenClass = TK_FIELDID;
                        t->lexeme[pos_in_lexeme - 1] = '\0'; 
                        return;
				}//end switch inner for case 1  of switch_outer
				break;
			case 2:
				switch(c){
					case 'b':
					case 'c':
					case 'd':
						if(pos_in_lexeme == 21) {
                            // LENGTH OF ID > 20
                            while(c >= 'b' && c <= 'd') c = getStream(fp);
                            while(c >= '2' && c <= '7') c = getStream(fp);
                            pos_in_buffer--;
                            t->tokenClass = TK_ERROR;
                            t->lexeme[21] = '\0';
                            sprintf(t->lexeme, "%s...: identifier too long", t->lexeme);
                            return;
                        }
                        break;
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
						if(pos_in_lexeme == 21) {
                            // LENGTH OF ID > 20
                            while(c >= '2' && c <= '7') c = getStream(fp);
                            pos_in_buffer--;
                            t->tokenClass = TK_ERROR;
                            t->lexeme[21] = '\0';
                            sprintf(t->lexeme, "%s...: identifier too long", t->lexeme);
                            return;
                        }
                        s = 21; // done
                        break;
                    default: // TK_ID found
                        pos_in_buffer--;
                        t->tokenClass = TK_ID;
                        t->lexeme[pos_in_lexeme - 1] = '\0';
                        return;
				}
				break;
			case 21:
				switch(c){
					case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                        if(pos_in_lexeme == 21) {
                            // LENGTH OF ID > 20
                            while(c >= '2' && c <= '7') c = getStream(fp);
                            pos_in_buffer--;
                            t->tokenClass = TK_ERROR;
                            t->lexeme[21] = '\0';
                            sprintf(t->lexeme, "%s...: identifier too long", t->lexeme);
                            return;
                        }
                        break;
                    default: // TK_ID found
                        pos_in_buffer--;
                        t->tokenClass = TK_ID;
                        t->lexeme[pos_in_lexeme - 1] = '\0';
                        return;
                }
                break;
            case 39:
                switch(c){
                    case 'a':
                        s = 40;
                        break;
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                        s = 2;
                        break;
                    case 'b':
                    case 'c':
                    case 'd':
                    case 'e':
                    case 'f':
                    case 'g':
                    case 'h':
                    case 'i':
                    case 'j':
                    case 'k':
                    case 'l':
                    case 'm':
                    case 'n':
                    case 'o':
                    case 'p':
                    case 'q':
                    case 'r':
                    case 's':
                    case 't':
                    case 'u':
                    case 'v':
                    case 'w':
                    case 'x':
                    case 'y':
                    case 'z':
                        s = 189;
                        break;
                    default: // TK_FIELDID found
                        s = 189;
                        pos_in_buffer--; // the current character is part of the next token
                        t->tokenClass = TK_FIELDID;
                        t->lexeme[pos_in_lexeme - 1] = '\0'; 
                        return;
                }
                break;
            case 40:
                switch(c){
                    case 'l':
                        s = 41;
                        break;
                    case 'a':
                    case 'b':
                    case 'c':
                    case 'd':
                    case 'e':
                    case 'f':
                    case 'g':
                    case 'h':
                    case 'i':
                    case 'j':
                    case 'k':
                    case 'm':
                    case 'n':
                    case 'o':
                    case 'p':
                    case 'q':
                    case 'r':
                    case 's':
                    case 't':
                    case 'u':
                    case 'v':
                    case 'w':
                    case 'x':
                    case 'y':
                    case 'z':
                        s = 189;
                        break;
                    default: // TK_FIELDID found
                        s = 189;
                        pos_in_buffer--; // the current character is part of the next token
                        t->tokenClass = TK_FIELDID;
                        t->lexeme[pos_in_lexeme - 1] = '\0'; 
                        return;
                }
                break;
            case 41:
                switch(c){
                    case 'l':
                        s = 42;
                        break;
                    case 'a':
                    case 'b':
                    case 'c':
                    case 'd':
                    case 'e':
                    case 'f':
                    case 'g':
                    case 'h':
                    case 'i':
                    case 'j':
                    case 'k':
                    case 'm':
                    case 'n':
                    case 'o':
                    case 'p':
                    case 'q':
                    case 'r':
                    case 's':
                    case 't':
                    case 'u':
                    case 'v':
                    case 'w':
                    case 'x':
                    case 'y':
                    case 'z':
                        s = 189;
                        break;
                    default: // TK_FIELDID found
                        pos_in_buffer--; // the current character is part of the next token
                        t->tokenClass = TK_FIELDID;
                        t->lexeme[pos_in_lexeme - 1] = '\0'; 
                        return;
                }
                break;
            case 42:
                switch(c){
                    case 'a':
                    case 'b':
                    case 'c':
                    case 'd':
                    case 'e':
                    case 'f':
                    case 'g':
                    case 'h':
                    case 'i':
                    case 'j':
                    case 'k':
                    case 'l':
                    case 'm':
                    case 'n':
                    case 'o':
                    case 'p':
                    case 'q':
                    case 'r':
                    case 's':
                    case 't':
                    case 'u':
                    case 'v':
                    case 'w':
                    case 'x':
                    case 'y':
                    case 'z':
                        s = 189;
                        break;
                    default:
                        pos_in_buffer--; // the current character is part of the next token
                        t->tokenClass = TK_CALL;
                        t->lexeme[pos_in_lexeme - 1] = '\0'; 
                        return;
                }//end inner switch of case 42 of outer switch
                break;
            case 43:
                switch(c) {
                    case 'a':
                    case 'b':
                    case 'c':
                    case 'd':
                    case 'e':
                    case 'f':
                    case 'g':
                    case 'h':
                    case 'i':
                    case 'j':
                    case 'k':
                    case 'l':
                    case 'm':
                    case 'n':
                    case 'o':
                    case 'p':
                    case 'q':
                    case 'r':
                    case 's':
                    case 't':
                    case 'u':
                    case 'v':
                    case 'w':
                    case 'x':
                    case 'y':
                    case 'z':
                    case 'A':
                    case 'B':
                    case 'C':
                    case 'D':
                    case 'E':
                    case 'F':
                    case 'G':
                    case 'H':
                    case 'I':
                    case 'J':
                    case 'K':
                    case 'L':
                    case 'M':
                    case 'N':
                    case 'O':
                    case 'P':
                    case 'Q':
                    case 'R':
                    case 'S':
                    case 'T':
                    case 'U':
                    case 'V':
                    case 'W':
                    case 'X':
                    case 'Y':
                    case 'Z':
                        s = 44;
                        break;
                    default:
                        if(sanitizeError(c))
                            strcpy(t->lexeme, "unexpected character after '_'");
                        else
                            sprintf(t->lexeme, "unexpected character: '%c' after '_'", c);
                        t->tokenClass = TK_ERROR;
                        t->line_num = line_num; 
                        if(c == '\n')
                            line_num++;
                        return;
                }
                break;
                
            case 44:
                switch(c) {
                    case 'a':
                    case 'b':
                    case 'c':
                    case 'd':
                    case 'e':
                    case 'f':
                    case 'g':
                    case 'h':
                    case 'i':
                    case 'j':
                    case 'k':
                    case 'l':
                    case 'm':
                    case 'n':
                    case 'o':
                    case 'p':
                    case 'q':
                    case 'r':
                    case 's':
                    case 't':
                    case 'u':
                    case 'v':
                    case 'w':
                    case 'x':
                    case 'y':
                    case 'z':
                    case 'A':
                    case 'B':
                    case 'C':
                    case 'D':
                    case 'E':
                    case 'F':
                    case 'G':
                    case 'H':
                    case 'I':
                    case 'J':
                    case 'K':
                    case 'L':
                    case 'M':
                    case 'N':
                    case 'O':
                    case 'P':
                    case 'Q':
                    case 'R':
                    case 'S':
                    case 'T':
                    case 'U':
                    case 'V':
                    case 'W':
                    case 'X':
                    case 'Y':
                    case 'Z':
                        if(pos_in_lexeme == 31) {
                            // Length of Function Identifier > 30
                            while((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) 
                                c = getStream(fp);
                            while(c >= '0' && c <= '9') 
                                c = getStream(fp);
                            pos_in_buffer--;
                            t->tokenClass = TK_ERROR;
                            t->lexeme[31] = '\0';
                            sprintf(t->lexeme, "%s...: function identifier too long", t->lexeme);
                            return;
                        }
                        break;
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        if(pos_in_lexeme == 31) {
                            // Length of Function Identifier > 30
                            while(c >= '0' && c <= '9') 
                                c = getStream(fp);
                            pos_in_buffer--;
                            t->tokenClass = TK_ERROR;
                            t->lexeme[31] = '\0';
                            sprintf(t->lexeme, "%s...: function identifier too long", t->lexeme);
                            return;
                        }
                        s = 45;
                        break;
                    default:
                        // TK_FUNNID or TK_MAIN
                        t->lexeme[pos_in_lexeme - 1] = '\0';
                        pos_in_buffer--;
                        if(strcmp(t->lexeme, "_main") == 0)
                            t->tokenClass = TK_MAIN;
                        else
                            t->tokenClass = TK_FUNID;
                        return;
                }
                break;

            case 45:
                switch(c) {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        if(pos_in_lexeme == 31) {
                            // Length of Function Identifier > 30
                            while(c >= '0' && c <= '9') 
                                c = getStream(fp);
                            pos_in_buffer--;
                            t->tokenClass = TK_ERROR;
                            t->lexeme[31] = '\0';
                            sprintf(t->lexeme, "%s...: function identifier too long", t->lexeme);
                            return;
                        }
                        break;
                    default: // TK_FUNID found
                        pos_in_buffer--;
                        t->tokenClass = TK_FUNID;
                        t->lexeme[pos_in_lexeme - 1] = '\0';
                        return;
                }
                break;

			case 189:
				switch(c){
					case 'a':
                    case 'b':
                    case 'c':
                    case 'd':
                    case 'e':
                    case 'f':
                    case 'g':
                    case 'h':
                    case 'i':
                    case 'j':
                    case 'k':
                    case 'l':
                    case 'm':
                    case 'n':
                    case 'o':
                    case 'p':
                    case 'q':
                    case 'r':
                    case 's':
                    case 't':
                    case 'u':
                    case 'v':
                    case 'w':
                    case 'x':
                    case 'y':
                    case 'z':
                        if(pos_in_lexeme == 21) {
                            /* FIELDID > 20 swallow up characters
                             * till the (possible)end of the identifier
                             * and return error
                             */
                            while(c >= 'a' && c <= 'z') 
                                c = getStream(fp);
                            pos_in_buffer--; // the last character (not [a-z] is possibly part of the next token)
                            t->tokenClass = TK_ERROR;
                            t->lexeme[21] = '\0';
                            sprintf(t->lexeme, "%s...: field identifier too long", t->lexeme);
                            return;
                        }
                        break;
                    default: // TK_FIELDID found
                        pos_in_buffer--; // the current character is part of the next token
                        if(pos_in_lexeme > 2){
                            t->lexeme[pos_in_lexeme - 1] = '\0'; // the current character is not part of the identifier
                            hash_value = hash_function(t->lexeme,h->size);
                            current_pointer = &(h->table[hash_value]);
                            if(strcmp(current_pointer->value,"null")==0){
                                t->tokenClass = TK_FIELDID;
                                return;
                            }
                            else{
                                while(current_pointer != NULL){
                                    if(strcmp(current_pointer->value,t->lexeme)==0){
                                        //printf("in\n");
                                        t->tokenClass = tokenClass(current_pointer->token);
                                        return;
                                    }
                                    current_pointer = current_pointer->next;
                                }
                            }
                            
                            /*
                            if(strcmp(t->lexeme, "call") == 0)
                                t->tokenClass = TK_CALL;
                            else if(strcmp(t->lexeme, "else") == 0)
                                t->tokenClass = TK_ELSE;
                            else if(strcmp(t->lexeme, "end") == 0)
                                t->tokenClass = TK_END;
                            else if(strcmp(t->lexeme, "endif") == 0)
                                t->tokenClass = TK_ENDIF;
                            else if(strcmp(t->lexeme, "endwhile") == 0)
                                t->tokenClass = TK_ENDWHILE;
                            else if(strcmp(t->lexeme, "endrecord") == 0)
                                t->tokenClass = TK_ENDRECORD;                                     
                            else if(strcmp(t->lexeme, "global") == 0)
                                t->tokenClass = TK_GLOBAL;
                            else if(strcmp(t->lexeme, "if") == 0)
                                t->tokenClass = TK_IF;
                            else if(strcmp(t->lexeme, "input") == 0)
                                t->tokenClass = TK_INPUT;
                            else if(strcmp(t->lexeme, "int") == 0)
                                t->tokenClass = TK_INT;
                            else if(strcmp(t->lexeme, "list") == 0)
                                t->tokenClass = TK_LIST;
                            else if(strcmp(t->lexeme, "output") == 0)
                                t->tokenClass = TK_OUTPUT;
                            else if(strcmp(t->lexeme, "parameter") == 0)
                                t->tokenClass = TK_PARAMETER;
                            else if(strcmp(t->lexeme, "parameters") == 0)
                                t->tokenClass = TK_PARAMETERS;
                            else if(strcmp(t->lexeme, "read") == 0)
                                t->tokenClass = TK_READ;
                            else if(strcmp(t->lexeme, "real") == 0)
                                t->tokenClass = TK_REAL;
                            else if(strcmp(t->lexeme, "record") == 0)
                                t->tokenClass = TK_RECORD;
                            else if(strcmp(t->lexeme, "return") == 0)
                                t->tokenClass = TK_RETURN;
                            else if(strcmp(t->lexeme, "then") == 0)
                                t->tokenClass = TK_THEN;
                            else if(strcmp(t->lexeme, "type") == 0)
                                t->tokenClass = TK_TYPE;
                            else if(strcmp(t->lexeme, "while") == 0)
                                t->tokenClass = TK_WHILE;
                            else if(strcmp(t->lexeme, "with") == 0)
                                t->tokenClass = TK_WITH;
                            else if(strcmp(t->lexeme, "write") == 0)
                                t->tokenClass = TK_WRITE;
                            else
                                t->tokenClass = TK_FIELDID; // a normal field id
                            */

                            t->tokenClass = TK_FIELDID; // a normal field id
                            return;
                        }
                        else if(pos_in_lexeme == 2){
                            t->tokenClass = TK_ERROR;
                            t->lexeme[1] = '\0';
                            sprintf(t->lexeme, "%s...: field identifier too short. Minimum Length = 2", t->lexeme);
                            return;    
                        }
                }
                break;
            case 190:
            	switch(c) {
                    case '=': // TK_LE found
                        s = 194; //FINAL STATE
                        t->tokenClass = TK_LE;
                        return;
                    case '-':
                        s = 191; //done
                        break;
                    default:
                        pos_in_buffer--;
                        t->tokenClass = TK_LT;
                        t->line_num = line_num;
                        return;
                }
                break;
            case 191:
            	switch(c) {
                    case '-':
                        s = 192; //done
                        break;
                    default:
                        pos_in_buffer--;
                        strcpy(t->lexeme, "Unknown token: '<-'");
                        t->tokenClass = TK_ERROR;
                        return;
                }
                break;
            case 192:
            	switch(c) {
                    case '-': // TK_ASSIGNOP found
                        s = 193; //FINAL STATE
                        t->tokenClass = TK_ASSIGNOP;
                        return;
                    default:
                        pos_in_buffer--;
                        strcpy(t->lexeme, "Unknown token: '<--'");
                        t->tokenClass = TK_ERROR;
                        return;
                }
                break;
            case 195:
            	switch(c) {
            		case '=':
            			s = 196; //FINAL STATE
            			t->tokenClass = TK_EQ;
            			return;
            		default:
            			if(sanitizeError(c))
                            strcpy(t->lexeme, "Unknown token: '='");
                        else
                            sprintf(t->lexeme, "expected '=' before '%c'", c);
                        t->tokenClass = TK_ERROR;
                        t->line_num = line_num;
                        if(c == '\n')
                            line_num++;
                        return;
            	}
            	break;
            case 197:
            	switch(c) {
                    case '=': // TK_GE found
                        s = 198; // FINAL STATE
                        t->tokenClass = TK_GE;
                        return;
                    default: // TK_GT found
                        pos_in_buffer--;
                        t->tokenClass = TK_GT;
                        return;
                }
                break;
            case 199:
            	switch(c) {
                    case '=': // TK_NE found
                    	s = 200; //FINAL STATE
                        t->tokenClass = TK_NE;
                        return;
                    default:
                        if(sanitizeError(c))
                            strcpy(t->lexeme, "Unknown token: '!'");
                        else
                            sprintf(t->lexeme, "expected '=' before '%c'", c);
                        t->tokenClass = TK_ERROR;
                        t->line_num = line_num;
                        if(c == '\n')
                            line_num++;
                        return;
                }
                break;
            case 201:
            	switch(c) {
                    case '@':
                        s = 202; //done
                        break;
                    default:
                        if(sanitizeError(c))
                            strcpy(t->lexeme, "Unknown token: '@'");
                        else
                            sprintf(t->lexeme, "expected '@' before '%c'", c);
                        t->tokenClass = TK_ERROR;
                        t->line_num = line_num;
                        if(c == '\n')
                            line_num++;
                        return;
                }
                break;
            case 202:
            	switch(c) {
                    case '@':
                        s = 203; //FINAL STATE
                        t->tokenClass = TK_OR;
                        return;
                    default:
                        if(sanitizeError(c))
                            strcpy(t->lexeme, "Unknown token: '@'");
                        else
                            sprintf(t->lexeme, "expected '@' before '%c'", c);
                        t->tokenClass = TK_ERROR;
                        t->line_num = line_num;
                        if(c == '\n')
                            line_num++;
                        return;
                }
                break;
            case 204:
            	// comment state - so ignore input till the end of the line
                switch(c) {
                    case '\n':
                        line_num++;
                        s = 0;
                        pos_in_lexeme = 0;
                        break;
                    case 3: // eof in comment
                        t->tokenClass = TK_EOF;
                        return;
                    default:
                        ;
                }
                break;
            case 217:
            	switch(c) {
                    case '&':
                        s = 218; //done
                        break;
                    default:
                        if(sanitizeError(c))
                            strcpy(t->lexeme, "Unknown token: '&'");
                        else
                            sprintf(t->lexeme, "expected '&' before '%c'", c);
                        t->tokenClass = TK_ERROR;
                        t->line_num = line_num;
                        if(c == '\n')
                            line_num++;
                        return;
                }
                break;
            case 218:
            	switch(c) {
                    case '&': // TK_AND found
                    	s = 219; //FINAL STATE
                        t->tokenClass = TK_AND;
                        return;
                    default:
                        if(sanitizeError(c))
                            strcpy(t->lexeme, "Unknown token: '&&'");
                        else
                            sprintf(t->lexeme, "expected '&' before '%c'", c);
                        t->tokenClass = TK_ERROR;
                        t->line_num = line_num;
                        if(c == '\n')
                            line_num++;
                        return;
                }
                break;
            case 220:
                switch(c) {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        if(pos_in_lexeme == 21) {
                            // limiting size of num to 20. confirm with ma'am
                            while(c >= '0' && c <= '9') 
                                c = getStream(fp);
                            if(c == '.') {
                                // check possibility of rnum
                                c = getStream(fp);
                                if(c >= '0' && c <= '9') {
                                    c = getStream(fp);
                                    if(c < '0' || c > '9')
                                        pos_in_buffer -= 3;
                                }
                                else
                                    pos_in_buffer -= 2;
                            }
                            else
                                pos_in_buffer--;

                            t->tokenClass = TK_ERROR;
                            t->lexeme[21] = '\0';
                            sprintf(t->lexeme, "%s...: numeric constant too long", t->lexeme);
                            return;
                        }
                        break;
                    case '.':
                        s = 221;
                        break;
                    default: // TK_NUM found
                        pos_in_buffer--;
                        t->tokenClass = TK_NUM;
                        t->lexeme[pos_in_lexeme - 1] = '\0';
                        return;
                }
                break;
            case 221:
                switch(c) {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        s = 222;
                        break;
                    default:
                        pos_in_buffer--;
                        t->lexeme[pos_in_lexeme - 1] = '\0';
                        t->tokenClass = TK_ERROR;
                        sprintf(t->lexeme, "%s: Unknown real constant", t->lexeme);
                        return;
                }
                break;
            case 222:
                switch(c) {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9': 
                        // TK_RNUM found
                        t->tokenClass = TK_RNUM;
                        t->lexeme[pos_in_lexeme] = '\0';
                        return;
                    default:
                        pos_in_buffer--;
                        t->lexeme[pos_in_lexeme - 1] = '\0';
                        t->tokenClass = TK_ERROR;
                        sprintf(t->lexeme, "%s: Unknown real constant", t->lexeme);
                        return;
                }
                break;
            case 223:
                switch(c) {
                    case 'a':
                    case 'b':
                    case 'c':
                    case 'd':
                    case 'e':
                    case 'f':
                    case 'g':
                    case 'h':
                    case 'i':
                    case 'j':
                    case 'k':
                    case 'l':
                    case 'm':
                    case 'n':
                    case 'o':
                    case 'p':
                    case 'q':
                    case 'r':
                    case 's':
                    case 't':
                    case 'u':
                    case 'v':
                    case 'w':
                    case 'x':
                    case 'y':
                    case 'z':
                        s = 224;
                        break;
                    default:
                        if(sanitizeError(c))
                            strcpy(t->lexeme, "unexpected character after '#'");
                        else
                            sprintf(t->lexeme, "unexpected character: '%c' after '#'", c);
                        t->tokenClass = TK_ERROR;
                        t->line_num = line_num;
                        if(c == '\n')
                            line_num++;
                        return;
                }
                break;
            case 224:
                switch(c) {
                    case 'a':
                    case 'b':
                    case 'c':
                    case 'd':
                    case 'e':
                    case 'f':
                    case 'g':
                    case 'h':
                    case 'i':
                    case 'j':
                    case 'k':
                    case 'l':
                    case 'm':
                    case 'n':
                    case 'o':
                    case 'p':
                    case 'q':
                    case 'r':
                    case 's':
                    case 't':
                    case 'u':
                    case 'v':
                    case 'w':
                    case 'x':
                    case 'y':
                    case 'z':
                        break;
                    default: 
                    // TK_RECORDID found
                        pos_in_buffer--;
                        t->tokenClass = TK_RECORDID;
                        t->lexeme[pos_in_lexeme - 1] = '\0';
                        return;
                }
                break;
            case 225:
                pos_in_lexeme = 0; // reset lexeme pointer to start of lexeme
                switch(c) {
                    case '\n':
                        line_num++;
                    case ' ':
                    case '\t':
                    case '\f':
                    case '\v':
                    case '\r':
                    case 0:
                        break;
                    case 3:
                        t->tokenClass = TK_EOF;
                        return;
                    default: // return to start state
                        pos_in_buffer--;
                        s = 0;
                }
                break;
            default:
				;
		} //end switch_outer
	} // end while
} //end function

/*
int main()
{
    FILE* fp;
    tokenInfo *t;
    hashtable* table;
    
    h = hash_keywords();
    t= (tokenInfo*) malloc(sizeof(tokenInfo));
    fp = fopen("testcase1.txt","r");
    getNextToken(fp,t);
    printf("%llu: ",t->line_num);
    printf(" Token: %s",tokenName(t->tokenClass));
    printf(" Lexeme: %s\n",t->lexeme);

    while(t->tokenClass != TK_EOF){
        t= (tokenInfo*) malloc(sizeof(tokenInfo));
        getNextToken(fp,t);
        printf("%llu: ",t->line_num);
        printf(" Token: %s",tokenName(t->tokenClass));
        printf(" Lexeme: %s\n",t->lexeme);
    }
}
*/
