#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "register_types.h"

#include "gdpy_module.h"
#include "python_resource_format_loader.h"
#include "python_resource_format_saver.h"
#include "python_script.h"
#include "python_script_language.h"

#include <iostream>
#include <windows.h>

PythonScriptLanguage *python_script_language = 0;
Ref<PythonResourceFormatLoader> python_resource_format_loader;
Ref<PythonResourceFormatSaver> python_resource_format_saver;

void initialize_gdpy_module(ModuleInitializationLevel p_level) {
    if (p_level == MODULE_INITIALIZATION_LEVEL_SERVERS)
    {
        PyImport_AppendInittab("_gdpy", &PyInit__gdpy);
        Py_Initialize();
        {
            auto gdpy_module = PyImport_ImportModule("_gdpy");
            if (!gdpy_module)
            {
                std::cerr << "Failed to load _gdpy." << std::endl;
                Py_FinalizeEx();
                return;
            }
            Py_DECREF(gdpy_module);
        }
        
        GDREGISTER_CLASS(PythonScript);
		python_script_language = memnew(PythonScriptLanguage);
		ScriptServer::register_language(python_script_language);
        
        python_resource_format_loader.instantiate();
		ResourceLoader::add_resource_format_loader(python_resource_format_loader);
        
        python_resource_format_saver.instantiate();
		ResourceSaver::add_resource_format_saver(python_resource_format_saver);
    }
}

void uninitialize_gdpy_module(ModuleInitializationLevel p_level) {
    if (p_level == MODULE_INITIALIZATION_LEVEL_SERVERS)
    {
        if (python_script_language)
        {
            ScriptServer::unregister_language(python_script_language);
            memdelete(python_script_language);
            python_script_language = 0;
        }
        
        ResourceLoader::remove_resource_format_loader(python_resource_format_loader);
		python_resource_format_loader.unref();

		ResourceSaver::remove_resource_format_saver(python_resource_format_saver);
		python_resource_format_saver.unref();
        
        Py_FinalizeEx();
    }
}
