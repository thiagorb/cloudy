#include <stdio.h>
#include <stdlib.h>
#include "cloudytk.h"
#include "client.h"
#include "app.h"
#include "syscall.h"

#define assert(condition, msg) if ((condition) == 0) {printf("%s\n", (msg)); exit(1);}

int main(int argc, char** argv) {
    /*
    struct pipe *p1 = pipe_create();
    struct pipe *p2 = pipe_create();
    struct tunnel *t1 = tunnel_create_from_pipe(p1, p2);
    struct tunnel *t2 = tunnel_create_from_pipe(p2, p1);
    printf("%i\n", tunnel_write_int(t1, 451));
    printf("%i\n", tunnel_ready(t2));
    int i = 0;
    printf("%i\n", tunnel_read_int(t2, &i));
    printf("%i\n", i);
    return 0;
///*
}


int inutil() {
  /**/  
    initialize_command_handlers();
    syscall_initialize();
    cloudy_commands_initialize();
    
    struct socket* server_socket = socket_create();
    assert(server_socket != 0, "Error creating server socket");
    assert(!socket_listen(server_socket, 30000), "Error listening on socket");
    while (1) {
        struct socket* client_socket = socket_accept(server_socket);
        struct client* client = client_create(client_socket);
        thread_start(client_handle, client);
        //client_handle(client);
    }
    return 0;
}

