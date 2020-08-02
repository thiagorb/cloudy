#ifndef CLIENT_H
#define	CLIENT_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "cloudytk.h"
#include "mutex.h"

struct app_config {
    char *app_path;
    char *icon_path;
    struct app_config *next;
};

struct app_state_change_listener {
    struct app *listener;
    int callback_id;
    struct app_state_change_listener *next;
};

struct client {
    struct tunnel* tunnel;
    struct app *current_app;
    struct app *login_app;
    struct app *menu_app;
    struct app *apps;
    int connected;
    struct client *next;
    struct app_config *app_config;
    struct app_state_change_listener *app_state_change_listeners;
    struct mutex *app_state_change_listeners_lock;
    struct dimension device_resolution;
    struct command_queue command_queue;
    unsigned int bytes_sent;
    unsigned int bytes_received;
    FILE *client_log;
};

struct app *client_app_start(struct client *client, char* path);
struct client *client_create(struct socket *socket);
//void client_apps_handle(struct client* client);
void client_handle(struct client* client);
void initialize_command_handlers();

#ifdef	__cplusplus
}
#endif

#endif	/* CLIENT_H */

