#ifndef PYTHON_ERROR_H
#define PYTHON_ERROR_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "core/error/error_macros.h"
#include "python_ref.h"

#define REPORT_PYTHON_ERROR()\
    {\
        PythonRef exception(PyErr_GetRaisedException());\
        if (exception)\
        {\
            PyErr_Clear();\
            PyErr_DisplayException(exception);\
            PythonRef exception_str(PyObject_Repr(exception));\
            if (exception_str)\
            {\
                ERR_PRINT(PyUnicode_AsUTF8(exception_str));\
            }\
            else\
            {\
                ERR_PRINT("problem displaying python error");\
            }\
        }\
    }

#endif