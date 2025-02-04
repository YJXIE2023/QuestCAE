#ifndef QUEST_ADD_GEOMETRIES_TO_PYTHON_HPP
#define QUEST_ADD_GEOMETRIES_TO_PYTHON_HPP

#include <pybind11/pybind11.h>

namespace Quest::Python{
    
    void AddGeometriesToPython(pybind11::module& m);

}

#endif //QUEST_ADD_GEOMETRIES_TO_PYTHON_HPP