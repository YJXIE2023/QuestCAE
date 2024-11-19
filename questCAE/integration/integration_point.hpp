#ifndef QUEST_INTEGRATION_POINT_HPP
#define QUEST_INTEGRATION_POINT_HPP

// 项目头文件
#include "includes/define.hpp"
#include "geometries/point.hpp"

namespace Quest{

    /**
     * @class IntegrationPoint
     * @brief 定义了IntegrationPoint类，用于描述单元格的积分点
     *        该类继承自Point类，并添加了权重属性mWeight
     * @tparam TDimension 维度
     * @tparam TDataType 点数据类型
     * @tparam TWeightType 权重类型
     */
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


            /**
             * @brief 默认构造函数
             */
            IntegrationPoint():BaseType(), mWeight(0.0){};


            /**
             * @brief 构造函数
             * @param NewX 点的x坐标
             */
            explicit IntegrationPoint(const TDataType& NewX): BaseType(NewX), mWeight(0.0){};


            /**
             * @brief 构造函数
             * @param NewX 点的x坐标
             * @param NewW 权重
             */
            IntegrationPoint(const TDataType& NewX, const TDataType& NewW): BaseType(NewX), mWeight(NewW){};


            /**
             * @brief 构造函数
             * @param NewX 点的x坐标
             * @param NewY 点的y坐标
             * @param NewW 点的权重
             */
            IntegrationPoint(const TDataType& NewX, const TDataType& NewY, const TDataType& NewW): BaseType(NewX, NewY), mWeight(NewW){};


            /**
             * @brief 构造函数
             * @param NewX 点的x坐标
             * @param NewY 点的y坐标
             * @param NewZ 点的z坐标
             * @param NewW 点的权重
             */
            IntegrationPoint(const TDataType& NewX, const TDataType& NewY, const TDataType& NewZ, const TDataType& NewW): BaseType(NewX, NewY, NewZ), mWeight(NewW){};


            /**
             * @brief 复制构造函数
             */
            IntegrationPoint(const IntegrationPoint& rOtherIntegrationPoint): 
                BaseType(rOtherIntegrationPoint), mWeight(rOtherIntegrationPoint.mWeight){};


            /**
             * @brief 复制构造函数
             * @tparam TOtherDimension 维度
             */
            template<std::size_t TOtherDimension>
            IntegrationPoint(const IntegrationPoint<TOtherDimension, TDataType, TWeightType>& rOtherIntegrationPoint): 
                BaseType(rOtherIntegrationPoint), mWeight(rOtherIntegrationPoint.mWeight){};


            /**
             * @brief 构造函数
             * @param rOtherPoint 点对象
             */
            explicit IntegrationPoint(const PointType& rOtherPoint):
                BaseType(rOtherPoint), mWeight(){};


            /**
             * @brief 构造函数
             * @param rOtherPoint 点对象
             * @param NewWeight 权重
             */
            IntegrationPoint(const PointType& rOtherPoint, TWeightType NewWeight):
                BaseType(rOtherPoint), mWeight(NewWeight){};


            /**
             * @brief 构造函数
             * @param rOtherCoordinates 点坐标数组(Array1d<doble,TDimension>类型)
             */
            explicit IntegrationPoint(const CoordinatesArrayType& rOtherCoordinates):
                BaseType(rOtherCoordinates), mWeight(){};


            /**
             * @brief 构造函数
             * @param rOtherCoordinates 点坐标数组(Array1d<doble,TDimension>类型)
             * @param NewWeight 权重
             */
            IntegrationPoint(const CoordinatesArrayType& rOtherCoordinates, TWeightType NewWeight):
                BaseType(rOtherCoordinates), mWeight(NewWeight){};


            /**
             * @brief 构造函数
             * @param rOtherCoordinates 向量表达式类型
             */
            template<typename TVectorType>
            explicit IntegrationPoint(const vector_expression<TVectorType>& rOtherCoordinates):
                BaseType(rOtherCoordinates), mWeight(){};


            /**
             * @brief 构造函数
             * @param rOtherCoordinates 向量表达式类型
             * @param NewWeight 权重
             */
            template<typename TVectorType>
            IntegrationPoint(const vector_expression<TVectorType>& rOtherCoordinates, TWeightType NewWeight):
                BaseType(rOtherCoordinates), mWeight(NewWeight){};


            /**
             * @brief 构造函数
             * @param rOtherCoordinates 标准库中Vector类型
             */
            explicit IntegrationType(const std::vector<TDataType>& rOtherCoordinates):
                BaseType(rOtherCoordinates), mWeight(){};


            /**
             * @brief 构造函数
             * @param rOtherCoordinates 标准库中Vector类型
             * @param NewWeight 权重
             */
            IntegrationType(const std::vector<TDataType>& rOtherCoordinates, TWeightType NewWeight):
                BaseType(rOtherCoordinates), mWeight(NewWeight){};


            /**
             * @brief 析构函数
             */
            ~IntegrationType() override {}


            /**
             * @brief 赋值运算符
             */
            IntegrationPoint& operator = (const IntegrationPoint& rOtherIntegrationPoint){
                BaseType::operator=(rOtherIntegrationPoint);
                mWeight = rOtherIntegrationPoint.mWeight;
                return *this;
            }


            /**
             * @brief 赋值运算符
             * @param rOtherPoint 点对象
             */
            IntegrationPoint& operator = (const PointType& rOtherPoint){
                BaseType::operator=(rOtherPoint);
                return *this;
            }


            /**
             * @brief 比较运算符
             * 判断两个积分点的权重是否相等
             */
            bool operator == (const IntegrationPoint& rOther){
                return (BaseType::operator==(rOther) && mWeight == rOther.mWeight);
            }


            /**
             * @brief 赋值运算符
             */
            template<std::size_t TOtherDimension>
            IntegrationPoint& operator = (const IntegrationPoint<TOtherDimension>& rOther){
                BaseType::operator=(rOther);
                mWeight = rOther.mWeight;

                return *this;
            }


            /**
             * @brief 返回权重
             */
            TWeightType Wight() const{
                return mWeight;
            }


            /**
             * @brief 返回权重
             */
            TWeightType& Wight(){
                return mWeight;
            }


            /**
             * @brief 设置权重
             */
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


            /**
             * @brief 序列化函数
             */
            void save(Serializer& rSerializer) const override{
                QUEST_SEILIZER_SAVE_BASE_CLASS(rSerializer, BaseType);
                rSerializer.save("Weight", mWeight);
            }


            /**
             * @brief 反序列化函数
             */
            void load(Serializer& rSerializer) override{
                QUEST_SEILIZER_LOAD_BASE_CLASS(rSerializer, BaseType);
                rSerializer.load("Weight", mWeight);
            }

        private:
            /**
             * @brief 权重
             */
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