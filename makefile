all:clean
	gcc lexer.c helper_functions.c ff.c parser.c driver.c -o stage1exe -g
ff:
	gcc helper_functions.c ff.c -o ff -g
clean:
	rm -f .fuse_hidd* ff parser
