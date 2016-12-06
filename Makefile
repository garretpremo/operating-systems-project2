all:
	gcc -g -Wall p2.c process.c memory.c -o a

test:
	gcc -g -Wall test.c memory.c -o t
