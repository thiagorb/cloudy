#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "app.h"
#include "tunnel.h"
#include "pipe.h"
#include "command.h"
#include "syscall.h"

struct app* app_start(char* path) {
    struct app *app = (struct app *)malloc(sizeof(struct app));
    struct pipe *pipe_srv_app = pipe_create(1);
    struct pipe *pipe_app_srv = pipe_create(0);
    app->pid = fork();
    if (app->pid) {
        pipe_readonly(pipe_app_srv);
        pipe_writeonly(pipe_srv_app);
    } else {
        client_disconnect_all();
        pipe_writeonly(pipe_app_srv);
        pipe_readonly(pipe_srv_app);
        dup2(pipe_srv_app->fd_in, 3);
        dup2(pipe_app_srv->fd_out, 4);
        close(pipe_srv_app->fd_in);
        close(pipe_app_srv->fd_out);
        pipe_srv_app->fd_in = 3;
        pipe_app_srv->fd_out = 4;
        //execlp("valgrind", "valgrind", path, (char *) 0);
        execlp(path, path, (char *) 0);
        struct tunnel *server_tunnel = tunnel_create_from_pipe(pipe_srv_app, pipe_app_srv);
        struct command cmd;
        cmd.id = APP_NOT_FOUND;
        cloudy_put_command(server_tunnel, &cmd);
        exit(1);
    }
    app->client = NULL;
    app->next = NULL;
    app->running = 1;
    app->path = path;
    app->tunnel = tunnel_create_from_pipe(pipe_app_srv, pipe_srv_app);
    return app;
}

void app_destroy(struct app *app) {
    app->running = 0;
    waitpid(app->pid, NULL, 0);
    tunnel_destroy(app->tunnel);
    // Falta verificar condições de corrida
    struct app **p = &app->client->apps;
    while (*p) {
    	if (*p == app) {
            *p = app->next;
            break;
    	}
    	p = &(*p)->next;
    }
    if (app->client->current_app == app) {
        app->client->current_app = app->client->current_app->next;
    if (!app->client->current_app && app->client->apps) app->client->current_app = app->client->apps;
    if (app->client->current_app) {
        struct command response;
        response.id = APP_REPAINT;
        cloudy_put_command(app->client->current_app->tunnel, &response);
    }
    }
    if (app->client->login_app == app) app->client->login_app = NULL;
    if (app->client->menu_app == app) app->client->menu_app = NULL;
    free(app);
}

void app_start_handler(struct client *client, struct command *cmd, struct app *app) {
    deliver_app_state_change_event(client);
    struct command response;
    response.id = APP_RESIZE;
    set_rect(&response.data.rectangle, 0, 0, client->device_resolution.width, client->device_resolution.height);
    cloudy_put_command(app->tunnel, &response);
}

void app_minimize_handler(struct client *client, struct command *cmd, struct app *app) {
    if (app == client->current_app) {
        client->current_app = client->menu_app;
        struct command response;
        response.id = APP_REPAINT;
        set_rect(&response.data.rectangle, 0, 0, client->device_resolution.width, client->device_resolution.height);
        cloudy_put_command(client->menu_app->tunnel, &response);
    }
}

void app_end_handler(struct client *client, struct command *cmd, struct app *app) {
    app->running = 0;
}

void app_exec_handler(struct client *client, struct command *cmd, struct app *app) {
    if (!strcmp(cmd->data.variant.content, "./cloudy")) {
        //fclose(client->client_log);
        exit(0);
    } else {
        client_app_start(client, cmd->data.variant.content);
    }
}

void app_show_handler(struct client *client, struct command *cmd, struct app *app) {
    struct app *a;
    for (a = client->apps; a; a = a->next) {
        if (a->pid == cmd->data.code) {
            client->current_app = a;
            struct command response;
            response.id = APP_REPAINT;
            cloudy_put_command(app->client->current_app->tunnel, &response);
            break;
        }
    }
}

void app_syscall_handler(struct client *client, struct command *cmd, struct app *app) {
    syscall_caller(&cmd->data.variant, app);
    free(cmd->data.variant.content);
}

void do_login_handler(struct client *client, struct command *cmd, struct app *app) {
    char *login = cmd->data.variant.content;
    char *pass = &login[strlen(login) + 1];
    struct command response;
    response.id = LOGIN;
    response.data.code = !(strcmp(login, "thiago") || strcmp(pass, "1234"));
    client->menu_app = client_app_start(client, "./apps/menuapp");
    cloudy_put_command(app->tunnel, &response);
}
