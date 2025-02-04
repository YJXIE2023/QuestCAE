#ifndef QUEST_ADD_IO_TO_PYTHON_HPP
#define QUEST_ADD_IO_TO_PYTHON_HPP

// 系统头文件
#include <pybind11/pybind11.h>

namespace Quest::Python{

    void AddIOToPython(pybind11::module& m);

}

#endif //QUEST_ADD_IO_TO_PYTHON_HPP