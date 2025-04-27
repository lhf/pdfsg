/* raw.c -- raw graphics with pdfsg */

#include <stdio.h>
#include "pdfsg.h"

int main(void)
{
    int L=400;
    const char *output="output.pdf";
    struct pdf_doc *pdf=pdf_newdoc(output);
    if (pdf==NULL) { perror(output); return 1; }
    pdf_newpage(pdf,L,L,"raw graphics");
    pdf_setfont(pdf,"Times-Roman");
    pdf_setfont(pdf,"Helvetica");
    pdf_setfont(pdf,"Symbol");
    pdf_setfont(pdf,"Times-Italic");
    pdf_setfont(pdf,"Helvetica−Oblique");
    char b[BUFSIZ];
    while (fgets(b,sizeof(b),stdin)!=NULL) pdf_addraw(pdf,b);
    pdf_enddoc(pdf);
    return 0;
}

