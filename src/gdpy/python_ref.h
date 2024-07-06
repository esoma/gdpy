#ifndef PYTHON_REF_H
#define PYTHON_REF_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>

class PythonRef
{
private:
    PyObject *ref;
public:
    void release();
    PythonRef(PyObject *ref, bool strong = true);
    ~PythonRef();
    
    operator bool() const;
    operator PyObject *() const;
    operator PyTupleObject *() const;
};

#endif 