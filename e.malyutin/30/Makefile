server: server.c
	$(CC) -o $@ $^ $(CFLAGS) -lsocket

client: client.c
	$(CC) -o $@ $^ $(CFLAGS) -lsocket

clean:
	rm -f client server
