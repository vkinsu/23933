#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/un.h>
#include <string.h>


#define SOCKET_PATH "/tmp/unix_sock"


int main(){
	int client_sock, i = 0;
	struct sockaddr_un server_addr;
	char message[256];
	char c;


	// creation of socket
	if ((client_sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
		perror("socket failed");
		exit(1);
	}

	// setting address if the server
	server_addr.sun_family = AF_UNIX;
	strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

	// connecting to the server
	if (connect(client_sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1){
		perror("connect failed");
		close(client_sock);
		exit(2);
	}

	// sending of message on server
	write(fileno(stdout), "Enter your messaage to the server:\n", 35);

	while(1){
		fgets(message, 256, stdin);
		if (strlen(message) == 1 && message[0] == '\n'){
			write(client_sock, "\0", 1);
			break;
		}
		message[strlen(message) - 1] = '\0';
		write(client_sock, message, 256);


	}

	

	close(client_sock);

	return 0;
}
