#include <stdlib.h>
#include <stdio.h>
#include "tunnel.h"
#include "socket.h"
#include "pipe.h"

struct tunnel *tunnel_create_from_socket(struct socket *socket) {
    struct tunnel *tunnel = (struct tunnel *)malloc(sizeof(struct tunnel));
    tunnel->readobj = socket;
    tunnel->writeobj = socket;
    tunnel->read = (int (*)(void *, char *, int))socket_read;
    tunnel->write = (int (*)(void *, char *, int))socket_write;
    tunnel->destroy = (int (*)(void *))socket_destroy;
}

struct tunnel *tunnel_create_from_pipe(struct pipe *readobj, struct pipe *writeobj) {
    struct tunnel *tunnel = (struct tunnel *)malloc(sizeof(struct tunnel));
    tunnel->readobj = readobj;
    tunnel->writeobj = writeobj;
    tunnel->read = (int (*)(void *, char *, int))pipe_read;
    tunnel->write = (int (*)(void *, char *, int))pipe_write;
    tunnel->destroy = (int (*)(void *))pipe_destroy;
}

int tunnel_read(struct tunnel *tunnel, char* buffer, int bytes) {
    return tunnel->read(tunnel->readobj, buffer, bytes);
}

int tunnel_read_int(struct tunnel *tunnel, int *dest) {
    int ret = tunnel->read(tunnel->readobj, (char*)dest, sizeof(int));
    return ret;
}

int tunnel_write(struct tunnel *tunnel, char* buffer, int bytes) {
    return tunnel->write(tunnel->writeobj, buffer, bytes);
}

int tunnel_write_int(struct tunnel *tunnel, int i) {
    return tunnel->write(tunnel->writeobj, (char*)&i, sizeof(int));
}

void tunnel_destroy(struct tunnel *tunnel) {
    tunnel->destroy(tunnel->readobj);
    if (tunnel->readobj != tunnel->writeobj) tunnel->destroy(tunnel->writeobj);
    free(tunnel);
}