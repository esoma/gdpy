#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "gdpy_module.h"
#include "python_script.h"
#include "python_script_instance.h"
#include "python_script_language.h"
#include "python_ref.h"
#include "python_error.h"
#include "python_gil.h"

#include "core/io/file_access.h"

#include <iostream>

static PyObject *get_analyze_function(const char *function_name)
{
    auto analyze_module = PyImport_ImportModule("gdpy._analyze");
    if (!analyze_module){ return 0;}
    auto func = PyObject_GetAttrString(analyze_module, function_name);
    Py_DECREF(analyze_module);
    if (!func){ return 0; }
    return func;
}

Error PythonScript::import(bool reload)
{
    PythonGil python_gil;
    
    auto *code = PyUnicode_FromFormat(
        "from gdpy._import import import_script;"
        "module = import_script('%s', %s)",
        get_path().utf8().get_data(),
        reload ? "True" : "False"
    );
    if (!code)
    {
        Py_DECREF(code);
        ERR_FAIL_V_MSG(FAILED, "failed to create code object");
    }

    auto dict = PyDict_New();
    if (!dict)
    {
        Py_DECREF(code);
        Py_DECREF(dict);
        ERR_FAIL_V_MSG(FAILED, "failed to create PythonScript dict");
    }
    
    auto ret = PyRun_String(
        PyUnicode_AsUTF8(code),
        Py_file_input,
        dict,
        dict
    );
    Py_DECREF(code);
    if (!ret)
    {
        Py_DECREF(dict);
        auto exception = PyErr_GetRaisedException();
        PyErr_Clear();
        PyErr_DisplayException(exception);
        Py_DECREF(exception);
        ERR_FAIL_V_MSG(FAILED, "script failed to execute");
    }
    Py_DECREF(ret);
    
    auto module = PyDict_GetItemString(dict, "module");
    Py_DECREF(dict);
    if (!module)
    {
        ERR_FAIL_V_MSG(FAILED, "module not found");
    }
    auto module_name_py = PyObject_GetAttrString(module, "__name__");
    Py_DECREF(module);
    if (!module_name_py)
    {
        ERR_FAIL_V_MSG(FAILED, "module name not set");
    }
    
    module_name = PyUnicode_AsUTF8(module_name_py);
    Py_DECREF(module_name_py);

    return OK;
}

Error PythonScript::load(bool reload)
{
    Error err;
	Ref<FileAccess> f = FileAccess::open(get_path(), FileAccess::READ, &err);
    if (err){ return err; }
    
    Vector<uint8_t> sourcef;
    uint64_t len = f->get_length();
    sourcef.resize(len + 1);
	uint8_t *w = sourcef.ptrw();
    uint64_t r = f->get_buffer(w, len);
	ERR_FAIL_COND_V(r != len, ERR_CANT_OPEN);
    w[len] = 0;
    
    String s;
	if (s.parse_utf8((const char *)w) != OK) {
		ERR_FAIL_V_MSG(ERR_INVALID_DATA, "Script '" + get_path() + "' contains invalid unicode (UTF-8), so it was not loaded. Please ensure that scripts are saved in valid UTF-8 unicode.");
	}
    
    this->source = s;

    return import(reload);
}

bool PythonScript::can_instantiate() const
{
    return true;
}


Ref<Script> PythonScript::get_base_script() const
{
    return Ref<Script>();
}


StringName PythonScript::get_global_name() const
{
	return "<PythonScript::get_global_name>";
}


bool PythonScript::inherits_script(const Ref<Script> &p_script) const
{
    return false;
}


StringName PythonScript::get_instance_base_type() const
{
    return "<PythonScript::get_instance_base_type>";
}


ScriptInstance *PythonScript::instance_create(Object *p_this)
{
    std::cout << "<PythonScript::instance_create>" << std::endl;
    auto instance = memnew(PythonScriptInstance);
    instance->script = Ref<PythonScript>(this);
    {
        PythonGil python_gil;
        
        auto *code = PyUnicode_FromFormat(
            "from gdpy._import import import_script;"
            "module = import_script('%s', False);"
            "from gdpy._script import get_module_script;"
            "script = get_module_script(module.__name__);"
            "instance = script(_gdpy_variant=variant)",
            get_path().utf8().get_data()
        );
        if (!code)
        {
            PyErr_Clear();
            Py_DECREF(code);
            ERR_PRINT("failed to create code object");
            return 0;
        }
        
        auto dict = PyDict_New();
        if (!dict)
        {
            PyErr_Clear();
            Py_DECREF(code);
            Py_DECREF(dict);
            ERR_PRINT("failed to create PythonScript dict");
            return 0;
        }
        
        auto variant_wrapper = VariantWrapper_create(Variant(p_this));
        if (!variant_wrapper)
        {
            PyErr_Clear();
            Py_DECREF(code);
            Py_DECREF(dict);
            ERR_PRINT("failed to create object VariantWrapper");
            return 0;
        }
        if (PyDict_SetItemString(dict, "variant", variant_wrapper) != 0)
        {
            Py_DECREF(variant_wrapper);
            Py_DECREF(code);
            Py_DECREF(dict);
            ERR_PRINT("failed to add variant to script dict");
            return 0;
        }
        Py_DECREF(variant_wrapper);
        
        auto ret = PyRun_String(
            PyUnicode_AsUTF8(code),
            Py_file_input,
            dict,
            dict
        );
        Py_DECREF(code);
        if (!ret)
        {
            Py_DECREF(dict);
            auto exception = PyErr_GetRaisedException();
            PyErr_Clear();
            PyErr_DisplayException(exception);
            Py_DECREF(exception);
            ERR_PRINT("script failed to execute");
            return 0;
        }
        Py_DECREF(ret);
        
        auto py_instance = PyDict_GetItemString(dict, "instance");
        if (!py_instance)
        {
            PyErr_Clear();
            Py_DECREF(dict);
            ERR_PRINT("instance not found");
            return 0;
        }
        Py_INCREF(py_instance);
        Py_DECREF(dict);
        std::cout << "CREATE " << (void*)py_instance << std::endl;
        instance->py_instance = py_instance;
    }
    return instance;
}


bool PythonScript::instance_has(const Object *p_this) const
{
    return false;
}


bool PythonScript::has_source_code() const
{
    return true;
}


String PythonScript::get_source_code() const
{
	return source;
}


void PythonScript::set_source_code(const String &p_code)
{
    this->source = p_code;
}


Error PythonScript::reload(bool p_keep_state)
{
    return load(true);
}


#ifdef TOOLS_ENABLED
Vector<DocData::ClassDoc> PythonScript::get_documentation() const
{
    return Vector<DocData::ClassDoc>();
}


String PythonScript::get_class_icon_path() const
{
    return "<PythonScript::get_class_icon_path>";
}
#endif


bool PythonScript::has_method(const StringName &p_method) const
{
    std::cout << "<PythonScript::has_method>" << std::endl;
    return false;
}


MethodInfo PythonScript::get_method_info(const StringName &p_method) const
{
    std::cout << "<PythonScript::get_method_info>" << std::endl;
    return MethodInfo();
}    


bool PythonScript::is_tool() const
{
    return false;
}


bool PythonScript::is_valid() const
{
    return true;
}


bool PythonScript::is_abstract() const
{
    return false;
}


ScriptLanguage *PythonScript::get_language() const {
	return PythonScriptLanguage::get_singleton();
}


bool PythonScript::has_script_signal(const StringName &p_signal) const
{
    std::cout << "<PythonScript::has_script_signal>" << std::endl;
    return false;
}


void PythonScript::get_script_signal_list(List<MethodInfo> *r_list) const
{
    std::cout << "<PythonScript::get_script_signal_list>" << std::endl;
}


bool
PythonScript::get_property_default_value(
    const StringName &p_property,
    Variant &r_value
)
const
{
    PythonGil python_gil;
    
    PythonRef script_module(PyImport_ImportModule("gdpy._script"));
    if (!script_module){ REPORT_PYTHON_ERROR(); return false; }

    PythonRef get_property_default_value(PyObject_GetAttrString(
        script_module,
        "get_property_default_value"
    ));
    script_module.release();
    if (!get_property_default_value){ REPORT_PYTHON_ERROR(); return false; }

    PythonRef default_value(PyObject_CallFunction(
        get_property_default_value,
        "ss",
        module_name.c_str(),
        String(p_property).utf8().get_data()
    ));
    get_property_default_value.release();
    if (!default_value){ REPORT_PYTHON_ERROR(); return false; }
    if (default_value == Py_None){ return false; }

    auto variant = VariantWrapper_get_variant(default_value);
    if (!variant){ REPORT_PYTHON_ERROR(); return false; }    
    r_value = *variant;

    return true;
}


void PythonScript::get_script_method_list(List<MethodInfo> *r_list) const
{
    std::cout << "<PythonScript::get_script_method_list>" << std::endl;
}


void PythonScript::get_script_property_list(List<PropertyInfo> *r_list) const
{
    PythonGil python_gil;
    
    PythonRef script_module(PyImport_ImportModule("gdpy._script"));
    if (!script_module){ REPORT_PYTHON_ERROR(); return; }

    PythonRef get_properties(PyObject_GetAttrString(
        script_module,
        "get_properties"
    ));
    script_module.release();
    if (!get_properties){ REPORT_PYTHON_ERROR(); return; }
    
    PythonRef properties(PyObject_CallFunction(
        get_properties,
        "s",
        module_name.c_str()
    ));
    get_properties.release();
    if (!properties){ REPORT_PYTHON_ERROR(); return; }
    
    auto property_count = PyTuple_Size(properties);
    if (PyErr_Occurred()){ REPORT_PYTHON_ERROR(); return; }

    for (Py_ssize_t i = 0; i < property_count; i++)
    {
        auto py_property_info = PyTuple_GET_ITEM(properties, i);
        PropertyInfo property_info;
        {
            PythonRef py_type(PyObject_GetAttrString(py_property_info, "type"));
            if (!py_type){ REPORT_PYTHON_ERROR(); return; }
            
            property_info.type = (Variant::Type)PyLong_AsLong(py_type);
            if (PyErr_Occurred()){ REPORT_PYTHON_ERROR(); return; }
        }
        {
            PythonRef py_name(PyObject_GetAttrString(py_property_info, "name"));
            if (!py_name){ REPORT_PYTHON_ERROR(); return; }
            auto name = PyUnicode_AsUTF8(py_name);
            if (!name){ REPORT_PYTHON_ERROR(); return; }
            property_info.name = name;
        }
        {
            PythonRef py_class_name(PyObject_GetAttrString(py_property_info, "class_name"));
            if (!py_class_name){ REPORT_PYTHON_ERROR(); return; }
            auto class_name = PyUnicode_AsUTF8(py_class_name);
            if (!class_name){ REPORT_PYTHON_ERROR(); return; }
            property_info.class_name = class_name;
        }
        {
            PythonRef py_hint(PyObject_GetAttrString(py_property_info, "hint"));
            if (!py_hint){ REPORT_PYTHON_ERROR(); return; }
            property_info.hint = (PropertyHint)PyLong_AsLong(py_hint);
            if (PyErr_Occurred()){ REPORT_PYTHON_ERROR(); return; }
        }
        {
            PythonRef py_hint_string(PyObject_GetAttrString(py_property_info, "hint_string"));
            if (!py_hint_string){ REPORT_PYTHON_ERROR(); return; }
            auto hint_string = PyUnicode_AsUTF8(py_hint_string);
            if (!hint_string){ REPORT_PYTHON_ERROR(); return; }
            property_info.hint_string = hint_string;
        }
        {
            PythonRef py_usage(PyObject_GetAttrString(py_property_info, "usage"));
            if (!py_usage){ REPORT_PYTHON_ERROR(); return; }
            property_info.usage = PyLong_AsLong(py_usage);
            if (PyErr_Occurred()){ REPORT_PYTHON_ERROR(); return; }
        }
        r_list->push_back(property_info);
    }
    return;
}


const Variant PythonScript::get_rpc_config() const
{
    return Dictionary();
}


void PythonScript::_bind_methods()
{
}
