#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include "cstreammultiplexer.h"

static int registeredSockets[SOCKETS];
static void csm_distributeMessage(int source, char *msg);

void csm_reset_sockets() {
	memset(&registeredSockets, 0, sizeof(int) * SOCKETS);
}

bool csm_readFromDescriptor(int filedes, char *buffer, size_t buffer_size) {
	ssize_t size;
	memset(buffer, '\0', buffer_size);
	size = read(filedes, buffer, buffer_size);
	if ( size < 0 ) {
		return false;
	} else if ( size == 0 ) {
		return false;
	}
	return true;
}

bool csm_registerSocket(int socket) {
	int i;
	for ( i = 0; i < SOCKETS; i++ ) {
		if ( registeredSockets[i] == 0 ) {
			registeredSockets[i] = socket;
			return true;
		}
	}
	return false;
}

void csm_unregisterSocket(int socket) {
	int i;
	for ( i = 0; i < SOCKETS; i++ ) {
		if ( registeredSockets[i] == socket ) {
			registeredSockets[i] = 0;
			return;
		}
	}
}

void csm_closeSockets() {
	int i;
	for ( i = 0; i < SOCKETS; i++ ) {
		if ( registeredSockets[i] != 0 ) {
			if ( registeredSockets[i] != STDIN_FILENO && registeredSockets[i] != STDOUT_FILENO && registeredSockets[i] != STDERR_FILENO ) {
				close(registeredSockets[i]);
			}
			registeredSockets[i] = 0;
		}
	}
}

static void csm_distributeMessage(int source, char *msg) {
	int i;
	for ( i = 0; i < SOCKETS; i++ ) {
		if ( registeredSockets[i] != source && registeredSockets[i] != 0 ) {
			write(registeredSockets[i], msg, strlen(msg));
		}
	}
}

bool csm_messageCall(fd_set *active_fd_set, int source, char *buffer, size_t buffer_size) {
	if ( csm_readFromDescriptor(source, buffer, buffer_size) == false ) {
		close(source);
		FD_CLR (source, active_fd_set);
		csm_unregisterSocket(source);
	} else {
		if ( !strcmp(buffer, CMD_QUIT) ) {
			csm_distributeMessage(source, buffer);
			csm_closeSockets();
			return false;
		} else if ( !strcmp(buffer, CMD_STATUS) ) {
			csm_distributeMessage(source, CMD_STATUS);
			csm_distributeMessage(source, RESP_STATUS);
		} else {
			csm_distributeMessage(source, buffer);
		}
	}
	return true;
}