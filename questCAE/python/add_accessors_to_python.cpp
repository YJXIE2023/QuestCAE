// 项目头文件
#include "python/add_accessors_to_python.hpp"
#include "includes/define_python.hpp"
#include "includes/accessor.hpp"
#include "includes/properties.hpp"

using AccessorBindType = std::unique_ptr<Kratos::Accessor>;

PYBIND11_MAKE_OPAQUE(AccessorBindType);

namespace Quest::Python{

    namespace py = pybind11;

    void AddAccessorsToPython(py::module& m){
        py::class_<AccessorBindType>(m, "Accessor")
            .def_static("Create", []{
                return std::make_unique<Accessor>();
            });
    }

}