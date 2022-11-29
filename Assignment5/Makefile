all: keygen.o
	cc -o keygen keygen.o 

keygen: keygen.c
	cc -c keygen.c                    

enc_server: enc_server.c
	cc -c enc_server.c

enc_client: enc_client.c
	cc -c enc_client.c

dec_server: dec_server.c
	cc -c dec_server.c

dec_client: dec_client.c
	cc -c dec_client.c

clean:
	rm -f *.o  keygen enc_server enc_client dec_server dec_client
