# Notes on pdfsg

The code in pdfsg is based on [PDFGen](https://github.com/AndreRenaud/PDFGen), public domain code by [Andre Renaud](https://github.com/AndreRenaud).

I've trimmed pdfgen.c extensively so that pdfsg handles only graphics and text.
I've removed support for bookmarks, images, links, outlines.

Since pdfsg outputs graphics contents immediately, I've removed the code for dynamic strings. The result is that pdfsg uses little memory: essentially only for PDF documents and objects, which have a fixed size.

I've replaced flexible arrays by linked lists. Objects are now part of three linked lists: all objects in a document, all objects of the same type, all streams in a page. These list don't need to be traversed frequently, mostly only when the document is closed.

I've kept the very nice PDF core in pdfgen, especially the arrays of linked lists indexed by object types. This architecture is very flexible and makes adding new types an easy task.

To support immediate graphics, I've added objects of type length, which store the length of streams. (PDF is designed to allow such lengths to be indirect objects precisely for immediate graphics.)
When a new page is created, both one stream and one length object are created.
The stream is automatically closed when another page or another group is created.
Once pdfsg had standalone streams, it was natural to add groups to the API.
Groups can be reused in any page without duplication.

The API in pdfsg differs from the API in pdfgen. Here are the main differences:
- the only pointer used is the pointer to the pdf document.
- pages, groups, and fonts are given integer IDs, the object index in the pdf.
- pages are never passed as arguments, only the pdf document, except for 
pdf_addgroup.
- color especification uses explicit RGB.

The rest of the API in pdfsg is the standard for modern 2d graphics, which are all based on the PDF imaging model. There are also several convenience functions.

Here is the structure of a typical program using pdfsg:
```
    struct pdf_doc *pdf = pdf_newdoc(output.pdf");

    p=pdf_newpage(pdf,W,H,TITLE);
    ... graphics contents
    
    g=pdf_newgroup(pdf);
    ... graphics contents
    pdf_endgroup(pdf);

    pdf_addgroup(pdf,p,g);

    p=pdf_newpage(pdf,W,H,TITLE);
    ... graphics contents

    pdf_enddoc(pdf);
```