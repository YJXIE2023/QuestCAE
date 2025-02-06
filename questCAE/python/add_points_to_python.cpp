#include "includes/define_python.hpp"
#include "geometries/point.hpp"
#include "python/add_points_to_python.hpp"

namespace Quest{

    template<class TPointType> inline
    void PointSetX(TPointType& ThisPoint, double Value){
        ThisPoint.X() = Value;
    }

    template<class TPointType> inline
    void PointSetY(TPointType& ThisPoint, double Value){
        ThisPoint.Y() = Value;
    }

    template<class TPointType> inline
    void PointSetZ(TPointType& ThisPoint, double Value){
        ThisPoint.Z() = Value;
    }

    template<class TPointType> inline
    double PointGetX(TPointType& ThisPoint){
        return ThisPoint.X();
    }

    template<class TPointType> inline
    double PointGetY(TPointType& ThisPoint){
        return ThisPoint.Y();
    }

    template<class TPointType> inline
    double PointGetZ(TPointType& ThisPoint){
        return ThisPoint.Z();
    }
    


    void AddPointsToPython(pybind11::module& m){
        namespace py = pybind11;

        py::class_<Point, Point::Pointer, array_1d<double,3> >(m,"Point")
            .def(py::init<double, double, double>())
            .def(py::init<vector_expression<Vector> >())
            .def(py::init<Array1d<double,3>>())
            .def_property("X", PrintGetX<Point>, PointSetX<Point>)
            .def_property("Y", PrintGetY<Point>, PointSetY<Point>)
            .def_property("Z", PrintGetZ<Point>, PointSetZ<Point>)
            .def("__str__", PrintObject<Point>);
    }
}