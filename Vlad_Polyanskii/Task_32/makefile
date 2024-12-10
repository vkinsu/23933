all: server client

server: server.c
	cc server.c -lthread -lpthread -lsocket -lnsl -laio -o server

client: client.c
	cc client.c -lthread -lpthread -lsocket -lnsl -laio -o client

# all: server client

# server: server.c
# 	gcc server.c -o server

# client: client.c
# 	gcc client.c -o client