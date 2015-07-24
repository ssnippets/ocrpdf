from distutils.core import setup, Extension

MPDF_LIB='./mupdf/build/release/'

ocrpdf = Extension('ocrpdf',
                    extra_link_args=[MPDF_LIB + 'libmupdf.a', MPDF_LIB + 'libmujs.a', MPDF_LIB + 'libfreetype.a', MPDF_LIB + 'libjbig2dec.a', MPDF_LIB + 'libjpeg.a', MPDF_LIB + 'libopenjpeg.a', MPDF_LIB + 'libz.a'],
                    include_dirs = ['mupdf/include/'],
                    libraries = ['lept','tesseract', 'crypto', 'm', 'freetype', 'jbig2dec', 'openjpeg', 'z'],
                    library_dirs = ['/usr/local/lib', './mupdf/build/release'],
                    sources = ['conv.c', 'ocrpdf.c',])

setup (name = 'ocrpdf',
       version = '0.1',
       description = 'OCR a PDF file',
       author = 'Jorge Sanchez',
       author_email = 'jorge.ud@gmail.com',
       url = 'https://docs.python.org/extending/building',
       long_description = '''
This module allows for passing in a PDF file and returns OCR results. It depends on Tesseract and Leptonica.
''',
       ext_modules = [ocrpdf])

