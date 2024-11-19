#ifndef QUEST_QUADRATURE_HPP
#define QUEST_QUADRATURE_HPP

// 项目头文件
#include "integration/integration_point.hpp"

namespace Quest{

    template<std::size_t TOrder>
    struct Pow{
        static inline std::size_t Value(std::size_t X){
            return X*Pow<TOrder-1>::Value(X);
        }
    };

    template<>
    struct Pow<1>{
        static inline std::size_t Value(std::size_t X){
            return X;
        }
    };
    

    /**
     * @brief 基类模板，用于管理等效积分点
     * @tparam TQuadraturePointsType 管理等效积分点的类型
     * @tparam TDimension 积分维度
     * @tparam TIntegrationPointType 积分点类型
     */
    template<typename TQuadraturePointsType, std::size_t TDimension = TQuadraturePointsType::Dimension, typename TIntegrationPointType = IntegrationPoint<TDimension>>
    class Quadrature{
        public:
            QUEST_CLASS_POINTER_DEFINITION(Quadrature);

            /**
             * @brief 等效积分点的类型
             */
            using IntegrationPointType = TIntegrationPointType;

            /**
             * @brief 存储等效积分点的数组
             */
            using IntegrationPointsArrayType = std::vector<IntegrationPointType>;

            /**
             * @brief 初始化等效积分点数组
             */
            using InitialIntegrationPointsArrayType = typename TQuadraturePointsType::IntegraaationPointsArrayType;

            /**
             * @brief 积分点中的点类型
             */
            using PointType = typename IntegrationPointType::PointType;

            using SizeType = std::size_t;
            using IndexType = std::size_t;

        public:
            /**
             * @brief 默认构造函数
             */
            Quadrature() {}


            /**
             * @brief 复制构造函数
             */
            Quadrature(const Quadrature& rOther) {}


            /**
             * @brief 析构函数
             */
            virtual ~Quadrature() {}


            /**
             * @brief 重载赋值运算符
             */
            Quadrature& operator=(const Quadrature& rOther) {}


            /**
             * @brief 获取等效积分点的数量
             */
            static SizeType IntegrationPointsNumber(){
                return Pow<TDimension - TQuadraturePointsType::Dimension +1>::Value(TQuadraturePointsType::IntegrationPointsNumber());
            }


            /**
             * @brief 获取所有等效积分点
             */
            static const IntegrationPointsArrayType& IntegrationPoints(){
                return msIntegrationPointsVector;
            }


            /**
             * @brief 通用的获取等效积分点的函数接口
             */
            static const IntegrationPointsArrayType GenerateIntegrationPoints(){
                IntegrationPointsArrayType results;
                IntegrationPoint(results, Quadrature<TQuadraturePointsType, TDimension, TIntegrationPointType>());
                return results;
            }


            /**
             * @brief 获取等效积分点（等效积分点类型的维度 = 待求积分维度）
             */
            static void IntegrationPoints(
                IntegrationPointsArrayType& Result,
                const Quadrature<TQuadraturePointsType, TQuadraturePointsType::Dimension, TIntegrationPointType>& Dummy
            ){
                const auto integration_points = TQuadraturePointsType::IntegrationPoints();
                for(SizeType i = 0; i < TQuadraturePointsType::IntegrationPointsNumber(); ++i){
                    Result.push_back(integration_points[i]);
                }
            }


            virtual std::string Info() const{
                std::stringstream buffer;
                buffer << TDimension << " dimensional quadrature with " << IntegrationPointsNumber() << " integration points";
                return buffer.str();
            }


            virtual void PrintInfo(std::ostream& rOStream) const{
                rOStream << Info();
            }


            virtual void PrintData(std::ostream& rOStream) const{
                IndexType i;
                for(i = 0; i < msIntegrationPointsVector.size()-1; ++i){
                    rOStream << msIntegrationPointsVector[i] << " , " << std::endl;
                }
                rOStream << msIntegrationPointsVector[i];
            }


            template<typename TOtherQuadraturePointsType, std::size_t TOtherDimension, typename TOtherIntegrationPointType>
            friend class Quadrature;

        protected:

        private:

        private:
            /**
             * @brief 等效积分点数组
             */
            static const IntegrationPointsArrayType msIntegrationPointsVector;

    };


    #ifdef __SUNPRO_CC
    template<typename TQuadraturePointsType, std::size_t TDimension, typename TIntegrationPointType>
    const typename Quadrature<TQuadraturePointsType, TDimension, TIntegrationPointType>::IntegrationPointsArrayType
    Quadrature<TQuadraturePointsType, TDimension, TIntegrationPointType>::msIntegrationPointsVector = 
        Quadrature<TQuadraturePointsType, TDimension, TIntegrationPointType>::IntegrationPoints();
    #else
    template<typename TQuadraturePointsType, std::size_t TDimension, typename TIntegrationPointType>
    const typename Quadrature<TQuadraturePointsType, TDimension, TIntegrationPointType>::IntegrationPointsArrayType
    Quadrature<TQuadraturePointsType, TDimension, TIntegrationPointType>::msIntegrationPointsVector(
        Quadrature<TQuadraturePointsType, TDimension, TIntegrationPointType>::GenerateIntegrationPoints()
    )
    #endif


    template<typename TQuadraturePointsType, std::size_t TDimension, typename TIntegrationPointType>
    inline std::istream& operator >> (std::istream& rIStream, Quadrature<TQuadraturePointsType, TDimension, TIntegrationPointType>& rThis);


    template<typename TQuadraturePointsType, std::size_t TDimension, typename TIntegrationPointType>
    inline std::ostream& operator << (std::ostream& rOStream, const Quadrature<TQuadraturePointsType, TDimension, TIntegrationPointType>& rThis){
        rThis.PrintInfo(rOStream);
        rOStream << std::endl;
        rThis.PrintData(rOStream);

        return rOStream;
    }

} // namespace Quest

#endif //QUEST_QUADRATURE_HPP