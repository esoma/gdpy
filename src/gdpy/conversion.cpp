
#include "conversion.h"
#include <iostream>
bool pyobject_to_variant(PyObject *py, Variant &variant)
{
    if (py == Py_None)
    {
        return true;
    }
    if (PyUnicode_Check(py))
    {
        auto chr_py = PyUnicode_AsUTF8(py);
        if (!chr_py){ return false; }
        variant = chr_py;
        return true;
    }
    if (PyLong_Check(py))
    {
        auto long_py = PyLong_AsLong(py);
        if (long_py == -1 && PyErr_Occurred()){ return false; }
        variant = long_py;
        return true;
    }
    PyErr_Format(PyExc_TypeError, "unable to convert %R to godot Variant", py);
    return false;
}
