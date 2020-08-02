#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interface.h"
#include "bytestream.h"
#include "stdbool.h"

struct app_config {
    char *app_path;
    char *icon_path;
    struct cloudy_widget *button;
    struct app_config *next;
};

struct running_app {
    struct app_config *config;
    int pid;
    int remove;
    struct cloudy_widget *button;
    struct running_app *next;
};

struct cloudy_widget *interface;

struct cloudy_widget *lbMyApps;
struct cloudy_widget *lbRunningApps;

struct app_config *app_config = NULL;
struct running_app *running_apps = NULL;

void app_button_click(struct cloudy_widget *button, struct position *p) {
    struct app_config *app_cfg;
    for (app_cfg = app_config; app_cfg; app_cfg = app_cfg->next) {
        if (app_cfg->button == button) {
            cloudy_app_exec(app_cfg->app_path);
        }
    }
}

void running_app_button_click(struct cloudy_widget *button, struct position *p) {
    
    struct running_app *app;
    for (app = running_apps; app; app = app->next) {
        if (app->button == button) {
            cloudy_app_show(app->pid);
        }
    }
}

void get_apps_config(struct variant *data) {
    char *stream = data->content;
    int napps = bytestream_read_int(&stream);
    //printf("%i\n", napps);
    int i;
    struct app_config **cfg = &app_config;
    for (i = 0; i < napps; i++) {
        struct app_config *c = (struct app_config *)malloc(sizeof(struct app_config));
        
        c->app_path = bytestream_read_string(&stream);
        c->icon_path = bytestream_read_string(&stream);
        //printf("%s %s\n", c->app_path, c->icon_path);
        
        c->button = cloudy_icon_button_create(c->icon_path);
        c->button->click = app_button_click;
        cloudy_widget_move(c->button, 10 + i * 33, 30);
        cloudy_widget_add(interface, c->button);
        c->next = NULL;
        
        *cfg = c;
        cfg = &c->next;
    }
}

void running_apps_change(struct variant *data) {
    //printf("RUNNING APPS CHANGE\n");
    struct running_app *run_app;
    for (run_app = running_apps; run_app; run_app = run_app->next) {
        run_app->remove = true;
    }
    
    char *stream = data->content;
    int napps = bytestream_read_int(&stream);
    int i;
    for (i = 0; i < napps; i++) {
        char *app_path = bytestream_read_string(&stream);
        int pid = bytestream_read_int(&stream);
        for (run_app = running_apps; run_app; run_app = run_app->next) {
            if (run_app->pid == pid) {
                run_app->remove = false;
                break;
            }
        }
        if (!run_app) {
            struct app_config *c;
            //printf("app_config %x\n", app_config);
            for (c = app_config; c; c = c->next) {
                if (!strcmp(c->app_path, app_path)) {
                    struct running_app *new_app = (struct running_app *)malloc(sizeof(struct running_app));
                    new_app->button = cloudy_icon_button_create(c->icon_path);
                    new_app->config = c;
                    new_app->pid = pid;
                    new_app->remove = false;
                    new_app->next = running_apps;
                    running_apps = new_app;
                    new_app->button->click = running_app_button_click;
                    cloudy_widget_add(interface, new_app->button);
                    break;
                }
            }
        }
        free(app_path);
    }
    
    i = 0;
    struct running_app **papp = &running_apps;
    while (*papp) {
        run_app = *papp;
        if (run_app->remove) {
            cloudy_widget_remove(interface, run_app->button);
            cloudy_widget_destroy(run_app->button);
            *papp = run_app->next;
            free(run_app);
        } else {
            cloudy_widget_move(run_app->button, 10 + i * 33, interface->rectangle.dimension.height / 2 + 20);
            i++;
            papp = &run_app->next;
        }
    }
    cloudy_system_call(running_apps_change, 1);
    cloudy_widget_repaint(interface);
}

void interface_resize(struct cloudy_widget *interface) {
    int width = interface->rectangle.dimension.width;
    int height = interface->rectangle.dimension.height;
    
    cloudy_widget_move(lbRunningApps, 10, height / 2);
}

int main() {
    printf("Início do menu app\n");
    cloudy_app_initialize();
    
    interface = cloudy_get_app_interface();
    interface->resize = interface_resize;
    
    lbMyApps = cloudy_label_create("My apps");
    cloudy_widget_add(interface, lbMyApps);
    cloudy_widget_resize(lbMyApps, 10, 10, 70, 20);
    lbRunningApps = cloudy_label_create("Running apps");
    cloudy_widget_resize(lbRunningApps, 10, 10, 70, 20);
    cloudy_widget_add(interface, lbRunningApps);
    
    cloudy_system_call(get_apps_config, 0);
    
    cloudy_system_call(running_apps_change, 1);
    cloudy_app_start();
    return 0;
}
