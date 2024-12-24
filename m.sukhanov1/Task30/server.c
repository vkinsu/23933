#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#define SOCKET_PATH "/tmp/unix_sock"
#define BUFFER_SIZE 256

int main(){
	int server_sock, client_sock;
	struct sockaddr_un server_addr;
	char buffer[BUFFER_SIZE] = {0};

	//creation of socket
	if ((server_sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
		perror("socket failed");
		exit(1);
	}
	
	// setting the socket address
	server_addr.sun_family = AF_UNIX;
	strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);
	
	// deleting of the old socket file if it exists
	
	unlink(SOCKET_PATH);

	// linkng socket to address
	if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1){
		perror("bind failed");
		close(server_sock);
		exit(2);
	}
	
	if (listen(server_sock, 5) == -1){
		perror("listen failed");
		close(server_sock);
		exit(3);
	}

	printf("Server listening on %s\n", SOCKET_PATH);

	// accepting connection from the client
	if ((client_sock = accept(server_sock, NULL, NULL)) == -1){
		perror("accept failed");
		close(server_sock);
		exit(4);
	}
	while(1){
		ssize_t n = recv(client_sock, buffer, BUFFER_SIZE, 0);
		if (n == 1 && buffer[0] == '\0')
			break;
		for (int i = 0; i < BUFFER_SIZE; i++){
			buffer[i] = toupper(buffer[i]);
		}
		if (n > 0){
			printf("received from client: %s\n", buffer);
		}
		else if (n == 0){
			printf("\nconnection closed by client\n");
			break;
		}
		else{
			perror("\nrecv failed\n");
			exit(5);
		}
	}
	close(client_sock);
	close(server_sock);
	unlink(SOCKET_PATH);

	return 0;

}

