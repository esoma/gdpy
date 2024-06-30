
#include "python_script.h"
#include "python_script_language.h"
#include "python_resource_format_saver.h"

Error
PythonResourceFormatSaver::save(
    const Ref<Resource> &p_resource,
    const String &p_path,
    uint32_t p_flags
)
{
	Ref<PythonScript> scr = p_resource;
	ERR_FAIL_COND_V(scr.is_null(), ERR_INVALID_PARAMETER);

	String source = scr->get_source_code();
	{
		Error err;
		Ref<FileAccess> file = FileAccess::open(p_path, FileAccess::WRITE, &err);

		ERR_FAIL_COND_V_MSG(err, err, "Cannot save PythonScript file '" + p_path + "'.");

		file->store_string(source);
		if (file->get_error() != OK && file->get_error() != ERR_FILE_EOF) {
			return ERR_CANT_CREATE;
		}
	}

	if (ScriptServer::is_reload_scripts_on_save_enabled()) {
		PythonScriptLanguage::get_singleton()->reload_tool_script(p_resource, true);
	}

	return OK;
}

void
PythonResourceFormatSaver::get_recognized_extensions(
    const Ref<Resource> &p_resource,
    List<String> *p_extensions
) const
{
	if (Object::cast_to<PythonScript>(*p_resource)) {
		p_extensions->push_back("py");
	}
}

bool
PythonResourceFormatSaver::recognize(
    const Ref<Resource> &p_resource
)
const
{
	return Object::cast_to<PythonScript>(*p_resource) != nullptr;
}