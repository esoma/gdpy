#ifndef PYTHON_RESOURCE_FORMAT_SAVER_H
#define PYTHON_RESOURCE_FORMAT_SAVER_H

#include "core/io/resource_saver.h"

class PythonResourceFormatSaver : public ResourceFormatSaver
{
public:
    virtual Error save(const Ref<Resource> &p_resource, const String &p_path, uint32_t p_flags = 0);
	virtual void get_recognized_extensions(const Ref<Resource> &p_resource, List<String> *p_extensions) const;
	virtual bool recognize(const Ref<Resource> &p_resource) const;
};

#endif