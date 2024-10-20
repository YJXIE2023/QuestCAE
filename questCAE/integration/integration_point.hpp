/*----------------------------------
积分点类，描述有限元单元中的积分点信息
用于定义积分过程所需的权重和位置
----------------------------------*/

#ifndef QUEST_INTEGRATION_POINT_HPP
#define QUEST_INTEGRATION_POINT_HPP

// 项目头文件
#include "includes/define.hpp"
#include "geometries/point.hpp"

namespace Quest{

    template<std::size_t TDimension, typename TDataType = double, typename TWeightType = double>
    class IntegrationPoint: public Point{
        public:
            QUEST_CLASS_POINTER_DEFINITION(IntegrationPoint);

            using BaseType = Point;
            using PointType = Point;
            using CoordinatesArrayType = Point::CoordinatesArrayType;
            using IndexType = Point::IndexType;

            template<std::size_t TOtherDimension, typename TOtherDataType, typename TOtherWeightType>
            friend class IntegrationPoint;

            IntegrationPoint():BaseType(), mWeight(0.0){};

            explicit IntegrationPoint(const TDataType& NewX): BaseType(NewX), mWeight(0.0){};

            IntegrationPoint(const TDataType& NewX, const TDataType& NewW): BaseType(NewX), mWeight(NewW){};

            IntegrationPoint(const TDataType& NewX, const TDataType& NewY, const TDataType& NewW): BaseType(NewX, NewY), mWeight(NewW){};

            IntegrationPoint(const TDataType& NewX, const TDataType& NewY, const TDataType& NewZ, const TDataType& NewW): BaseType(NewX, NewY, NewZ), mWeight(NewW){};

            IntegrationPoint(const IntegrationPoint& rOtherIntegrationPoint): 
                BaseType(rOtherIntegrationPoint), mWeight(rOtherIntegrationPoint.mWeight){};

            template<std::size_t TOtherDimension>
            IntegrationPoint(const IntegrationPoint<TOtherDimension, TDataType, TWeightType>& rOtherIntegrationPoint): 
                BaseType(rOtherIntegrationPoint), mWeight(rOtherIntegrationPoint.mWeight){};

            explicit IntegrationPoint(const PointType& rOtherPoint):
                BaseType(rOtherPoint), mWeight(){};

            IntegrationPoint(const PointType& rOtherPoint, TWeightType NewWeight):
                BaseType(rOtherPoint), mWeight(NewWeight){};

            explicit IntegrationPoint(const CoordinatesArrayType& rOtherCoordinates):
                BaseType(rOtherCoordinates), mWeight(){};

            IntegrationPoint(const CoordinatesArrayType& rOtherCoordinates, TWeightType NewWeight):
                BaseType(rOtherCoordinates), mWeight(NewWeight){};

            template<typename TVectorType>
            explicit IntegrationPoint(const vector_expression<TVectorType>& rOtherCoordinates):
                BaseType(rOtherCoordinates), mWeight(){};

            template<typename TVectorType>
            IntegrationPoint(const vector_expression<TVectorType>& rOtherCoordinates, TWeightType NewWeight):
                BaseType(rOtherCoordinates), mWeight(NewWeight){};

            explicit IntegrationType(const std::vector<TDataType>& rOtherCoordinates):
                BaseType(rOtherCoordinates), mWeight(){};

            IntegrationType(const std::vector<TDataType>& rOtherCoordinates, TWeightType NewWeight):
                BaseType(rOtherCoordinates), mWeight(NewWeight){};

            ~IntegrationType() override {}

            IntegrationPoint& operator = (const IntegrationPoint& rOtherIntegrationPoint){
                BaseType::operator=(rOtherIntegrationPoint);
                mWeight = rOtherIntegrationPoint.mWeight;
                return *this;
            }

            IntegrationPoint& operator = (const PointType& rOtherPoint){
                BaseType::operator=(rOtherPoint);
                return *this;
            }

            bool operator == (const IntegrationPoint& rOther){
                return (BaseType::operator==(rOther) && mWeight == rOther.mWeight);
            }

            template<std::size_t TOtherDimension>
            IntegrationPoint& operator = (const IntegrationPoint<TOtherDimension& rOther){
                BaseType::operator=(rOther);
                mWeight = rOther.mWeight;

                return *this;
            }

            TWeightType Wight() const{
                return mWeight;
            }

            TWeightType& Wight(){
                return mWeight;
            }

            void SetWeight(TWeightType NewWeight){
                mWeight = NewWeight;
            }

            std::string Info() const override{
                std::stringstream buffer;
                buffer << TDimension << " dimendional integration point";
                return buffer.str();
            }

            void PrintInfo(std::ostream& rOstream) const override{
                rOstream << this->Info();
            }

            void PrintData(std::ostream& rOstream) const override{
                if(!TDimension){
                    return;
                }

                rOstream << "(" << this->operator[](0);
                for(IndexType i = 1; i < TDimension; ++i){
                    rOstream << " , " << this->operator[](i);
                }

                rOstream << "), weight = " << mWeight;
            }

        protected:

        private:
            friend class Serializer;

            void save(Serializer& rSerializer) const override{
                QUEST_SEILIZER_SAVE_BASE_CLASS(rSerializer, BaseType);
                rSerializer.save("Weight", mWeight);
            }

            void load(Serializer& rSerializer) override{
                QUEST_SEILIZER_LOAD_BASE_CLASS(rSerializer, BaseType);
                rSerializer.load("Weight", mWeight);
            }

        private:
            TWeightType mWeight;

    };

    template<std::size_t TDimension, typename TDataType, typename TWeightType>
    inline std::istream& operator >> (std::istream& rIstream, IntegrationPoint<TDimension, TDataType, TWeightType>& rThis);

    template<std::size_t TDimension, typename TDataType, typename TWeightType>
    inline std::ostream& operator << (std::ostream& rOstream, const IntegrationPoint<TDimension, TDataType, TWeightType>& rThis){
        rThis.PrintInfo(rOstream);
        rThis.PrintData(rOstream);

        return rOstream;
    }

} // namespace Quest

#endif //QUEST_INTEGRATION_POINT_HPP