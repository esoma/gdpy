
#include "gdpy_module.h"
#include "python_gil.h"
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
    std::cout << "PythonScriptInstance::set" << std::endl;
    return false;
}


bool
PythonScriptInstance::get(
    const StringName &p_name,
    Variant &r_ret
)
const
{
    PythonGil python_gil;
    
    PythonRef code(PyUnicode_FromFormat(
        "from gdpy._script import get_property_value;"
        "variant = get_property_value(instance, '%s')",
        String(p_name).utf8().get_data()
    ));
    if (!code.ref)
    {
        PyErr_Clear();
        ERR_PRINT("failed to create code object");
        return false;
    }

    PythonRef dict(PyDict_New());
    if (!dict.ref)
    {
        PyErr_Clear();
        ERR_PRINT("failed to create PythonScript dict");
        return false;
    }

    if (PyDict_SetItemString(dict.ref, "instance", py_instance) != 0)
    {
        PyErr_Clear();
        ERR_PRINT("failed to add instance to script dict");
        return false;
    }
    
    PythonRef ret(PyRun_String(
        PyUnicode_AsUTF8(code.ref),
        Py_file_input,
        dict.ref,
        dict.ref
    ));
    code.release();
    if (!ret.ref)
    {
        PythonRef exception(PyErr_GetRaisedException());
        PyErr_Clear();
        PyErr_DisplayException(exception.ref);
        ERR_PRINT("script failed to execute");
        return false;
    }
    
    PythonRef py_variant(PyDict_GetItemString(dict.ref, "variant"), false);
    dict.release();
    if (!py_variant.ref)
    {
        PyErr_Clear();
        ERR_PRINT("variant not found");
        return false;
    }
    
    auto variant = VariantWrapper_get_variant(py_variant.ref);
    if (!variant)
    {
        PythonRef exception(PyErr_GetRaisedException());
        PyErr_Clear();
        PyErr_DisplayException(exception.ref);
        ERR_PRINT("failed to get variant");
        return false;
    }    
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
    std::cout << "PythonScriptInstance::property_can_revert" << std::endl;
    return false;
}


bool PythonScriptInstance::property_get_revert(
    const StringName &p_name,
    Variant &r_ret
)
const
{
    std::cout << "PythonScriptInstance::property_get_revert" << std::endl;
    return false;
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
