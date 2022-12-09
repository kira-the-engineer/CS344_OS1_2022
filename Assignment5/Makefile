all: funcs.o keygen.o enc_server.o enc_client.o dec_server.o dec_client.o
	cc -o keygen keygen.o funcs.o
	cc -o enc_server enc_server.o funcs.o
	cc -o enc_client enc_client.o funcs.o
	cc -o dec_client dec_client.o funcs.o
	cc -o dec_server dec_server.o funcs.o

funcs: funcs.h funcs.c
	cc - c funcs.c

keygen: funcs.h keygen.c 
	cc -c keygen.c                    

enc_server: funcs.h enc_server.c
	cc -c enc_server.c

enc_client: funcs.h enc_client.c
	cc -c enc_client.c

dec_server: funcs.h dec_server.c
	cc -c dec_server.c

dec_client: funcs.h ec_client.c
	cc -c dec_client.c

clean:
	rm -f *.o  keygen enc_server enc_client dec_server dec_client
