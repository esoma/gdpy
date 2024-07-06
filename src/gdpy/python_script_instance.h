#ifndef PYTHON_SCRIPT_INSTANCE_H
#define PYTHON_SCRIPT_INSTANCE_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "core/object/script_language.h"

#include "python_script.h"

class PythonScriptInstance : public ScriptInstance
{
private:
    friend PythonScript;
    Ref<PythonScript> script;
    PyObject *py_instance;
    Object *object_instance;
public:
    virtual bool set(const StringName &p_name, const Variant &p_value);
    virtual bool get(const StringName &p_name, Variant &r_ret) const;
    
    virtual void get_property_list(List<PropertyInfo> *p_properties) const;
    virtual Variant::Type get_property_type(const StringName &p_name, bool *r_is_valid = nullptr) const;
    virtual void validate_property(PropertyInfo &p_property) const;
    virtual bool property_can_revert(const StringName &p_name) const;
    virtual bool property_get_revert(const StringName &p_name, Variant &r_ret) const;
    
    virtual void get_method_list(List<MethodInfo> *p_list) const;
    virtual bool has_method(const StringName &p_method) const;
	virtual Variant callp(const StringName &p_method, const Variant **p_args, int p_argcount, Callable::CallError &r_error);

    virtual void notification(int p_notification, bool p_reversed = false);
    
    virtual Ref<Script> get_script() const;
    
    virtual ScriptLanguage *get_language();
    
    PythonScriptInstance();
    ~PythonScriptInstance();
};

#endif 