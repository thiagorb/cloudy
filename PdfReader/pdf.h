#ifndef PDF_H
#define	PDF_H

#include <poppler.h>

struct pdf_file {
    PopplerDocument *document;
    int nPages;
};

void pdf_init();
struct pdf_file *pdf_load(char *path);
void pdf_unload(struct pdf_file *pdf);
void pdf_render_page_to_cairo(struct pdf_file *pdf, cairo_surface_t *surface, int page_num, int fitwidth, int fitheight, double factor, double dx, double dy);
char *pdf_render_page(struct pdf_file *pdf, int page_num, unsigned int *data_length, int fitwidth, int fitheight, double factor, double dx, double dy);
char *cairo_to_png(cairo_surface_t *surface, unsigned int *data_length);

#endif	/* PDF_H */
