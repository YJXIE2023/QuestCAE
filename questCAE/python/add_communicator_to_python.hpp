#ifndef QUEST_ADD_COMMUNICATOR_TO_PYTHON_HPP
#define QUEST_ADD_COMMUNICATOR_TO_PYTHON_HPP

// 第三方库文件
#include <pybind11/pybind11.h>

namespace Quest::Python{

    void AddCommunicatorToPython(pybind11::module &m);

}

#endif //QUEST_ADD_COMMUNICATOR_TO_PYTHON_HPP