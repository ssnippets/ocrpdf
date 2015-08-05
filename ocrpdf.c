#include <Python.h>
#include "conv.h"

static PyObject *
pyocr_iterate(PyObject *self, PyObject *args)
{
   char *filename;
   int zoom;
//   int rotation;
   char* tmp_text;
   PyObject* rtv;

   //   zoom = 80;
   if (!PyArg_ParseTuple(args, "si", &filename, &zoom)){
       return NULL;
   }
   
   fz_rect* chip_bounds = (fz_rect*) malloc(sizeof(fz_rect));
   chip_bounds->x0 = 1;
   chip_bounds->y0 = 1;
   chip_bounds->x1 = 1;
   chip_bounds->y1 = 1;

   // don't pass in a rotation, just filename and zoom:
   tmp_text = iterate(filename, zoom, 0, 0, *chip_bounds);

   rtv = Py_BuildValue("s", tmp_text);
   free(tmp_text);
   free(chip_bounds);
   return rtv;

}

static PyObject * pyocr_match_pdf_text(PyObject *self, PyObject *args){

    char* filename;
    char* regex;
    int zoom;
    int* rtv;
    int size;
    int i;

    if(!PyArg_ParseTuple(args, "ssi", &filename, &regex, &zoom)){
        return NULL;
    }
   
    fz_rect* chip_bounds = (fz_rect*) malloc(sizeof(fz_rect));
    chip_bounds->x0 = 1;
    chip_bounds->y0 = 1;
    chip_bounds->x1 = 1;
    chip_bounds->y1 = 1; 

    match_pdf_text(filename, regex, zoom, &rtv, &size, 0, *chip_bounds);

    PyObject* tuple = PyTuple_New(size);

    for (i = 0; i < size; i++){
        PyTuple_SetItem(tuple, i, Py_BuildValue("O", rtv[i]? Py_False : Py_True));
    }
    free(rtv);
    free(chip_bounds);
    return tuple;

}
static PyObject * pyocr_match_pdf_chip(PyObject *self, PyObject *args){

    char* filename;
    char* regex;
    int zoom;
    int* rtv;
    int size;
    int i;
    float x0,y0,x1,y1;
    if(!PyArg_ParseTuple(args, "ssiffff", &filename, &regex, &zoom, &x0, &y0, &x1, &y1)){
        return NULL;
    }
   
    fz_rect* chip_bounds = (fz_rect*) malloc(sizeof(fz_rect));
    chip_bounds->x0 = x0;
    chip_bounds->y0 = y0;
    chip_bounds->x1 = x1;
    chip_bounds->y1 = y1; 

    if(x0 + x1 <= 1){
        PyErr_Format(PyExc_ValueError, "Error, x0+x1 <= 1");
        return NULL;
    } else if(y0 + y1 <= 1){
        PyErr_Format(PyExc_ValueError, "Error, y0+y1 <= 1");
        return NULL;
    }
    match_pdf_text(filename, regex, zoom, &rtv, &size, 1, *chip_bounds);

    PyObject* tuple = PyTuple_New(size);

    for (i = 0; i < size; i++){
        PyTuple_SetItem(tuple, i, Py_BuildValue("O", rtv[i]? Py_False : Py_True));
    }
    free(rtv);
    free(chip_bounds);
    return tuple;

}
static PyMethodDef Methods[] = {
    {"ocr_pdf",  pyocr_iterate, METH_VARARGS, 
        "Perform OCR on a file and get the text"},
    {"ocr_match",  pyocr_match_pdf_text, METH_VARARGS,
        "OCR a PDF file."},
    {"ocr_chip", pyocr_match_pdf_chip, METH_VARARGS,
        "OCR chipouts from PDF file:\n"
        "The following constraints apply: x0 + x1 > 1, y0 + y1 > 1"
        "filename - string pdf filename to search \n"
        "regex - string regex to match in document\n"
        "zoom - int zoomlevel, change if detection is poor\n"
        "x0 - float [0:1] chipout percent of left\n"
        "y0 - float [0:1] chipout percent of top\n"
        "x1 - float [0:1] chipout percent of right\n"
        "y1 - float [0:1] chipout percent of bottom\n"
    },
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC
initocrpdf(void)
{
    (void) Py_InitModule("ocrpdf", Methods);
}

