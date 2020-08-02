#ifndef APP_H
#define	APP_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include "pipe.h"
#include "command.h"
#include "client.h"

struct app {
    int pid;
    int running;
    struct tunnel *tunnel;
    char* path;
    struct app *next;
    struct client *client;
};

struct app *app_start(char* path);
void app_destroy(struct app *app);
void app_start_handler(struct client *client, struct command *cmd, struct app *app);
void app_end_handler(struct client *client, struct command *cmd, struct app *app);
void app_exec_handler(struct client *client, struct command *cmd, struct app *app);
void app_show_handler(struct client *client, struct command *cmd, struct app *app);
void do_login_handler(struct client *client, struct command *cmd, struct app *app);
void app_minimize_handler(struct client *client, struct command *cmd, struct app *app);
void app_syscall_handler(struct client *client, struct command *cmd, struct app *app);
void set_system_callback_handler(struct client *client, struct command *cmd, struct app *app);

#ifdef	__cplusplus
}
#endif

#endif	/* APP_H */

