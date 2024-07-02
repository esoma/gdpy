
#include "gdpy_module.h"
#include <iostream>

/* MetaPathFinder */
typedef struct
{
    PyObject_HEAD
} MetaPathFinder;

static PyTypeObject MetaPathFinderType = {
    PyVarObject_HEAD_INIT(0, 0)
    "_gdpy.MetaPathFinder",
    sizeof(MetaPathFinder),
    0
};

static PyObject *
MetaPathFinder_find_spec(MetaPathFinder *self, PyObject *args, PyObject *kwargs)
{
    static char *keywords[] = {"fullname", "path", "target", 0};
    PyObject *fullname;
    PyObject *path_;
    PyObject *target_;
    if (!PyArg_ParseTupleAndKeywords(
        args, kwargs, "UO|O", keywords,
        &fullname, &path_, &target_
    ))
    {
        std::cout << "PyArg_ParseTupleAndKeywords failed" << std::endl;
        PyErr_Print();
        return 0;
    }
    
    bool is_package = false;
    
    auto parts = PyObject_CallMethod(fullname, "split", "s", ",");
    if (!parts){ return 0; }
    auto parts_length = PySequence_Length(parts);
    if (parts_length == -1){ return 0; }
    if (parts_length == 1)
    {
        is_package = true;
        Py_DECREF(parts);
        parts = 0;
    }
    else
    {
        // parts = parts[1:]
        {
            auto sliced_parts = PySequence_GetSlice(parts, 1, -1);
            if (!sliced_parts)
            {
                Py_DECREF(parts);
                return 0;
            }
            auto old_parts = parts;
            parts = sliced_parts;
            Py_DECREF(old_parts);
        }
        // path = "/".join(parts)
        auto slash = PyUnicode_FromString("/");
        if (!slash)
        {
            Py_DECREF(parts);
            return 0;
        }
        auto path = PyObject_CallMethod(slash, "join", "O", parts);
        Py_DECREF(slash);
        slash = 0;
        Py_DECREF(parts);
        parts = 0;
        if (!path){ return 0; }
        // path = "res://" + path
        {
            auto res_prefix = PyUnicode_FromString("res://");
            if (!res_prefix)
            {
                Py_DECREF(path);
                return 0;
            }
            auto new_path = PyNumber_Add(res_prefix, path);
            Py_DECREF(path);
            Py_DECREF(res_prefix);
            if (!new_path){ return 0; }
            path = new_path;
        }
        std::cout << "GDPY META: " << PyUnicode_AsUTF8(path) << std::endl;
        Py_DECREF(path);
    }
    
    std::cout << "GDPY META: " << PyUnicode_AsUTF8(fullname) << std::endl;
        
    Py_RETURN_NONE;
};

static PyMethodDef MetaPathFinder_method[] = {
    {"find_spec", (PyCFunction)MetaPathFinder_find_spec, METH_VARARGS | METH_KEYWORDS},
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


int add_meta_path_finder()
{
    auto sys_module = PyImport_ImportModule("sys");
    if (!sys_module){ return 0; }
    
    PyObject *py_meta_path_finder = PyObject_CallNoArgs(
        (PyObject *)&MetaPathFinderType
    );
    if (!py_meta_path_finder)
    {
        Py_DECREF(sys_module);
        return 0;
    }
    
    auto meta_path = PyObject_GetAttrString(sys_module, "meta_path");
    Py_DECREF(sys_module);
    if (!meta_path)
    {
        Py_DECREF(py_meta_path_finder);
        return 0;
    }
    
    auto result = PyList_Append(meta_path, py_meta_path_finder);
    Py_DECREF(py_meta_path_finder);
    Py_DECREF(meta_path);
    return result;
}


PyMODINIT_FUNC PyInit__gdpy()
{
    MetaPathFinderType.tp_new = PyType_GenericNew;
    MetaPathFinderType.tp_methods = MetaPathFinder_method;
    
    OutputStreamType.tp_new = PyType_GenericNew;
    OutputStreamType.tp_methods = OutputStream_method;
    
    if (PyType_Ready(&OutputStreamType) < 0){ return 0; }
    if (PyType_Ready(&MetaPathFinderType) < 0){ return 0; }
    
    PyObject *module = PyModule_Create(&gdpy_module_def);
    if (!module){ return 0; }
    
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
    if (add_meta_path_finder() != 0)
    {
        Py_DECREF(module);
        return 0;
    }
    return module;
}
