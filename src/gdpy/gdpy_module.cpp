
#include "gdpy_module.h"
#include <iostream>

#include "core/object/class_db.h"
#include "conversion.h"
#include <vector>


/* Variant */
typedef struct
{
    PyObject_HEAD
    Variant *variant;
} Variant_;

static PyTypeObject VariantType = {
    PyVarObject_HEAD_INIT(0, 0)
    "_gdpy.Variant",
    sizeof(Variant_),
    0
};

static PyObject *
Variant_create(Variant &variant)
{
    if (
        variant.get_type() == Variant::Type::NIL ||
        (variant.get_type() == Variant::Type::OBJECT && variant.is_null())
    )
    {
        Py_RETURN_NONE;
    }
    Variant_ *self = (Variant_ *)VariantType.tp_alloc(&VariantType, 0);
    if (!self){ return 0; }
    self->variant = new Variant(variant);
    return (PyObject *)self;
}

static void
Variant_dealloc(Variant_ *self)
{
    if (self->variant)
    {
        delete self->variant;
    }
    Py_TYPE(self)->tp_free((PyObject *)self);
}


static PyObject *
Variant_call_method(PyObject *self, PyObject *args)
{
    PyObject *py_method_name;
    PyObject *py_args;
    if (!PyArg_ParseTuple(args, "UO", &py_method_name, &py_args))
    {
        return 0;
    }
    
    auto arg_count = PySequence_Length(py_args);
    if (arg_count == -1){ return 0; }
    
    const char *method_name = PyUnicode_AsUTF8(py_method_name);
    if (!method_name){ return 0; }
    
    std::vector<Variant> v_args(arg_count);
    std::vector<const Variant*> v_argsp(arg_count);
    for (Py_ssize_t i = 0; i < arg_count; i++)
    {
        v_argsp[i] = &v_args[i];
        auto py_arg = PySequence_GetItem(py_args, i);
        if (!py_arg){ return 0; }
        auto conversion = pyobject_to_variant(py_arg, v_args[i]);
        Py_DECREF(py_arg);
        if (!conversion){ return 0; }
    }
    
    Callable::CallError error;
    Variant ret;
    ((Variant_ *)self)->variant->callp(  
        method_name,
        &v_argsp[0],
        arg_count,
        ret,
        error
    );
    switch(error.error)
    {
        case Callable::CallError::CALL_ERROR_INVALID_METHOD:
            PyErr_Format(PyExc_TypeError, "invalid method");
            return 0;
        case Callable::CallError::CALL_ERROR_INVALID_ARGUMENT:
            PyErr_Format(PyExc_TypeError, "invalid argument");
            return 0;
        case Callable::CallError::CALL_ERROR_TOO_MANY_ARGUMENTS:
            PyErr_Format(PyExc_TypeError, "too many arguments");
            return 0;
        case Callable::CallError::CALL_ERROR_TOO_FEW_ARGUMENTS:
            PyErr_Format(PyExc_TypeError, "too few arguments");
            return 0;
        case Callable::CallError::CALL_ERROR_INSTANCE_IS_NULL:
            PyErr_Format(PyExc_TypeError, "instance is null");
            return 0;
        case Callable::CallError::CALL_ERROR_METHOD_NOT_CONST:
            PyErr_Format(PyExc_TypeError, "method not const");
            return 0;
    }
    return Variant_create(ret);
}


static PyObject *
Variant_narrow_bool(Variant_ *self, PyObject *unused)
{
    if (self->variant->operator bool())
    {
        Py_RETURN_TRUE;
    }
    Py_RETURN_FALSE;
};

static PyObject *
Variant_get_target(Variant_ *self, PyObject *unused)
{
    return PyUnicode_FromString("Variant");
};

static PyObject *
Variant_narrow_float(Variant_ *self, PyObject *unused)
{
    return PyFloat_FromDouble(self->variant->operator double());
};

static PyObject *
Variant_narrow_int(Variant_ *self, PyObject *unused)
{
    return PyLong_FromLong(self->variant->operator int64_t());
};

static PyObject *
Variant_narrow_str(Variant_ *self, PyObject *unused)
{
    return PyUnicode_FromString(
        (self->variant->operator String()).utf8().get_data()
    );
};

static PyMethodDef Variant_method[] = {
    {"call_method", (PyCFunction)Variant_call_method, METH_VARARGS},
    {"narrow_bool", (PyCFunction)Variant_narrow_bool, METH_NOARGS},
    {"narrow_float", (PyCFunction)Variant_narrow_float, METH_NOARGS},
    {"narrow_int", (PyCFunction)Variant_narrow_int, METH_NOARGS},
    {"narrow_str", (PyCFunction)Variant_narrow_str, METH_NOARGS},
    {0}
};

/* OutputStream */
typedef struct
{
    PyObject_HEAD
    std::ostream *stream;
} OutputStream;

static PyTypeObject OutputStreamType = {
    PyVarObject_HEAD_INIT(0, 0)
    "_gdpy.OutputStream",
    sizeof(OutputStream),
    0
};

static PyObject *
OutputStream_write(OutputStream *self, PyObject *input)
{
    const char *raw_input = PyUnicode_AsUTF8(input);
    if (!raw_input){ return 0; }
    (*self->stream) << raw_input;
    Py_RETURN_NONE;
};

static PyObject *
OutputStream_flush(OutputStream *self, PyObject *unused)
{
    self->stream->flush();
    Py_RETURN_NONE;
};

static PyMethodDef OutputStream_method[] = {
    {"write", (PyCFunction)OutputStream_write, METH_O},
    {"flush", (PyCFunction)OutputStream_flush, METH_NOARGS},
    {0}
};

/* Module */
static PyModuleDef gdpy_module_def = {
    PyModuleDef_HEAD_INIT,
    "_gdpy",
    "gdpy internals",
    -1
};

static PyObject *
class_db_get_method(PyObject *self, PyObject *args)
{
    PyObject *py_class_name;
    PyObject *py_method_name;
    if (!PyArg_ParseTuple(args, "UU", &py_class_name, &py_method_name))
    {
        return 0;
    }
    const char *class_name = PyUnicode_AsUTF8(py_class_name);
    if (!class_name){ return 0; }
    const char *method_name = PyUnicode_AsUTF8(py_method_name);
    if (!method_name){ return 0; }
    
    auto method = ClassDB::get_method(class_name, method_name);
    if (!method)
    {
        PyErr_Format(
            PyExc_RuntimeError,
            "%s.%s does not exist",
            class_name,
            method_name
        );
        return 0;
    }
    
    return PyCapsule_New(method, "_gdpy.MethodBind", 0);
}

static PyObject *
call_method_bind(PyObject *self, PyObject *args)
{
    PyObject *py_method_bind;
    PyObject *py_instance;
    PyObject *py_args;
    if (!PyArg_ParseTuple(args, "OOO", &py_method_bind, &py_instance, &py_args))
    {
        return 0;
    }
    
    auto arg_count = PySequence_Length(py_args);
    if (arg_count == -1){ return 0; }

    auto method_bind = (MethodBind *)PyCapsule_GetPointer(
        py_method_bind,
        "_gdpy.MethodBind"
    );
    if (!method_bind){ return 0; }
    
    std::vector<Variant> v_args(arg_count);
    std::vector<const Variant*> v_argsp(arg_count);
    for (Py_ssize_t i = 0; i < arg_count; i++)
    {
        v_argsp[i] = &v_args[i];
        auto py_arg = PySequence_GetItem(py_args, i);
        if (!py_arg){ return 0; }
        auto conversion = pyobject_to_variant(py_arg, v_args[i]);
        Py_DECREF(py_arg);
        if (!conversion){ return 0; }
    }

    Object *instance = 0;
    if (py_instance != Py_None)
    {
        instance = ((Variant_ *)py_instance)->variant->operator Object *();
    }
    Callable::CallError error;
    auto ret = method_bind->call(instance, &v_argsp[0], arg_count, error);
    switch(error.error)
    {
        case Callable::CallError::CALL_ERROR_INVALID_METHOD:
            PyErr_Format(PyExc_TypeError, "invalid method");
            return 0;
        case Callable::CallError::CALL_ERROR_INVALID_ARGUMENT:
            PyErr_Format(PyExc_TypeError, "invalid argument");
            return 0;
        case Callable::CallError::CALL_ERROR_TOO_MANY_ARGUMENTS:
            PyErr_Format(PyExc_TypeError, "too many arguments");
            return 0;
        case Callable::CallError::CALL_ERROR_TOO_FEW_ARGUMENTS:
            PyErr_Format(PyExc_TypeError, "too few arguments");
            return 0;
        case Callable::CallError::CALL_ERROR_INSTANCE_IS_NULL:
            PyErr_Format(PyExc_TypeError, "instance is null");
            return 0;
        case Callable::CallError::CALL_ERROR_METHOD_NOT_CONST:
            PyErr_Format(PyExc_TypeError, "method not const");
            return 0;
    }
    return Variant_create(ret);
}

static PyMethodDef gdpy_module_methods[] = {
    {"class_db_get_method", (PyCFunction)class_db_get_method, METH_VARARGS},
    {"call_method_bind", (PyCFunction)call_method_bind, METH_VARARGS},
    {0}
};

/* initialization */
int set_sys_output_stream(const char *name, std::ostream &stream)
{
    auto sys_module = PyImport_ImportModule("sys");
    if (!sys_module){ return 0; }
    
    PyObject *py_output_stream = PyObject_CallNoArgs(
        (PyObject *)&OutputStreamType
    );
    if (!py_output_stream)
    {
        Py_DECREF(sys_module);
        return 0;
    }
    ((OutputStream *)py_output_stream)->stream = &stream;
    
    auto result = PyObject_SetAttrString(sys_module, name, py_output_stream);
    Py_DECREF(py_output_stream);
    Py_DECREF(sys_module);
    return result;
}

PyMODINIT_FUNC PyInit__gdpy()
{
    VariantType.tp_methods = Variant_method;
    VariantType.tp_dealloc = (destructor)Variant_dealloc;
    
    OutputStreamType.tp_new = PyType_GenericNew;
    OutputStreamType.tp_methods = OutputStream_method;
    
    if (PyType_Ready(&VariantType) < 0){ return 0; }
    if (PyType_Ready(&OutputStreamType) < 0){ return 0; }
    
    gdpy_module_def.m_methods = gdpy_module_methods;
    PyObject *module = PyModule_Create(&gdpy_module_def);
    if (!module){ return 0; }
    
    Py_INCREF(&VariantType);
    if (PyModule_AddObject(module, "Variant", (PyObject *)&VariantType) < 0) {
        Py_DECREF(&VariantType);
        Py_DECREF(module);
        return 0;
    }
    
    if (set_sys_output_stream("stdout", std::cout) != 0)
    {
        Py_DECREF(module);
        return 0;
    }
    if (set_sys_output_stream("stderr", std::cerr) != 0)
    {
        Py_DECREF(module);
        return 0;
    }

    return module;
}
