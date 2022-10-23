all: main.o funcs.o
	cc -o movies_by_year main.o funcs.o

main.o: funcs.h main.c
	cc -c main.c

funcs.o: funcs.h funcs.c
	cc -c funcs.c

clean:
	rm movies_by_year main.o funcs.o
