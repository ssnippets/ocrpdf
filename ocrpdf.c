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
	
   // don't pass in a rotation, just filename and zoom:
   tmp_text = iterate(filename, zoom, 0);

   rtv = Py_BuildValue("s", tmp_text);
   free(tmp_text);
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
    match_pdf_text(filename, regex, zoom, &rtv, &size);

    PyObject* tuple = PyTuple_New(size);

    for (i = 0; i < size; i++){
        PyTuple_SetItem(tuple, i, Py_BuildValue("O", rtv[i]? Py_False : Py_True));
    }
    free(rtv);
    return tuple;

}
static PyMethodDef Methods[] = {
    {"ocr_pdf",  pyocr_iterate, METH_VARARGS, 
        "Perform OCR on a file and get the text"},
    {"ocr_match",  pyocr_match_pdf_text, METH_VARARGS,
     "OCR a PDF file."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC
initocrpdf(void)
{
    (void) Py_InitModule("ocrpdf", Methods);
}

