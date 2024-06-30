#ifndef PYTHON_SCRIPT_LANGUAGE_H
#define PYTHON_SCRIPT_LANGUAGE_H

#include "core/object/script_language.h"

class PythonScriptLanguage : public ScriptLanguage
{
public:
    PythonScriptLanguage();
    ~PythonScriptLanguage();

private:
    static PythonScriptLanguage *singleton;

public:
    static PythonScriptLanguage *get_singleton();

    virtual void init() override;
    virtual void finish() override;
    
    virtual String get_type() const override;
    virtual String get_name() const override;
    virtual String get_extension() const override;
    
    virtual void get_reserved_words(List<String> *p_words) const override;
    virtual bool is_control_flow_keyword(String p_keywords) const override;
    virtual void get_comment_delimiters(List<String> *p_delimiters) const override;
    virtual void get_doc_comment_delimiters(List<String> *p_delimiters) const override;
	virtual void get_string_delimiters(List<String> *p_delimiters) const override;
    virtual bool validate(const String &p_script, const String &p_path = "", List<String> *r_functions = nullptr, List<ScriptLanguage::ScriptError> *r_errors = nullptr, List<ScriptLanguage::Warning> *r_warnings = nullptr, HashSet<int> *r_safe_lines = nullptr) const override;
    virtual Script *create_script() const override;
    virtual bool has_named_classes() const override;
    virtual bool supports_builtin_mode() const override;
    virtual int find_function(const String &p_function, const String &p_code) const override;
    virtual String make_function(const String &p_class, const String &p_name, const PackedStringArray &p_args) const override;
    virtual void auto_indent_code(String &p_code, int p_from_line, int p_to_line) const override;
    virtual void add_global_constant(const StringName &p_variable, const Variant &p_value) override;

    virtual Ref<Script> make_template(const String &p_template, const String &p_class_name, const String &p_base_class_name) const override;

    virtual String debug_get_error() const override;
    virtual int debug_get_stack_level_count() const override;
    virtual int debug_get_stack_level_line(int p_level) const override;
    virtual String debug_get_stack_level_function(int p_level) const override;
    virtual String debug_get_stack_level_source(int p_level) const override;
    virtual void debug_get_stack_level_locals(int p_level, List<String> *p_locals, List<Variant> *p_values, int p_max_subitems = -1, int p_max_depth = -1) override;
    virtual void debug_get_stack_level_members(int p_level, List<String> *p_members, List<Variant> *p_values, int p_max_subitems = -1, int p_max_depth = -1) override;
    virtual void debug_get_globals(List<String> *p_globals, List<Variant> *p_values, int p_max_subitems = -1, int p_max_depth = -1) override;
    virtual String debug_parse_stack_level_expression(int p_level, const String &p_expression, int p_max_subitems = -1, int p_max_depth = -1) override;

    virtual void reload_all_scripts() override;
    virtual void reload_tool_script(const Ref<Script> &p_script, bool p_soft_reload) override;

    virtual void get_recognized_extensions(List<String> *p_extensions) const override;

    virtual void get_public_functions(List<MethodInfo> *p_functions) const override;
	virtual void get_public_constants(List<Pair<String, Variant>> *p_constants) const override;
	virtual void get_public_annotations(List<MethodInfo> *p_annotations) const override;

    virtual void profiling_start() override;
	virtual void profiling_stop() override;
    virtual int profiling_get_accumulated_data(ProfilingInfo *p_info_arr, int p_info_max) override;
	virtual int profiling_get_frame_data(ProfilingInfo *p_info_arr, int p_info_max) override;
};

#endif