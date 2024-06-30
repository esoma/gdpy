
#include "python_script.h"
#include "python_script_language.h"

Error PythonScript::load()
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
    
    return OK;
}

bool PythonScript::can_instantiate() const
{
    return false;
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
    return 0; // PythonScriptInstance
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
}


Error PythonScript::reload(bool p_keep_state)
{
    return OK;
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
    return false;
}


MethodInfo PythonScript::get_method_info(const StringName &p_method) const
{
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
    return false;
}


void PythonScript::get_script_signal_list(List<MethodInfo> *r_list) const
{
}


bool
PythonScript::get_property_default_value(
    const StringName &p_property,
    Variant &r_value
)
const
{
    return false;
}


void PythonScript::get_script_method_list(List<MethodInfo> *r_list) const
{
}


void PythonScript::get_script_property_list(List<PropertyInfo> *r_list) const
{
}


const Variant PythonScript::get_rpc_config() const
{
    return Dictionary();
}


void PythonScript::_bind_methods()
{
}
