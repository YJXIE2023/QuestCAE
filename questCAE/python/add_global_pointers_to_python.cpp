#include "python/add_global_pointers_to_python.hpp"
#include "container/global_pointers_vector.hpp"
#include "includes/define_python.hpp"
#include "includes/node.hpp"
#include "includes/element.hpp"
#include "includes/condition.hpp"

namespace Quest::Python {

    void AddGlobalPointersToPython(pybind11::module& m){
        namespace py = pybind11;

        py::class_<GlobalPointer<Node>>(m,"GlobalNodePointer");
        py::class_<GlobalPointer<Element>>(m,"GlobalElementPointer");    
        py::class_<GlobalPointer<Condition>>(m,"GlobalConditionPointer");

        py::class_<GlobalPointersVector<Node>>(m,"GlobalNodePointersVector");
        py::class_<GlobalPointersVector<Element>>(m,"GlobalElementPointersVector");
        py::class_<GlobalPointersVector<Condition>>(m,"GlobalConditionPointersVector");
    }

}