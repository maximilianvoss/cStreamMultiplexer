#include <sys/termios.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include "client.h"
#include "server.h"
#include "config.h"

struct termios orig_termios;
int registeredSockets[MAX_CONNECTIONS];

void reset_terminal_mode() {
	tcsetattr(0, TCSANOW, &orig_termios);
}

void set_conio_terminal_mode() {
	struct termios new_termios;

	tcgetattr(0, &orig_termios);
	memcpy(&new_termios, &orig_termios, sizeof(new_termios));

	atexit(reset_terminal_mode);
	cfmakeraw(&new_termios);
	tcsetattr(0, TCSANOW, &new_termios);
}

int readFromDescriptor(int filedes, char *buffer) {
	int size;
	memset(buffer, '\0', BUFFER_SIZE);
	size = read(filedes, buffer, BUFFER_SIZE);
	if ( size < 0 ) {
		return -1;
	} else if ( size == 0 ) {
		return -1;
	}
	return 0;
}

void registerSocket(int socket) {
	int i;
	for ( i = 0; i < MAX_CONNECTIONS + 1; i++ ) {
		if ( registeredSockets[i] == 0 ) {
			registeredSockets[i] = socket;
			return;
		}
	}
}

void unregisterSocket(int socket) {
	int i;
	for ( i = 0; i < MAX_CONNECTIONS + 1; i++ ) {
		if ( registeredSockets[i] == socket ) {
			registeredSockets[i] = 0;
			return;
		}
	}
}

void closeSockets(int socket) {
	int i;
	for ( i = 0; i < MAX_CONNECTIONS + 1; i++ ) {
		if ( registeredSockets[i] != 0 ) {
			if ( registeredSockets[i] != STDIN_FILENO && registeredSockets[i] != STDOUT_FILENO && registeredSockets[i] != socket ) {
				close(registeredSockets[i]);
			}
			registeredSockets[i] = 0;
		}
	}
}

void distributeMessage(int source, char *msg) {
	int i;
	for ( i = 0; i < MAX_CONNECTIONS + 1; i++ ) {
		if ( registeredSockets[i] != source && registeredSockets[i] != 0 ) {
			write(registeredSockets[i], msg, strlen(msg));
		}
	}
}

int main(int argc, char **argv) {
	bool isClient = false;
	bool isServer = false;
	bool hasToQuit = false;
	fd_set active_fd_set;
	fd_set read_fd_set;
	struct sockaddr_in clientname;
	char buffer[BUFFER_SIZE];
	int socket;
	int i;

	signal(13, (void *) 1);

	set_conio_terminal_mode();
	memset(&registeredSockets, '\0', sizeof(int) * MAX_CONNECTIONS);

	if ( argc == 2 ) {
		if ( !strcmp(argv[1], "client") ) {
			isClient = true;
			isServer = false;
		} else if ( !strcmp(argv[1], "server") ) {
			isClient = false;
			isServer = true;
		}
	}
	if ( isClient == false && isServer == false ) {
		fprintf(stderr, "Error: argument is missing\n");
		return 1;
	}

	FD_ZERO (&active_fd_set);
	FD_SET(STDIN_FILENO, &active_fd_set);
	registerSocket(STDOUT_FILENO);

	if ( isClient ) {
		socket = client_connect();
	} else if ( isServer ) {
		socket = server_connect();
	}
	if ( socket < 0 ) {
		fprintf(stderr, "Error: open socket\n");
		return 1;
	}
	FD_SET(socket, &active_fd_set);
	registerSocket(socket);

	while ( !hasToQuit ) {
		read_fd_set = active_fd_set;
		if ( select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0 ) {
			fprintf(stderr, "Error: select\n");
			close(socket);
			return 1;
		}
		for ( i = 0; i < FD_SETSIZE; i++ ) {
			if ( FD_ISSET (i, &read_fd_set) ) {
				if ( isServer && i == socket ) {
					socklen_t size = sizeof(clientname);
					int newClient = accept(socket, (struct sockaddr *) &clientname, &size);
					if ( newClient < 0 ) {
						fprintf(stderr, "Error: accept\n");
						close(socket);
						exit(EXIT_FAILURE);
					}
					FD_SET (newClient, &active_fd_set);
					registerSocket(newClient);
				} else {
					if ( readFromDescriptor(i, buffer) < 0 ) {
						fprintf(stderr, "Error: read\n");
						close(i);
						FD_CLR (i, &active_fd_set);
						unregisterSocket(i);
					} else {
						distributeMessage(i, buffer);
						if ( !strcmp(buffer, "q") ) {
							fprintf(stderr, "Gonna quit\n");
							closeSockets(socket);
							hasToQuit = true;
						}
					}
				}
			}
		}
	}

	close(socket);
	return 0;
}