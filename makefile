CFLAGS=-std=gnu99 -Wall -g -pedantic -pthread

all: server client
	gcc $(CFLAGS) shared.o comms.o server.o -o server
	gcc $(CFLAGS) shared.o comms.o client.o -o client

server: comms shared server.o
	gcc $(CFLAGS) -c server.c -o server.o 
	
client: comms shared client.o
	gcc $(CFLAGS) -c client.c -o client.o

shared: comms shared.o
	gcc $(CFLAGS) -c shared.c -o shared.o

comms: comms.o
	gcc $(CFLAGS) -c comms.c -o comms.o

clean: 
	rm *.o
