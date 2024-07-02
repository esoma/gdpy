#ifndef CONVERSION_H
#define CONVERSION_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "core/variant/variant.h"

bool pyobject_to_variant(PyObject *py, Variant &variant);
PyObject *variant_to_pyobject(Variant &variant);

#endif