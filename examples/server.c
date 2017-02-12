#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/signal.h>
#include <fcntl.h>
#include "../cstreammultiplexer.h"
#include "../terminal.h"

#define PORT_NUMBER 1235
#define BUFFER_SIZE 4096

#define MAX_CONNECTIONS SOCKETS - 3

int server_connect();

int server_connect() {
	int serverSocket = 0;
	struct sockaddr_in serv_addr;

	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(PORT_NUMBER);

	if ( bind(serverSocket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0 ) {
		fprintf(stderr, "Error: bind socket\n");
		return -1;
	}

	if ( listen(serverSocket, MAX_CONNECTIONS) < 0 ) {
		fprintf(stderr, "Error: listen\n");
	}
	return serverSocket;
}


int main(int argc, char **argv) {
	bool hasToQuit = false;
	fd_set active_fd_set;
	fd_set read_fd_set;
	struct sockaddr_in clientname;
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

	socket = server_connect();
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
				if ( i == socket ) {
					socklen_t size = sizeof(clientname);
					int newClient = accept(socket, (struct sockaddr *) &clientname, &size);
					if ( newClient < 0 ) {
						fprintf(stderr, "Error: accept\n");
						close(socket);
						return -1;
					}
					FD_SET (newClient, &active_fd_set);
					csm_registerSocket(newClient);
				} else {
					hasToQuit = !csm_messageCall(&active_fd_set, i, buffer, BUFFER_SIZE);
				}
			}
		}
	}

	return 0;
}