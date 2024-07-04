#ifndef PYTHON_SCRIPT_H
#define PYTHON_SCRIPT_H

#include "core/object/script_language.h"

class PythonScript : public Script
{
    GDCLASS(PythonScript, Script);
    
    String source;
    
    Error import(bool reload);
public:
    Error load(bool reload);

    virtual bool can_instantiate() const override;
    
    virtual Ref<Script> get_base_script() const override;
    virtual StringName get_global_name() const override;
    bool inherits_script(const Ref<Script> &p_script) const override;
    
    virtual StringName get_instance_base_type() const override;
    virtual ScriptInstance *instance_create(Object *p_this) override;
    virtual bool instance_has(const Object *p_this) const override;

    virtual bool has_source_code() const override;
    virtual String get_source_code() const override;
    virtual void set_source_code(const String &p_code) override;
    virtual Error reload(bool p_keep_state = false) override;
    
#ifdef TOOLS_ENABLED
	virtual Vector<DocData::ClassDoc> get_documentation() const override;
    virtual String get_class_icon_path() const override;
#endif
    
    virtual bool has_method(const StringName &p_method) const override;
    virtual MethodInfo get_method_info(const StringName &p_method) const override;
    
    bool is_tool() const override;
    virtual bool is_valid() const override;
    virtual bool is_abstract() const override;
    
    virtual ScriptLanguage *get_language() const override;

    virtual bool has_script_signal(const StringName &p_signal) const override;
	virtual void get_script_signal_list(List<MethodInfo> *r_signals) const override;
    
    bool get_property_default_value(const StringName &p_property, Variant &r_value) const override;
    
    virtual void get_script_method_list(List<MethodInfo> *p_list) const override;
    
    virtual void get_script_property_list(List<PropertyInfo> *p_list) const override;
    
    virtual const Variant get_rpc_config() const override;
    
protected:
    static void _bind_methods();
};

#endif 