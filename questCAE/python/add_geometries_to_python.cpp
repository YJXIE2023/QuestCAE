#include "includes/define_python.hpp"
#include "geometries/point.hpp"
#include "includes/node.hpp"
#include "python/containers_interface.hpp"
#include "python/add_geometries_to_python.hpp"
#include "geometries/geometry.hpp"


namespace Quest::Python{

    using IndexType = std::size_t;
    using SizeType = std::size_t;
    using NodeType = Node;
    using NodeContainerType = PointerVector<NodeType>;
    using GeometryType = Geometry<NodeType>;
    using PointsArrayType = typename GeometryType::PointsArrayType;
    using IntegrationPointsArrayType = typename GeometryType::IntegrationPointsArrayType;
    using GeometriesArrayType = typename GeometryType::GeometriesArrayType;
    using CoordinatesArrayType = typename Point::CoordinatesArrayType;


    const PointerVector<Node>& ConstGetPoints( GeometryType& geom ) { return geom.Points(); }
    PointerVector<Node>& GetPoints( GeometryType& geom ) { return geom.Points(); }

    void SetId1(GeometryType& dummy, IndexType geometry_id){
        return(dummy.SetId(geometry_id));
    }


    void SetId2(GeometryType& dummy, const std::string& geometry_name){
        return(dummy.SetId(geometry_name));
    }


    bool IsIdGeneratedFromString1(GeometryType& dummy){
        return(dummy.IsIdGeneratedFromString());
    }


    bool IsIdSelfAssigned1(GeometryType& dummy){
        return(dummy.IsIdSelfAssigned());
    }


    template<typename TDataType>
    void Assign(GeometryType& dummy, const Variable<TDataType>& rVariable, TDataType Value){
        dummy.Assign(rVariable, Value);
    }


    template<typename TDataType>
    TDataType Calculate(GeometryType& dummy, const Variable<TDataType>& rVariable){
        TDataType Output;
        dummy.Calculate(rVariable, Output);
        return Output;
    }


    void AddGeometriesToPython(pybind11::module& m){
        namespace py = pybind11;

        using NodeType = Node;
        using pNodeType = NodeType::Pointer;
        using GeometryType = Geometry<NodeType>;

        py::class_<GeometryType, pGeometryType>(m, "Geometry")
            .def(py::init<>())
            .def(py::init< IndexType >())
            .def(py::init< std::string >())
            .def(py::init< GeometryType::PointsArrayType& >())
            .def(py::init< IndexType, GeometryType::PointsArrayType& >())
            .def(py::init< std::string, GeometryType::PointsArrayType& >())
            // 编号相关函数
            .def_property("Id", &GeometryType::Id, SetId1)
            .def("SetId", SetId1)
            .def("SetId", SetId2)
            .def("IsIdGeneratedFromString", IsIdGeneratedFromString1)
            .def("IsIdSelfAssigned", IsIdSelfAssigned1)
            .def_static("GenerateId", &GeometryType::GenerateId)
            // 维度相关函数
            .def("WorkingSpaceDimension", &GeometryType::WorkingSpaceDimension)
            .def("LocalSpaceDimension", &GeometryType::LocalSpaceDimension)
            .def("DomainSize", &GeometryType::DomainSize)
            .def("EdgesNumber", &GeometryType::EdgesNumber)
            .def("PointsNumber", &GeometryType::PointsNumber)
            .def("PointsNumberInDirection", &GeometryType::PointsNumberInDirection)
            .def("PolynomialDegree", &GeometryType::PolynomialDegree)
            // 几何数据
            .def("GetDefaultIntegrationMethod", &GeometryType::GetDefaultIntegrationMethod)
            .def("GetGeometryFamily", &GeometryType::GetGeometryFamily)
            .def("GetGeometryType", &GeometryType::GetGeometryType)
            // 几何部分
            .def("GetGeometryPart", [](GeometryType& self, IndexType Index){return(self.GetGeometryPart(Index));})
            .def_property_reeadonly_static("BACKGROUND_GEOMETRY_INDEX", [](py::object){return GeometryType::BACKGROUND_GEOMETRY_INDEX;})
            // 积分
            .def("IntegrationPointsNumber", [](GeometryType& self){return(self.IntegrationPointsNumber());})
            // 求积相关
            .def("CreateQuadraturePointGeometries", [](GeometryType& self,
                GeometriesArrayType& rResultGeometries, IndexType NumberOfShapeFunctionDerivatives)
                {
                    IntegrationInfo integration_info = self.GetDefaultIntegrationInfo();
                    return(self.CreateQuadraturePointGeometries(rResultGeometries, NumberOfShapeFunctionDerivatives, integration_info));
                })
            .def("CreateQuadraturePointGeometries", [](GeometryType& self,
                GeometriesArrayType& rResultGeometries, IndexType NumberOfShapeFunctionDerivatives, std::vector<std::array<double,4>>& rIntegrationPoints)
                {
                    IntegrationPointsArrayType integration_points(rIntegrationPoints.size());
                    for( IndexType i = 0; i < rIntegrationPoints.size(); ++i){
                        IntegrationPoint<3> point_tmp(rIntegrationPoints[i][0],rIntegrationPoints[i][1],rIntegrationPoints[i][2],rIntegrationPoints[i][3]);
                        integration_points[i] = point_tmp;
                    }
                    IntegrationInfo integration_info = self.GetDefaultIntegrationInfo();
                    return(self.CreateQuadraturePointGeometries(rResultGeometries, NumberOfShapeFunctionDerivatives, integration_points, integration_info)); 
                })
            // 法向量
            .def("Normal", [](GeometryType& self, CoordinatesArrayType& LocalCoords)
                { return(self.Normal(LocalCoords)); })
            .def("Normal", [](GeometryType& self, IndexType IntegrationPointIndex)
                { return(self.Normal(IntegrationPointIndex)); })
            .def("UnitNormal", [](GeometryType& self, CoordinatesArrayType& LocalCoords)
                { return(self.UnitNormal(LocalCoords)); })
            .def("UnitNormal", [](GeometryType& self, IndexType IntegrationPointIndex)
                { return(self.UnitNormal(IntegrationPointIndex)); })
            // 雅可比矩阵
            .def("Jacobian", [](GeometryType& self, IndexType IntegrationPointIndex)
                { Matrix results; return(self.Jacobian(results, IntegrationPointIndex)); })
            .def("DeterminantOfJacobian", [](GeometryType& self)
                { Vector results; return(self.DeterminantOfJacobian(results)); })
            .def("DeterminantOfJacobian", [](GeometryType& self, IndexType IntegrationPointIndex)
                { return(self.DeterminantOfJacobian(IntegrationPointIndex)); })
            // 形函数值
            .def("ShapeFunctionsValues", [](GeometryType& self)
                { return(self.ShapeFunctionsValues()); })
            .def("ShapeFunctionDerivatives", [](GeometryType& self, IndexType DerivativeOrderIndex,
                IndexType IntegrationPointIndex)
                { return(self.ShapeFunctionDerivatives(DerivativeOrderIndex, IntegrationPointIndex, self.GetDefaultIntegrationMethod())); })
            // 坐标轴映射
            .def("GlobalCoordinates", [](GeometryType& self, CoordinatesArrayType& LocalCoordinates){
                CoordinatesArrayType result = ZeroVector( 3 );
                return(self.GlobalCoordinates(result, LocalCoordinates)); })
            // 几何相关函数
            .def("Center", &GeometryType::Center)
            .def("Length",&GeometryType::Length)
            .def("Area",&GeometryType::Area)
            .def("Volume",&GeometryType::Volume)
            // 赋值
            .def("Assign", Assign<bool>)
            .def("Assign", Assign<int>)
            .def("Assign", Assign<double>)
            .def("Assign", Assign<Array1d<double, 2>>)
            .def("Assign", Assign<Array1d<double, 3>>)
            .def("Assign", Assign<Array1d<double, 6>>)
            .def("Assign", Assign<Vector>)
            .def("Assign", Assign<Matrix>)
            // 计算
            .def("Calculate", Calculate<bool>)
            .def("Calculate", Calculate<int>)
            .def("Calculate", Calculate<double>)
            .def("Calculate", Calculate<Array1d<double, 2>>)
            .def("Calculate", Calculate<Array1d<double, 3>>)
            .def("Calculate", Calculate<Array1d<double, 6>>)
            .def("Calculate", Calculate<Vector>)
            .def("Calculate", Calculate<Matrix>)
            // 信息
            .def("Info", &GeometryType::Info)
            // 打印
            .def("__str__", PrintObject<GeometryType>)
            // 与节点相关
            .def("__getitem__", [](GeometryType& self, unsigned int i){return self(i);} )
            .def("__iter__",    [](GeometryType& self){return py::make_iterator(self.begin(), self.end());},  py::keep_alive<0,1>())
            .def("__len__",     [](GeometryType& self){return self.PointsNumber();} );
    }

}