/*
* pdfsg.c
* simple graphics library for PDF
* Luiz Henrique de Figueiredo <lhf@tecgraf.puc-rio.br>
* 13 Apr 2025 11:35:14
* This code is hereby placed in the public domain and also under the MIT license
* Based on public domain code by Andre Renaud: github.com/AndreRenaud/PDFGen
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "pdfsg.h"

#define MYNAME		"pdfsg"
#define append(N,L,H,T)	do { if (!H) H=T=N; else T=T->L=N; } while (0)
#define new(t)		(t *) alloc(sizeof(t))
#define strcopy(d,s)	strncpy(d,s,sizeof(d)-1)[sizeof(d)-1]='\0'

static void *alloc(size_t s)
{
    void *p = calloc(1,s);
    if (p != NULL) return p;
    fprintf(stderr,"%s: not enough memory\n",MYNAME);
    exit(1);
    return p;
}

enum {
    OBJ_catalog,
    OBJ_font,
    OBJ_info,
    OBJ_length,
    OBJ_page,
    OBJ_pages,
    OBJ_stream,
    OBJ_N
};

struct pdf_obj {
    int type;
    int index;
    long offset;
    struct pdf_obj *next;    /* next of this type */
    struct pdf_obj *succ;    /* next in document */
    struct pdf_obj *link;    /* next in page */
    union {
        struct {
            char name[216];
            int index;
        } font;
        char info[256];
        size_t length;
        struct {
            struct pdf_obj *head;
            struct pdf_obj *tail;
            float width;
            float height;
            char label[216];
        } page;
    };
};

struct pdf_doc {
    int index;
    struct pdf_obj *head;
    struct pdf_obj *tail;
    struct pdf_obj *first[OBJ_N];
    struct pdf_obj *last[OBJ_N];
    long offset;     /* of current stream */
    float x0,y0;
    int font;
    int evenodd;
    FILE *fp;
};

static struct pdf_obj *pdf_add_object(struct pdf_doc *pdf, int type)
{
    struct pdf_obj *obj = new(struct pdf_obj);
    obj->type = type;
    obj->index = ++pdf->index;
    append(obj,succ,pdf->head,pdf->tail);
    append(obj,next,pdf->first[obj->type],pdf->last[obj->type]);
    return obj;
}

static struct pdf_obj *find_object(struct pdf_doc *pdf, int index, int type)
{
    struct pdf_obj *o;
    for (o = pdf->last[type]; o; o = o->next)
        if (o->index == index) return o;
    for (o = pdf->first[type]; o; o = o->next)
        if (o->index == index) return o;
    return NULL;
}

#define EOL	"\r\n"
#define TAB1	" "
#define TAB2	TAB1 TAB1
#define TAB3	TAB1 TAB1 TAB1

static void pdf_save_object(struct pdf_doc *pdf, struct pdf_obj *obj)
{
    FILE *fp = pdf->fp;

    if (obj->offset != 0) return;

    obj->offset = ftell(fp);
    fprintf(fp, "%d 0 obj" EOL, obj->index);
    switch (obj->type) {
    struct pdf_obj *o;
    int i;

    case OBJ_catalog:
        fprintf(fp, "<<" EOL);
        fprintf(fp, TAB1 "/Type /Catalog" EOL);
        fprintf(fp, TAB1 "/Pages %d 0 R" EOL, pdf->first[OBJ_pages]->index);
        fprintf(fp, TAB1 "/PageLabels << /Nums [" EOL);
        i = 0;
        for (o = pdf->first[OBJ_page]; o; o = o->next)
            fprintf(fp, TAB2 "%d << /P (%s) >>" EOL, i++, o->page.label);
        fprintf(fp, TAB1 "] >>" EOL);
        fprintf(fp, ">>" EOL);
        break;

    case OBJ_font:
        fprintf(fp, "<<" EOL);
        fprintf(fp, TAB1 "/Type /Font" EOL);
        fprintf(fp, TAB1 "/Subtype /Type1" EOL);
        fprintf(fp, TAB1 "/BaseFont /%s" EOL, obj->font.name);
     /* fprintf(fp, TAB1 "/Encoding /WinAnsiEncoding" EOL); */
        fprintf(fp, ">>" EOL);
        break;

    case OBJ_info: {
        time_t now = time(NULL);
        char b[32];
        strftime(b, sizeof(b), "%Y%m%d%H%M%S%z", localtime(&now));
        fprintf(fp, "<<" EOL);
        if (obj->info[0])
            fprintf(fp, TAB1 "%s" EOL, obj->info);
        else
            fprintf(fp, TAB1 "/Producer (%s)" EOL, MYNAME);
        fprintf(fp, TAB1 "/CreationDate (D:%.17s'%s')" EOL, b, b+17);
        fprintf(fp, ">>" EOL);
        break;
    }

    case OBJ_length:
        fprintf(fp, "%lu" EOL, (unsigned long) obj->length);
        break;

    case OBJ_page:
        fprintf(fp, "<<" EOL);
        fprintf(fp, TAB1 "/Type /Page" EOL);
        fprintf(fp, TAB1 "/Parent %d 0 R" EOL, pdf->first[OBJ_pages]->index);
        fprintf(fp, TAB1 "/MediaBox [ 0 0 %f %f ]" EOL,
                obj->page.width, obj->page.height);
        fprintf(fp, TAB1 "/Contents [" EOL);
        for (o = obj->page.head; o; o = o->link)
            fprintf(fp, TAB2 "%d 0 R" EOL, o->index);
        fprintf(fp, TAB1 "]" EOL);
        fprintf(fp, ">>" EOL);
        break;

    case OBJ_pages:
        fprintf(fp, "<<" EOL);
        fprintf(fp, TAB1 "/Type /Pages" EOL);
        fprintf(fp, TAB1 "/Kids [" EOL);
        i = 0;
        for (o = pdf->first[OBJ_page]; o; o = o->next) {
            fprintf(fp, TAB2 "%d 0 R" EOL, o->index); i++;
        }
        fprintf(fp, TAB1 "]" EOL);
        fprintf(fp, TAB1 "/Count %d" EOL, i);
        fprintf(fp, TAB1 "/Resources <<" EOL);
        fprintf(fp, TAB2 "/ProcSet [ /PDF /Text ]" EOL);
        fprintf(fp, TAB2 "/Font <<" EOL);
        for (o = pdf->first[OBJ_font]; o; o = o->next)
            fprintf(fp, TAB3 "/F%d %d 0 R" EOL, o->font.index, o->index);
        fprintf(fp, TAB2 ">>" EOL);
        fprintf(fp, TAB2 "/ExtGState <<" EOL);
        for (i = 0; i <= 10; i++) {
            float v = (10-i)*0.1;
            fprintf(fp, TAB3 "/GS%-2d << /ca %.1f /CA %.1f >>" EOL, i, v, v);
        }
        fprintf(fp, TAB2 ">>" EOL);
        fprintf(fp, TAB1 ">>" EOL);
        fprintf(fp, ">>" EOL);
        break;

    case OBJ_stream:
        break;

    }
    fprintf(fp, "endobj" EOL);
}

static struct pdf_doc *pdf_create(void)
{
    struct pdf_doc *pdf = new(struct pdf_doc);
    pdf_add_object(pdf, OBJ_catalog);
    pdf_add_object(pdf, OBJ_pages);
    pdf_add_object(pdf, OBJ_info);
    return pdf;
}

static void pdf_destroy(struct pdf_doc *pdf)
{
    struct pdf_obj *obj = pdf->head;
    struct pdf_obj *o;
    while (obj != NULL) {
        o = obj->succ; free(obj); obj = o;
    }
    free(pdf);
}

/* PDF external ============================================================ */

struct pdf_doc *pdf_newdoc(const char *filename)
{
    struct pdf_doc *pdf;
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) return NULL;
    fprintf(fp, "%%PDF-1.4" EOL);
    fprintf(fp, "%%%c%c%c" EOL, 0xab, 0xcd, 0xef);
    pdf = pdf_create();
    pdf->fp = fp;
    return pdf;
}

int pdf_newpage(struct pdf_doc *pdf, float width, float height, const char *label)
{
    struct pdf_obj *obj = pdf_add_object(pdf, OBJ_page);
    struct pdf_obj *o;
    obj->page.width = width;
    obj->page.height = height;
    strcopy(obj->page.label,label);
    pdf_newgroup(pdf);
    o = pdf->last[OBJ_stream];
    append(o,link,obj->page.head,obj->page.tail);
    return obj->index;
}

void pdf_setinfo(struct pdf_doc *pdf, const char *info)
{
    struct pdf_obj *obj = pdf->first[OBJ_info];
    strcopy(obj->info,info);
}

int pdf_setfont(struct pdf_doc *pdf, const char *font)
{
    struct pdf_obj *obj;
    int index = 0;
    for (obj = pdf->first[OBJ_font]; obj; obj = obj->next) {
        if (strcmp(obj->font.name, font) == 0)
            break;
        index = obj->font.index;
    }
    if (!obj) {
        obj = pdf_add_object(pdf, OBJ_font);
        strcopy(obj->font.name,font);
        obj->font.index = index + 1;
    }
    pdf->font = obj->font.index;
    return pdf->font;
}

static unsigned long hash(unsigned long hash, const void *data, size_t len)
{
    const unsigned char *d = (const unsigned char *) data;
    for (; len; len--) {
        hash = ((hash << 5) + hash) + *d++;
    }
    return hash;
}

int pdf_enddoc(struct pdf_doc *pdf)
{
    struct pdf_obj *obj;
    long offset;
    FILE *fp = pdf->fp;
    pdf_endgroup(pdf);
    for (obj = pdf->head; obj; obj = obj->succ)
        pdf_save_object(pdf, obj);
    offset = ftell(fp);
    fprintf(fp, "xref" EOL);
    fprintf(fp, "0 %d" EOL, pdf->index + 1);
    fprintf(fp, "0000000000 65535 f" EOL);
    for (obj = pdf->head; obj; obj = obj->succ)
        fprintf(fp, "%010ld 00000 n" EOL, obj->offset);
    fprintf(fp, "trailer" EOL);
    fprintf(fp, "<<" EOL);
    fprintf(fp, TAB1 "/Size %d" EOL, pdf->index + 1);
    fprintf(fp, TAB1 "/Root %d 0 R" EOL, pdf->first[OBJ_catalog]->index);
    fprintf(fp, TAB1 "/Info %d 0 R" EOL, pdf->first[OBJ_info]->index);
    {
    unsigned long id1, id2;
    time_t now = time(NULL);
    obj = pdf->first[OBJ_info];
    id1 = pdf->index;
    id1 = hash(id1, obj->info, sizeof(obj->info));
    id1 = hash(id1, &now, sizeof(now));
    id2 = hash(id1, pdf, sizeof(*pdf));
    id2 = hash(id2, &now, sizeof(now));
    fprintf(fp, TAB1 "/ID [<%16.16lX> <%16.16lX>]" EOL, id1, id2);
    }
    fprintf(fp, ">>" EOL);
    fprintf(fp, "startxref" EOL);
    fprintf(fp, "%ld" EOL, offset);
    fprintf(fp, "%%%%EOF" EOL);
    {
    int rc = 0;
    rc |= ferror(fp);
    rc |= fclose(fp);
    pdf_destroy(pdf);
    return rc;
    }
}

/* PDF external graphics =================================================== */

#define x0	pdf->x0
#define y0	pdf->y0

static int pdf_newstream(struct pdf_doc *pdf)
{
    struct pdf_obj *obj = pdf_add_object(pdf, OBJ_stream);
    struct pdf_obj *len = pdf_add_object(pdf, OBJ_length);
    obj->offset = ftell(pdf->fp);
    fprintf(pdf->fp, "%d 0 obj" EOL, obj->index);
    fprintf(pdf->fp, "<< /Length %d 0 R >>" EOL, len->index);
    fprintf(pdf->fp, "stream" EOL);
    pdf->offset = ftell(pdf->fp);
    x0 = y0 = 0.0;
    return obj->index;
}

static void pdf_endstream(struct pdf_doc *pdf)
{
    struct pdf_obj *obj = pdf->last[OBJ_length];
    obj->length = ftell(pdf->fp)-(pdf->offset);
    pdf->offset = 0;
    fprintf(pdf->fp, "endstream" EOL);
    fprintf(pdf->fp, "endobj" EOL);
}

void pdf_endgroup(struct pdf_doc *pdf)
{
    if (pdf->offset != 0) pdf_endstream(pdf);
}

int pdf_newgroup(struct pdf_doc *pdf)
{
    pdf_endgroup(pdf);
    return pdf_newstream(pdf);
}

void pdf_addgroup(struct pdf_doc *pdf, int p, int g)
{
    struct pdf_obj *page = find_object(pdf,p,OBJ_page);
    struct pdf_obj *obj  = find_object(pdf,g,OBJ_stream);
    if (page && obj)
        append(obj,link,page->page.head,page->page.tail);
}

void pdf_concat(struct pdf_doc *pdf, float a, float b, float c, float d, float x, float y)
{
    fprintf(pdf->fp, "%f %f %f %f %f %f cm" EOL, a,b,c,d,x,y);
}

void pdf_moveto(struct pdf_doc *pdf, float x1, float y1)
{
    fprintf(pdf->fp, "%f %f m" EOL, x1,y1);
    x0 = x1; y0 = y1;
}

void pdf_lineto(struct pdf_doc *pdf, float x1, float y1)
{
    fprintf(pdf->fp, "%f %f l" EOL, x1,y1);
    x0 = x1; y0 = y1;
}

void pdf_curveto(struct pdf_doc *pdf, float x1, float y1, float x2, float y2, float x3, float y3)
{
    fprintf(pdf->fp, "%f %f %f %f %f %f c" EOL, x1,y1,x2,y2,x3,y3);
    x0 = x3; y0 = y3;
}

void pdf_conicto(struct pdf_doc *pdf, float x1, float y1, float x2, float y2)
{
    float xc1 = (x0+2.0*x1)/3.0;
    float yc1 = (y0+2.0*y1)/3.0;
    float xc2 = (x2+2.0*x1)/3.0;
    float yc2 = (y2+2.0*y1)/3.0;
    pdf_curveto(pdf,xc1,yc1,xc2,yc2,x2,y2);
    x0 = x2; y0 = y2;
}

void pdf_rectangle(struct pdf_doc *pdf, float x, float y, float width, float height)
{
    fprintf(pdf->fp, "%f %f %f %f re" EOL, x, y, width, height);
}

void pdf_closepath(struct pdf_doc *pdf)
{
    fprintf(pdf->fp, "h" EOL);
}

void pdf_setevenodd(struct pdf_doc *pdf, int v)
{
    pdf->evenodd = v;
}

void pdf_fill(struct pdf_doc *pdf)
{
    fprintf(pdf->fp, pdf->evenodd ? "f*" EOL : "f" EOL);
}

void pdf_stroke(struct pdf_doc *pdf)
{
    fprintf(pdf->fp, "S" EOL);
}

void pdf_fillstroke(struct pdf_doc *pdf)
{
    fprintf(pdf->fp, pdf->evenodd ? "B*" EOL : "B" EOL);
}

void pdf_setopacity(struct pdf_doc *pdf, float v)
{
    fprintf(pdf->fp, "/GS%.0f gs" EOL, 10*v);
}

void pdf_setfill(struct pdf_doc *pdf, float r, float g, float b)
{
    fprintf(pdf->fp, "%f %f %f rg" EOL, r,g,b);
}

void pdf_setstroke(struct pdf_doc *pdf, float r, float g, float b)
{
    fprintf(pdf->fp, "%f %f %f RG" EOL, r,g,b);
}

void pdf_setgray(struct pdf_doc *pdf, float f, float s)
{
    fprintf(pdf->fp, "%f g %f G" EOL, f,s);
}

void pdf_setlinewidth(struct pdf_doc *pdf, float w)
{
    fprintf(pdf->fp, "%f w" EOL, w);
}

void pdf_setlinejoin(struct pdf_doc *pdf, int v)
{
    fprintf(pdf->fp, "%d j" EOL, v);
}

void pdf_setlinecap(struct pdf_doc *pdf, int v)
{
    fprintf(pdf->fp, "%d J" EOL, v);
}

void pdf_setmiterlimit(struct pdf_doc *pdf, int v)
{
    fprintf(pdf->fp, "%d M" EOL, v);
}

void pdf_setdash(struct pdf_doc *pdf, float a[], int n, int d)
{
    int i;
    fprintf(pdf->fp, "[ ");
    for (i = 0; i < n; i++)
        fprintf(pdf->fp, "%f ", a[i]);
    fprintf(pdf->fp, "] %d d" EOL, d);
}

void pdf_clip(struct pdf_doc *pdf)
{
    fprintf(pdf->fp, pdf->evenodd ? "W* n" EOL : "W n" EOL);
}

void pdf_save(struct pdf_doc *pdf)
{
    fprintf(pdf->fp, "q" EOL);
}

void pdf_restore(struct pdf_doc *pdf)
{
    fprintf(pdf->fp, "Q" EOL);
}

/* PDF external graphics (convenience) ===================================== */

#define DEGREES 0.017453292519943

void pdf_setcolor(struct pdf_doc *pdf, float r, float g, float b, float a)
{
    pdf_setfill(pdf,r,g,b);
    pdf_setstroke(pdf,r,g,b);
    if (a>0) pdf_setopacity(pdf,a);
}

void pdf_scale(struct pdf_doc *pdf, float x, float y)
{
    pdf_concat(pdf,x,0,0,y,0,0);
}

void pdf_translate(struct pdf_doc *pdf, float x, float y)
{
    pdf_concat(pdf,1,0,0,1,x,y);
}

void pdf_rotate(struct pdf_doc *pdf, float angle)
{
    angle *= DEGREES;
    pdf_concat(pdf,
                cosf(angle), sinf(angle),
               -sinf(angle), cosf(angle), 0, 0);
}

void pdf_line(struct pdf_doc *pdf, float x1, float y1, float x2, float y2)
{
    fprintf(pdf->fp, "%f %f m %f %f l S" EOL, x1, y1, x2, y2);
}

void pdf_triangle(struct pdf_doc *pdf, float x1, float y1, float x2, float y2, float x3, float y3)
{
    fprintf(pdf->fp, "%f %f m %f %f l %f %f l h" EOL, x1, y1, x2, y2, x3, y3);
}

void pdf_quad(struct pdf_doc *pdf, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
{
    fprintf(pdf->fp, "%f %f m %f %f l %f %f l %f %f l h" EOL, x1, y1, x2, y2, x3, y3, x4, y4);
}

void pdf_polyline(struct pdf_doc *pdf, float x[], float y[], int n)
{
    int i;
    pdf_moveto(pdf,x[0],y[0]);
    for (i = 1; i < n; i++)
        pdf_lineto(pdf,x[i],y[i]);
}

void pdf_polygon(struct pdf_doc *pdf, float x[], float y[], int n)
{
    pdf_polyline(pdf,x,y,n);
    pdf_closepath(pdf);
}

/*
https://spencermortensen.com/articles/bezier-circle/
https://stackoverflow.com/questions/1734745/how-to-create-circle-with-bézier-curves
https://stackoverflow.com/questions/734076/how-to-best-approximate-a-geometrical-arc-with-a-bezier-curve
*/

void pdf_ellipse(struct pdf_doc *pdf, float x, float y, float xr, float yr)
{
    float s  = 0.55228474983079; /* 0.55191502449351 is better */
    float lx = s * xr;
    float ly = s * yr;
    fprintf(pdf->fp, "%f %f m ", x+xr, y);
    fprintf(pdf->fp, "%f %f %f %f %f %f c ", x+xr, y-ly, x+lx, y-yr, x, y-yr);
    fprintf(pdf->fp, "%f %f %f %f %f %f c ", x-lx, y-yr, x-xr, y-ly, x-xr, y);
    fprintf(pdf->fp, "%f %f %f %f %f %f c ", x-xr, y+ly, x-lx, y+yr, x, y+yr);
    fprintf(pdf->fp, "%f %f %f %f %f %f c" EOL, x+lx, y+yr, x+xr, y+ly, x+xr, y);
}

void pdf_circle(struct pdf_doc *pdf, float x, float y, float r)
{
    pdf_ellipse(pdf,x,y,r,r);
}

void pdf_text(struct pdf_doc *pdf, const char *text, int font, float size, float x, float y, float angle, int mode)
{
    fprintf(pdf->fp, "BT" EOL);
    fprintf(pdf->fp, TAB1 "%d Tr" EOL, mode);
    if (font==0) font = pdf->font;
    if (angle != 0) {
        angle *= DEGREES;
        fprintf(pdf->fp, TAB1 "%f %f %f %f %f %f Tm" EOL,
                 cosf(angle), sinf(angle),
                -sinf(angle), cosf(angle), x, y);
    } else
        fprintf(pdf->fp, TAB1 "%f %f Td" EOL, x, y);
    fprintf(pdf->fp, TAB1 "/F%d %f Tf" EOL, font, size);
    fprintf(pdf->fp, TAB1 "(%s) Tj" EOL, text);
    fprintf(pdf->fp, "ET" EOL);
}

void pdf_addraw(struct pdf_doc *pdf, const char *code)
{
    fprintf(pdf->fp, "%s", code);
}

#undef MYNAME
#undef append
#undef new
#undef strcopy
#undef EOL
#undef TAB1
#undef TAB2
#undef TAB3
#undef x0
#undef y0

