/*---------------------------------
存储和处理与积分点相关的信息
---------------------------------*/

#ifndef QUEST_INTEGRATION_INFO_HPP
#define QUEST_INTEGRATION_INFO_HPP

// 项目头文件
#include "includes/define.hpp"
#include "geometries/geometry_data.hpp"
#include "container/data_value_container.hpp"
#include "container/flags.hpp"

namespace Quest{

    class QUEST_API(QUEST_CORE) IntegrationInfo: public Flags{
        public:
            QUEST_CLASS_INTRUSIVE_POINTER_DEFINITION(IntegrationInfo);

            using SizeType = typename Point::IndexType;
            using IndexType = typename Point::IndexType;
            using IntegrationMethod = GeometryData::IntegrationMethod;

            QUEST_DEFINE_LOCAL_FLAG(DO_NOT_CREATE_TESSELLATION_ON_SLAVE);

            enum class QuadratureMethod{
                Default,
                GAUSS,
                EXTENDED_GAUSS,
                GRID
            };

        public:
            IntegrationInfo(SizeType LocalSpaceDimension, IntegrationMethod ThisIntegrationMethod);

            IntegrationInfo(SizeType LocalSpaceDimension, SizeType NumberOfIntegrationPointsPerSpan, QuadratureMethod ThisQuadratureMethod = QuadratureMethod::GAUSS);

            IntegrationInfo(
                const std::vector<SizeType>& NumberOfIntegrationPointsPerSpanVector,
                const std::vector<QuadratureMethod>& ThisQuadratureMethodVector
            );

            SizeType LocalSpaceDimension(){
                return mNumberOfIntegrationPointsPerSpanVector.size();
            }

            void SetIntegrationMethod(IndexType DimensionIndex, IntegrationMethod ThisIntegrationMethod);

            SizeType GetNumberOfIntegrationPointsPerSpan(IndexType DimensionIndex) const;

            void SetNumberOfIntegrationPointsPerSpan(IndexType DimensionIndex, SizeType NumberOfIntegrationPointsPerSpan);

            QuadratureMethod GetQuadratureMethod(IndexType DimensionIndex) const{
                return mQuadratureMethodVector[DimensionIndex];
            }

            void SetQuadratureMethod(IndexType DimensionIndex, QuadratureMethod ThisQuadratureMethod);

            IntegrationMethod GetIntegrationMethod(IndexType DimensionIndex) const;

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

            std::string Info() const override{
                std::stringstream buffer;
                buffer << " Integration info with local space dimension: " << mNumberOfIntegrationPointsPerSpanVector.size()
                    << " and number of integration points per spans: " << mNumberOfIntegrationPointsPerSpanVector;
                return buffer.str();
            }

            void PrintInfo(std::ostream& rOstream) const override{
                rOstream << this->Info();
            }

            void PrintData(std::ostream& rOstream) const override{
                rOstream << " Integration info with local space dimension: " << mNumberOfIntegrationPointsPerSpanVector.size()
                    << " and number of integration points per spans: " << mNumberOfIntegrationPointsPerSpanVector;
            }

        protected:

        private:

        private:
            std::vector<SizeType> mNumberOfIntegrationPointsPerSpanVector;
            std::vector<QuadratureMethod> mQuadratureMethodVector;

    };

    template<std::size_t TDimension, typename TDataType, typename TWeightType>
    inline std::istream& operator >> (std::istream& rIstream, IntegrationInfo& rThis);

    template<std::size_t TDimension, typename TDataType, typename TWeightType>
    inline std::ostream& operator << (std::ostream& rOstream, const IntegrationInfo& rThis){
        rThis.PrintInfo(rOstream);
        rThis.PrintData(rOstream);

        return rOstream;
    }

} // namespace Quest

#endif //QUEST_INTEGRATION_INFO_HPP