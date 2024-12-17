all: server client1 client2

server: server.c
	$(CC) -o $@ $^ $(CFLAGS)

client1: client1.c
	$(CC) -o $@ $^ $(CFLAGS)

client2: client2.c
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f client1 client2 server
