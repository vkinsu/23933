all: server client

server: server.c
	cc server.c -lthread -lpthread -lsocket -lnsl -o server

client: client.c
	cc client.c -lthread -lpthread -lsocket -lnsl -o client