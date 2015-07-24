#include <mupdf/fitz.h>
//#include <leptonica/allheaders.h>
//#include <tesseract/capi.h>
char* iterate(char* filename, int zoom, int rotation);
char* readFromImage(fz_pixmap *);
void concat_str(char*, char*);
void die(const char *errstr);
void _my_init(char* filename);
void _my_release();
int get_pagecount(char* filename);
void render(char *filename, int pagenumber, int zoom, int rotation, int write_file, int do_chipout);
char* readFromImage(fz_pixmap *pix);
void concat_str(char* dest, char* src);
char* iterate(char* filename, int zoom, int rotation);

