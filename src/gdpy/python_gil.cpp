
#include "python_gil.h"
#include <iostream>
#include <thread>

PythonGil::PythonGil()
{
    gstate = PyGILState_Ensure();
}

PythonGil::~PythonGil()
{
    PyGILState_Release(gstate);
}
