#include <Python.h>
#include "corenlp/document.h"
#include "corenlp/xmlparser.h"
#include "cooc/nchains.h"

static PyObject *
extract_untyped_chains(PyObject *self, PyObject* args)
{
   
    char *path = NULL;   
    if (!PyArg_ParseTuple(args, "s", &path)) {
        return NULL;
    }
                    
    document_t * doc = cu_build_cnlp_docs_file (path); 
    chambers_nc_untyped_array_t *nchains = cu_extract_chambers_nc_untyped_array (doc);
    PyObject* nchains_tuple = PyTuple_New(nchains->num_chains);
    for (int c=0; c < nchains->num_chains; c++) {
        chambers_nc_untyped_t *nchain = (chambers_nc_untyped_t *) nchains->chains[c];
        PyObject* nchain_tuple = PyTuple_New(2);
        PyTuple_SetItem(nchain_tuple, 0, PyUnicode_FromString(nchain->protag));
        
        PyObject* events_tuple = PyTuple_New(nchain->num_events);
        for (int e=0; e < nchain->num_events; e++) {
            PyTuple_SetItem(
                events_tuple, e, PyUnicode_FromString(nchain->events[e]));
        }
        PyTuple_SetItem(nchain_tuple, 1, events_tuple);

        PyTuple_SetItem(nchains_tuple, c, nchain_tuple);
    }

    cu_chambers_nc_untyped_array_free (&nchains); 
    cu_document_free (&doc, TRUE);

    return nchains_tuple;
}

static PyMethodDef xyzzy_methods[] = {
    {"extract_untyped_chains", extract_untyped_chains,  1,
     "Return the meaning of everything."},
    {NULL,      NULL}       /* sentinel */
};

PyMODINIT_FUNC
initnchain(void)
{
    PyImport_AddModule("nchain");
    Py_InitModule("nchain", xyzzy_methods);
}

