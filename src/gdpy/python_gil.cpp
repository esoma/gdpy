
#include "python_gil.h"
#include <iostream>
#include <thread>

PythonGil::PythonGil()
{
    std::cout << "ACQUIRE GIL: " << std::hash<std::thread::id>{}(std::this_thread::get_id()) << std::endl;
    gstate = PyGILState_Ensure();
}

PythonGil::~PythonGil()
{
    PyGILState_Release(gstate);
    std::cout << "RELEASE GIL: " << std::hash<std::thread::id>{}(std::this_thread::get_id()) << std::endl;
}
