all:clean
	gcc lexer.c helper_functions.c ff.c parser.c -o parser -g
	gcc lexer.c helper_functions.c ff.c parser2.c -o parser22 -g
clean:
	rm -f .fuse_hidd* ff parser
