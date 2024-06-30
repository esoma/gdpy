
#include "python_script.h"
#include "python_script_language.h"

#include <iostream>

PythonScriptLanguage *PythonScriptLanguage::singleton = 0;

PythonScriptLanguage::PythonScriptLanguage()
{
    ERR_FAIL_COND(singleton);
    singleton = this;
}

PythonScriptLanguage::~PythonScriptLanguage()
{
}

PythonScriptLanguage *PythonScriptLanguage::get_singleton()
{
    return singleton;
}

void PythonScriptLanguage::init()
{
}


void PythonScriptLanguage::finish()
{
}


String PythonScriptLanguage::get_name() const
{
	return "Python";
}


String PythonScriptLanguage::get_type() const
{
    return "Python";
}


String PythonScriptLanguage::get_extension() const
{
    return "py";
}


void PythonScriptLanguage::get_reserved_words(List<String> *p_words) const
{
}


bool PythonScriptLanguage::is_control_flow_keyword(String p_keyword) const
{
    return false;
}


void PythonScriptLanguage::get_comment_delimiters(List<String> *p_delimiters) const
{
}


void PythonScriptLanguage::get_doc_comment_delimiters(List<String> *p_delimiters) const
{
}


void PythonScriptLanguage::get_string_delimiters(List<String> *p_delimiters) const
{
}


bool
PythonScriptLanguage::validate(
    const String &p_script,
    const String &p_path, List<String> *r_functions,
    List<ScriptLanguage::ScriptError> *r_errors,
    List<ScriptLanguage::Warning> *r_warnings,
    HashSet<int> *r_safe_lines
)
const
{
    return true;
}


Script *PythonScriptLanguage::create_script() const
{
	return memnew(PythonScript);
}


bool PythonScriptLanguage::has_named_classes() const
{
    return false;
}


bool PythonScriptLanguage::supports_builtin_mode() const {
	return false;
}


int
PythonScriptLanguage::find_function(
    const String &p_function,
    const String &p_code
)
const
{
    return -1;
}


String
PythonScriptLanguage::make_function(
    const String &p_class,
    const String &p_name,
    const PackedStringArray &p_args
)
const
{
    return "<PythonScriptLanguage::make_function>";
}


void
PythonScriptLanguage::auto_indent_code(
    String &p_code,
    int p_from_line,
    int p_to_line
)
const 
{
}


void
PythonScriptLanguage::add_global_constant(
    const StringName &p_variable,
    const Variant &p_value
)
{
}


Ref<Script>
PythonScriptLanguage::make_template(
    const String &p_template,
    const String &p_class_name,
    const String &p_base_class_name
)
const
{
    Ref<PythonScript> scr;
    scr.instantiate();
    return scr;
}


String PythonScriptLanguage::debug_get_error() const
{
	return "<PythonScriptLanguage::debug_get_error>";
}


int PythonScriptLanguage::debug_get_stack_level_count() const
{
    return 1;
}


int PythonScriptLanguage::debug_get_stack_level_line(int p_level) const 
{
    return 0;
}


String PythonScriptLanguage::debug_get_stack_level_function(int p_level) const 
{
    return "<PythonScriptLanguage::debug_get_stack_level_function>";
}


String PythonScriptLanguage::debug_get_stack_level_source(int p_level) const
{
    return "<PythonScriptLanguage::debug_get_stack_level_source>";
}


void
PythonScriptLanguage::debug_get_stack_level_locals(
    int p_level,
    List<String> *p_locals,
    List<Variant> *p_values,
    int p_max_subitems,
    int p_max_depth
)
{
}


void
PythonScriptLanguage::debug_get_stack_level_members(
    int p_level,
    List<String> *p_members,
    List<Variant> *p_values,
    int p_max_subitems,
    int p_max_depth
)
{
}


void
PythonScriptLanguage::debug_get_globals(
    List<String> *p_globals,
    List<Variant> *p_values,
    int p_max_subitems,
    int p_max_depth
)
{
}


String
PythonScriptLanguage::debug_parse_stack_level_expression(
    int p_level,
    const String &p_expression,
    int p_max_subitems,
    int p_max_depth
)
{
	return "<PythonScriptLanguage::debug_parse_stack_level_expression>";
}


void PythonScriptLanguage::reload_all_scripts()
{
}


void
PythonScriptLanguage::reload_tool_script(
    const Ref<Script> &p_script,
    bool p_soft_reload
)
{
}


void
PythonScriptLanguage::get_recognized_extensions(List<String> *p_extensions)
const
{
	p_extensions->push_back("py");
}


void
PythonScriptLanguage::get_public_functions(List<MethodInfo> *p_functions)
const
{
}


void PythonScriptLanguage::get_public_constants(
    List<Pair<String, Variant>> *p_constants
)
const
{
}


void PythonScriptLanguage::get_public_annotations(
    List<MethodInfo> *p_annotations
)
const
{
}


void PythonScriptLanguage::profiling_start()
{
}


void PythonScriptLanguage::profiling_stop()
{
}


int
PythonScriptLanguage::profiling_get_accumulated_data(
    ProfilingInfo *p_info_arr,
    int p_info_max
)
{
    return 0;
}    


int
PythonScriptLanguage::profiling_get_frame_data(
    ProfilingInfo *p_info_arr,
    int p_info_max
)
{
    return 0;
}    
