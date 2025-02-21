#include "python/add_globals_to_python.h"
#include "includes/define_python.hpp"
#include "includes/global_variables.hpp"

namespace Quest::Python{

    void AddGlobalsToPython(pybind11::module& m){
        namespace py = pybind11;
        using DataLocation = Quest::Globals::DataLocation;

        auto m_globals = m.def_submodule("Globals");

        py::enum_<DataLocation>(m_globals, "DataLocation")
            .value("NodeHistorical", DataLocation::NodeHistorical)
            .value("NodeNonHistorical", DataLocation::NodeNonHistorical)
            .value("Element", DataLocation::Element)
            .value("Condition", DataLocation::Condition)
            .value("ProcessInfo", DataLocation::ProcessInfo)
            .value("ModelPart", DataLocation::ModelPart);
    }

}