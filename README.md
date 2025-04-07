# pdfsg

This is a simple graphics C library that outputs PDF.
Besides providing a standard modern 2d graphics API and several convenience functions, its main features are:

- outputs graphics contents immediately, thus requiring little memory
- supports groups that can be reused without duplication
- supports fill and stroke transparency

pdfsg is based on [PDFGen](https://github.com/AndreRenaud/PDFGen), public domain code by [Andre Renaud](https://github.com/AndreRenaud). See [NOTES.md](NOTES.md) for details on how that code was used and adapted.

The data in glyph.c was obtained using my [igg](https://github.com/lhf/igg).

This code is hereby placed in the public domain and also under the MIT license.
