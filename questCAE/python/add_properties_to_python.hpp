#ifndef QUEST_ADD_PROPERTIES_TO_PYTHON_H
#define QUEST_ADD_PROPERTIES_TO_PYTHON_H

// 系统头文件
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace Quest::Python{

    void AddPropertiesToPython(pybind11::module& m);

}

#endif //QUEST_ADD_PROPERTIES_TO_PYTHON_H