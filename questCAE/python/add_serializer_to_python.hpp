#ifndef QUEST_ADD_SERIALIZER_TO_PYTHON_HPP
#define QUEST_ADD_SERIALIZER_TO_PYTHON_HPP

// 系统头文件
#include <pybind11/pybind11.h>

namespace Quest::Python{

    void AddSerializerToPython(pybind11::module& m);

}

#endif //QUEST_ADD_SERIALIZER_TO_PYTHON_HPP