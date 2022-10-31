all: main.o builtins.o
	cc -o smallsh main.o builtins.o

main: builtins.h main.c
	cc -c main.c

builtins: builtins.h builtins.c
	cc -c builtins.c

clean:
	rm smallsh main.o builtins.o
