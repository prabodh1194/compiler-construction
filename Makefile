all:clean
	gcc ff.c -o ff -g
clean:
	rm -f .fusehidd* ff
