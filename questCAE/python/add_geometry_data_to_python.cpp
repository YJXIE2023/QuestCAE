#include "geometries/geometry_data.hpp"
#include "python/add_geometry_data_to_python.hpp"

namespace Quest::Python{

    void AddGeometryDataToPython(pybind11::module& m){
        namespace py = pybind11;

        auto integration_method = py::enum_<GeometryData::IntegrationMethod>(m, "GeometryData_IntegrationMethod")
            .value("GI_GAUSS_1", GeometryData::IntegrationMethod::GI_GAUSS_1)
            .value("GI_GAUSS_2", GeometryData::IntegrationMethod::GI_GAUSS_2)
            .value("GI_GAUSS_3", GeometryData::IntegrationMethod::GI_GAUSS_3)
            .value("GI_GAUSS_4", GeometryData::IntegrationMethod::GI_GAUSS_4)
            .value("GI_GAUSS_5", GeometryData::IntegrationMethod::GI_GAUSS_5)
            .value("GI_EXTENDED_GAUSS_1", GeometryData::IntegrationMethod::GI_EXTENDED_GAUSS_1)
            .value("GI_EXTENDED_GAUSS_2", GeometryData::IntegrationMethod::GI_EXTENDED_GAUSS_2)
            .value("GI_EXTENDED_GAUSS_3", GeometryData::IntegrationMethod::GI_EXTENDED_GAUSS_3)
            .value("GI_EXTENDED_GAUSS_4", GeometryData::IntegrationMethod::GI_EXTENDED_GAUSS_4)
            .value("GI_EXTENDED_GAUSS_5", GeometryData::IntegrationMethod::GI_EXTENDED_GAUSS_5);

        auto geometry_family = py::enum_<GeometryData::QuestGeometryFamily>(m, "GeometryData_QuestGeometryFamily")
            .value("Quest_NoElement", GeometryData::QuestGeometryFamily::Quest_NoElement)
            .value("Quest_Point", GeometryData::QuestGeometryFamily::Quest_Point)
            .value("Quest_Linear", GeometryData::QuestGeometryFamily::Quest_Linear)
            .value("Quest_Triangle", GeometryData::QuestGeometryFamily::Quest_Triangle)
            .value("Quest_Quadrilateral", GeometryData::QuestGeometryFamily::Quest_Quadrilateral)
            .value("Quest_Tetrahedra", GeometryData::QuestGeometryFamily::Quest_Tetrahedra)
            .value("Quest_Hexahedra", GeometryData::QuestGeometryFamily::Quest_Hexahedra)
            .value("Quest_Prism", GeometryData::QuestGeometryFamily::Quest_Prism)
            .value("Quest_Pyramid", GeometryData::QuestGeometryFamily::Quest_Pyramid)
            .value("Quest_Nurbs", GeometryData::QuestGeometryFamily::Quest_Nurbs)
            .value("Quest_Brep", GeometryData::QuestGeometryFamily::Quest_Brep)
            .value("Quest_Quadrature_Geometry", GeometryData::QuestGeometryFamily::Quest_Quadrature_Geometry)
            .value("Quest_Composite", GeometryData::QuestGeometryFamily::Quest_Composite)
            .value("Quest_generic_family", GeometryData::QuestGeometryFamily::Quest_generic_family);

        auto geometry_type = py::enum_<GeometryData::QuestGeometryType>(m, "GeometryData_QuestGeometryType")
            .value("Quest_generic_type", GeometryData::QuestGeometryType::Quest_generic_type)
            .value("Quest_Hexahedra3D20", GeometryData::QuestGeometryType::Quest_Hexahedra3D20)
            .value("Quest_Hexahedra3D27", GeometryData::QuestGeometryType::Quest_Hexahedra3D27)
            .value("Quest_Hexahedra3D8", GeometryData::QuestGeometryType::Quest_Hexahedra3D8)
            .value("Quest_Prism3D15", GeometryData::QuestGeometryType::Quest_Prism3D15)
            .value("Quest_Prism3D6", GeometryData::QuestGeometryType::Quest_Prism3D6)
            .value("Quest_Pyramid3D13", GeometryData::QuestGeometryType::Quest_Pyramid3D13)
            .value("Quest_Pyramid3D5", GeometryData::QuestGeometryType::Quest_Pyramid3D5)
            .value("Quest_Quadrilateral2D4", GeometryData::QuestGeometryType::Quest_Quadrilateral2D4)
            .value("Quest_Quadrilateral2D8", GeometryData::QuestGeometryType::Quest_Quadrilateral2D8)
            .value("Quest_Quadrilateral2D9", GeometryData::QuestGeometryType::Quest_Quadrilateral2D9)
            .value("Quest_Quadrilateral3D4", GeometryData::QuestGeometryType::Quest_Quadrilateral3D4)
            .value("Quest_Quadrilateral3D8", GeometryData::QuestGeometryType::Quest_Quadrilateral3D8)
            .value("Quest_Quadrilateral3D9", GeometryData::QuestGeometryType::Quest_Quadrilateral3D9)
            .value("Quest_Tetrahedra3D10", GeometryData::QuestGeometryType::Quest_Tetrahedra3D10)
            .value("Quest_Tetrahedra3D4", GeometryData::QuestGeometryType::Quest_Tetrahedra3D4)
            .value("Quest_Triangle2D3", GeometryData::QuestGeometryType::Quest_Triangle2D3)
            .value("Quest_Triangle2D6", GeometryData::QuestGeometryType::Quest_Triangle2D6)
            .value("Quest_Triangle2D10", GeometryData::QuestGeometryType::Quest_Triangle2D10)
            .value("Quest_Triangle2D15", GeometryData::QuestGeometryType::Quest_Triangle2D15)
            .value("Quest_Triangle3D3", GeometryData::QuestGeometryType::Quest_Triangle3D3)
            .value("Quest_Triangle3D6", GeometryData::QuestGeometryType::Quest_Triangle3D6)
            .value("Quest_Line2D2", GeometryData::QuestGeometryType::Quest_Line2D2)
            .value("Quest_Line2D3", GeometryData::QuestGeometryType::Quest_Line2D3)
            .value("Quest_Line2D4", GeometryData::QuestGeometryType::Quest_Line2D4)
            .value("Quest_Line2D5", GeometryData::QuestGeometryType::Quest_Line2D5)
            .value("Quest_Line3D2", GeometryData::QuestGeometryType::Quest_Line3D2)
            .value("Quest_Line3D3", GeometryData::QuestGeometryType::Quest_Line3D3)
            .value("Quest_Point2D", GeometryData::QuestGeometryType::Quest_Point2D)
            .value("Quest_Point3D", GeometryData::QuestGeometryType::Quest_Point3D)
            .value("Quest_Sphere3D1", GeometryData::QuestGeometryType::Quest_Sphere3D1)
            .value("Quest_Nurbs_Curve", GeometryData::QuestGeometryType::Quest_Nurbs_Curve)
            .value("Quest_Nurbs_Surface", GeometryData::QuestGeometryType::Quest_Nurbs_Surface)
            .value("Quest_Nurbs_Volume", GeometryData::QuestGeometryType::Quest_Nurbs_Volume)
            .value("Quest_Nurbs_Curve_On_Surface", GeometryData::QuestGeometryType::Quest_Nurbs_Curve_On_Surface)
            .value("Quest_Surface_In_Nurbs_Volume", GeometryData::QuestGeometryType::Quest_Surface_In_Nurbs_Volume)
            .value("Quest_Brep_Curve", GeometryData::QuestGeometryType::Quest_Brep_Curve)
            .value("Quest_Brep_Surface", GeometryData::QuestGeometryType::Quest_Brep_Surface)
            .value("Quest_Brep_Curve_On_Surface", GeometryData::QuestGeometryType::Quest_Brep_Curve_On_Surface)
            .value("Quest_Quadrature_Point_Geometry", GeometryData::QuestGeometryType::Quest_Quadrature_Point_Geometry)
            .value("Quest_Coupling_Geometry", GeometryData::QuestGeometryType::Quest_Coupling_Geometry)
            .value("Quest_Quadrature_Point_Curve_On_Surface_Geometry", GeometryData::QuestGeometryType::Quest_Quadrature_Point_Curve_On_Surface_Geometry)
            .value("Quest_Quadrature_Point_Surface_In_Volume_Geometry", GeometryData::QuestGeometryType::Quest_Quadrature_Point_Surface_In_Volume_Geometry);


        py::class_<GeometryData, GeometryData::Pointer>(m, "GeometryData")
            .def_property_readonly_static("IntegrationMethod", [integration_method](py::object) { return integration_method; })
            .def_property_readonly_static("QuestGeometryFamily", [geometry_family](py::object) { return geometry_family; })
            .def_property_readonly_static("QuestGeometryType", [geometry_type](py::object) { return geometry_type; });
    }

}