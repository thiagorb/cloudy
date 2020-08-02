#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interface.h"
#include "stdbool.h"
#include "calc.h"

enum buttons {
    BUTTON_0, 
    BUTTON_1, BUTTON_2, BUTTON_3, 
    BUTTON_4, BUTTON_5, BUTTON_6, 
    BUTTON_7, BUTTON_8, BUTTON_9,
    BUTTON_ADD, BUTTON_SUB, BUTTON_DIV, 
    BUTTON_MUL, BUTTON_DOT, BUTTON_C, 
    BUTTON_POW, BUTTON_EQ,
    TOTAL_BUTTONS
};

struct cloudy_widget *interface;
struct cloudy_widget *btClose = NULL;
struct cloudy_widget *btMinimize = NULL;
struct cloudy_widget *lbDisplay = NULL;
struct cloudy_widget *buttons[TOTAL_BUTTONS];
char *buttons_text[] = {
    "0", 
    "1", "2", "3", 
    "4", "5", "6", 
    "7", "8", "9", 
    "+", "-", "/", 
    "*", ".", "C", 
    "^", "="
};

void button_click(struct cloudy_widget *button, struct position *p) {
    char text[strlen(getDisplayText()) + 1];
    strcpy(text, getDisplayText());
    inputChar(((struct cloudy_button *)button)->text[0]);
    if (strcmp(text, getDisplayText())) {
        cloudy_widget_repaint(lbDisplay);
    }
}

void btClose_click(struct cloudy_widget *w, struct position *p) {
    cloudy_app_terminate();
}

void btMinimize_click(struct cloudy_widget *w, struct position *p) {
    cloudy_app_minimize();
}

void interface_resize(struct cloudy_widget *interface) {
    int width = interface->rectangle.dimension.width;
    int height = interface->rectangle.dimension.height;

    cloudy_widget_resize(btClose, width - 20, 0, 20, 20);
    cloudy_widget_resize(btMinimize, width - 40, 0, 20, 20);
    
    int row_height = (height - 20) / 5;
    int col_width = width / 5;
    /*   0 1 2 3 4
     * 0 D D D D D
     * 1       c *
     * 2       ^ /
     * 3       + -
     * 4   .   + =
     *
     */
    cloudy_widget_resize(lbDisplay, 0, 20, width, row_height);
    cloudy_widget_resize(buttons[BUTTON_ADD], 3 * col_width, 20 + 3 * row_height, col_width - 1, row_height * 2 - 1);
    cloudy_widget_resize(buttons[BUTTON_SUB], 4 * col_width, 20 + 3 * row_height, col_width - 1, row_height - 1);
    cloudy_widget_resize(buttons[BUTTON_DIV], 4 * col_width, 20 + 2 * row_height, col_width - 1, row_height - 1);
    cloudy_widget_resize(buttons[BUTTON_MUL], 4 * col_width, 20 + 1 * row_height, col_width - 1, row_height - 1);
    cloudy_widget_resize(buttons[BUTTON_DOT], 1 * col_width, 20 + 4 * row_height, col_width - 1, row_height - 1);
    cloudy_widget_resize(buttons[BUTTON_C]  , 3 * col_width, 20 + 1 * row_height, col_width - 1, row_height - 1);
    cloudy_widget_resize(buttons[BUTTON_POW], 3 * col_width, 20 + 2 * row_height, col_width - 1, row_height - 1);
    cloudy_widget_resize(buttons[BUTTON_EQ],  4 * col_width, 20 + 4 * row_height, col_width - 1, row_height - 1);
    
    cloudy_widget_resize(buttons[BUTTON_0], 0, 20 + 4 * row_height, col_width - 1, row_height - 1);
    int i;
    for (i = 0; i < 9; i++) {
        cloudy_widget_resize(buttons[i + 1], (i % 3) * col_width, 20 + (3 - i / 3) * row_height, col_width - 1, row_height - 1);
    }
}

int main() {
    printf("Início do calc app\n");
    cloudy_app_initialize();
    
    interface = cloudy_get_app_interface();
    interface->resize = interface_resize;
    
    btClose = cloudy_button_create();
    ((struct cloudy_button *)btClose)->text = "x";
    btClose->click = btClose_click;
    cloudy_widget_add(interface, btClose);
    
    btMinimize = cloudy_button_create();
    ((struct cloudy_button *)btMinimize)->text = "_";
    btMinimize->click = btMinimize_click;
    cloudy_widget_add(interface, btMinimize);
    
    lbDisplay = cloudy_label_create(getDisplayText());
    cloudy_widget_add(interface, lbDisplay);
    
    int i;
    for (i = 0; i < TOTAL_BUTTONS; i++) {
        buttons[i] = cloudy_button_create();
        ((struct cloudy_button *)buttons[i])->text = buttons_text[i];
        buttons[i]->click = button_click;
        cloudy_widget_add(interface, buttons[i]);
    }
    
    cloudy_app_start();
    return 0;
}
