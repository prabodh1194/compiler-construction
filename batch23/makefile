all:clean
	gcc lexer.c helper_functions.c ff.c parser.c symboltable.c populateHashTable.c codegen.c driver.c -o toycompiler -g
ff:
	gcc helper_functions.c ff.c -o ff -g
clean:
	rm -f .fuse_hidd* ff parser stage1exe
