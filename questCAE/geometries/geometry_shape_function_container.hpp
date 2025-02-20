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

    /**
     * @class GeometryShapeFunctionContainer
     * @brief 存储形函数的容器
     * @tparam TIntegrationMethodType 积分方法类型
     */
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

            /**
             * @brief 构造函数（多种积分方法）
             * @param ThisDefalutMethod 默认积分方法
             * @param ThisIntegrationPoints 积分点数组
             * @param ThisShapeFunctionsValues 单元形函数在所有积分点的计算值
             * @param ThisShapeFunctionsLocalGradients 形函数对于单元局部坐标的梯度值
             */
            GeometryShapeFunctionContainer(
                IntegrationMethod ThisDefalutMethod,
                const IntegrationPointsContainerType& ThisIntegrationPoints,
                const ShapeFunctionsValuesContainerType& ThisShapeFunctionsValues,
                const ShapeFunctionsLocalGradientsContainerType& ThisShapeFunctionsLocalGradients):
                mDefaultMethod(ThisDefalutMethod),
                mIntegrationPoints(ThisIntegrationPoints),
                mShapeFunctionsValues(ThisShapeFunctionsValues),
                mShapeFunctionsLocalGradients(ThisShapeFunctionsLocalGradients){}

            /**
             * @brief 构造函数（单一积分方法）
             * @param ThisDefalutMethod 默认积分方法
             * @param ThisIntegrationPoint 积分点
             * @param ThisShapeFunctionsValues 单元形函数在积分点的计算值
             * @param ThisShapeFunctionsGradients 形函数对于单元局部坐标的梯度值
             */
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

            /**
             * @brief 构造函数（单一积分方法）
             * @param ThisDefalutMethod 默认积分方法
             * @param ThisIntegrationPoint 积分点
             * @param ThisShapeFunctionsValues 单元形函数在积分点的计算值
             * @param ThisShapeFunctionsDerivatives 形函数对于单元局部坐标的导数值
             */
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


            /**
             * @brief 默认构造函数
             */
            GeometryShapeFunctionContainer():
                mIntegrationPoints({}), 
                mShapeFunctionsValues({}), 
                mShapeFunctionsLocalGradients({}), 
                mShapeFunctionsDerivatives({}){}

            /**
             * @brief 复制构造函数
             */
            GeometryShapeFunctionContainer(const GeometryShapeFunctionContainer& rOther):
                mDefaultMethod(rOther.mDefaultMethod),
                mIntegrationPoints(rOther.mIntegrationPoints),
                mShapeFunctionsValues(rOther.mShapeFunctionsValues),
                mShapeFunctionsLocalGradients(rOther.mShapeFunctionsLocalGradients),
                mShapeFunctionsDerivatives(rOther.mShapeFunctionsDerivatives){}

            /**
             * @brief 析构函数
             */
            virtual ~GeometryShapeFunctionContainer(){}

            /**
             * @brief 赋值运算符
             */
            GeometryShapeFunctionContainer& operator = (const GeometryShapeFunctionContainer& rOther){
                mDefaultMethod = rOther.mDefaultMethod;
                mIntegrationPoints = rOther.mIntegrationPoints;
                mShapeFunctionsValues = rOther.mShapeFunctionsValues;
                mShapeFunctionsLocalGradients = rOther.mShapeFunctionsLocalGradients;
                mShapeFunctionsDerivatives = rOther.mShapeFunctionsDerivatives;

                return *this;
            }

            /**
             * @brief 获取默认积分方法
             */
            IntegrationMethod DefaultIntegrationMethod() const{
                return mDefaultMethod;
            }

            /**
             * @brief 判断是否有指定积分方法
             * @param ThisMethod 指定的积分方法
             */
            bool HasIntegrationMethod(IntegrationMethod ThisMethod) const{
                return (!mIntegrationPoints[static_cast<int>(ThisMethod)].empty());
            }

            /**
             * @brief 获取默认积分方法的积分点数量
             */
            SizeType IntegrationPointsNumber() const{
                return mIntegrationPoints[static_cast<int>(mDefaultMethod)].size();
            }

            /**
             * @brief 获取指定积分方法的积分点数量
             * @param ThisMethod 指定的积分方法
             */
            SizeType IntegrationPointsNumber(IntegrationMethod ThisMethod) const{
                return mIntegrationPoints[static_cast<int>(ThisMethod)].size();
            }

            /**
             * @brief 获取默认积分方法的积分点
             */
            const IntegrationPointsArrayType& IntegrationPoints() const{
                return mIntegrationPoints[static_cast<int>(mDefaultMethod)];
            }

            /**
             * @brief 获取指定积分方法的积分点
             */
            const IntegrationPointsArrayType& IntegrationPoints(IntegrationMethod ThisMethod) const{
                return mIntegrationPoints[static_cast<int>(ThisMethod)];
            }

            /**
             * @brief 获取默认积分方法的形函数在所有积分点的计算值
             */
            const Matrix& ShapeFunctionsValues() const{
                return mShapeFunctionsValues[static_cast<int>(mDefaultMethod)];
            }

            /**
             * @brief 获取指定积分方法的形函数在所有积分点的计算值
             */
            const Matrix& ShapeFunctionsValues(IntegrationMethod ThisMethod) const{
                return mShapeFunctionsValues[static_cast<int>(ThisMethod)];
            }

            /**
             * @brief 获取积分方法的指定形函数分量在指定积分点的计算值
             * @param IntegrationPointIndex 指定的积分点索引
             * @param ShapeFunctionIndex 指定的形函数索引
             * @param ThisMethod 指定的积分方法
             */
            double ShapeFunctionValue(IndexType IntegrationPointIndex, IndexType ShapeFunctionIndex, IntegrationMethod ThisMethod) const{
                QUEST_DEBUG_ERROR_IF(mShapeFunctionsValues[static_cast<int>(ThisMethod)].size1() <= IntegrationPointIndex)
                    << "No existing integration point" << std::endl;

                QUEST_DEBUG_ERROR_IF(mShapeFunctionsValues[static_cast<int>(ThisMethod)].size2() <= ShapeFunctionIndex)
                    << "No existing shape function value" << std::endl;

                return mShapeFunctionsValues[static_cast<int>(ThisMethod)](IntegrationPointIndex, ShapeFunctionIndex);
            }

            /**
             * @brief 获取默认积分方法的指定形函数分量在指定积分点的计算值
             */
            double ShapeFunctionValue(IndexType IntegrationPointIndex, IndexType ShapeFunctionIndex) const{
                return mShapeFunctionsValues[static_cast<int>(mDefaultMethod)](IntegrationPointIndex, ShapeFunctionIndex);
            }

            /**
             * @brief 获取默认积分方法的所有积分点的形函数一阶导数矩阵
             */
            const ShapeFunctionsGradientsType& ShapeFunctionsLocalGradients() const{
                return mShapeFunctionsLocalGradients[static_cast<int>(mDefaultMethod)];
            }

            /**
             * @brief 获取指定积分方法的所有积分点的形函数一阶导数矩阵
             * @param ThisMethod 指定的积分方法
             */
            const ShapeFunctionsGradientsType& ShapeFunctionsLocalGradients(IntegrationMethod ThisMethod) const{
                return mShapeFunctionsLocalGradients[static_cast<int>(ThisMethod)];
            }

            /**
             * @brief 获取默认积分方法的指定积分点的形函数一阶导数矩阵
             */
            const Matrix& ShapeFunctionLocalGradient(IndexType IntegrationPointIndex) const{
                return ShapeFunctionLocalGradient(IntegrationPointIndex, mDefaultMethod);
            }

            /**
             * @brief 获取指定积分方法的指定积分点的形函数一阶导数矩阵
             * @param ThisMethod 指定的积分方法
             * @param IntegrationPointIndex 指定的积分点索引
             */
            const Matrix& ShapeFunctionLocalGradient(IndexType IntegrationPointIndex, IntegrationMethod ThisMethod) const{
                QUEST_DEBUG_ERROR_IF(mShapeFunctionsLocalGradients[static_cast<int>(ThisMethod)].size() <= IntegrationPointIndex)
                    << "No existing integration point" << std::endl;

                return mShapeFunctionsLocalGradients[static_cast<int>(ThisMethod)][IntegrationPointIndex];
            }

            /**
             * @brief 获取指定积分方法的指定积分点的形函数一阶导数矩阵
             * @param IntegrationPointIndex 指定的积分点索引
             * @param ShapeFunctionIndex 指定的形函数索引
             * @param ThisMethod 指定的积分方法
             */
            const Matrix& ShapeFunctionLocalGradient(IndexType IntegrationPointIndex, IndexType ShapeFunctionIndex, IntegrationMethod ThisMethod) const{
                QUEST_DEBUG_ERROR_IF(mShapeFunctionsLocalGradients[static_cast<int>(ThisMethod)].size() <= IntegrationPointIndex)
                    << "No existing integration point" << std::endl;

                return mShapeFunctionsLocalGradients[static_cast<int>(ThisMethod)][IntegrationPointIndex];
            }

            /**
             * @brief 获取形函数在指定积分点、指定导数阶的导数值
             * @param DerivativeOrderIndex 指定的导数阶
             * @param IntegrationPointIndex 指定的积分点索引
             * @param ThisMethod 指定的积分方法
             */
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

            /**
             * @brief 获取形函数在指定积分点、指定导数阶和方向上的导数值
             * @param IntegrationPointIndex 指定的积分点索引
             * @param DerivativeOrderIndex 指定的导数阶
             * @param DerivativeOrderRowIndex 指定的导数方向
             * @param ShapeFunctionIndex 指定的形函数索引
             * @param ThisMethod 指定的积分方法
             */
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
            /**
             * @brief 默认积分方法
             * @details IntegrationMethod为枚举类
             */
            IntegrationMethod mDefaultMethod;

            /**
             * @brief 存储所有积分方法的积分点的数组
             * @details std::array<
             *     std::vector<IntegrationPoint<3>>, 
             *     static_cast<int>(IntegrationMethod::NumberOfIntegrationMethods)
             * >
             */
            IntegrationPointsContainerType mIntegrationPoints;

            /**
             * @brief 存储单元型函数在所有积分点的计算值（局部坐标系）
             * @details std::array<Matrix, static_cast<int>(IntegrationMethod::NumberOfIntegrationMethods)>
             *   以四边形单元为例，矩阵的值如下：
             *   积分点索引   |   形函数1的值   |   形函数2的值   |   形函数3的值   |   形函数4的值   |
             *       1      |     0.622      |     0.166     |     0.044     |     0.166     |       
             *       2      |     0.166      |     0.622     |     0.166     |     0.044     |       
             *       3      |     0.044      |     0.166     |     0.622     |     0.166     |       
             *       4      |     0.166      |     0.044     |     0.166     |     0.622     |  
             *  Matrix 乘 节点位移向量 [u1, u2, u3, u4]T 可以获得所有积分点的位移值     
             */
            ShapeFunctionsValuesContainerType mShapeFunctionsValues;

            /**
             * @brief 存储形函数对于单元局部坐标的导数值（一阶导数）
             * @details std::array<DenseVector<Matrix>, static_cast<int>(IntegrationMethod::NumberOfIntegrationMethods)>
             *  应变-位移矩阵（形函数一阶导数矩阵）
             *  ∂Ni   ∂Ni
             *  --- = ---
             *  ∂x    ∂ξj
             *  DenseVector<Matrix> 存储的是每个积分点的形函数一阶导数矩阵的值，Matrix乘节点位移可获得该积分点的应变
             */
            ShapeFunctionsLocalGradientsContainerType mShapeFunctionsLocalGradients;

            /**
             * @brief 存储形函数对于单元局部坐标的导数值（高阶导数，最小为二阶导数）
             * @details std::array<
             *     DenseVector<DenseVector<Matrix>>, 
             *     static_cast<int>(IntegrationMethod::NumberOfIntegrationMethods)
             * >
             */
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