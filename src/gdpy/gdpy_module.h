#ifndef GDPY_MODULE_H
#define GDPY_MODULE_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>

extern "C"
{
    
PyMODINIT_FUNC PyInit__gdpy();

}

#endif