#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <byteswap.h>
#include "app.h"
#include "client.h"
#include "cloudytk.h"
#include "thread.h"
#include "mutex.h"

typedef void (*command_handler)(struct client *client, struct command *cmd, struct app *app);
command_handler command_handlers[TOTAL_COMMANDS];
struct client *clients = NULL;
long long last_ack = 0;
const char *last_cmd = NULL;

void untreated_command_handler(struct client *client, struct command *cmd, struct app *app) {
    char *from = "client";
    if (app) from = app->path;
    printf("Server received untreated command from %s: [id = \"%x\"]\n", from, cmd->id);
}

void command_handler_caller(struct client *client, struct command *cmd, struct app *app) {
    if (cmd->id < TOTAL_COMMANDS) {
        command_handlers[cmd->id](client, cmd, app);
    } else {
        untreated_command_handler(client, cmd, app);
    }
}

void app_handle(struct app *app) {
    // Apps commands being received
    while (app->running) {
        struct command cmd;
        if (!cloudy_get_command(app->tunnel, &cmd)) {
            command_handler_caller(app->client, &cmd, app);
        } else {
            app->running = 0;
        }
    }
    printf("app encerrada: %s\n", app->path);
    app_destroy(app);
    deliver_app_state_change_event(app->client);
}

char *strclone(char *src) {
    char *ret = (char *)malloc(sizeof(char) * (strlen(src) + 1));
    strcpy(ret, src);
    return ret;
}

void client_load_app_config(struct client *client) {
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;
    doc = xmlReadFile("./apps_config.xml", NULL, 0);
    root_element = xmlDocGetRootElement(doc);
    
    struct app_config **p_app_cfg = &client->app_config;
    xmlNode *xml_app;
    for (xml_app = root_element->children; xml_app; xml_app = xml_app->next) {
        if (xml_app->type == XML_ELEMENT_NODE && !strcmp(xml_app->name, "Application")) {
            *p_app_cfg = (struct app_config *)malloc(sizeof(struct app_config));
            (*p_app_cfg)->app_path = NULL;
            (*p_app_cfg)->icon_path = NULL;
            (*p_app_cfg)->next = NULL;
            
            xmlAttr *attr;
            for (attr = xml_app->properties; attr; attr = attr->next) {
                if (attr->type == XML_ATTRIBUTE_NODE) {
                    if (!strcmp(attr->name, "Path")) {
                        (*p_app_cfg)->app_path = strclone(attr->children->content);
                    } else if (!strcmp(attr->name, "Icon")) {
                        (*p_app_cfg)->icon_path = strclone(attr->children->content);
                    }
                }
            }
            p_app_cfg = &(*p_app_cfg)->next;
        }
    }
    xmlFreeDoc(doc);
}

void client_handle(struct client *client) {
    client_load_app_config(client);
    while (client->connected) {
        struct command cmd;
        if (!cloudy_get_command(client->tunnel, &cmd)) {
            client->bytes_received += cloudy_command_total_bytes(&cmd);
            command_handler_caller(client, &cmd, NULL);
            last_cmd = cloudy_command_name(cmd.id);
        } else {
            untreated_command_handler(client, &cmd, NULL);
            client->connected = 0;
        }
    }
}

struct app *client_app_start(struct client *client, char* path) {
    struct app *app = app_start(path);
    app->next = client->apps;
    app->client = client;
    client->current_app = app;
    client->apps = app;
    thread_start((void (*)(void *))app_handle, app);
    return app;
}

struct client *client_create(struct socket *socket) {
    struct client *client = (struct client *)malloc(sizeof(struct client));
    client->tunnel = tunnel_create_from_socket(socket);
    client->apps = NULL;
    client->current_app = NULL;
    client->menu_app = NULL;
    client->login_app = NULL;
    client->connected = 1;
    client->next = clients;
    client->app_config = NULL;
    client->app_state_change_listeners = NULL;
    client->command_queue.first = NULL;
    client->command_queue.last = NULL;
    client->command_queue.total_bytes = 0;
    client->app_state_change_listeners_lock = mutex_create();
    client->bytes_sent = 0;
    client->bytes_received = 0;
    client->client_log = stdout; //fopen("log.txt", "wb");
    clients = client;
}

void client_destroy(struct client* client) {
    tunnel_destroy(client->tunnel);
    struct app* app = client->apps;
    while (app) {
        struct app* next = app->next;
        app_destroy(app);
        app = next;
    }
    // Falta retirar da lista de clientes
    mutex_destroy(client->app_state_change_listeners_lock);
    free(client);
}

void client_disconnect_all() {
    struct client *client = clients;
    while (client) {
        struct client *next = client->next;
        client_destroy(client);
        client = next;
    }
}

void client_app_not_found_handler(struct client *client, struct command *cmd, struct app *app) {
    printf("App not found: [path = \"%s\"]\n", app->path);
    app_destroy(app);
}

void client_cmd_init_handler(struct client *client, struct command *cmd, struct app *app) {
    client->device_resolution = cmd->data.dimension;
    client->login_app = client_app_start(client, "./apps/loginapp");
}

void client_cmd_click_handler(struct client *client, struct command *cmd, struct app *app) {
    if (client->current_app) cloudy_put_command(client->current_app->tunnel, cmd);
}

void client_cmd_callback_handler(struct client *client, struct command *cmd, struct app *app) {
    //printf("code %i\n", cmd->data.code);
    if (client->current_app) cloudy_put_command(client->current_app->tunnel, cmd);
}

void client_cmd_ack_handler(struct client *client, struct command *cmd, struct app *app) {
    long long time = bswap_64(cmd->data.ack.time);
    long long delay = time - last_ack;
    if (last_ack && cmd->data.ack.id == SU_REFRESH) {
        fprintf(client->client_log, "ack: {cmd: %s, delay: %lli, sent: %i, received: %i}\n", last_cmd, delay, client->bytes_sent, client->bytes_received);
    }
    last_ack = time;
}

void client_cmd_data_handler(struct client *client, struct command *cmd, struct app *app) {
    if (client->current_app) cloudy_put_command(client->current_app->tunnel, cmd);
    free(cmd->data.variant.content);
}

void client_cmd_key_press_handler(struct client *client, struct command *cmd, struct app *app) {

}

void client_flush_queue(struct client *client) {
    client->bytes_sent += client->command_queue.total_bytes + sizeof(int);
    tunnel_write_int(client->tunnel, client->command_queue.total_bytes);
    while (client->command_queue.first) {
        struct command *cmd = &client->command_queue.first->cmd;
        cloudy_put_command(client->tunnel, cmd);
        if (commands_parameters_size[cmd->id] == VARIABLE_COMMAND_SIZE) free(cmd->data.variant.content);
        struct command_queue_node *next = client->command_queue.first->next;
        free(client->command_queue.first);
        client->command_queue.first = next;
    }
    client->command_queue.first = NULL;
    client->command_queue.last = NULL;
    client->command_queue.total_bytes = 0;
}

void command_forward_handler(struct client *client, struct command *cmd, struct app *app) {
    if (app != client->current_app) return;
    struct command_queue_node *new = (struct command_queue_node *)malloc(sizeof(struct command_queue_node));
    new->cmd = *cmd;
    new->next = NULL;
    if (client->command_queue.last) {
        client->command_queue.last->next = new;
    } else {
        client->command_queue.first = new;
    }
    client->command_queue.last = new;
    client->command_queue.total_bytes += cloudy_command_total_bytes(cmd);
    if (cmd->id == SU_REFRESH || cmd->id == APP_INPUT_DATA) {
        client_flush_queue(client);
    }
}

void initialize_command_handlers() {
    int i;
    for (i = 0; i < TOTAL_COMMANDS; i++) {
        command_handlers[i] = untreated_command_handler;
    }
    command_handlers[APP_START] = app_start_handler;
    command_handlers[APP_MINIMIZE] = app_minimize_handler;
    command_handlers[APP_NOT_FOUND] = client_app_not_found_handler;
    command_handlers[APP_EXEC] = app_exec_handler;
    command_handlers[APP_SHOW] = app_show_handler;
    command_handlers[APP_END] = app_end_handler;
    command_handlers[APP_SYSCALL] = app_syscall_handler;
    command_handlers[DO_LOGIN] = do_login_handler;
    command_handlers[CMD_INIT] = client_cmd_init_handler;
    command_handlers[CMD_CLICK] = client_cmd_click_handler;
    command_handlers[CMD_KEYPRESS] = client_cmd_key_press_handler;
    command_handlers[CMD_CALLBACK] = client_cmd_callback_handler;
    command_handlers[CMD_ACK] = client_cmd_ack_handler;
    command_handlers[SU_CHANGE_CLIP_RECT] = command_forward_handler;
    command_handlers[SU_CHANGE_COLOR] = command_forward_handler;
    command_handlers[SU_DRAW_RECT] = command_forward_handler;
    command_handlers[SU_FILL_RECT] = command_forward_handler;
    command_handlers[SU_DRAW_STRING] = command_forward_handler;
    command_handlers[SU_DRAW_IMAGE] = command_forward_handler;
    command_handlers[SU_REFRESH] = command_forward_handler;
    command_handlers[SU_SET_DRAW_POSITION] = command_forward_handler;
    command_handlers[APP_INPUT_DATA] = command_forward_handler;
    command_handlers[CMD_DATA] = client_cmd_data_handler;
    command_handlers[SET_CALLBACK] = command_forward_handler;
}
