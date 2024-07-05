
#include "gdpy_module.h"
#include <iostream>

#include "core/object/class_db.h"
#include <vector>


/* Variant */
typedef struct
{
    PyObject_HEAD
    Variant *variant;
} VariantWrapper;

static PyTypeObject VariantWrapperType = {
    PyVarObject_HEAD_INIT(0, 0)
    "_gdpy.VariantWrapper",
    sizeof(VariantWrapper),
    0
};

static PyObject *
VariantWrapper_create(Variant &variant)
{
    if (
        variant.get_type() == Variant::Type::NIL ||
        (variant.get_type() == Variant::Type::OBJECT && variant.is_null())
    )
    {
        Py_RETURN_NONE;
    }
    VariantWrapper *self = (VariantWrapper *)VariantWrapperType.tp_alloc(&VariantWrapperType, 0);
    if (!self){ return 0; }
    self->variant = new Variant(variant);
    return (PyObject *)self;
}

static void
VariantWrapper_dealloc(VariantWrapper *self)
{
    if (self->variant)
    {
        delete self->variant;
    }
    Py_TYPE(self)->tp_free((PyObject *)self);
}


static PyObject *
VariantWrapper_call_method(PyObject *self, PyObject *args)
{
    PyObject *py_method_name;
    PyObject *py_args;
    if (!PyArg_ParseTuple(args, "UO", &py_method_name, &py_args))
    {
        return 0;
    }
    
    auto arg_count = PyTuple_Size(py_args);
    if (arg_count == -1){ return 0; }
    
    const char *method_name = PyUnicode_AsUTF8(py_method_name);
    if (!method_name){ return 0; }
    
    std::vector<const Variant*> v_args(arg_count);
    for (Py_ssize_t i = 0; i < arg_count; i++)
    {
        auto py_arg = (VariantWrapper *)PyTuple_GET_ITEM(py_args, i);
        if (!py_arg){ return 0; }
        auto is_instance = PyObject_IsInstance(
            (PyObject *)py_arg,
            (PyObject *)&VariantWrapperType
        );
        if (is_instance == -1){ return 0; }
        if (is_instance == 0)
        {
            PyErr_Format(PyExc_TypeError, "expected VariantWrapper");
            return 0;
        }
        v_args[i] = py_arg->variant;
    }
    
    Callable::CallError error;
    Variant ret;
    ((VariantWrapper *)self)->variant->callp(  
        method_name,
        &v_args[0],
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
    return VariantWrapper_create(ret);
}

static PyObject *
VariantWrapper_create_bool(PyObject *self, PyObject *obj)
{
    auto value = PyObject_IsTrue(obj);
    if (value == -1){ return 0; }
    return VariantWrapper_create(Variant((bool)value));
}

static PyObject *
VariantWrapper_create_int(PyObject *self, PyObject *obj)
{
    auto value = PyLong_AsLong(obj);
    if (value == -1 && PyErr_Occurred()){ return 0; }
    return VariantWrapper_create(Variant(value));
}

static PyObject *
VariantWrapper_create_nil(PyObject *self, PyObject *obj)
{
    return VariantWrapper_create(Variant());
}

static PyObject *
VariantWrapper_create_String(PyObject *self, PyObject *obj)
{
    auto value = PyUnicode_AsUTF8(obj);
    if (!value){ return 0; }
    return VariantWrapper_create(Variant(String(value)));
}

static PyObject *
VariantWrapper_narrow_bool(VariantWrapper *self, PyObject *unused)
{
    if (self->variant->operator bool())
    {
        Py_RETURN_TRUE;
    }
    Py_RETURN_FALSE;
};

static PyObject *
VariantWrapper_get_target(VariantWrapper *self, PyObject *unused)
{
    return PyUnicode_FromString("Variant");
};

static PyObject *
VariantWrapper_narrow_float(VariantWrapper *self, PyObject *unused)
{
    return PyFloat_FromDouble(self->variant->operator double());
};

static PyObject *
VariantWrapper_narrow_int(VariantWrapper *self, PyObject *unused)
{
    return PyLong_FromLong(self->variant->operator int64_t());
};

static PyObject *
VariantWrapper_narrow_str(VariantWrapper *self, PyObject *unused)
{
    return PyUnicode_FromString(
        (self->variant->operator String()).utf8().get_data()
    );
};

static PyMethodDef VariantWrapper_method[] = {
    {"call_method", (PyCFunction)VariantWrapper_call_method, METH_VARARGS},
    {"create_bool", (PyCFunction)VariantWrapper_create_bool, METH_O | METH_STATIC},
    {"create_int", (PyCFunction)VariantWrapper_create_int, METH_O | METH_STATIC},
    {"create_nil", (PyCFunction)VariantWrapper_create_nil, METH_NOARGS | METH_STATIC},
    {"create_String", (PyCFunction)VariantWrapper_create_String, METH_O | METH_STATIC},
    {"narrow_bool", (PyCFunction)VariantWrapper_narrow_bool, METH_NOARGS},
    {"narrow_float", (PyCFunction)VariantWrapper_narrow_float, METH_NOARGS},
    {"narrow_int", (PyCFunction)VariantWrapper_narrow_int, METH_NOARGS},
    {"narrow_str", (PyCFunction)VariantWrapper_narrow_str, METH_NOARGS},
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
    
    auto arg_count = PyTuple_Size(py_args);
    if (arg_count == -1){ return 0; }

    auto method_bind = (MethodBind *)PyCapsule_GetPointer(
        py_method_bind,
        "_gdpy.MethodBind"
    );
    if (!method_bind){ return 0; }
    
    std::vector<const Variant*> v_args(arg_count);
    for (Py_ssize_t i = 0; i < arg_count; i++)
    {
        auto py_arg = (VariantWrapper *)PyTuple_GET_ITEM(py_args, i);
        if (!py_arg){ return 0; }
        auto is_instance = PyObject_IsInstance(
            (PyObject *)py_arg,
            (PyObject *)&VariantWrapperType
        );
        if (is_instance == -1){ return 0; }
        if (is_instance == 0)
        {
            PyErr_Format(PyExc_TypeError, "expected VariantWrapper");
            return 0;
        }
        v_args[i] = py_arg->variant;
    }

    Object *instance = 0;
    if (py_instance != Py_None)
    {
        instance = ((VariantWrapper *)py_instance)->variant->operator Object *();
    }
    Callable::CallError error;
    auto ret = method_bind->call(instance, &v_args[0], arg_count, error);
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
    return VariantWrapper_create(ret);
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
    VariantWrapperType.tp_methods = VariantWrapper_method;
    VariantWrapperType.tp_dealloc = (destructor)VariantWrapper_dealloc;
    
    OutputStreamType.tp_new = PyType_GenericNew;
    OutputStreamType.tp_methods = OutputStream_method;
    
    if (PyType_Ready(&VariantWrapperType) < 0){ return 0; }
    if (PyType_Ready(&OutputStreamType) < 0){ return 0; }
    
    gdpy_module_def.m_methods = gdpy_module_methods;
    PyObject *module = PyModule_Create(&gdpy_module_def);
    if (!module){ return 0; }
    
    Py_INCREF(&VariantWrapperType);
    if (PyModule_AddObject(module, "VariantWrapper", (PyObject *)&VariantWrapperType) < 0) {
        Py_DECREF(&VariantWrapperType);
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
