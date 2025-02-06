#include "includes/define_python.hpp"
#include "python/add_quaternion_to_python.hpp"
#include "utilities/quaternion.hpp"

namespace Quest{
    namespace py = pybind11;

    double QuaternionGetX(Quaternion<double>& ThisQuaternion) { return ThisQuaternion.X(); }
    double QuaternionGetY(Quaternion<double>& ThisQuaternion) { return ThisQuaternion.Y(); }
    double QuaternionGetZ(Quaternion<double>& ThisQuaternion) { return ThisQuaternion.Z(); }
    double QuaternionGetW(Quaternion<double>& ThisQuaternion) { return ThisQuaternion.W(); }
    void QuaternionSetX(Quaternion<double>& ThisQuaternion, double Value) { ThisQuaternion.SetX(Value); }
    void QuaternionSetY(Quaternion<double>& ThisQuaternion, double Value) { ThisQuaternion.SetY(Value); }
    void QuaternionSetZ(Quaternion<double>& ThisQuaternion, double Value) { ThisQuaternion.SetZ(Value); }
    void QuaternionSetW(Quaternion<double>& ThisQuaternion, double Value) { ThisQuaternion.SetW(Value); }


    void AddQuaternionToPython(pybind11::module& m){
        py::class_<Quaternion<double>, Quaternion<double>::Pointer>(m, "Quaternion")
            .def(py::init<>())
            .def("__str__", PrintObject<Quaternion<double>>)
            .def_property("x", &QuaternionGetX, &QuaternionSetX)
            .def_property("y", &QuaternionGetY, &QuaternionSetY)
            .def_property("z", &QuaternionGetZ, &QuaternionSetZ)
            .def_property("w", &QuaternionGetW, &QuaternionSetW)
            .def("Normalize", &Quaternion<double>::Normalize)
            .def_static("Identity",  &Quaternion<double>::Identity)
            .def("ToRotationMatrix", &Quaternion<double>::ToRotationMatrix<Matrix>)
            .def("MultiplyQuaternion", [](Quaternion<double>& self, Quaternion<double>& a, Quaternion<double>& b){return Quaternion<double>(operator*(a,b));});
    }
}