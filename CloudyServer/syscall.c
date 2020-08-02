#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "syscall.h"
#include "client.h"
#include "app.h"
#include "bytestream.h"
#include "mutex.h"

typedef void (*syscall_handler)(struct app *app, int callback_id, char *args);
syscall_handler syscall_handlers[LAST_SYSCALL_CODE];

/*
 * returns:
 *      -number of registered apps
 *      -length + data of app path string
 *      -length + data of icon path string
 */
void syscall_get_apps_config(struct app *app, int callback_id, char *args) {
    int data_size = sizeof(int);
    int napps = 0;
    struct app_config *cfg;
    for (cfg = app->client->app_config; cfg; cfg = cfg->next) {
        napps++;
        data_size += sizeof(int) + strlen(cfg->app_path);
        data_size += sizeof(int) + strlen(cfg->icon_path);
    }
    char temp_data[data_size];
    char *stream = temp_data;
    
    //printf("%i\n", napps);
    bytestream_write_int(&stream, napps);
    for (cfg = app->client->app_config; cfg; cfg = cfg->next) {
        //printf("%s %s\n", cfg->app_path, cfg->icon_path);
        bytestream_write_string(&stream, cfg->app_path);
        bytestream_write_string(&stream, cfg->icon_path);
    }
    struct command cmd;
    cmd.id = CMD_CALLBACK;
    cmd.data.code = callback_id;
    cloudy_put_command(app->tunnel, &cmd);
    cmd.id = CMD_DATA;
    cmd.data.variant.content = temp_data;
    cmd.data.variant.size = data_size;
    cloudy_put_command(app->tunnel, &cmd);
}

void deliver_app_state_change_event(struct client *client) {
    int data_size = sizeof(int);
    int napps = 0;
    struct app *app;
    for (app = client->apps; app; app = app->next) {
        napps++;
        data_size += sizeof(int) + strlen(app->path) + sizeof(int);
    }
    char temp_data[data_size];
    char *stream = temp_data;
    bytestream_write_int(&stream, napps);
    for (app = client->apps; app; app = app->next) {
        bytestream_write_string(&stream, app->path);
        bytestream_write_int(&stream, app->pid);
    }
    
    mutex_lock(client->app_state_change_listeners_lock);
    struct app_state_change_listener *ascl = client->app_state_change_listeners;
    while (ascl) {
        struct app_state_change_listener *next = ascl->next;
        
        struct command cmd;
        cmd.id = CMD_CALLBACK;
        cmd.data.code = ascl->callback_id;
        cloudy_put_command(ascl->listener->tunnel, &cmd);
        cmd.id = CMD_DATA;
        cmd.data.variant.content = temp_data;
        cmd.data.variant.size = data_size;
        cloudy_put_command(ascl->listener->tunnel, &cmd);
        
        free(ascl);
        ascl = next;
    }
    client->app_state_change_listeners = NULL;
    mutex_unlock(client->app_state_change_listeners_lock);
}

void syscall_add_app_state_change_listener(struct app *app, int callback_id, char *args) {
    struct app_state_change_listener *listener = (struct app_state_change_listener *)malloc(sizeof(struct app_state_change_listener));
    listener->listener = app;
    listener->callback_id = callback_id;
    mutex_lock(app->client->app_state_change_listeners_lock);
    listener->next = app->client->app_state_change_listeners;
    app->client->app_state_change_listeners = listener;
    mutex_unlock(app->client->app_state_change_listeners_lock);
}

void syscall_initialize() {
    syscall_handlers[GET_APPS_CONFIG] = syscall_get_apps_config;
    syscall_handlers[ADD_APP_STATE_CHANGE_LISTENER] = syscall_add_app_state_change_listener;
}

void syscall_caller(struct variant *syscall_data, struct app *app) {
    char *stream = syscall_data->content;
    int callback_id = bytestream_read_int(&stream);
    int sys_call_code = bytestream_read_int(&stream);
    if (sys_call_code < 0 || sys_call_code >= LAST_SYSCALL_CODE) {
        printf("Invalid syscall code: %i\n", sys_call_code);
        return;
    }
    syscall_handlers[sys_call_code](app, callback_id, stream);
}