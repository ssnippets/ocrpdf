# ocrpdf
Perform OCR on PDF files with Tesseract (and in Python).

The goal of this project was to be able to read in PDF files and OCR them. In this task, we needed to pull out a 
known area of the PDF and parse it to see if it contained some known text. Other alternatives were not very efficient
since they ended up writing out to file and reading back. We ended up using muPDF to read in the document, chipping out
the necessary chunk, converting it to an image readable by Leptonica, and sending it over to Tessearact. 


Installation
------------
Read the build.sh, modify it for your desired language, or environment and run. We're building muPDF from source 
to link it to the Python bindings.

The included build.sh is currently set up for Debian-flavored Linux (apt). 

Feel free to mess around with the C executable, to compile, use something similar to:

```
gcc -g -o build/release/example -Iinclude ../conv.c build/release/libmupdf.a build/release/libmujs.a \
build/release/libfreetype.a build/release/libjbig2dec.a build/release/libjpeg.a \
build/release/libopenjpeg.a build/release/libz.a -lm -lcrypto -ltesseract -lmupdf -llept
```

Running
-------
In python:
```
from ocrpdf import ocr_pdf
text = ocr_pdf('<filename>', <zoom_level>)
```
Where zoom_level is an integer representing a percentage (100 = 100%) of how far to zoom into. If you're not
getting great OCR results, consider zooming in.

```
from ocrpdf import ocr_match
rtv = ocr_match('<filename>', '<regex>', <zoom_level>)
```
Will iterate through pages looking for a regex match and return a boolean tuple of pages where the string is found. It will also store png files in the same folder as the pdf.

```
from ocrpdf import ocr_match
rtv = ocr_chip('<filename>', '<regex>', <zoom_level>, x0, y0, x1, y1)
```
Works similar to ocr_match, however, it will only search on a chipout of the image as defined by x0, y0, x1, y1. Where each of [x0, y0, x1, y1] correspond to a percentage of (left, top, right, bottom) of the image from [0,1]. Note that x0+x1 > 1 and y0+y1 > 1.
