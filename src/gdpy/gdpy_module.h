#ifndef GDPY_MODULE_H
#define GDPY_MODULE_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "core/variant/variant.h"

PyObject *VariantWrapper_create(const Variant &variant);
Variant *VariantWrapper_get_variant(PyObject *self);

extern "C"
{
    
PyMODINIT_FUNC PyInit__gdpy();

}

#endif