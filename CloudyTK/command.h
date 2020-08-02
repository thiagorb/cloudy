#ifndef COMMAND_H
#define	COMMAND_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "command_def.h"
#include "cloudy_widgets.h"
#include "tunnel.h"
    
#define VARIABLE_COMMAND_SIZE -1

struct ack {
    int id;
    long long time;
};

struct command {
    enum command_id id;
    
    // Command parameters
    union {
        // CMD_DATA
        struct variant variant;
        
        // SU_DRAW_IMAGE
        // SU_DRAW_STRING
        
        // SU_CHANGE_COLOR
        union color color;
        
        // APP_RESIZE
        // SU_DRAW_RECT
        // SU_FILL_RECT
        // SU_CHANGE_CLIP_RECT
        struct rectangle rectangle;
        
        // 
        struct dimension dimension;
        
        // CMD_CLICK
        struct position position;
        
        struct ack ack;
        
        // CMD_KEYPRESS, LOGIN
        int code;
    } data;
};

struct command_queue {
    struct command_queue_node *first;
    struct command_queue_node *last;
    unsigned int total_bytes;
};

struct command_queue_node {
    struct command_queue_node *next;
    struct command cmd;
};

typedef void (*callback_handler)(void *args, struct variant *data);

int cloudy_get_command(struct tunnel *tunnel, struct command *cmd);
void cloudy_put_command(struct tunnel *tunnel, struct command *cmd);
int commands_parameters_size[TOTAL_COMMANDS];
void cloudy_commands_initialize();
void consume_callback(int id, struct variant *data);
void cloudy_set_callback(struct tunnel* tunnel, void *args, callback_handler callback);
int create_callback(void *args, callback_handler callback);
const char *cloudy_command_name(enum command_id id);
unsigned int cloudy_command_total_bytes(struct command *cmd);

#ifdef	__cplusplus
}
#endif

#endif	/* COMMAND_H */

