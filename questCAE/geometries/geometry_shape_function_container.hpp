/*--------------------------------
用于存储形函数及其导数的计算值
---------------------------------*/

#ifndef QUEST_GEOMETRY_SHAPE_FUNCTION_CONTAINER_HPP
#define QUEST_GEOMETRY_SHAPE_FUNCTION_CONTAINER_HPP

// 系统头文件
#include <array>
#include <vector>

// 项目头文件
#include "includes/define.hpp"
#include "includes/ublas_interface.hpp"
#include "includes/serializer.hpp"
#include "integration/integration_point.hpp"

namespace Quest{

    template<typename TIntegrationMethodType>
    class GeometryShapeFunctionContainer{
        public:
            QUEST_CLASS_POINTER_DEFINITION(GeometryShapeFunctionContainer);

            using IntegrationMethod = TIntegrationMethodType;

            using IndexType = std::size_t;
            using SizeType = std::size_t;

            using IntegrationPointType = IntegrationPoint<3>;
            using IntegrationPointsArrayType = std::vector<IntegrationPointType>;
            using IntegrationPointsContainerType = std::array<IntegrationPointsArrayType, static_cast<int>(IntegrationMethod::NumberOfIntegrationMethods)>;

            using ShapeFunctionsValuesContainerType = std::array<Matrix, static_cast<int>(IntegrationMethod::NumberOfIntegrationMethods)>;

            using ShapeFunctionsGradientsType = DenseVector<Matrix>;
            using ShapeFunctionsLocalGradientsContainerType = std::array<DenseVector<Matrix>, static_cast<int>(IntegrationMethod::NumberOfIntegrationMethods)>;

            using ShapeFunctionsDerivativesType = DenseVector<Matrix>;
            using ShapeFunctionsDerivativesIntegrationPointArrayType = DenseVector<ShapeFunctionsDerivativesType>;
            using ShapeFunctionsDerivativesContainerType = std::array<ShapeFunctionsDerivativesIntegrationPointArrayType, static_cast<int>(IntegrationMethod::NumberOfIntegrationMethods)>;


            GeometryShapeFunctionContainer(
                IntegrationMethod ThisDefalutMethod,
                const IntegrationPointsContainerType& ThisIntegrationPoints,
                const ShapeFunctionsValuesContainerType& ThisShapeFunctionsValues,
                const ShapeFunctionsLocalGradientsContainerType& ThisShapeFunctionsLocalGradients):
                mDefaultMethod(ThisDefalutMethod),
                mIntegrationPoints(ThisIntegrationPoints),
                mShapeFunctionsValues(ThisShapeFunctionsValues),
                mShapeFunctionsLocalGradients(ThisShapeFunctionsLocalGradients){}

            
            GeometryShapeFunctionContainer(
                IntegrationMethod ThisDefalutMethod,
                const IntegrationPointType& ThisIntegrationPoint,
                const Matrix& ThisShapeFunctionsValues,
                const Matrix& ThisShapeFunctionsGradients):
                mDefaultMethod(ThisDefalutMethod)
            {
                IntegrationPointsArrayType ips(1);
                ips[0] = ThisIntegrationPoint;
                mIntegrationPoints[static_cast<int>(ThisDefalutMethod)] = ips;

                mShapeFunctionsValues[static_cast<int>(ThisDefalutMethod)] = ThisShapeFunctionsValues;

                ShapeFunctionsGradientsType DN_De_array(1);
                DN_De_array[0] = ThisShapeFunctionsGradients;
                mShapeFunctionsLocalGradients[static_cast<int>(ThisDefalutMethod)] = DN_De_array;
            }


            GeometryShapeFunctionContainer(
                IntegrationMethod ThisDefalutMethod,
                const IntegrationPointType& ThisIntegrationPoint,
                const Matrix& ThisShapeFunctionValues,
                const DenseVector<Matrix>& ThisShapeFunctionsDerivatives):
                mDefaultMethod(ThisDefalutMethod)
            {
                IntegrationPointsArrayType ips(1);
                ips[0] = ThisIntegrationPoint;
                mIntegrationPoints[static_cast<int>(ThisDefalutMethod)] = ips;

                mShapeFunctionsValues[static_cast<int>(ThisDefalutMethod)] = ThisShapeFunctionValues;

                if(ThisShapeFunctionsDerivatives.size() > 0){
                    ShapeFunctionsGradientsType DN_De_array(1);
                    DN_De_array[0] = ThisShapeFunctionsDerivatives[0];
                    mShapeFunctionsLocalGradients[static_cast<int>(ThisDefalutMethod)] = DN_De_array;
                }
                if(ThisShapeFunctionsDerivatives.size() > 1){
                    ShapeFunctionsDerivativesIntegrationPointArrayType derivatives_array(ThisShapeFunctionsDerivatives.size()-1);
                    for(IndexType i = 1; i < ThisShapeFunctionsDerivatives.size(); i++){
                        ShapeFunctionsDerivativesType DN_De_i_array(1);
                        DN_De_i_array[0] = ThisShapeFunctionsDerivatives[i];
                        derivatives_array[i-1] = DN_De_i_array;
                    }
                    mShapeFunctionsDerivatives[static_cast<int>(ThisDefalutMethod)] = derivatives_array;
                }
            }


            GeometryShapeFunctionContainer():
                mIntegrationPoints({}), 
                mShapeFunctionsValues({}), 
                mShapeFunctionsLocalGradients({}), 
                mShapeFunctionsDerivatives({}){}


            GeometryShapeFunctionContainer(const GeometryShapeFunctionContainer& rOther):
                mDefaultMethod(rOther.mDefaultMethod),
                mIntegrationPoints(rOther.mIntegrationPoints),
                mShapeFunctionsValues(rOther.mShapeFunctionsValues),
                mShapeFunctionsLocalGradients(rOther.mShapeFunctionsLocalGradients),
                mShapeFunctionsDerivatives(rOther.mShapeFunctionsDerivatives){}


            virtual ~GeometryShapeFunctionContainer(){}


            GeometryShapeFunctionContainer& operator = (const GeometryShapeFunctionContainer& rOther){
                mDefaultMethod = rOther.mDefaultMethod;
                mIntegrationPoints = rOther.mIntegrationPoints;
                mShapeFunctionsValues = rOther.mShapeFunctionsValues;
                mShapeFunctionsLocalGradients = rOther.mShapeFunctionsLocalGradients;
                mShapeFunctionsDerivatives = rOther.mShapeFunctionsDerivatives;

                return *this;
            }


            IntegrationMethod DefaultIntegrationMethod() const{
                return mDefaultMethod;
            }


            bool HasIntegrationMethod(IntegrationMethod ThisMethod) const{
                return (!mIntegrationPoints[static_cast<int>(ThisMethod)].empty());
            }


            SizeType IntegrationPointsNumber() const{
                return mIntegrationPoints[static_cast<int>(mDefaultMethod)].size();
            }


            SizeType IntegrationPointsNumber(IntegrationMethod ThisMethod) const{
                return mIntegrationPoints[static_cast<int>(ThisMethod)].size();
            }


            const IntegrationPointsArrayType& IntegrationPoints() const{
                return mIntegrationPoints[static_cast<int>(mDefaultMethod)];
            }


            const IntegrationPointsArrayType& IntegrationPoints(IntegrationMethod ThisMethod) const{
                return mIntegrationPoints[static_cast<int>(ThisMethod)];
            }


            const Matrix& ShapeFunctionsValues() const{
                return mShapeFunctionsValues[static_cast<int>(mDefaultMethod)];
            }


            const Matrix& ShapeFunctionsValues(IntegrationMethod ThisMethod) const{
                return mShapeFunctionsValues[static_cast<int>(ThisMethod)];
            }


            double ShapeFunctionValue(IndexType IntegrationPointIndex, IndexType ShapeFunctionIndex, IntegrationMethod ThisMethod) const{
                QUEST_DEBUG_ERROR_IF(mShapeFunctionsValues[static_cast<int>(ThisMethod)].size1() <= IntegrationPointIndex)
                    << "No existing integration point" << std::endl;

                QUEST_DEBUG_ERROR_IF(mShapeFunctionsValues[static_cast<int>(ThisMethod)].size2() <= ShapeFunctionIndex)
                    << "No existing shape function value" << std::endl;

                return mShapeFunctionsValues[static_cast<int>(ThisMethod)](IntegrationPointIndex, ShapeFunctionIndex);
            }


            double ShapeFunctionValue(IndexType IntegrationPointIndex, IndexType ShapeFunctionIndex) const{
                return mShapeFunctionsValues[static_cast<int>(mDefaultMethod)](IntegrationPointIndex, ShapeFunctionIndex);
            }


            const ShapeFunctionsGradientsType& ShapeFunctionsLocalGradients() const{
                return mShapeFunctionsLocalGradients[static_cast<int>(mDefaultMethod)];
            }


            const ShapeFunctionsGradientsType& ShapeFunctionsLocalGradients(IntegrationMethod ThisMethod) const{
                return mShapeFunctionsLocalGradients[static_cast<int>(ThisMethod)];
            }


            const Matrix& ShapeFunctionLocalGradient(IndexType IntegrationPointIndex) const{
                return ShapeFunctionLocalGradient(IntegrationPointIndex, mDefaultMethod);
            }


            const Matrix& ShapeFunctionLocalGradient(IndexType IntegrationPointIndex, IntegrationMethod ThisMethod) const{
                QUEST_DEBUG_ERROR_IF(mShapeFunctionsLocalGradients[static_cast<int>(ThisMethod)].size() <= IntegrationPointIndex)
                    << "No existing integration point" << std::endl;

                return mShapeFunctionsLocalGradients[static_cast<int>(ThisMethod)][IntegrationPointIndex];
            }


            const Matrix& ShapeFunctionLocalGradient(IndexType IntegrationPointIndex, IndexType ShapeFunctionIndex, IntegrationMethod ThisMethod) const{
                QUEST_DEBUG_ERROR_IF(mShapeFunctionsLocalGradients[static_cast<int>(ThisMethod)].size() <= IntegrationPointIndex)
                    << "No existing integration point" << std::endl;

                return mShapeFunctionsLocalGradients[static_cast<int>(ThisMethod)][IntegrationPointIndex];
            }


            const Matrix& ShapeFunctionDerivatives(IndexType DerivativeOrderIndex, IndexType IntegrationPointIndex, IntegrationMethod ThisMethod) const{
                QUEST_DEBUG_ERROR_IF(DerivativeOrderIndex == 0)
                    << "Shape function cannot be accessed through ShapeFunctionDerivatives()" << std::endl;
                
                if (DerivativeOrderIndex == 1){
                    return mShapeFunctionsLocalGradients[static_cast<int>(ThisMethod)][IntegrationPointIndex];
                }

                QUEST_DEBUG_ERROR_IF(mShapeFunctionsDerivatives[static_cast<int>(ThisMethod)][DerivativeOrderIndex - 2].size() < IntegrationPointIndex)
                    << "Not enough integration points within geometry_shape_function_container. Geometry_shape_function_container has "
                    << mShapeFunctionsDerivatives[static_cast<int>(ThisMethod)][DerivativeOrderIndex - 2].size() 
                    << " integration points, Called integration point index: " << IntegrationPointIndex << std::endl;

                return mShapeFunctionsDerivatives[static_cast<int>(ThisMethod)][DerivativeOrderIndex - 2][IntegrationPointIndex];
            }


            double ShapeFunctionDerivativeValue(
                IndexType IntegrationPointIndex,
                IndexType DerivativeOrderIndex,
                IndexType DerivativeOrderRowIndex,
                IndexType ShapeFunctionIndex,
                IntegrationMethod ThisMethod
            ){
                if (DerivativeOrderIndex == 0){
                    return mShapeFunctionsValues[static_cast<int>(ThisMethod)](IntegrationPointIndex, ShapeFunctionIndex);
                }

                if (DerivativeOrderIndex == 1){
                    return mShapeFunctionsLocalGradients[static_cast<int>(ThisMethod)][IntegrationPointIndex](ShapeFunctionIndex, DerivativeOrderRowIndex);
                }

                return mShapeFunctionsDerivatives[static_cast<int>(ThisMethod)][DerivativeOrderIndex - 2][IntegrationPointIndex](ShapeFunctionIndex, DerivativeOrderRowIndex);
            }


            virtual std::string Info() const{
                std::stringstream buffer;
                buffer << "shape function container";
                return buffer.str();
            }


            virtual void PrintInfo(std::ostream& rOstream) const{
                rOstream << this->Info();
            }


            virtual void PrintData(std::ostream& rOstream) const{}

        protected:

        private:
            friend class Serializer;

            virtual void save(Serializer& rSerializer) const{
                rSerializer.save("IngrationPoints", mIntegrationPoints);
                rSerializer.save("ShapeFunctionsValues", mShapeFunctionsValues);
                rSerializer.save("ShapeFunctionsLocalGradients", mShapeFunctionsLocalGradients);
                rSerializer.save("ShapeFunctionsDerivatives", mShapeFunctionsDerivatives);
            }

            virtual void load(Serializer& rSerializer){
                QUEST_ERROR << "load function for GeometryShapeFunctionContainer is not implemented yet." << std::endl;
                rSerializer.load("IngrationPoints", mIntegrationPoints);
                rSerializer.load("ShapeFunctionsValues", mShapeFunctionsValues);
                rSerializer.load("ShapeFunctionsLocalGradients", mShapeFunctionsLocalGradients);
                rSerializer.load("ShapeFunctionsDerivatives", mShapeFunctionsDerivatives);
            }

        private:
            IntegrationMethod mDefaultMethod;
            IntegrationPointsContainerType mIntegrationPoints;
            ShapeFunctionsValuesContainerType mShapeFunctionsValues;
            ShapeFunctionsLocalGradientsContainerType mShapeFunctionsLocalGradients;
            ShapeFunctionsDerivativesContainerType mShapeFunctionsDerivatives;

    };

    template<typename TIntegrationMethodType>
    inline std::istream& operator >> (std::istream& rIstream, GeometryShapeFunctionContainer<TIntegrationMethodType>& rThis);

    template<typename TIntegrationMethodType>
    inline std::ostream& operator << (std::ostream& rOstream, const GeometryShapeFunctionContainer<TIntegrationMethodType>& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }

}

#endif //QUEST_GEOMETRY_SHAPE_FUNCTION_CONTAINER_HPP