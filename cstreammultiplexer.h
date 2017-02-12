#ifndef CSTREAMMULTIPLEXER_CSTREAMMULTIPLEXER_H
#define CSTREAMMULTIPLEXER_CSTREAMMULTIPLEXER_H

#define SOCKETS 20

#define CMD_QUIT "!Me:Quit!"
#define CMD_STATUS "!Me:Status!"
#define RESP_STATUS "!Me:Running!"

void csm_reset_sockets();
bool csm_registerSocket(int socket);
void csm_unregisterSocket(int socket);
void csm_closeSockets();

bool csm_readFromDescriptor(int filedes, char *buffer, size_t buffer_size);
bool csm_messageCall(fd_set *active_fd_set, int source, char *buffer, size_t buffer_size);

#endif