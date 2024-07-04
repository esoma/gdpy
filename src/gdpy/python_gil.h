#ifndef GDPY_PYTHON_GIL_H
#define GDPY_PYTHON_GIL_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>

class PythonGil
{
private:
    PyGILState_STATE gstate;
public:
    PythonGil();
    ~PythonGil();
};

#endif