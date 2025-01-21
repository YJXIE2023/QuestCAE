#ifndef QUEST_ADD_CONSTRAINT_TO_PYTHON_HPP
#define QUEST_ADD_CONSTRAINT_TO_PYTHON_HPP

// 第三方库文件
#include <pybind11/pybind11.h>

namespace Quest::Python{

    void AddConstraintToPython(pybind11::module& m);

}

#endif //QUEST_ADD_CONSTRAINT_TO_PYTHON_HPP