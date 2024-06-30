
#include "python_script.h"
#include "python_resource_format_loader.h"

Ref<Resource>
PythonResourceFormatLoader::load(
    const String &p_path,
    const String &p_original_path,
    Error *r_error,
    bool p_use_sub_threads,
    float *r_progress,
    CacheMode p_cache_mode
)
{
    Ref<PythonScript> scr;
    scr.instantiate();
    scr->set_path(p_path, false);
    *r_error = scr->load();
    return scr;
}

void
PythonResourceFormatLoader::get_recognized_extensions(
    List<String> *p_extensions
)
const
{
	p_extensions->push_back("py");
}

bool
PythonResourceFormatLoader::handles_type(
    const String &p_type
)
const
{
	return (p_type == "Script" || p_type == "PythonScript");
}

String
PythonResourceFormatLoader::get_resource_type(
    const String &p_path
)
const
{
	String el = p_path.get_extension().to_lower();
	if (el == "py") {
		return "PythonScript";
	}
	return "";
}