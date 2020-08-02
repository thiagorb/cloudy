#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "socket.h"
#include <netinet/tcp.h>

#define MAXCONNECTIONS 128

struct socket* socket_create() {
    int m_sock;
    m_sock = socket(AF_INET,
            SOCK_STREAM,
            0);
    if (m_sock == -1) return NULL;
    int on = 1;
    if (setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (const char*) &on, sizeof ( on)) == -1)
        return NULL;
    if (setsockopt(m_sock, IPPROTO_TCP, TCP_NODELAY, (const char*) &on, sizeof ( on)) == -1)
        return NULL;
    struct socket* ret = (struct socket*)malloc(sizeof(struct socket));
    ret->m_sock = m_sock;
    return ret;
}

int socket_listen(struct socket* socket, int port) {
    if (!socket) return 1;
    struct sockaddr_in *m_addr = &socket->m_addr;
    m_addr->sin_family = AF_INET;
    m_addr->sin_addr.s_addr = INADDR_ANY;
    m_addr->sin_port = htons(port);

    int bind_return = bind(socket->m_sock, (struct sockaddr *)m_addr, sizeof(struct sockaddr_in));
    if (bind_return == -1) {
        return 1;
    }
    
    int listen_return = listen(socket->m_sock, MAXCONNECTIONS);
    if (listen_return == -1) {
        return 1;
    }
    return 0;
}

struct socket* socket_accept(struct socket* server_socket) {
    int addr_length = sizeof(struct sockaddr_in);
    int m_sock = accept(server_socket->m_sock, (struct sockaddr*)&server_socket->m_addr, (socklen_t*)&addr_length);
    if (m_sock <= 0) {
        return NULL;
    } else {
        struct socket* new_socket = (struct socket*)malloc(sizeof(struct socket));
        new_socket->m_sock = m_sock;
        return new_socket;
    }
}

int socket_read(struct socket *socket, char* buffer, int bytes) {
    int n = 0;
    while (n < bytes) {
        int status = recv(socket->m_sock, &buffer[n], bytes - n, 0);
        if (status <= 0) {
            return 1;
        }
        n += status;
    }
    return 0;
}

int socket_write(struct socket *socket, char* buffer, int bytes) {
    int n = 0;
    while (n < bytes) {
        int status = send(socket->m_sock, &buffer[n], bytes - n, 0);
        if (status == -1) {
            return 1;
        }
        n += status;
    }
    return 0;
}

void socket_destroy(struct socket *socket) {
    close(socket->m_sock);
    free(socket);
}