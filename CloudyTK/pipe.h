#ifndef PIPE_H
#define	PIPE_H

#ifdef	__cplusplus
extern "C" {
#endif

struct pipe {
    int fd_in;
    int fd_out;
    
};
    
struct pipe *pipe_create();
struct pipe *pipe_from_fd(int fd_in, int fd_out);
void pipe_readonly(struct pipe *);
void pipe_writeonly(struct pipe *);
int pipe_read(struct pipe *pipe, char* buffer, int bytes);
int pipe_write(struct pipe *pipe, char* buffer, int bytes);
void pipe_destroy(struct pipe *pipe);
    
#ifdef	__cplusplus
}
#endif

#endif	/* PIPE_H */

