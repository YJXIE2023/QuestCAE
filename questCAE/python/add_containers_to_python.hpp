#ifndef QUEST_ADD_CONTAINERS_TO_PYTHON_HPP
#define QUEST_ADD_CONTAINERS_TO_PYTHON_HPP

// 第三方头文件
#include <pybind11/pybind11.h>

namespace Quest::Python{

    void  AddContainersToPython(pybind11::module& m);

}

#endif //QUEST_ADD_CONTAINERS_TO_PYTHON_HPP