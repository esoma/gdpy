
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


PyObject *variant_to_pyobject(Variant &variant)
{
    switch(variant.get_type())
    {
        case Variant::Type::NIL:
            Py_RETURN_NONE;
        case Variant::Type::INT:
            return PyLong_FromLong(variant.operator int64_t());
        case Variant::Type::OBJECT:
            if (variant.is_null()){ Py_RETURN_NONE; }
            // objptr = variant.operator Object *()
            break;
        case Variant::Type::PACKED_INT64_ARRAY:
        {
            std::cout << (variant.operator PackedInt64Array()).get(0) << std::endl;
            break;
        }
    }
    PyErr_Format(
        PyExc_TypeError,
        "unable to convert %s to godot Variant (%s)",
        variant.stringify().utf8().get_data(),
        Variant::get_type_name(variant.get_type()).utf8().get_data()
    );
    return 0;
}