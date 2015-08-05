#include <mupdf/fitz.h>
//#include <leptonica/allheaders.h>
//#include <tesseract/capi.h>
char* iterate(char* filename, int zoom, int rotation, int chipout, fz_rect chip_bounds);
char* readFromImage(fz_pixmap *);
void concat_str(char*, char*);
void die(const char *errstr);
void _my_init(char* filename);
void _my_release(void);
int get_pagecount(char* filename);
int render(char *filename, int pagenumber, int zoom, int rotation, int write_file, int do_chipout, fz_rect chip_bounds);
char* readFromImage(fz_pixmap *pix);
void concat_str(char* dest, char* src);
char* iterate(char* filename, int zoom, int rotation, int chipout, fz_rect chip_bounds);
void match_pdf_text(char* filename, char* regex, int zoom, int** rtv, int* size, int chipout, fz_rect chip_bounds);
