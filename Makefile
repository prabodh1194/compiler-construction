all:clean
	gcc lexer.c helper_functions.c ff.c parser.c driver.c -o parser -g
clean:
	rm -f .fuse_hidd* ff parser
