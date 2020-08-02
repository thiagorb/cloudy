#ifndef CLOUDY_WIDGETS_H
#define	CLOUDY_WIDGETS_H

#ifdef	__cplusplus
extern "C" {
#endif    
    
struct position {
    int x;
    int y;
};
    
struct dimension {
    int width;
    int height;
};

struct rectangle {
    struct position position;
    struct dimension dimension;
};
    
union color {
    int rgb;
    unsigned char i[4];
};

struct variant {
	int id;
    int size;
    void* content;
};

struct cloudy_graphics {
    struct rectangle rectangle;
    union color color;
};

struct cloudy_widget {
    struct rectangle rectangle;
    void (*repaint)(struct cloudy_widget *widget);
    void (*resize)(struct cloudy_widget *widget);
    void (*click)(struct cloudy_widget *widget, struct position *p);
    void (*destroy)(struct cloudy_widget *widget);
    int visible;
    struct cloudy_widget *next;
    struct cloudy_widget *parent;
    struct cloudy_widget *children;
};

struct cloudy_textbox {
    struct cloudy_widget widget;
    void (*text_changed)(struct cloudy_textbox *textbox);
    char* text;
    int password_mask;
};

struct cloudy_button {
    struct cloudy_widget widget;
    char* text;
    char* icon;
    int icon_size;
    union color color;
};

#define rectangle_data(a) (a).position.x, (a).position.y, (a).dimension.width, (a).dimension.height
#define color_data(a) (a).i[3], (a).i[2], (a).i[1], (a).i[0]

struct cloudy_widget *cloudy_widget_create();
void cloudy_widget_destroy(struct cloudy_widget *widget);
void cloudy_interface_repaint(struct cloudy_widget *interface);
struct cloudy_widget *cloudy_textbox_create();
struct cloudy_widget *cloudy_label_create(char *text);
struct cloudy_widget *cloudy_button_create();
struct cloudy_widget *cloudy_icon_button_create(char *icon_path);
void cloudy_change_color(unsigned int a, unsigned int r, unsigned int g, unsigned int b);
void cloudy_widget_add(struct cloudy_widget *container, struct cloudy_widget *child);
void cloudy_widget_remove(struct cloudy_widget *container, struct cloudy_widget *child);
void cloudy_widget_resize(struct cloudy_widget *widget, int x, int y, int width, int height);
void cloudy_widget_move(struct cloudy_widget *widget, int x, int y);
void set_rect(struct rectangle *rectangle, int x, int y, int width, int height);
void cloudy_textbox_set_text(struct cloudy_widget *textbox, char *text);
int rectangle_contains(struct rectangle *r, struct position *p);

#ifdef	__cplusplus
}
#endif

#endif	/* CLOUDY_WIDGETS_H */

