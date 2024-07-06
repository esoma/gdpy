
#include "python_gil.h"
#include <iostream>
#include <thread>

PythonGil::PythonGil()
{
    gstate = PyGILState_Ensure();
    held = true;
}

PythonGil::~PythonGil()
{
    release();
}

void PythonGil::release()
{
    if (held)
    {
        PyGILState_Release(gstate);
        held = false;
    }
}
