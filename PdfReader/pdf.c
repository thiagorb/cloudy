#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <poppler.h>
#include <cairo.h>
#include "pdf.h"

#define IMAGE_DPI 30

struct page_content_chunk {
    unsigned int length;
    char *data;
    unsigned int totalLength;
    struct page_content_chunk *next;
};

static cairo_status_t write_png_to_stream (void *in_closure, const unsigned char *data, unsigned int length)
{
    struct page_content_chunk **closure = (struct page_content_chunk **) in_closure;
    struct page_content_chunk *current = (struct page_content_chunk *)malloc(sizeof(struct page_content_chunk));
    current->data = (char *)malloc(sizeof(char) * length);
    current->length = length;
    current->next = *closure;
    memcpy (current->data, data, length);
    current->totalLength = current->length;
    if (current->next) {
        current->totalLength += current->next->totalLength;
    }
    *closure = current;
    return CAIRO_STATUS_SUCCESS;
}

void pdf_init() {
    g_type_init ();
}

struct pdf_file *pdf_load(char *path) {
    struct pdf_file *pdf = (struct pdf_file *)malloc(sizeof(struct pdf_file));
    GError *error = NULL;
    gchar *absolute, *uri;
    if (g_path_is_absolute(path)) {
        absolute = g_strdup (path);
    } else {
        gchar *dir = g_get_current_dir ();
        absolute = g_build_filename (dir, path, (gchar *) 0);
        free (dir);
    }
    
    uri = g_filename_to_uri (absolute, NULL, &error);
    free (absolute);
    if (uri == NULL) {
        printf("%s\n", error->message);
        return NULL;
    }
    
    pdf->document = poppler_document_new_from_file (uri, NULL, &error);
    if (pdf->document == NULL) {
        printf("%s\n", error->message);
        return NULL;
    }
    pdf->nPages = poppler_document_get_n_pages(pdf->document);
    return pdf;
}

struct page_content_chunk *revert_page_data(struct page_content_chunk *page_data, struct page_content_chunk *previous) {
    if (!page_data) return previous;
    struct page_content_chunk *next = page_data->next;
    page_data->next = previous;
    return revert_page_data(next, page_data);
}

void pdf_render_page_to_cairo(struct pdf_file *pdf, cairo_surface_t *surface, int page_num, int fitwidth, int fitheight, double factor, double dx, double dy) {
    PopplerPage *page = poppler_document_get_page (pdf->document, page_num - 1);
    
    double width, height;
    poppler_page_get_size (page, &width, &height);
    width *= factor;
    height *= factor;
    double fitratio = fitwidth / (double)fitheight;
    double ratio = width / (double)height;
    double scale;
    if (ratio < fitratio) {
        scale = fitheight / height;
    } else {
        scale = fitwidth / width;
    }
    cairo_t *crb = cairo_create (surface);
    cairo_set_source_rgb (crb, 0, 0, 0);
    cairo_paint (crb);
    cairo_translate(crb, fitwidth / 2, fitheight / 2);
    cairo_scale(crb, factor, factor);
    cairo_translate(crb, -dx, -dy);
    cairo_scale(crb, scale, scale);
    cairo_translate(crb, -width / 2, -height / 2);
    cairo_set_source_rgb (crb, 1, 1, 1);
    cairo_rectangle (crb, 0, 0, width, height);
    cairo_fill (crb);
    
    cairo_set_operator (crb, CAIRO_OPERATOR_DEST_OVER);
    cairo_set_source_rgb (crb, 1, 1, 1);
    cairo_scale(crb, factor, factor);
    poppler_page_render(page, crb);
    g_object_unref(page);
    cairo_paint (crb);
    
    cairo_status_t status = cairo_status(crb);
    if (status)
        printf("%s\n", cairo_status_to_string (status));
    cairo_destroy (crb);
}

char *cairo_to_png(cairo_surface_t *surface, unsigned int *data_length) {    
    struct page_content_chunk *page_data = NULL;
    struct page_content_chunk *page_data_next = NULL;
    cairo_surface_write_to_png_stream (surface, write_png_to_stream, &page_data);
    *data_length = page_data->totalLength;
    page_data = revert_page_data(page_data, NULL);
    char *ret = (char *)malloc(sizeof(char) * (*data_length));
    char *p = ret;
    while (page_data) {
        memcpy(p, page_data->data, page_data->length);
        p += page_data->length;
        page_data_next = page_data->next;
        free(page_data->data);
        free(page_data);
        page_data = page_data_next;
    }
    return ret;
}

char *pdf_render_page(struct pdf_file *pdf, int page_num, unsigned int *data_length, int fitwidth, int fitheight, double factor, double dx, double dy) {
    PopplerPage *page = poppler_document_get_page (pdf->document, page_num - 1);
    
    double width, height;
    poppler_page_get_size (page, &width, &height);
    width *= factor;
    height *= factor;
    double fitratio = fitwidth / (double)fitheight;
    double ratio = width / (double)height;
    double scale;
    if (ratio < fitratio) {
        scale = fitheight / height;
    } else {
        scale = fitwidth / width;
    }

    cairo_surface_t *surface2 = cairo_image_surface_create (CAIRO_FORMAT_RGB24, fitwidth, fitheight);
    cairo_t *crb = cairo_create (surface2);
    cairo_set_source_rgb (crb, 0, 0, 0);
    cairo_paint (crb);
    cairo_translate(crb, fitwidth / 2, fitheight / 2);
    cairo_scale(crb, factor, factor);
    cairo_translate(crb, -dx, -dy);
    cairo_scale(crb, scale, scale);
    cairo_translate(crb, -width / 2, -height / 2);
    cairo_set_source_rgb (crb, 1, 1, 1);
    cairo_rectangle (crb, 0, 0, width, height);
    cairo_fill (crb);
    
    cairo_set_operator (crb, CAIRO_OPERATOR_DEST_OVER);
    cairo_set_source_rgb (crb, 1, 1, 1);
    cairo_scale(crb, factor, factor);
    poppler_page_render(page, crb);
    g_object_unref(page);
    cairo_paint (crb);
    
    cairo_status_t status = cairo_status(crb);
    if (status)
        printf("%s\n", cairo_status_to_string (status));
    cairo_destroy (crb);
    
    struct page_content_chunk *page_data = NULL;
    struct page_content_chunk *page_data_next = NULL;
    cairo_surface_write_to_png_stream (surface2, write_png_to_stream, &page_data);
    *data_length = page_data->totalLength;
    page_data = revert_page_data(page_data, NULL);
    char *ret = (char *)malloc(sizeof(char) * (*data_length));
    char *p = ret;
    while (page_data) {
        memcpy(p, page_data->data, page_data->length);
        p += page_data->length;
        page_data_next = page_data->next;
        free(page_data->data);
        free(page_data);
        page_data = page_data_next;
    }
    cairo_surface_destroy (surface2);
    return ret;
}

void pdf_unload(struct pdf_file *pdf) {
    g_object_unref(pdf->document);
}
