#ifndef PYTHON_REF_H
#define PYTHON_REF_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>

class PythonRef
{
private:
    size_t ref_count;
public:
    void release();
    PyObject *ref;
    PythonRef(PyObject *ref, bool strong = true);
    ~PythonRef();
};

#endif 