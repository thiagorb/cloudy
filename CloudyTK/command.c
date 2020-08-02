#include <stdlib.h>
#include <stdio.h>
#include "tunnel.h"
#include "command.h"

#undef ENUM_BEGIN
#undef ENUM
#undef ENUM_END
#define ENUM_BEGIN(typ) const char * typ ## _string [] = {
#define ENUM(nam) #nam
#define ENUM_END };
#undef COMMAND_DEF_H
#include "command_def.h"

struct callback *callbacks = NULL;
unsigned int last_callback_id = 0;

struct callback {
    int id;
    void *args;
    callback_handler callback;
    struct callback *next;
};

int cloudy_get_command(struct tunnel *tunnel, struct command *cmd) {
    cmd->id = TOTAL_COMMANDS;
    if (!tunnel_read_int(tunnel, (int*)&cmd->id)) {
        if (cmd->id < TOTAL_COMMANDS) {
            //printf("recebendo comando: %i, data = ", cmd->id);
            if (commands_parameters_size[cmd->id] == VARIABLE_COMMAND_SIZE) {
                cmd->data.variant.size = 0;
                tunnel_read_int(tunnel, &cmd->data.variant.size);
                //printf("%i bytes\n", cmd->data.variant.size);
                cmd->data.variant.content = malloc(cmd->data.variant.size);
                tunnel_read(tunnel, cmd->data.variant.content, cmd->data.variant.size);
            } else {
                //printf("%i bytes\n", commands_parameters_size[cmd->id]);
                tunnel_read(tunnel, (void*)&cmd->data, commands_parameters_size[cmd->id]);
            }
            return 0;
        } else {
            return 1;
        }
    }
    return 1;
}

unsigned int cloudy_command_total_bytes(struct command *cmd) {
    unsigned int total_bytes = 0;
    total_bytes += sizeof(cmd->id);
    int size = commands_parameters_size[cmd->id];
    if (size == VARIABLE_COMMAND_SIZE) {
        total_bytes += sizeof(cmd->data.variant.size) + cmd->data.variant.size;
    } else {
        total_bytes += size;
    }
    return total_bytes;
}

void cloudy_put_command(struct tunnel *tunnel, struct command *cmd) {
    tunnel_write_int(tunnel, cmd->id);
    
    void* content;
    int size = commands_parameters_size[cmd->id];
    if (size == VARIABLE_COMMAND_SIZE) {
        size = cmd->data.variant.size;
        content = cmd->data.variant.content;
        tunnel_write_int(tunnel, size);
    } else {
        content = &cmd->data;
    }
    tunnel_write(tunnel, content, size);
    //printf("enviando comando: %i, data = %i bytes\n", cmd->id, size);
}

int create_callback(void *args, callback_handler callback) {
    struct callback *c = (struct callback *)malloc(sizeof(struct callback));
    c->id = last_callback_id++;
    c->next = callbacks;
    c->args = args;
    c->callback = callback;
    callbacks = c;
    return c->id;
}

void create_and_send_callback(struct tunnel *tunnel, void *args, callback_handler callback, enum command_id cmd_id) {
    struct command cmd;
    cmd.id = cmd_id;
    cmd.data.code = create_callback(args, callback);
    cloudy_put_command(tunnel, &cmd);
}

void cloudy_set_callback(struct tunnel *tunnel, void *args, callback_handler callback) {
    create_and_send_callback(tunnel, args, callback, SET_CALLBACK);
}

void consume_callback(int id, struct variant *data) {
    struct callback **pcb = &callbacks;
    while (*pcb) {
        struct callback *cb = *pcb;
        if (cb->id == id) {
            *pcb = cb->next;
            cb->callback(cb->args, data);
            free(cb);
            return;
        }
        pcb = &cb->next;
    }
    printf("callback not found!! %i\n", id);
}

const char *cloudy_command_name(enum command_id id) {
    return command_id_string[id];
}

void cloudy_commands_initialize() {
    struct command cmd;
    commands_parameters_size[SU_CHANGE_COLOR] = sizeof(union color);
    commands_parameters_size[SU_CHANGE_CLIP_RECT] = sizeof(struct rectangle);
    commands_parameters_size[SU_DRAW_STRING] = VARIABLE_COMMAND_SIZE;
    commands_parameters_size[SU_DRAW_IMAGE] = VARIABLE_COMMAND_SIZE;
    commands_parameters_size[SU_DRAW_RECT] = sizeof(struct dimension);
    commands_parameters_size[SU_FILL_RECT] = sizeof(struct dimension);
    commands_parameters_size[SU_SET_DRAW_POSITION] = sizeof(struct position);
    commands_parameters_size[SU_REFRESH] = 0;
    commands_parameters_size[CMD_INIT] = sizeof(struct dimension);
    commands_parameters_size[CMD_CLICK] = sizeof(struct position);
    commands_parameters_size[CMD_KEYPRESS] = sizeof(cmd.data.code);
    commands_parameters_size[CMD_DATA] = VARIABLE_COMMAND_SIZE;
    commands_parameters_size[CMD_ACK] = sizeof(struct ack);
    commands_parameters_size[APP_NOT_FOUND] = 0;
    commands_parameters_size[APP_START] = 0;
    commands_parameters_size[APP_MINIMIZE] = 0;
    commands_parameters_size[APP_END] = 0;
    commands_parameters_size[APP_EXEC] = VARIABLE_COMMAND_SIZE;
    commands_parameters_size[APP_SHOW] = sizeof(cmd.data.code);
    commands_parameters_size[APP_RESIZE] = sizeof(struct rectangle);
    commands_parameters_size[APP_REPAINT] = 0;
    commands_parameters_size[APP_INPUT_DATA] = 0;
    commands_parameters_size[APP_SYSCALL] = VARIABLE_COMMAND_SIZE;
    commands_parameters_size[DO_LOGIN] = VARIABLE_COMMAND_SIZE;
    commands_parameters_size[LOGIN] = sizeof(cmd.data.code);
    commands_parameters_size[CMD_CALLBACK] = sizeof(cmd.data.code);
    commands_parameters_size[SET_CALLBACK] = sizeof(cmd.data.code);
}
