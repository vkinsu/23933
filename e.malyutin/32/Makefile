all: server client1 client2

server: server.c
	$(CC) -o $@ $^ $(CFLAGS) -lrt -lsocket

client1: client1.c
	$(CC) -o $@ $^ $(CFLAGS) -lrt -lsocket

client2: client2.c
	$(CC) -o $@ $^ $(CFLAGS) -lrt -lsocket

clean:
	rm -f client1 client2 server
