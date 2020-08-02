#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>
#include "cloudy_widgets.h"
#include "cloudy_app.h"
#include "interface.h"
#include "command.h"

extern struct tunnel *app_tunnel;
extern struct cloudy_widget *app_focus;
extern struct cloudy_graphics client_graphics;
struct cloudy_graphics current_graphics;

int button_order = 0;
union color button_colors[] = {0xFFFF8000, 0xFF2020FF, 0xFFFF00FF, 0xFFF00000, 0xFF20FF20, 0xFF00FFFF};
#define num_button_colors (sizeof(button_colors) / sizeof(*button_colors))

void set_rect(struct rectangle *rectangle, int x, int y, int width, int height) {
    rectangle->position.x = x;
    rectangle->position.y = y;
    rectangle->dimension.width = width;
    rectangle->dimension.height = height;
}

int rectangle_equals(struct rectangle *r1, struct rectangle *r2) {
    if (r1->position.x != r2->position.x) return 0;
    if (r1->position.y != r2->position.y) return 0;
    if (r1->dimension.width != r2->dimension.width) return 0;
    if (r1->dimension.height != r2->dimension.height) return 0;
    return 1;
}

int rectangle_contains(struct rectangle *r, struct position *p) {
    return (r->position.x <= p->x) &&
           (r->position.y <= p->y) && 
           (r->position.x + r->dimension.width > p->x) &&
           (r->position.y + r->dimension.height > p->y);
}

void create_graphics(struct cloudy_widget *widget, struct cloudy_graphics *graphics) {
    struct position *pos = &graphics->rectangle.position;
    struct dimension *dim = &graphics->rectangle.dimension;
    graphics->rectangle = widget->rectangle;
    struct cloudy_widget *w = widget->parent;
    while (w) {
        struct position *p = &w->rectangle.position;
        struct dimension *d = &w->rectangle.dimension;
        pos->x += p->x;
        pos->y += p->y;
        if (pos->x + dim->width > d->width) dim->width = d->width - pos->x;
        if (pos->y + dim->height > d->height) dim->height = d->height - pos->y;
        w = w->parent;
    }
    graphics->color.rgb = 0x0;
}

void cloudy_update_graphics() {
    struct command cmd;
    if (!rectangle_equals(&client_graphics.rectangle, &current_graphics.rectangle)) {
        cmd.id = SU_CHANGE_CLIP_RECT;
        cmd.data.rectangle = current_graphics.rectangle;
        cloudy_put_command(app_tunnel, &cmd);
    }
    if (client_graphics.color.rgb != current_graphics.color.rgb) {
        cmd.id = SU_CHANGE_COLOR;
        cmd.data.color = current_graphics.color;
        cloudy_put_command(app_tunnel, &cmd);
    }
    client_graphics = current_graphics;
}

void cloudy_change_color(unsigned int a, unsigned int r, unsigned int g, unsigned int b) {
    unsigned rgb = r + (g << 8) + (b << 16) + (a << 24);
    current_graphics.color.rgb = rgb;
}

void cloudy_set_draw_position(int x, int y) {
    cloudy_update_graphics();
    struct command cmd;
    cmd.id = SU_SET_DRAW_POSITION;
    cmd.data.position.x = x + client_graphics.rectangle.position.x;
    cmd.data.position.y = y + client_graphics.rectangle.position.y;
    cloudy_put_command(app_tunnel, &cmd);
}

void cloudy_draw_string(char *string, int x, int y) {
    cloudy_update_graphics();
    struct command cmd;
    cloudy_set_draw_position(x, y);
    cmd.id = SU_DRAW_STRING;
    //printf("string: %s\n", string);
    cmd.data.variant.size = strlen(string) + 1;
    cmd.data.variant.content = string;
    cloudy_put_command(app_tunnel, &cmd);
}

void cloudy_draw_rect(int x, int y, int width, int height) {
    cloudy_update_graphics();
    struct command cmd;
    cloudy_set_draw_position(x, y);
    cmd.id = SU_DRAW_RECT;
    cmd.data.dimension.width = width - 1;
    cmd.data.dimension.height = height - 1;
    cloudy_put_command(app_tunnel, &cmd);
}

void cloudy_draw_image(char* image_data, int image_length, int x, int y) {
    struct command cmd;
    cloudy_set_draw_position(x, y);
    cmd.id = SU_DRAW_IMAGE;
    //printf("image: %s\n", image_data);
    cmd.data.variant.size = image_length;
    cmd.data.variant.content = image_data;
    cloudy_put_command(app_tunnel, &cmd);
}

void cloudy_fill_rect(int x, int y, int width, int height) {
    struct command cmd;
    cloudy_set_draw_position(x, y);
    cmd.id = SU_FILL_RECT;
    cmd.data.dimension.width = width;
    cmd.data.dimension.height = height;
    cloudy_put_command(app_tunnel, &cmd);
}

void cloudy_widget_resize(struct cloudy_widget *widget, int x, int y, int width, int height) {
    set_rect(&widget->rectangle, x, y, width, height);
    if (widget->resize) widget->resize(widget);
}

void cloudy_widget_move(struct cloudy_widget *widget, int x, int y) {
    widget->rectangle.position.x = x;
    widget->rectangle.position.y = y;
}

void cloudy_widget_repaint_without_refresh(struct cloudy_widget *widget) {
    if (!widget->visible) return;
    create_graphics(widget, &current_graphics);
    widget->repaint(widget);
}

void cloudy_widget_repaint(struct cloudy_widget *widget) {
    if (!widget->repaint) return;
    if (!widget->visible) return;
    cloudy_widget_repaint_without_refresh(widget);
    struct command cmd;
    cmd.id = SU_REFRESH;
    cloudy_put_command(app_tunnel, &cmd);
}

void cloudy_interface_repaint(struct cloudy_widget *interface) {
    //printf("interface repaint\n");
    client_graphics.color.rgb = 0;
    cloudy_change_color(255, 255, 255, 255);
    cloudy_fill_rect(0, 0, interface->rectangle.dimension.width, interface->rectangle.dimension.height);
    struct cloudy_widget *child = interface->children;
    while (child) {
        cloudy_widget_repaint_without_refresh(child);
        child = child->next;
    }
}

void cloudy_widget_initialize(struct cloudy_widget *widget) {
    set_rect(&widget->rectangle, 0, 0, 0, 0);
    widget->repaint = NULL;
    widget->resize = NULL;
    widget->click = NULL;
    widget->destroy = NULL;
    widget->visible = true;
    widget->children = NULL;
    widget->next = NULL;
    widget->parent = NULL;
}

struct cloudy_widget *cloudy_widget_create() {
    struct cloudy_widget *widget = (struct cloudy_widget *)malloc(sizeof(struct cloudy_widget));
    cloudy_widget_initialize(widget);
    return widget;
}

void cloudy_widget_destroy(struct cloudy_widget *widget) {
    if (widget->destroy) {
        widget->destroy(widget);
    } else {
        printf("Trying to destroy widget that does not implement destroy method!\n");
    }
}

void cloudy_widget_add(struct cloudy_widget *container, struct cloudy_widget *child) {
    child->parent = container;
    child->next = container->children;
    container->children = child;
}

void cloudy_widget_remove(struct cloudy_widget *container, struct cloudy_widget *child) {
    struct cloudy_widget **pw = &container->children;
    while (*pw) {
        if (*pw == child) {
            *pw = child->next;
            child->parent = NULL;
            child->next = NULL;
            break;
        }
        pw = &(*pw)->next;
    }
}

void cloudy_textbox_repaint(struct cloudy_widget *textbox) {
    cloudy_change_color(255, 255, 255, 255);
    cloudy_fill_rect(0, 0, textbox->rectangle.dimension.width, textbox->rectangle.dimension.height);
    cloudy_change_color(255, 0, 0, 0);
    cloudy_draw_rect(0, 0, textbox->rectangle.dimension.width, textbox->rectangle.dimension.height);
    struct cloudy_textbox *tx = (struct cloudy_textbox *)textbox;
    if (tx->text) {
        if (tx->password_mask) {
            int len = strlen(tx->text);
            char mask[len + 1];
            memset(mask, tx->password_mask, len);
            mask[len] = 0;
            cloudy_draw_string(mask, 5, textbox->rectangle.dimension.height - 5);
        } else {
            cloudy_draw_string(tx->text, 5, textbox->rectangle.dimension.height - 5);
        }
    }
}

void cloudy_label_repaint(struct cloudy_widget *label) {
    cloudy_change_color(255, 255, 255, 255);
    cloudy_fill_rect(0, 0, label->rectangle.dimension.width, label->rectangle.dimension.height);
    cloudy_change_color(255, 0, 0, 0);
    struct cloudy_textbox *tx = (struct cloudy_textbox *)label;
    if (tx->text) {
        cloudy_draw_string(tx->text, 0, label->rectangle.dimension.height - 5);
    }
}

void cloudy_button_repaint(struct cloudy_widget *button) {
    struct cloudy_button *bt = (struct cloudy_button *)button;
    cloudy_change_color(color_data(bt->color));
    cloudy_fill_rect(0, 0, button->rectangle.dimension.width, button->rectangle.dimension.height);
    if (bt->text) {
        cloudy_change_color(255, 255, 255, 255);
        cloudy_draw_string(bt->text, 5, button->rectangle.dimension.height - 5);
    }
    if (bt->icon) {
        cloudy_draw_image(bt->icon, bt->icon_size, 0, 0);
    }
}

void cloudy_textbox_set_text(struct cloudy_widget *textbox, char *text) {
    struct cloudy_textbox *tx = (struct cloudy_textbox *)textbox;
    if (tx->text) free(tx->text);
    tx->text = (char *)malloc(sizeof(char) * (strlen(text) + 1));
    strcpy(tx->text, text);
    if (tx->text_changed) tx->text_changed(tx);
}

void cloudy_textbox_data(void *textbox, struct variant *variant) {
    struct cloudy_textbox *tx = (struct cloudy_textbox *)textbox;
    if (tx->text) free(tx->text);
    tx->text = (char *)malloc(sizeof(char) * (variant->size + 1));
    memcpy(tx->text, variant->content, variant->size);
    tx->text[variant->size] = 0;
    free(variant->content);
    if (tx->text_changed) tx->text_changed(tx);
    cloudy_widget_repaint(textbox);
}

void cloudy_textbox_click(struct cloudy_widget *textbox, struct position *p) {
    if (!textbox->visible) return;
    cloudy_set_callback(app_tunnel, textbox, cloudy_textbox_data);
    struct command cmd;
    cmd.id = APP_INPUT_DATA;
    cloudy_put_command(app_tunnel, &cmd);
}

struct cloudy_widget *cloudy_textbox_create() {
    struct cloudy_textbox *textbox = (struct cloudy_textbox *)malloc(sizeof(struct cloudy_textbox));
    cloudy_widget_initialize(&textbox->widget);
    textbox->widget.repaint = cloudy_textbox_repaint;
    textbox->widget.click = cloudy_textbox_click;
    textbox->text_changed = NULL;
    textbox->text = NULL;
    textbox->password_mask = 0;
    return (struct cloudy_widget *)textbox;
}

struct cloudy_widget *cloudy_label_create(char *text) {
    struct cloudy_textbox *label = (struct cloudy_textbox *)malloc(sizeof(struct cloudy_textbox));
    cloudy_widget_initialize(&label->widget);
    label->widget.repaint = cloudy_label_repaint;
    label->text = text;
    label->password_mask = 0;
    return (struct cloudy_widget *)label;
}

void cloudy_button_destroy(struct cloudy_widget *widget) {
    struct cloudy_button *button = (struct cloudy_button *)widget;
    if (button->icon) free(button->icon);
    free(button);
}

struct cloudy_widget *cloudy_button_create() {
    struct cloudy_button *button = (struct cloudy_button *)malloc(sizeof(struct cloudy_button));
    cloudy_widget_initialize(&button->widget);
    button->widget.repaint = cloudy_button_repaint;
    button->widget.destroy = cloudy_button_destroy;
    button->text = NULL;
    button->icon = NULL;
    button->color = button_colors[(button_order++) % num_button_colors];
    return (struct cloudy_widget *)button;
}

struct cloudy_widget *cloudy_icon_button_create(char *icon_path) {
    struct cloudy_button *button = (struct cloudy_button *)cloudy_button_create();
    struct stat st;
    st.st_size = 0;
    stat(icon_path, &st);
    button->icon_size = st.st_size;
    FILE *icon = fopen(icon_path, "rb");
    button->icon = (char *)malloc(sizeof(char) * button->icon_size);
    fread(button->icon, sizeof(char), button->icon_size, icon);
    fclose(icon);
    button->widget.rectangle.dimension.width = ntohl(*(int *)(&button->icon[16]));
    button->widget.rectangle.dimension.height = ntohl(*(int *)(&button->icon[20]));
    return (struct cloudy_widget *)button;
}
