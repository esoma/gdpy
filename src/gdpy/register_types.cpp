
#include "register_types.h"

#include "python_resource_format_loader.h"
#include "python_resource_format_saver.h"
#include "python_script.h"
#include "python_script_language.h"

#include <iostream>

Ref<PythonResourceFormatLoader> python_resource_format_loader;
Ref<PythonResourceFormatSaver> python_resource_format_saver;

void initialize_gdpy_module(ModuleInitializationLevel p_level) {
    std::cout << "gdpy" << std::endl;
    
    if (p_level == MODULE_INITIALIZATION_LEVEL_SERVERS)
    {
        GDREGISTER_CLASS(PythonScript);
		auto python_script_language = memnew(PythonScriptLanguage);
		ScriptServer::register_language(python_script_language);
        
        python_resource_format_loader.instantiate();
		ResourceLoader::add_resource_format_loader(python_resource_format_loader);
        
        python_resource_format_saver.instantiate();
		ResourceSaver::add_resource_format_saver(python_resource_format_saver);
    }
    /*

#ifdef TOOLS_ENABLED
	if (p_level == MODULE_INITIALIZATION_LEVEL_SERVERS) {
		EditorNode::add_init_callback(_editor_init);

		gdscript_translation_parser_plugin.instantiate();
		EditorTranslationParser::get_singleton()->add_parser(gdscript_translation_parser_plugin, EditorTranslationParser::STANDARD);
	}
#endif // TOOLS_ENABLED
    */
}

void uninitialize_gdpy_module(ModuleInitializationLevel p_level) {
    /*
	if (p_level == MODULE_INITIALIZATION_LEVEL_SERVERS) {
		ScriptServer::unregister_language(script_language_gd);

		if (gdscript_cache) {
			memdelete(gdscript_cache);
		}

		if (script_language_gd) {
			memdelete(script_language_gd);
		}

		ResourceLoader::remove_resource_format_loader(resource_loader_gd);
		resource_loader_gd.unref();

		ResourceSaver::remove_resource_format_saver(resource_saver_gd);
		resource_saver_gd.unref();

		GDScriptParser::cleanup();
		GDScriptUtilityFunctions::unregister_functions();
	}

#ifdef TOOLS_ENABLED
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
		EditorTranslationParser::get_singleton()->remove_parser(gdscript_translation_parser_plugin, EditorTranslationParser::STANDARD);
		gdscript_translation_parser_plugin.unref();
	}
#endif // TOOLS_ENABLED
    */
}
