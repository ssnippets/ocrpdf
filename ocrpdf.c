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

   if (!PyArg_ParseTuple(args, "si", &filename, &zoom)){
       zoom = 200;
       if (!PyArg_ParseTuple(args, "s", &filename)){
           return NULL;
       }
   }
	
   // don't pass in a rotation, just filename and zoom:
   tmp_text = iterate(filename, zoom, 0);

   rtv = Py_BuildValue("s", tmp_text);
   free(tmp_text);
   return rtv;

}

static PyMethodDef Methods[] = {
    {"ocr_pdf",  pyocr_iterate, METH_VARARGS,
     "OCR a PDF file."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC
initocrpdf(void)
{
    (void) Py_InitModule("ocrpdf", Methods);
}

