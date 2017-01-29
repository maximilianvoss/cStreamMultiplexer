#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h> 
#include "config.h"

int client_connect() {
	int clientSocket = 0, n = 0;
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