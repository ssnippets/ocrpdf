/******************************************************************************
 * Integrate with muPDF to read a PDF file, 
 * convert it to an appropriate Leptonica
 * format, send it over to Tesseract and
 * get OCR results.
 *
 * Copyright (C) 2015 Jorge Sanchez <jorge.ud@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Additional Licenses: 
 * muPDF: http://www.gnu.org/licenses/agpl-3.0.html
 * Tesseract: https://www.apache.org/licenses/LICENSE-2.0
 * Leptonica: http://tpgit.github.io/UnOfficialLeptDocs/leptonica/about-the-license.html
 *
 * 
 * Base Code: http://mupdf.com/docs/example.c
 *
 *****************************************************************************/
#include <mupdf/fitz.h>
#include <leptonica/allheaders.h>
#include <tesseract/capi.h>
#include <string.h>
#include "conv.h"

fz_context *ctx;
fz_document *doc;
TessBaseAPI *tessAPI;
char* text;

void die(const char *errstr) {
    fputs(errstr, stderr);
    exit(1);
}

void _my_init(char* filename){
    ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
    // Register the default file types.
    fz_register_document_handlers(ctx);
    // Open the PDF, XPS or CBZ document.

    fz_try(ctx){
        doc = fz_open_document(ctx, filename);
    }
    fz_catch(ctx){
        doc = NULL;
        return;
    }
    //Load Tesseract API:
    tessAPI= TessBaseAPICreate();
    if(TessBaseAPIInit3(tessAPI, NULL, "eng") != 0)
        die("Error initialising tesseract\n");

    // allocate a single element so we can consistently free:
    text = (char*) malloc(1*sizeof(char));
    text[0] = '\0';
}


void _my_release(){
    // cleanup:
    fz_drop_document(ctx, doc);
    fz_drop_context(ctx);
    TessBaseAPIEnd(tessAPI);
    TessBaseAPIDelete(tessAPI);
 //   free(text);
}
int get_pagecount(char* filename){

    int pagecount;
    // Retrieve the number of pages (not used in this example).
    pagecount = fz_count_pages(ctx, doc);
    return pagecount;
}
void
render(char *filename, int pagenumber, int zoom, int rotation, int write_file, int do_chipout)
{
    //int pagecount;
    fz_page *page;
    fz_matrix transform;
    fz_rect bounds;
    fz_irect bbox;
    fz_pixmap *pix;
    fz_device *dev;

    // Load the page we want. Page numbering starts from zero.


    page = fz_load_page(ctx, doc, pagenumber ); //- 1);

    // Calculate a transform to use when rendering. This transform
    // contains the scale and rotation. Convert zoom percentage to a
    // scaling factor. Without scaling the resolution is 72 dpi.

    fz_rotate(&transform, rotation);
    fz_pre_scale(&transform, zoom / 100.0f, zoom / 100.0f);

    // Take the page bounds and transform them by the same matrix that
    // we will use to render the page.

    fz_bound_page(ctx, page, &bounds);
    fz_transform_rect(&bounds, &transform);

    // Create a blank pixmap to hold the result of rendering. The
    // pixmap bounds used here are the same as the transformed page
    // bounds, so it will contain the entire page. The page coordinate
    // space has the origin at the top left corner and the x axis
    // extends to the right and the y axis extends down.

    fz_round_rect(&bbox, &bounds);

    //chipout bottom left corner:
    if(do_chipout){
        bbox.y0 =  bbox.y1 - bbox.y1/5;
        bbox.x1 = bbox.x1/3;
    }

    pix = fz_new_pixmap_with_bbox(ctx, fz_device_rgb(ctx), &bbox);

    fz_clear_pixmap_with_value(ctx, pix, 0xff);

    // A page consists of a series of objects (text, line art, images,
    // gradients). These objects are passed to a device when the
    // interpreter runs the page. There are several devices, used for
    // different purposes:
    //
    //	draw device -- renders objects to a target pixmap.
    //
    //	text device -- extracts the text in reading order with styling
    //	information. This text can be used to provide text search.
    //
    //	list device -- records the graphic objects in a list that can
    //	be played back through another device. This is useful if you
    //	need to run the same page through multiple devices, without
    //	the overhead of parsing the page each time.

    // Create a draw device with the pixmap as its target.
    // Run the page with the transform.

    dev = fz_new_draw_device(ctx, pix);
    fz_run_page(ctx, page, dev, &transform, NULL);
    fz_drop_device(ctx, dev);

    // Save the pixmap to a file.
//    if(write_file){
//        fz_write_png(ctx, pix, "/tmp/out.png", 0);
//  }
    text = readFromImage (pix);

    fz_drop_pixmap(ctx, pix);
    fz_drop_page(ctx, page);
}

char* readFromImage(fz_pixmap *pix){
    // For in-memory recognition, convert the pixmap into a
    // format readable by Leptonica. Using PNG in this case,
    // then call the Tesseract API to get the OCR'ed text:
    fz_buffer * pix_png;
    int size;
    PIX *png;
    char* imgtext;

    pix_png = fz_new_png_from_pixmap(ctx, pix);
    size = fz_pixmap_size(ctx, pix);
    png = pixReadMemPng((l_uint8*)pix_png->data, size);
    TessBaseAPISetImage2(tessAPI, (struct Pix *) png);
    if(TessBaseAPIRecognize(tessAPI, NULL) != 0)
        die("Error in Tesseract recognition\n");
    if((imgtext = TessBaseAPIGetUTF8Text(tessAPI)) == NULL)
        die("Error getting text\n");
    concat_str(text, imgtext);
    TessDeleteText(imgtext);

    return text; // NEEDS CLEANED
}

//dynamically allocated string concatenation
void concat_str(char* dest, char* src){
    int len = strlen(dest) + strlen(src) +2;
    char* tmp = (char*) malloc(len*sizeof(char));
    strcpy(tmp, text);
    strcat(tmp,src);
    free(text);
    text = tmp;
}

char* iterate(char* filename, int zoom, int rotation){
    _my_init(filename);
    if(doc == NULL){
        text = (char*) malloc(32*sizeof(char));
        strcpy(text, "Error: File does not exist");
        fz_drop_context(ctx);
        return text;
    }
    int page_count = get_pagecount(filename);
    int i;
    for(i = 0; i < page_count; i++){
        render(filename, i, zoom, rotation, 0, 0);
    }
 //   fputs(text, stdout);

    _my_release();
    return text;
}
int main(int argc, char **argv)
{
    char *filename = argv[1];
    int zoom = 200;
    int rotation = 0;

    text = iterate(filename, zoom, rotation);
    printf("%s",text);
    free(text);
    //render(filename, pagenumber, zoom, rotation);

    return 0;
}
