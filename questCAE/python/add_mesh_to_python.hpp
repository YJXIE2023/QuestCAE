#ifndef QUEST_ADD_MESH_TO_PYTHON_HPP
#define QUEST_ADD_MESH_TO_PYTHON_HPP

// 系统头文件
#include <pybind11/pybind11.h>

namespace Quest{

    void AddNodeToPython(pybind11::module& m);
    void AddMeshToPython(pybind11::module& m);

}

#endif //QUEST_ADD_MESH_TO_PYTHON_HPP