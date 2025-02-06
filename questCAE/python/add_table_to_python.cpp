#include "includes/define_python.hpp"
#include "includes/table.hpp"
#include "python/add_table_to_python.hpp"

namespace Quest{
    namespace py = pybind11;

    using DoubleTableType = Table<double, double>;

    double TableGetNearestValue(DoubleTableType& ThisTable, double X){
        return ThisTable.GetNearestValue(X);
    }


    void AddTableToPython(pybind11::module& m){
        namespace py = pybind11;

        py::class_<DoubleTableType, DoubleTableType::Pointer>(m,"PiecewiseLinearTable")
            .def(py::init<>())
            .def(py::init<const Matrix&>())
            .def("GetValue", &DoubleTableType::GetValue)
            .def("GetDerivative",&DoubleTableType::GetDerivative)
            .def("GetNearestValue", TableGetNearestValue)
            .def("AddRow", &DoubleTableType::PushBack)
            .def("Clear", &DoubleTableType::Clear)
            .def("__str__", PrintObject<DoubleTableType>);
    }
}