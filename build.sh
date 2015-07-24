#!/bin/bash
sudo apt-get install libtesseract-dev libleptonica-dev tesseract-ocr-eng

git clone http://mupdf.com/repos/mupdf.git
git config --global url.http://git.insteadOf git://git

cd mupdf
git submodule update --init

HAVE_X11="no" XCFLAGS="-fPIC" make release

#For testing (builds an executable):
#gcc -g -o build/release/example -Iinclude ../conv.c build/release/libmupdf.a build/release/libmujs.a build/release/libfreetype.a build/release/libjbig2dec.a build/release/libjpeg.a build/release/libopenjpeg.a build/release/libz.a -lm -lcrypto -ltesseract -lmupdf -llept

cd ..
python setup.py install

