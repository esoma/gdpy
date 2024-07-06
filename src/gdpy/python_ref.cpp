
#include "python_ref.h"

PythonRef::PythonRef(PyObject *ref, bool strong)
{
    this->ref = ref;
    if (ref && !strong)
    {
        Py_INCREF(ref);
    }
}

PythonRef::~PythonRef()
{
    release();
}


void PythonRef::release()
{
    if (ref)
    {
        Py_DECREF(ref);
        ref = 0;
    }
}


PythonRef::operator bool() const
{
    return ref != 0;
}


PythonRef::operator PyObject *() const
{
    return ref;
}


PythonRef::operator PyTupleObject *() const
{
    return (PyTupleObject *)ref;
}