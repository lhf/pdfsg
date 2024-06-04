# Notes on pdfsg

I wrote pdfsg as a learning experience. PostScript is still my favorite format for vector graphics because it is pure text that is easily editable. Although PDF can be pure text, it is hard to edit by hand because one needs to fix offsets and stream sizes. Not fun. One needs a library to write PDF and do all the bookkeeping.

I had postponed learning about the internals of PDF until I found a very readable simple library that outputs PDF.
The code in pdfsg is based on [PDFGen](https://github.com/AndreRenaud/PDFGen), public domain code by [Andre Renaud](https://github.com/AndreRenaud).
The [PDF Reference](https://opensource.adobe.com/dc-acrobat-sdk-docs/pdfstandards/pdfreference1.4.pdf) is still the main document.

I've trimmed pdfgen.c extensively. Here are the main changes:

- Since pdfsg handles only graphics and text, I've removed support for bookmarks, images, links, outlines.

- Since pdfsg outputs graphics contents immediately, I've removed the code for dynamic strings. The result is that pdfsg uses little memory: essentially only for PDF documents and objects, all of which have a fixed size.

- I've replaced flexible arrays with linked lists. Objects are now part of three linked lists: all objects in a document, all objects of the same type, all streams in a page. These list don't need to be traversed frequently, mostly only when the document is closed.

- I've kept the very nice PDF core in pdfgen, especially the arrays of linked lists indexed by object types. This architecture is very flexible and makes adding new types an easy task.

- To support immediate graphics, I've added objects of type length, which store the length of streams. (PDF is designed to allow stream lengths to be indirect objects precisely to support immediate graphics.)
When a new page is created, one new stream and one new length object are created.
The stream is automatically closed when another page or another group is created.
Once pdfsg had standalone streams, it was natural to add groups to the API.
Groups can be reused in any page without duplication.
Page streams are actually anonymous groups.

The API in pdfsg differs from the API in pdfgen. Here are the main differences:
- the only pointer used is the pointer to the pdf document.
- pages, groups, and fonts are given integer IDs, the object index in the pdf.
- pages are never passed as arguments, only the pdf document, except for 
pdf_addgroup.
- color especification uses explicit RGB.

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
The rest of the API in pdfsg is the standard API for modern 2d graphics, which is based on the PDF imaging model. There are also several convenience functions.
