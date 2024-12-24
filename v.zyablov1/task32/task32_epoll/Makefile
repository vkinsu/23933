CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -D_GNU_SOURCE

SERVER_SOURCE = $(wildcard server_source/*.c)
CLIENT_SOURCE = $(wildcard client_source/*.c)

all: client server

client: $(CLIENT_SOURCE)
	$(CC) $(CFLAGS) $(CLIENT_SOURCE) -o client

server: $(SERVER_SOURCE)
	$(CC) $(CFLAGS) $(SERVER_SOURCE) -o server

clean:
	rm -f client server

