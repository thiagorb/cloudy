#include <stdio.h>
#include <stdlib.h>
#include <cairo.h>
#include "interface.h"
#include "pdf.h"

unsigned int data_length = 0;
char *content = NULL;
int zoom = 0;
int page;
double dx = 0;
double dy = 0;
struct pdf_file *pdf = NULL;

struct cloudy_widget *interface = NULL;
struct cloudy_widget *btOpen = NULL;
struct cloudy_widget *btClose = NULL;
struct cloudy_widget *btMinimize = NULL;
struct cloudy_widget *lbPage = NULL;
struct cloudy_widget *txPage = NULL;
struct cloudy_widget *lbTotalPages = NULL;
struct cloudy_widget *areaPdf = NULL;

char *files[] = {"./eaic.pdf", "./simplex.pdf", "./proposta.pdf", "./monografia.pdf"};
#define NFILES (sizeof(files) / sizeof(*files))
struct cloudy_widget *btFile[NFILES];

cairo_surface_t *surface = NULL;

char lbTotalPagesText[20];


#define DRAW_BLOCK_SIZE 32
void draw_surface() {
    int i;
    cairo_surface_t *block = cairo_image_surface_create (CAIRO_FORMAT_RGB16_565, DRAW_BLOCK_SIZE, DRAW_BLOCK_SIZE);
    cairo_t *cr = cairo_create (block);
    int blocksH = (areaPdf->rectangle.dimension.width + (DRAW_BLOCK_SIZE - 1)) / DRAW_BLOCK_SIZE;
    int blocksV = (areaPdf->rectangle.dimension.height + (DRAW_BLOCK_SIZE - 1)) / DRAW_BLOCK_SIZE;
    for (i = 0; i < blocksV; i++) {
        int j;
        for (j = 0; j < blocksH; j++) {
            cairo_set_source_surface(cr, surface, -j * DRAW_BLOCK_SIZE, -i * DRAW_BLOCK_SIZE);
            cairo_paint(cr);
            int block_length;
            char *block_data = cairo_to_png(block, &block_length);
            cloudy_draw_image(block_data, block_length, j * DRAW_BLOCK_SIZE, i * DRAW_BLOCK_SIZE);
            free(block_data);
        }
    }
    cairo_destroy (cr);
    cairo_surface_destroy (block);
}

void areaPdf_repaint(struct cloudy_widget *w) {
    if (content) free(content);
    //pdf_render_page_to_cairo(pdf, surface, page, w->rectangle.dimension.width, w->rectangle.dimension.height, zoom? 3.0: 1, zoom? dx: 0, zoom? dy: 0);
    content = pdf_render_page(pdf, page, &data_length, w->rectangle.dimension.width, w->rectangle.dimension.height, zoom? 3.0: 1, zoom? dx: 0, zoom? dy: 0);
    cloudy_change_color(255, 0, 0, 0);
    cloudy_fill_rect(0, 0, w->rectangle.dimension.width, w->rectangle.dimension.height);
    cloudy_draw_image(content, data_length, 0, 0);
    //draw_surface();
}

void areaPdf_click(struct cloudy_widget *w, struct position *p) {
    zoom = !zoom;
    dx = p->x - w->rectangle.dimension.width / 2;
    dy = p->y - w->rectangle.dimension.height / 2;
    cloudy_widget_repaint(w);
}

void interface_resize(struct cloudy_widget *interface) {
    int width = interface->rectangle.dimension.width;
    int height = interface->rectangle.dimension.height;

    cloudy_widget_resize(btClose, width - 20, 0, 20, 20);
    cloudy_widget_resize(btMinimize, width - 40, 0, 20, 20);
    if (areaPdf) {
        cloudy_widget_resize(areaPdf, 0, 20, width, height - 20);
        //cloudy_widget_resize(areaPdf, (width - 128) / 2, 15, 64, 64);
        surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, areaPdf->rectangle.dimension.width, areaPdf->rectangle.dimension.height);
    }
    
    int i;
    for (i = 0; i < NFILES; i++) {
        cloudy_widget_resize(btFile[i], 5, 25 + 25 * i, width - 10, 20);
    }
}

void btClose_click(struct cloudy_widget *w, struct position *p) {
    cloudy_app_terminate();
}

void btMinimize_click(struct cloudy_widget *w, struct position *p) {
    cloudy_app_minimize();
}

void txPage_text_changed(struct cloudy_textbox *txPage) {
    if (!pdf) return;
    page = atoi(txPage->text);
    if (page < 1 || page > pdf->nPages) cloudy_textbox_set_text((struct cloudy_widget *)txPage, "1");
    if (areaPdf) cloudy_widget_repaint(areaPdf);
}

#define STATE_FILE_CHOICE 0
#define STATE_PDF_VIEW 1
void alternate_state(int state) {
    btOpen->visible = state == STATE_PDF_VIEW;
    lbPage->visible = state == STATE_PDF_VIEW;
    txPage->visible = state == STATE_PDF_VIEW;
    lbTotalPages->visible = state == STATE_PDF_VIEW;
    areaPdf->visible = state == STATE_PDF_VIEW;
    
    int i;
    for (i = 0; i < NFILES; i++) {
        btFile[i]->visible = (state == STATE_FILE_CHOICE);
    }
    cloudy_widget_repaint(interface);
}

void btOpen_click(struct cloudy_widget *w, struct position *p) {
    alternate_state(STATE_FILE_CHOICE);
}

void btFile_click(struct cloudy_widget *w, struct position *p) {
    if (pdf) {
        pdf_unload(pdf);
        pdf = NULL;
    }
    if (content) {
        free(content);
        content = NULL;
    }
    char *file_path = ((struct cloudy_button *)w)->text;
    pdf = pdf_load(file_path);
    if (!pdf) {
        printf("Cannot load PDF\n");
        alternate_state(STATE_FILE_CHOICE);
        return;
    }
    printf("loading pdf %s\n", file_path);
    sprintf(lbTotalPagesText, "of %i", pdf->nPages);
    cloudy_textbox_set_text(txPage, "1");
    alternate_state(STATE_PDF_VIEW);
}

struct cloudy_widget *file_button(char *file_path) {
    struct cloudy_widget *bt = cloudy_button_create();
    ((struct cloudy_button *)bt)->text = file_path;
    bt->click = btFile_click;
    cloudy_widget_add(interface, bt);
    return bt;
}

int main() {
    pdf_init();
    
    cloudy_app_initialize();
    interface = cloudy_get_app_interface();
    interface->resize = interface_resize;
    
    btOpen = cloudy_button_create();
    ((struct cloudy_button *)btOpen)->text = "open";
    cloudy_widget_resize(btOpen, 0, 0, 40, 20);
    btOpen->click = btOpen_click;
    cloudy_widget_add(interface, btOpen);
    
    lbPage = cloudy_label_create("Page");
    cloudy_widget_resize(lbPage, 50, 0, 25, 20);
    cloudy_widget_add(interface, lbPage);
    
    txPage = cloudy_textbox_create();
    ((struct cloudy_textbox *)txPage)->text_changed = txPage_text_changed;
    cloudy_widget_resize(txPage, 86, 0, 30, 20);
    cloudy_widget_add(interface, txPage);
    
    lbTotalPages = cloudy_label_create(lbTotalPagesText);
    cloudy_widget_resize(lbTotalPages, 120, 0, 40, 20);
    cloudy_widget_add(interface, lbTotalPages);
    
    btClose = cloudy_button_create();
    ((struct cloudy_button *)btClose)->text = "x";
    btClose->click = btClose_click;
    cloudy_widget_add(interface, btClose);
    
    btMinimize = cloudy_button_create();
    ((struct cloudy_button *)btMinimize)->text = "_";
    btMinimize->click = btMinimize_click;
    cloudy_widget_add(interface, btMinimize);
    
    areaPdf = cloudy_widget_create();
    areaPdf->repaint = areaPdf_repaint;
    areaPdf->click = areaPdf_click;
    cloudy_widget_add(interface, areaPdf);
    
    int i;
    for (i = 0; i < NFILES; i++) {
        btFile[i] = file_button(files[i]);
    }
    
    alternate_state(STATE_FILE_CHOICE);
    cloudy_app_start();
    return 0;
}
