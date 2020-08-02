/* 
 * File:   tunnel.h
 * Author: thiago
 *
 * Created on 27 de Junho de 2012, 14:38
 */

#ifndef TUNNEL_H
#define	TUNNEL_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "socket.h"
#include "pipe.h"
#include "tunnel.h"
    
struct tunnel {
    void *readobj;
    void *writeobj;
    int (*read)(void *, char* buffer, int bytes);
    int (*write)(void *, char* buffer, int bytes);
    int (*destroy)(void *);
};

struct tunnel *tunnel_create_from_socket(struct socket *socket);
struct tunnel *tunnel_create_from_pipe(struct pipe *readobj, struct pipe *writeobj);
int tunnel_read(struct tunnel *tunnel, char* buffer, int bytes);
int tunnel_read_int(struct tunnel *tunnel, int *dest);
int tunnel_write_int(struct tunnel *tunnel, int i);
void tunnel_destroy(struct tunnel* tunnel);

#ifdef	__cplusplus
}
#endif

#endif	/* TUNNEL_H */

