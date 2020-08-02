#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pipe.h"
#include "cloudy_widgets.h"
#include "cloudy_app.h"
#include "command.h"
#include "tunnel.h"

typedef void (*command_handler)(struct command *cmd);
command_handler command_handlers[TOTAL_COMMANDS];

struct cloudy_widget *app_interface = NULL;
struct cloudy_widget *app_focus = NULL;
struct cloudy_graphics client_graphics;
struct tunnel *app_tunnel;
int app_running = 0;

void (*app_login_handler)(int success);

struct cloudy_widget *cloudy_get_app_interface() {
    return app_interface;
}

void cloudy_login(char* login, char* password) {
    int login_len = strlen(login);
    int pass_len = strlen(password);
    char content[login_len + pass_len + 2];
    strcpy(content, login);
    strcpy(&content[login_len + 1], password);
    
    struct command cmd;
    cmd.id = DO_LOGIN;
    cmd.data.variant.size = login_len + pass_len + 2;
    cmd.data.variant.content = content;
    cloudy_put_command(app_tunnel, &cmd);
}

void untreated_command_handler(struct command *cmd) {
    printf("Untreated command: [id = \"%i\"]\n", cmd->id);
    cloudy_app_terminate();
}

void app_resize_handler(struct command *cmd) {
    cloudy_widget_resize(app_interface, rectangle_data(cmd->data.rectangle));
    cloudy_widget_repaint(app_interface);
}

void app_repaint_handler(struct command *cmd) {
    cloudy_widget_repaint(app_interface);
}

void cmd_click_handler(struct command *cmd) {
    struct cloudy_widget* widget;
    struct cloudy_widget* child = app_interface;
    struct position *p = &cmd->data.position;
    while (child) {
        widget = child;
        p->x -= widget->rectangle.position.x;
        p->y -= widget->rectangle.position.y;
        child = widget->children;
        while (child) {
            if (child->visible && rectangle_contains(&child->rectangle, &cmd->data.position))
                break;
            child = child->next;
        }
    }
    if (widget->click) widget->click(widget, p);
}

void cmd_callback_handler(struct command *cmd) {
    //if (app_focus && app_focus->data) app_focus->data(app_focus, &cmd->data.variant);
    struct command data_cmd;
    cloudy_get_command(app_tunnel, &data_cmd);
    consume_callback(cmd->data.code, &data_cmd.data.variant);
}

void login_handler(struct command *cmd) {
    if (app_login_handler) app_login_handler(cmd->data.code);
}

void initialize_command_handlers() {
    int i;
    for (i = 0; i < TOTAL_COMMANDS; i++) {
        command_handlers[i] = untreated_command_handler;
    }
    command_handlers[APP_RESIZE] = app_resize_handler;
    command_handlers[APP_REPAINT] = app_repaint_handler;
    command_handlers[CMD_CLICK] = cmd_click_handler;
    command_handlers[CMD_CALLBACK] = cmd_callback_handler;
    command_handlers[LOGIN] = login_handler;
}

void cloudy_app_initialize() {
    struct pipe *pipe = pipe_from_fd(3, 4);
    app_tunnel = tunnel_create_from_pipe(pipe, pipe);
    app_interface = cloudy_widget_create();
    app_interface->resize = NULL;
    app_interface->repaint = cloudy_interface_repaint;
    client_graphics.color.rgb = 0x0;
    initialize_command_handlers();
    cloudy_commands_initialize();
    app_running = 1;
}

void cloudy_app_terminate() {
    app_running = 0;
    struct command cmd;
    cmd.id = APP_END;
    cloudy_put_command(app_tunnel, &cmd);
}

void cloudy_app_minimize() {
    struct command cmd;
    cmd.id = APP_MINIMIZE;
    cloudy_put_command(app_tunnel, &cmd);
}

void command_handler_caller(struct command *cmd) {
    if (cmd->id < TOTAL_COMMANDS) {
        command_handlers[cmd->id](cmd);
    } else {
        untreated_command_handler(cmd);
    }
}

void cloudy_set_app_repaint_event(void (*event)(struct cloudy_widget* widget)) {
    app_interface->repaint = event;
}

void cloudy_app_start() {
    struct command cmd;
    cmd.id = APP_START;
    cloudy_put_command(app_tunnel, &cmd);
    while (app_running) {
        if (!cloudy_get_command(app_tunnel, &cmd)) {
            command_handler_caller(&cmd);
        }
        if (!app_running) {
            printf("app end\n");
        }
    }
}

void cloudy_system_call_callback(void *callback, struct variant *variant) {
    //printf("syscall callback!!\n");
    void (*cb)(struct variant *data);
    cb = callback;
    cb(variant);
    free(variant->content);
}

void cloudy_system_call(void callback(struct variant *data), int sys_call_code) {
    //printf("syscall %i!!\n", sys_call_code);
    int callback_id = create_callback(callback, cloudy_system_call_callback);
    struct command cmd;
    cmd.id = APP_SYSCALL;
    char syscall_data[sizeof(int) * 2];
    char *stream = syscall_data;
    bytestream_write_int(&stream, callback_id);
    bytestream_write_int(&stream, sys_call_code);
    cmd.data.variant.size = sizeof(int) * 2;
    cmd.data.variant.content = syscall_data;
    cloudy_put_command(app_tunnel, &cmd);
}

void cloudy_app_exec(char *app_path) {
    struct command cmd;
    cmd.id = APP_EXEC;
    cmd.data.variant.size = strlen(app_path) + 1;
    //printf("%i %s\n", cmd.data.variant.size, app_path);
    cmd.data.variant.content = app_path;
    cloudy_put_command(app_tunnel, &cmd);
}

void cloudy_app_show(int pid) {
    struct command cmd;
    cmd.id = APP_SHOW;
    cmd.data.code = pid;
    cloudy_put_command(app_tunnel, &cmd);
}