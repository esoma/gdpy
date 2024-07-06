
#include "python_ref.h"

PythonRef::PythonRef(PyObject *ref, bool strong)
{
    this->ref = ref;
    ref_count = 1;
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
    if (!ref){ return; }
    for(size_t i = 0; i < ref_count; i++)
    {
        Py_DECREF(ref);
    }
    ref = 0;
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