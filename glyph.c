#include <stdio.h>

#include "pdfsg.h"

int main(void)
{
    int L=400;
    int p,g;
    const char *output = "output.pdf";
    struct pdf_doc *pdf = pdf_newdoc(output);
    if (pdf==NULL) { perror(output); return 1; }

    p=pdf_newpage(pdf,L,L,"glyph as text");
    pdf_concat(pdf,1,0,0,1,0,140);
    pdf_setfill(pdf,0.0,0.0,0.4);
    pdf_setfont(pdf,"Times-Roman");
    pdf_text(pdf,"g",0,512,0,0,0);
    pdf_setfont(pdf,"Helvetica");
    pdf_text(pdf,"text",0,24,L-80,0,0);

    g=pdf_newgroup(pdf);
    pdf_circle(pdf,L-50,-L/4,10);
    pdf_fillstroke(pdf);
    pdf_endgroup(pdf);
    pdf_addgroup(pdf,p,g);

    p=pdf_newpage(pdf,L,L,"glyph as path");
    pdf_concat(pdf,1,0,0,1,0,140);
    pdf_setfill(pdf,1.0,0.0,0.0);
    pdf_setfont(pdf,"Helvetica");
    pdf_text(pdf,"path",0,24,L-80,0,0);
    pdf_setfill(pdf,0.8,0.8,0.8);
    pdf_setstroke(pdf,1.0,0.0,0.0);
    pdf_moveto(pdf,50.25,-46.25);
    pdf_conicto(pdf,50.25,-67.25,73.875,-75.25);
    pdf_conicto(pdf,97.5,-83.25,128.0,-83.25);
    pdf_conicto(pdf,169.5,-83.25,195.5,-68.875);
    pdf_conicto(pdf,221.5,-54.5,221.5,-33.5);
    pdf_conicto(pdf,221.5,-16.75,200.5,-11.25);
    pdf_conicto(pdf,187.5,-8.0,151.25,-7.5);
    pdf_conicto(pdf,142.0,-7.25,131.75,-6.875);
    pdf_conicto(pdf,121.5,-6.5,113.0,-6.0);
    pdf_conicto(pdf,107.5,-5.75,95.25,-4.0);
    pdf_conicto(pdf,83.0,-2.25,77.0,-1.0);
    pdf_conicto(pdf,74.0,-1.0,62.25,-16.75);
    pdf_conicto(pdf,50.25,-32.75,50.25,-46.25);
    pdf_moveto(pdf,82.75,84.0);
    pdf_conicto(pdf,60.0,91.5,47.375,110.75);
    pdf_conicto(pdf,34.75,130.0,34.75,154.0);
    pdf_conicto(pdf,34.75,183.25,58.0,209.5);
    pdf_conicto(pdf,81.25,235.75,123.75,235.75);
    pdf_conicto(pdf,142.0,235.75,164.0,227.125);
    pdf_conicto(pdf,186.0,218.5,206.25,218.5);
    pdf_conicto(pdf,211.5,218.5,222.125,218.875);
    pdf_conicto(pdf,232.75,219.25,237.5,219.25);
    pdf_lineto(pdf,240.5,219.25);
    pdf_lineto(pdf,240.5,199.0);
    pdf_lineto(pdf,197.25,199.0);
    pdf_conicto(pdf,201.75,188.5,204.25,180.75);
    pdf_conicto(pdf,208.5,166.25,208.5,153.25);
    pdf_conicto(pdf,208.5,124.75,184.875,100.875);
    pdf_conicto(pdf,161.25,77.0,121.25,77.0);
    pdf_conicto(pdf,115.0,77.0,98.75,79.25);
    pdf_conicto(pdf,91.5,79.25,79.625,67.0);
    pdf_conicto(pdf,67.75,54.75,67.75,47.0);
    pdf_conicto(pdf,67.75,39.0,85.25,35.25);
    pdf_conicto(pdf,96.75,32.75,110.75,32.75);
    pdf_conicto(pdf,175.0,32.75,198.0,25.5);
    pdf_conicto(pdf,235.75,13.75,235.75,-25.0);
    pdf_conicto(pdf,235.75,-64.5,191.625,-88.0);
    pdf_conicto(pdf,147.5,-111.5,102.5,-111.5);
    pdf_conicto(pdf,61.5,-111.5,37.5,-94.875);
    pdf_conicto(pdf,13.5,-78.25,13.5,-60.0);
    pdf_conicto(pdf,13.5,-51.0,19.875,-42.375);
    pdf_conicto(pdf,26.25,-33.75,45.0,-16.75);
    pdf_lineto(pdf,61.5,-2.0);
    pdf_lineto(pdf,64.5,1.0);
    pdf_conicto(pdf,53.0,5.5,47.25,9.75);
    pdf_conicto(pdf,37.25,17.5,37.25,27.5);
    pdf_conicto(pdf,37.25,36.75,45.875,47.875);
    pdf_conicto(pdf,54.5,59.0,82.75,84.0);
    pdf_moveto(pdf,130.25,89.0);
    pdf_conicto(pdf,144.0,89.0,153.25,96.5);
    pdf_conicto(pdf,168.25,108.5,168.25,138.25);
    pdf_conicto(pdf,168.25,162.0,156.125,191.75);
    pdf_conicto(pdf,144.0,221.5,115.75,221.5);
    pdf_conicto(pdf,91.25,221.5,82.0,198.25);
    pdf_conicto(pdf,77.25,186.0,77.25,168.0);
    pdf_conicto(pdf,77.25,137.5,92.0,113.25);
    pdf_conicto(pdf,106.75,89.0,130.25,89.0);
    pdf_moveto(pdf,128.25,235.75);
    pdf_lineto(pdf,128.25,235.75);
    pdf_fillstroke(pdf);
    pdf_addgroup(pdf,p,g);

    p=pdf_newpage(pdf,L,L,"transparency");
    pdf_concat(pdf,1,0,0,1,0,140);
    pdf_setfill(pdf,0.0,0.0,0.4);
    pdf_setfont(pdf,"Times-Roman");
    pdf_text(pdf,"g",0,512,0,0,0);
    pdf_setfont(pdf,"Helvetica");
    pdf_text(pdf,"transparency",0,24,L-150,0,0);
    pdf_setfill(pdf,1.0,1.0,0.0);
    pdf_setstroke(pdf,0.0,0.0,0.0);
    pdf_setopacity(pdf,0.4);
    pdf_circle(pdf,120,155,100);
    pdf_fillstroke(pdf);
    pdf_addgroup(pdf,p,g);

    pdf_enddoc(pdf);
    return 0;
}
