
#include "gdpy_module.h"
#include "python_gil.h"
#include "python_error.h"
#include "python_script_instance.h"
#include "python_script_language.h"
#include "python_ref.h"
#include <iostream>

bool
PythonScriptInstance::set(
    const StringName &p_name,
    const Variant &p_value
)
{
    {
        PythonGil python_gil;
        
        PythonRef script_module(PyImport_ImportModule("gdpy._script"));
        if (!script_module){ REPORT_PYTHON_ERROR(); return false; }
        
        PythonRef set_property_value(PyObject_GetAttrString(
            script_module,
            "set_property_value"
        ));
        script_module.release();
        if (!set_property_value){ REPORT_PYTHON_ERROR(); return false; }
        
        PythonRef variant_wrapper(VariantWrapper_create(p_value));
        if (!variant_wrapper){ REPORT_PYTHON_ERROR(); return false; }
        
        PythonRef success(PyObject_CallFunction(
            set_property_value,
            "OsO",
            py_instance,
            String(p_name).utf8().get_data(),
            (PyObject *)variant_wrapper
        ));
        if (!success){ REPORT_PYTHON_ERROR(); return false; }
        
        if (success == Py_True){ return true; }
    }
    return ClassDB::set_property(object_instance, p_name, p_value);
}


bool
PythonScriptInstance::get(
    const StringName &p_name,
    Variant &r_ret
)
const
{
    PythonGil python_gil;
    
    PythonRef script_module(PyImport_ImportModule("gdpy._script"));
    if (!script_module){ REPORT_PYTHON_ERROR(); return false; }

    PythonRef get_property_value(PyObject_GetAttrString(
        script_module,
        "get_property_value"
    ));
    script_module.release();
    if (!get_property_value){ REPORT_PYTHON_ERROR(); return false; }
    
    PythonRef variant_wrapper(PyObject_CallFunction(
        get_property_value,
        "Os",
        py_instance,
        String(p_name).utf8().get_data()
    ));
    get_property_value.release();
    if (!variant_wrapper){ REPORT_PYTHON_ERROR(); return false; }
    if (variant_wrapper == Py_None)
    {
        python_gil.release();
        return ClassDB::get_property(object_instance, p_name, r_ret);
    }
    
    auto variant = VariantWrapper_get_variant(variant_wrapper);
    if (!variant){ REPORT_PYTHON_ERROR(); return false; }
    r_ret = *variant;
    
    return true;
}


void
PythonScriptInstance::get_property_list(
    List<PropertyInfo> *p_properties
)
const
{
    script->get_script_property_list(p_properties);
}


Variant::Type
PythonScriptInstance::get_property_type(
    const StringName &p_name,
    bool *r_is_valid
)
const
{
    *r_is_valid = false;
    std::cout << "PythonScriptInstance::get_property_type" << std::endl;
    return Variant::NIL;
}


void
PythonScriptInstance::validate_property(
    PropertyInfo &p_property
)
const
{
    //std::cout << "PythonScriptInstance::validate_property" << std::endl;
}


bool PythonScriptInstance::property_can_revert(const StringName &p_name) const
{
    PythonGil python_gil;
    
    PythonRef script_module(PyImport_ImportModule("gdpy._script"));
    if (!script_module){ REPORT_PYTHON_ERROR(); return false; }

    PythonRef get_property_can_revert(PyObject_GetAttrString(
        script_module,
        "get_property_can_revert"
    ));
    script_module.release();
    if (!get_property_can_revert){ REPORT_PYTHON_ERROR(); return false; }
    
    PythonRef result(PyObject_CallFunction(
        get_property_can_revert,
        "Os",
        py_instance,
        String(p_name).utf8().get_data()
    ));
    
    return result == Py_True;
}


bool PythonScriptInstance::property_get_revert(
    const StringName &p_name,
    Variant &r_ret
)
const
{
    return script->get_property_default_value(p_name, r_ret);
}


void PythonScriptInstance::get_method_list(List<MethodInfo> *p_list) const
{
    std::cout << "PythonScriptInstance::get_method_list" << std::endl;
}


bool PythonScriptInstance::has_method(const StringName &p_method) const
{
    std::cout << "PythonScriptInstance::has_method" << std::endl;
    return false;
}


Variant
PythonScriptInstance::callp(
    const StringName &p_method,
    const Variant **p_args,
    int p_argcount,
    Callable::CallError &r_error
)
{
    r_error.error = Callable::CallError::CALL_ERROR_INVALID_METHOD;
    return Variant();
}


void PythonScriptInstance::notification(int p_notification, bool p_reversed)
{
    //std::cout << "PythonScriptInstance::notification" << std::endl;
}


Ref<Script> PythonScriptInstance::get_script() const
{
	return script;
}


ScriptLanguage *PythonScriptInstance::get_language()
{
	return PythonScriptLanguage::get_singleton();
}


PythonScriptInstance::PythonScriptInstance()
{
}

PythonScriptInstance::~PythonScriptInstance()
{
    if (py_instance)
    {
        PythonGil python_gil;
        Py_DECREF(py_instance);
        py_instance = 0;
    }
}
