
#include "python_script_instance.h"
#include "python_script_language.h"
#include <iostream>

bool
PythonScriptInstance::set(
    const StringName &p_name,
    const Variant &p_value
)
{
    return false;
}


bool
PythonScriptInstance::get(
    const StringName &p_name,
    Variant &r_ret
)
const
{
    return false;
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
    return Variant::NIL;
}


void
PythonScriptInstance::validate_property(
    PropertyInfo &p_property
)
const
{
}


bool PythonScriptInstance::property_can_revert(const StringName &p_name) const
{
    return false;
}


bool PythonScriptInstance::property_get_revert(
    const StringName &p_name,
    Variant &r_ret
)
const
{
    return false;
}


void PythonScriptInstance::get_method_list(List<MethodInfo> *p_list) const
{
}


bool PythonScriptInstance::has_method(const StringName &p_method) const
{
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
}


Ref<Script> PythonScriptInstance::get_script() const
{
	return script;
}


ScriptLanguage *PythonScriptInstance::get_language()
{
	return PythonScriptLanguage::get_singleton();
}
