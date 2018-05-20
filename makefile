all: client server

client: client.c
  gcc -Wall -g -o client client.c -lrt -lpthread

server: server.c
	gcc -Wall -g -o server server.c -lrt -lpthread

clean:
	rm -fr client server *~ *.o
