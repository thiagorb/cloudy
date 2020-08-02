#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "pipe.h"

struct pipe *pipe_create() {
    struct pipe *n_pipe = (struct pipe *)malloc(sizeof(struct pipe));
    int p[2];
    pipe(p);
    n_pipe->fd_in = p[0];
    n_pipe->fd_out = p[1];
    return n_pipe;
}

struct pipe *pipe_from_fd(int fd_in, int fd_out) {
    struct pipe *n_pipe = (struct pipe *)malloc(sizeof(struct pipe));
    n_pipe->fd_in = fd_in;
    n_pipe->fd_out = fd_out;
    return n_pipe;
}

void pipe_readonly(struct pipe *pipe) {
    close(pipe->fd_out);
    pipe->fd_out = -1;
}

void pipe_writeonly(struct pipe *pipe) {
    close(pipe->fd_in);
    pipe->fd_in = -1;
}

int pipe_read(struct pipe *pipe, char* buffer, int bytes) {
    int n = 0;
    while (n < bytes) {
        int status = read(pipe->fd_in, &buffer[n], bytes - n);
        if (status <= 0) {
            return 1;
        }
        n += status;
    }
    return 0;
}

int pipe_write(struct pipe *pipe, char* buffer, int bytes) {
    int n = 0;
    while (n < bytes) {
        int status = write(pipe->fd_out, &buffer[n], bytes - n);
        if (status < 0) {
            return 1;
        }
        n += status;
    }
    return 0;
}

void pipe_destroy(struct pipe *pipe) {
    if (pipe->fd_in >= 0) {
        close(pipe->fd_in);
    }
    if (pipe->fd_out >= 0) {
        close(pipe->fd_out);
    }
    free(pipe);
}