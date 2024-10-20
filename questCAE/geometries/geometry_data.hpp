/*------------------------------------------------
存储了与有限元计算中的形函数、积分点和梯度相关的关键数据
------------------------------------------------*/

#ifndef QUEST_GEOMETRY_DATA_HPP
#define QUEST_GEOMETRY_DATA_HPP

// 项目头文件
#include "includes/ublas_interface.hpp"
#include "integration/integration_point.hpp"
#include "geometries/geometry_dimension.hpp"
#include "geometries/geometry_shape_function_container.hpp"

namespace Quest{

    class GeometryData{
        public:
            enum class IntegrationMethod{
                GI_GAUSS_1,
                GI_GAUSS_2,
                GI_GAUSS_3,
                GI_GAUSS_4,
                GI_GAUSS_5,
                GI_EXTENDED_GAUSS_1,
                GI_EXTENDED_GAUSS_2,
                GI_EXTENDED_GAUSS_3,
                GI_EXTENDED_GAUSS_4,
                GI_EXTENDED_GAUSS_5,
                NumberOfIntegrationMethods
            };

            enum class QuestGeometryFamily{
                Quest_NoElement,
                Quest_Point,
                Quest_Linear,
                Quest_Triangle,
                Quest_Quadrilateral,
                Quest_Tetrahedra,
                Quest_Hexahedra,
                Quest_Prism,
                Quest_Pyramid,
                Quest_Nurbs,
                Quest_Brep,
                Quest_Quadrature_Geometry,
                Quest_Composite,
                Quest_generic_family,
                NumberOfGeometryFamilies
            };

            enum class QuestGeomtryType{
                Quest_generic_type,
                Quest_Hexahedra3D20,
                Quest_Hexahedra3D27,
                Quest_Hexahedra3D8,
                Quest_Prism3D15,
                Quest_Prism3D6,
                Quest_Pyramid3D13,
                Quest_Pyramid3D5,
                Quest_Quadrilateral2D4,
                Quest_Quadrilateral2D8,
                Quest_Quadrilateral2D9,
                Quest_Quadrilateral3D4,
                Quest_Quadrilateral3D8,
                Quest_Quadrilateral3D9,
                Quest_Tetrahedra3D10,
                Quest_Tetrahedra3D4,
                Quest_Triangle2D3,
                Quest_Triangle2D6,
                Quest_Triangle2D10,
                Quest_Triangle2D15,
                Quest_Triangle3D3,
                Quest_Triangle3D6,
                Quest_Line2D2,
                Quest_Line2D3,
                Quest_Line2D4,
                Quest_Line2D5,
                Quest_Line3D2,
                Quest_Line3D3,
                Quest_Point2D,
                Quest_Point3D,
                Quest_Sphere3D1,
                Quest_Nurbs_Curve,
                Quest_Nurbs_Surface,
                Quest_Nurbs_Volume,
                Quest_Nurbs_Curve_On_Surface,
                Quest_Surface_In_Nurbs_Volume,
                Quest_Brep_Curve,
                Quest_Brep_Surface,
                Quest_Brep_Curve_On_Surface,
                Quest_Quadrature_Point_Geometry,
                Quest_Coupling_Geometry,
                Quest_Quadrature_Point_Curve_On_Surface_Geometry,
                Quest_Quadrature_Point_Surface_In_Volume_Geometry,
                NumberOfGeometryTypes
            };

            QUEST_CLASS_POINTER_DEFINITION(GeometryData);

            using IndexType = std::size_t;
            using SizeType = std::size_t;

            using IntegrationPointType = IntegrationPoint<3>;
            using IntegrationPointsArrayType = GeometryShapeFunctionContainer<IntegrationMethod>::IntegrationPointsArrayType;
            using IntegrationPointsContainerType = GeometryShapeFunctionContainer<IntegrationMethod>::IntegrationPointsContainerType;
            using ShapeFunctionsValuesContainerType = GeometryShapeFunctionContainer<IntegrationMethod>::ShapeFunctionsValuesContainerType;
            using ShapeFunctionsLocalGradientsContainerType = GeometryShapeFunctionContainer<IntegrationMethod>::ShapeFunctionsLocalGradientsContainerType;

            using ShapeFunctionsGradientsType = DenseVector<Matrix>;
            using ShapeFunctionsSecondDerivativesType = DenseVector<Matrix>;
            using ShapeFunctionsThirdDerivativesType = DenseVector<DenseVector<Matrix>>;

        public:
            GeometryData(const GeometryDimension* pThisGeometryDimension,
                IntegrationMethod ThisDefaultMethod,
                const IntegrationPointsContainerType& ThisIntegrationPoints,
                const ShapeFunctionsValuesContainerType& ThisShapeFunctionsValues,
                const ShapeFunctionsLocalGradientsContainerType& ThisShapeFunctionsLocalGradients):
                mpGeometryDimension(pThisGeometryDimension),
                mGeometryShapeFunctionContainer(ThisDefaultMethod, ThisIntegrationPoints, ThisShapeFunctionsValues, ThisShapeFunctionsLocalGradients)
            {}


            GeometryData(const GeometryDimension* pThisGeometryDimension,
                const GeometryShapeFunctionContainer<IntegrationMethod>& ThisGeometryShapeFunctionContainer):
                mpGeometryDimension(pThisGeometryDimension),
                mGeometryShapeFunctionContainer(ThisGeometryShapeFunctionContainer)
            {}


            GeometryData(const GeometryData& rOther):
                mpGeometryDimension(rOther.mpGeometryDimension),
                mGeometryShapeFunctionContainer(rOther.mGeometryShapeFunctionContainer)
            {}


            virtual ~GeometryData(){}


            GeometryData& operator = (const GeometryData& rOther){
                mpGeometryDimension = rOther.mpGeometryDimension;
                mGeometryShapeFunctionContainer = rOther.mGeometryShapeFunctionContainer;

                return *this;
            }


            void SetGeometryDimension(const GeometryDimension* pGeometryDimension){
                mpGeometryDimension = pGeometryDimension;
            }


            void SetGeometryShapeFunctionContainer(const GeometryShapeFunctionContainer<IntegrationMethod>& rGeometryShapeFunctionContainer){
                mGeometryShapeFunctionContainer = rGeometryShapeFunctionContainer;
            }


            const GeometryShapeFunctionContainer<IntegrationMethod>& GetGeometryShapeFunctionContainer() const{
                return mGeometryShapeFunctionContainer;
            }


            SizeType WorkingSpaceDimension() const{
                return mpGeometryDimension->WorkingSpaceDimension();
            }


            SizeType LocalSpaceDimension() const{
                return mpGeometryDimension->LocalSpaceDimension();
            }


            bool HasIntegrationMethod(IntegrationMethod ThisMethod) const{
                return mGeometryShapeFunctionContainer.HasIntegrationMethod(ThisMethod);
            }


            IntegrationMethod DefaultIntegrationMethod() const{
                return mGeometryShapeFunctionContainer.DefaultIntegrationMethod();
            }


            SizeType IntegrationPointsNumber() const{
                return mGeometryShapeFunctionContainer.IntegrationPointsNumber();
            }


            SizeType IntegrationPointsNumber(IntegrationMethod ThisMethod) const{
                return mGeometryShapeFunctionContainer.IntegrationPointsNumber(ThisMethod);
            }


            const IntegrationPointsArrayType& IntegrationPoints() const{
                return mGeometryShapeFunctionContainer.IntegrationPoints();
            }


            const IntegrationPointsArrayType& IntegrationPoints(IntegrationMethod ThisMethod) const{
                return mGeometryShapeFunctionContainer.IntegrationPoints(ThisMethod);
            }


            const Matrix& ShapeFunctionsValues() const{
                return mGeometryShapeFunctionContainer.ShapeFunctionsValues();
            }


            const Matrix& ShapeFunctionsValues(IntegrationMethod ThisMethod) const{
                return mGeometryShapeFunctionContainer.ShapeFunctionsValues(ThisMethod);
            }


            double ShapeFunctionValue(IndexType IntegrationPointIndex, IndexType ShapeFunctionIndex) const{
                return mGeometryShapeFunctionContainer.ShapeFunctionValue(IntegrationPointIndex, ShapeFunctionIndex);
            }


            double ShapeFunctionValue(IndexType IntegrationPointIndex, IndexType ShapeFunctionIndex, IntegrationMethod ThisMethod) const{
                return mGeometryShapeFunctionContainer.ShapeFunctionValue(IntegrationPointIndex, ShapeFunctionIndex, ThisMethod);
            }


            const ShapeFunctionsGradientsType& ShapeFunctionsLocalGradients() const{
                return mGeometryShapeFunctionContainer.ShapeFunctionsLocalGradients();
            }


            const ShapeFunctionsGradientsType& ShapeFunctionsLocalGradients(IntegrationMethod ThisMethod) const{
                return mGeometryShapeFunctionContainer.ShapeFunctionsLocalGradients(ThisMethod);
            }


            const Matrix& ShapeFunctionLocalGradient(IndexType IntegrationPointIndex) const{
                return mGeometryShapeFunctionContainer.ShapeFunctionLocalGradient(IntegrationPointIndex);
            }


            const Matrix& ShapeFunctionLocalGradient(IndexType IntegrationPointIndex, IntegrationMethod ThisMethod) const{
                return mGeometryShapeFunctionContainer.ShapeFunctionLocalGradient(IntegrationPointIndex, ThisMethod);
            }


            const Matrix& ShapeFunctionLocalGradient(IndexType IntegrationPointIndex, IndexType ShapeFunctionIndex, IntegrationMethod ThisMethod) const{
                return mGeometryShapeFunctionContainer.ShapeFunctionLocalGradient(IntegrationPointIndex, ThisMethod);
            }


            const Matrix& ShapeFunctionDerivatives(IndexType DerivativeOrderIndex, IndexType IntegrationPointIndex, IntegrationMethod ThisMethod) const{
                return mGeometryShapeFunctionContainer.ShapeFunctionDerivatives(DerivativeOrderIndex, IntegrationPointIndex, ThisMethod);
            }


            virtual std::string Info() const{
                std::stringstream buffer;
                buffer << "geometry data";
                return buffer.str();
            }

            virtual void PrintInfo(std::ostream& rOstream) const{
                rOstream << this->Info();
            }

            virtual void PrintData(std::ostream& rOstream) const{
                rOstream << "    Working space dimension : " << mpGeometryDimension->WorkingSpaceDimension() << std::endl;
                rOstream << "    Local space dimension   : " << mpGeometryDimension->LocalSpaceDimension();
            }

        protected:

        private:
            GeometryData() {}

            friend class Serializer;

            virtual void save(Serializer& rSerializer) const{
                rSerializer.save("GeometryDimension", mpGeometryDimension);
                rSerializer.save("GeometryShapeFunctionContainer", mGeometryShapeFunctionContainer);
            }

            virtual void load(Serializer& rSerializer){
                rSerializer.load("GeometryDimension", const_cast<GeometryDimension*>(mpGeometryDimension));
                rSerializer.load("GeometryShapeFunctionContainer", mGeometryShapeFunctionContainer);
            }

        private:
            const GeometryDimension* mpGeometryDimension;

            GeometryShapeFunctionContainer<IntegrationMethod> mGeometryShapeFunctionContainer;

    };

    inline std::istream& operator >> (std::istream& rIstream, GeometryData& rThis);

    inline std::ostream& operator << (std::ostream& rOstream, const GeometryData& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);
        
        return rOstream;
    }

} // namespace Quest

#endif //QUEST_GEOMETRY_DATA_HPP