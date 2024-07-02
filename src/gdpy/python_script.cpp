#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "python_script.h"
#include "python_script_instance.h"
#include "python_script_language.h"

#include "core/io/file_access.h"

#include <iostream>

Error PythonScript::import()
{
    std::cout << get_path().utf8().get_data() << std::endl;
    
    auto create_file_access = ClassDB::get_method("FileAccess", "open");
    std::cout << "???" << create_file_access << std::endl;
    Variant path("test");
    Variant mode(FileAccess::ACCESS_RESOURCES);
    const Variant *args[] = {
        &path,
        &mode
    };
    Callable::CallError error;
    auto res = create_file_access->call(0, args, 2, error);
    std::cout << "||||||" << res.stringify().utf8().get_data() << std::endl;
    
    auto file_access = FileAccess::create(FileAccess::ACCESS_RESOURCES);
    auto gdret = file_access->callp("is_open", 0, 0, error);
    std::cout << "||||||" << gdret.stringify().utf8().get_data() << std::endl;
    /*
    auto module = PyImport_ImportModule(get_path().utf8().get_data());
    if (!module)
    {
        PyErr_Clear();
        ERR_FAIL_V_MSG(FAILED, "script failed to execute");
    }
    */

    auto chr_source = source.utf8();
    
    auto dict = PyDict_New();
    if (!dict)
    {
        Py_DECREF(dict);
        ERR_FAIL_V_MSG(FAILED, "failed to create PythonScript dict");
    }
    auto ret = PyRun_String(
        chr_source.get_data(),
        Py_file_input,
        dict,
        dict
    );
    if (!ret)
    {
        PyErr_Clear();
        Py_DECREF(dict);
        ERR_FAIL_V_MSG(FAILED, "script failed to execute");
    }
    Py_DECREF(ret);
    
    std::cout << "dict: " << PyUnicode_AsUTF8(PyObject_Str(dict)) << std::endl;
    Py_DECREF(dict);

    return OK;
}

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

    return import();
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
    return memnew(PythonScriptInstance);
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
    std::cout << "<PythonScript::reload>" << std::endl;
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
    std::cout << "<PythonScript::get_property_default_value>" << std::endl;
    return false;
}


void PythonScript::get_script_method_list(List<MethodInfo> *r_list) const
{
    std::cout << "<PythonScript::get_script_method_list>" << std::endl;
}


void PythonScript::get_script_property_list(List<PropertyInfo> *r_list) const
{
    std::cout << "<PythonScript::get_script_property_list>" << std::endl;
}


const Variant PythonScript::get_rpc_config() const
{
    return Dictionary();
}


void PythonScript::_bind_methods()
{
}
