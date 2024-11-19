#ifndef QUEST_INTEGRATION_INFO_HPP
#define QUEST_INTEGRATION_INFO_HPP

// 项目头文件
#include "includes/define.hpp"
#include "geometries/geometry_data.hpp"
#include "container/data_value_container.hpp"
#include "container/flags.hpp"

namespace Quest{

    /**
     * @class IntegrationInfo
     * @brief 存储和处理关于单元积分的相关信息
     * @note 单元类中读取单元积分信息函数（创建积分点函数）中返回值（参数值）的类型
     */
    class QUEST_API(QUEST_CORE) IntegrationInfo: public Flags{
        public:
            /**
             * @brief 创建intrusive_ptr及其他智能指针的类型定义
             */
            QUEST_CLASS_INTRUSIVE_POINTER_DEFINITION(IntegrationInfo);

            using SizeType = typename Point::IndexType;
            using IndexType = typename Point::IndexType;
            using IntegrationMethod = GeometryData::IntegrationMethod;

            /**
             * @brief 本地标签，表示是否在从属单元上创建积分点
             */
            QUEST_DEFINE_LOCAL_FLAG(DO_NOT_CREATE_TESSELLATION_ON_SLAVE);

            /**
             * @enum QuadratureMethod
             * @brief 积分方法
             */
            enum class QuadratureMethod{
                Default,            ///< 默认积分方法
                GAUSS,              ///< 高斯积分
                EXTENDED_GAUSS,     ///< 扩展高斯积分
                GRID                ///< 网格积分
            };

        public:
            /**
             * @brief 构造函数
             * @param LocalSpaceDimension 单元维度
             * @param ThisIntegrationMethod 单元积分方法
             */
            IntegrationInfo(SizeType LocalSpaceDimension, IntegrationMethod ThisIntegrationMethod);


            /**
             * @brief 构造函数
             * @param LocalSpaceDimension 单元维度
             * @param NumberOfIntegrationPointsPerSpan 单元每个维度的积分点数量
             * @param ThisQuadratureMethod 单元积分方法
             */
            IntegrationInfo(SizeType LocalSpaceDimension, SizeType NumberOfIntegrationPointsPerSpan, QuadratureMethod ThisQuadratureMethod = QuadratureMethod::GAUSS);


            /**
             * @brief 构造函数
             * @param NumberOfIntegrationPointsPerSpanVector 单元每个维度的积分点数量
             * @param ThisQuadratureMethodVector 单元每个维度的积分方法
             */
            IntegrationInfo(
                const std::vector<SizeType>& NumberOfIntegrationPointsPerSpanVector,
                const std::vector<QuadratureMethod>& ThisQuadratureMethodVector
            );


            /**
             * @brief 获取单元维度
             */
            SizeType LocalSpaceDimension(){
                return mNumberOfIntegrationPointsPerSpanVector.size();
            }


            /**
             * @brief 设置某个维度的积分方法
             * @param DimensionIndex 维度索引
             * @param ThisIntegrationMethod 积分方法
             */
            void SetIntegrationMethod(IndexType DimensionIndex, IntegrationMethod ThisIntegrationMethod);


            /**
             * @brief 获取某个维度维度的积分点数量
             * @param DimensionIndex 维度索引
             */
            SizeType GetNumberOfIntegrationPointsPerSpan(IndexType DimensionIndex) const;


            /**
             * @brief 设置某个维度的积分点数量
             * @param DimensionIndex 维度索引
             * @param NumberOfIntegrationPointsPerSpan 积分点数量
             */
            void SetNumberOfIntegrationPointsPerSpan(IndexType DimensionIndex, SizeType NumberOfIntegrationPointsPerSpan);

            QuadratureMethod GetQuadratureMethod(IndexType DimensionIndex) const{
                return mQuadratureMethodVector[DimensionIndex];
            }


            /**
             * @brief 设置某个维度的积分方法
             * @param DimensionIndex 维度索引
             * @param ThisQuadratureMethod 积分方法
             */
            void SetQuadratureMethod(IndexType DimensionIndex, QuadratureMethod ThisQuadratureMethod);


            /**
             * @brief 获取某个维度的积分方法
             * @param DimensionIndex 维度索引
             */
            IntegrationMethod GetIntegrationMethod(IndexType DimensionIndex) const;


            /**
             * @brief 依据维度积分点数量获取该维度对应次数的积分方法
             * @param NumberOfIntegrationPointsPerSpan 积分点数量
             * @param ThisQuadratureMethod 数值求积分方法
             * @return 积分方法
             */
            static IntegrationMethod GetIntegrationMethod(SizeType NumberOfIntegrationPointsPerSpan, QuadratureMethod ThisQuadratureMethod){
                switch(NumberOfIntegrationPointsPerSpan){
                    case 1:
                        if(ThisQuadratureMethod == QuadratureMethod::GAUSS){
                            return IntegrationMethod::GI_GAUSS_1;
                        } else {
                            return IntegrationMethod::GI_EXTENDED_GAUSS_1;
                        }
                        break;
                    case 2:
                        if(ThisQuadratureMethod == QuadratureMethod::GAUSS){
                            return IntegrationMethod::GI_GAUSS_2;
                        } else {    
                            return IntegrationMethod::GI_EXTENDED_GAUSS_2;
                        }
                        break;
                    case 3:
                        if(ThisQuadratureMethod == QuadratureMethod::GAUSS){
                            return IntegrationMethod::GI_GAUSS_3;
                        } else {
                            return IntegrationMethod::GI_EXTENDED_GAUSS_3;
                        }
                        break;
                    case 4:
                        if(ThisQuadratureMethod == QuadratureMethod::GAUSS){
                            return IntegrationMethod::GI_GAUSS_4;
                        } else {
                            return IntegrationMethod::GI_EXTENDED_GAUSS_4;
                        }
                        break;
                    case 5:
                        if(ThisQuadratureMethod == QuadratureMethod::GAUSS){
                            return IntegrationMethod::GI_GAUSS_5;
                        } else {
                            return IntegrationMethod::GI_EXTENDED_GAUSS_5;
                        }
                        break;
                    case 0:
                        return IntegrationMethod::NumberOfIntegrationMethods;
                        break;
                }
                QUEST_WARNING("Evaluation of Integration Method")
                    << "Chosen combination of number of points per span and quadrature method does not has a corresponding IntegrationMethod in the KRATOS core."
                    << "NumberOfIntegrationPointsPerSpan: " << NumberOfIntegrationPointsPerSpan << std::endl;
                return IntegrationMethod::NumberOfIntegrationMethods;
            }


            /**
             * @brief 类信息函数
             */
            std::string Info() const override{
                std::stringstream buffer;
                buffer << " Integration info with local space dimension: " << mNumberOfIntegrationPointsPerSpanVector.size()
                    << " and number of integration points per spans: " << mNumberOfIntegrationPointsPerSpanVector;
                return buffer.str();
            }


            /**
             * @brief 类信息打印函数
             */
            void PrintInfo(std::ostream& rOstream) const override{
                rOstream << this->Info();
            }


            /**
             * @brief 数据打印函数
             */
            void PrintData(std::ostream& rOstream) const override{
                rOstream << " Integration info with local space dimension: " << mNumberOfIntegrationPointsPerSpanVector.size()
                    << " and number of integration points per spans: " << mNumberOfIntegrationPointsPerSpanVector;
            }

        protected:

        private:

        private:
            /**
             * @brief 存储每个维度的积分点数量
             */
            std::vector<SizeType> mNumberOfIntegrationPointsPerSpanVector;

            /**
             * @brief 存储每个维度的积分方法
             */
            std::vector<QuadratureMethod> mQuadratureMethodVector;

    };

    /**
     * @brief 重载输入输出流运算符
     */
    template<std::size_t TDimension, typename TDataType, typename TWeightType>
    inline std::istream& operator >> (std::istream& rIstream, IntegrationInfo& rThis);

    /**
     * @brief 重载输入输出流运算符
     */
    template<std::size_t TDimension, typename TDataType, typename TWeightType>
    inline std::ostream& operator << (std::ostream& rOstream, const IntegrationInfo& rThis){
        rThis.PrintInfo(rOstream);
        rThis.PrintData(rOstream);

        return rOstream;
    }

} // namespace Quest

#endif //QUEST_INTEGRATION_INFO_HPP