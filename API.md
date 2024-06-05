# pdfsg API

Only one group is active at any given time.

All pages remain active to receive new groups.

## Document management
```
struct pdf_doc *pdf_newdoc(const char *filename);
```
Creates a new document in the given file.
Returns NULL if it fails to open it.
Returns a pointer that should be used in all other functions.


```
int pdf_newpage(struct pdf_doc *pdf, float width, float height, const char *label);
```
Creates a new page with the given dimensions and label.
Creates an anonymous group for graphics contents.
Closes any previous group that is open.
Returns an opaque ID that can be used in `pdf_addgroup`.

```
void pdf_setinfo(struct pdf_doc *pdf, const char *info);
```
Set document info. Use PDF syntax.

```
int pdf_enddoc(struct pdf_doc *pdf);
```
Finishes and closes the PDF file.
Returns 0 if success, non-zero if failure to write the file.

## Groups

```
int pdf_newgroup(struct pdf_doc *pdf);
```
Creates a new group for graphics contents and make it current.
Closes any previous group that is open.
Returns an opaque ID that can be used in `pdf_addgroup`.

```
void pdf_endgroup(struct pdf_doc *pdf);
```
Closes any previous group that is open.

```
void pdf_addgroup(struct pdf_doc *pdf, int p, int g);
```
Add the group `g` to the page `p`. Does nothing if there is no such group or page.

## Graphics core primitives
These primitives are the standard ones and need no explanation.

```
void pdf_concat(struct pdf_doc *pdf, float a, float b, float c, float d, float x, float y);
```

```
void pdf_moveto(struct pdf_doc *pdf, float x1, float y1);
```

```
void pdf_lineto(struct pdf_doc *pdf, float x1, float y1);
```

```
void pdf_curveto(struct pdf_doc *pdf, float x1, float y1, float x2, float y2, float x3, float y3);
```

```
void pdf_conicto(struct pdf_doc *pdf, float x1, float y1, float x2, float y2);
```

```
void pdf_rectangle(struct pdf_doc *pdf, float x, float y, float width, float height);
```

```
void pdf_closepath(struct pdf_doc *pdf);
```

```
void pdf_fill(struct pdf_doc *pdf);
```

```
void pdf_stroke(struct pdf_doc *pdf);
```

```
void pdf_fillstroke(struct pdf_doc *pdf);
```

```
void pdf_clip(struct pdf_doc *pdf);
```

```
void pdf_save(struct pdf_doc *pdf);
```

```
void pdf_restore(struct pdf_doc *pdf);
```

# Graphics core attributes

```
int pdf_setfont(struct pdf_doc *pdf, const char *font);
```
Sets the current font to the given font.
Uses the given name as is. No check is made. Better sick to the standard PDF fonts.
Returns an opaque ID that can be used in `pdf_text`.

```
void pdf_setopacity(struct pdf_doc *pdf, float v);
```
Sets transparency for both fill and stroke.

```
void pdf_setfill(struct pdf_doc *pdf, float r, float g, float b);
```

```
void pdf_setstroke(struct pdf_doc *pdf, float r, float g, float b);
```

```
void pdf_setgray(struct pdf_doc *pdf, float f, float v);
```
Sets gray intensities for both fill and stroke.

```
void pdf_setlinewidth(struct pdf_doc *pdf, float w);
```

```
void pdf_setlinejoin(struct pdf_doc *pdf, int v);
```

```
void pdf_setlinecap(struct pdf_doc *pdf, int v);
```

```
void pdf_setmiterlimit(struct pdf_doc *pdf, int v);
```

```
void pdf_setdash(struct pdf_doc *pdf, float a[], int n, int d);
```

## Graphics convenience primitives

```
void pdf_setcolor(struct pdf_doc *pdf, float r, float g, float b, float a);
```

```
void pdf_scale(struct pdf_doc *pdf, float x, float y);
```

```
void pdf_translate(struct pdf_doc *pdf, float x, float y);
```

```
void pdf_rotate(struct pdf_doc *pdf, float angle);
```
The angle is in degrees.

```
void pdf_line(struct pdf_doc *pdf, float x1, float y1, float x2, float y2);
```

```
void pdf_triangle(struct pdf_doc *pdf, float x1, float y1, float x2, float y2, float x3, float y3);
```

```
void pdf_quad(struct pdf_doc *pdf, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
```

```
void pdf_polyline(struct pdf_doc *pdf, float x[], float y[], int n);
```

```
void pdf_polygon(struct pdf_doc *pdf, float x[], float y[], int n);
```

```
void pdf_ellipse(struct pdf_doc *pdf, float x, float y, float xr, float yr);
```

```
void pdf_circle(struct pdf_doc *pdf, float x, float y, float r);
```

```
void pdf_text(struct pdf_doc *pdf, const char *text, int font, float size, float x, float y, float angle);
```
If font is 0, then use the current font.
The angle is in degrees.

```
void pdf_addraw(struct pdf_doc *pdf, const char *code);
```
For the adventurous.

