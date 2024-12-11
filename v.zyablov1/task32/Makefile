CC = gcc
CFLAGS = -Wall -Wextra -std=c99
SERVER_SOURCE  = server.c client_handler.c
CLIENT_SOOURCE = client.c

all: client server

client: $(CLIENT_SOOURCE)
	$(CC) $(CFLAGS) $(CLIENT_SOOURCE) -o client 

server: $(SERVER_SOURCE)
	$(CC) $(CFLAGS) $(SERVER_SOURCE)  -o server

clean:
	rm -f client server
