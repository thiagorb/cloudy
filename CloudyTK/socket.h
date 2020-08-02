#ifndef SOCKET_H
#define	SOCKET_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <netinet/in.h>

struct socket {
    int m_sock;
    struct sockaddr_in m_addr;
};

struct socket* socket_create();
int socket_listen(struct socket* socket, int port);
struct socket* socket_accept(struct socket* server_socket);
int socket_read(struct socket *socket, char* buffer, int bytes);
int socket_write(struct socket *socket, char* buffer, int bytes);
void socket_destroy(struct socket *socket);

#ifdef	__cplusplus
}
#endif

#endif	/* SOCKET_H */

