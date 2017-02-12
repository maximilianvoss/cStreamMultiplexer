#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h> 
#include <stdbool.h>
#include <unistd.h>
#include <sys/signal.h>
#include <fcntl.h>
#include "../cstreammultiplexer.h"
#include "../terminal.h"

#define PORT_NUMBER 1235
#define BUFFER_SIZE 4096
#define SERVER_HOST "127.0.0.1"

int client_connect();

int client_connect() {
	int clientSocket = 0;
	struct sockaddr_in serv_addr;

	if ( ( clientSocket = socket(AF_INET, SOCK_STREAM, 0) ) < 0 ) {
		fprintf(stderr, "Error: open Socket\n");
		return -1;
	}

	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT_NUMBER);

	if ( inet_pton(AF_INET, SERVER_HOST, &serv_addr.sin_addr) <= 0 ) {
		fprintf(stderr, "Error: inet_pton\n");
		return -1;
	}

	if ( connect(clientSocket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0 ) {
		fprintf(stderr, "Error: connect\n");
		return -1;
	}
	return clientSocket;
}


int main(int argc, char **argv) {
	bool hasToQuit = false;
	fd_set active_fd_set;
	fd_set read_fd_set;
	char buffer[BUFFER_SIZE];
	int socket;
	int namedPipe;
	int i;

	signal(13, (void *) 1);

	terminal_conio_mode();
	csm_reset_sockets();

	FD_ZERO (&active_fd_set);
	FD_SET(STDIN_FILENO, &active_fd_set);
	csm_registerSocket(STDOUT_FILENO);

	for ( i = 2; i < argc; i++ ) {
		namedPipe = open(argv[i], O_RDWR);
		FD_SET(namedPipe, &active_fd_set);
		csm_registerSocket(namedPipe);
	}

	socket = client_connect();
	if ( socket < 0 ) {
		fprintf(stderr, "Error: open socket\n");
		return 1;
	}
	FD_SET(socket, &active_fd_set);
	csm_registerSocket(socket);

	while ( !hasToQuit ) {
		read_fd_set = active_fd_set;
		if ( select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0 ) {
			fprintf(stderr, "Error: select\n");
			close(socket);
			return 1;
		}
		for ( i = 0; i < FD_SETSIZE; i++ ) {
			if ( FD_ISSET (i, &read_fd_set) ) {
				hasToQuit = !csm_messageCall(&active_fd_set, i, buffer, BUFFER_SIZE);
			}
		}
	}
	return 0;
}