#ifndef PYTHON_ERROR_H
#define PYTHON_ERROR_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "core/error/error_macros.h"
#include "python_ref.h"

#define REPORT_PYTHON_ERROR()\
    {\
        PythonRef exception(PyErr_GetRaisedException());\
        if (exception.ref)\
        {\
            PyErr_Clear();\
            PyErr_DisplayException(exception.ref);\
            PythonRef exception_str(PyObject_CallFunction(exception.ref, "__str__"));\
            if (exception_str.ref)\
            {\
                ERR_PRINT(PyUnicode_AsUTF8(exception_str.ref));\
            }\
            else\
            {\
                ERR_PRINT("problem displaying python error");\
            }\
        }\
    }

#endif