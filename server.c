#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include "server.h"
#include "config.h"

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