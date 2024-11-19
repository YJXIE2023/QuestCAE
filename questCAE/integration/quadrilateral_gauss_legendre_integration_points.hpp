#ifndef QUEST_QUADRILATERAL_GAUSS_LEGENDRE_INTEGRATION_POINTS_HPP
#define QUEST_QUADRILATERAL_GAUSS_LEGENDRE_INTEGRATION_POINTS_HPP

// 项目头文件
#include "integration/quadrature.hpp"

namespace Quest{

    /**
     * @class QuadrilateralGaussLegendreIntegrationPoints1
     * @brief 管理二维正方形的Gauss-Legendre等效积分点（一次）
     */
    class QuadrilateralGaussLegendreIntegrationPoints1{
        public:
            QUEST_CLASS_POINTER_DEFINITION(QuadrilateralGaussLegendreIntegrationPoints1);

            static const unsigned int Dimension = 2;

            using IntegrationPointType = IntegrationPoint<2>;

            using IntegrationPointsArrayType = std::array<IntegrationPointType, 1>;

            static std::size_t IntegrationPointsNumber(){
                return 1;
            }


            static const IntegrationPointsArrayType& IntegrationPoints(){
                static const IntegrationPointsArrayType s_integration_points = {{
                    IntegrationPointType(0.0, 0.0, 4.0)
                }};
                return s_integration_points;
            }


            std::string Info() const{
                std::stringstream buffer;
                buffer << "Quadrilateral Gauss-Legendre quadrature 1 ";
                return buffer.str();
            }
    };


    /**
     * @class QuadrilateralGaussLegendreIntegrationPoints2
     * @brief 管理二维正方形的Gauss-Legendre等效积分点（二次）
     */
    class QuadrilateralGaussLegendreIntegrationPoints2{
        public:
            QUEST_CLASS_POINTER_DEFINITION(QuadrilateralGaussLegendreIntegrationPoints2);

            static const unsigned int Dimension = 2;

            using IntegrationPointType = IntegrationPoint<2>;

            using IntegrationPointsArrayType = std::array<IntegrationPointType, 4>;

            static std::size_t IntegrationPointsNumber(){
                return 4;
            }


            static const IntegrationPointsArrayType& IntegrationPoints(){
                static const IntegrationPointsArrayType s_integration_points = {{
                    IntegrationPointType( -1.00/std::sqrt(3.0) , -1.00/std::sqrt(3.0), 1.00 ),
                    IntegrationPointType(  1.00/std::sqrt(3.0) , -1.00/std::sqrt(3.0), 1.00 ),
                    IntegrationPointType(  1.00/std::sqrt(3.0) ,  1.00/std::sqrt(3.0), 1.00 ),
                    IntegrationPointType( -1.00/std::sqrt(3.0) ,  1.00/std::sqrt(3.0), 1.00 )
                }};
                return s_integration_points;
            }


            std::string Info() const{
                std::stringstream buffer;
                buffer << "Quadrilateral Gauss-Legendre quadrature 2 ";
                return buffer.str();
            }
    };


    /**
     * @class QuadrilateralGaussLegendreIntegrationPoints3
     * @brief 管理二维正方形的Gauss-Legendre等效积分点（三次）
     */
    class QuadrilateralGaussLegendreIntegrationPoints3{
        public:
            QUEST_CLASS_POINTER_DEFINITION(QuadrilateralGaussLegendreIntegrationPoints2);

            static const unsigned int Dimension = 2;

            using IntegrationPointType = IntegrationPoint<2>;

            using IntegrationPointsArrayType = std::array<IntegrationPointType, 9>;

            static std::size_t IntegrationPointsNumber(){
                return 9;
            }


            static const IntegrationPointsArrayType& IntegrationPoints(){
                static const IntegrationPointsArrayType s_integration_points = {{
                    IntegrationPointType( -std::sqrt(3.00/5.00) , -std::sqrt(3.00/5.00), 25.00/81.00 ),
                    IntegrationPointType( 0.00 , -std::sqrt(3.00/5.00), 40.00/81.00 ),
                    IntegrationPointType(  std::sqrt(3.00/5.00) , -std::sqrt(3.00/5.00), 25.00/81.00 ),
                    IntegrationPointType( -std::sqrt(3.00/5.00), 0.00, 40.00/81.00 ),
                    IntegrationPointType( 0.00 , 0.00, 64.00/81.00 ),
                    IntegrationPointType( std::sqrt(3.00/5.00), 0.00, 40.00/81.00 ),
                    IntegrationPointType( -std::sqrt(3.00/5.00), std::sqrt(3.00/5.00), 25.00/81.00 ),
                    IntegrationPointType( 0.00, std::sqrt(3.00/5.00), 40.00/81.00 ),
                    IntegrationPointType( std::sqrt(3.00/5.00), std::sqrt(3.00/5.00), 25.00/81.00 )
                }};
                return s_integration_points;
            }


            std::string Info() const{
                std::stringstream buffer;
                buffer << "Quadrilateral Gauss-Legendre quadrature 3 ";
                return buffer.str();
            }
    };


    /**
     * @class QuadrilateralGaussLegendreIntegrationPoints4
     * @brief 管理二维正方形的Gauss-Legendre等效积分点（四次）
     */
    class QuadrilateralGaussLegendreIntegrationPoints4{
        public:
            QUEST_CLASS_POINTER_DEFINITION(QuadrilateralGaussLegendreIntegrationPoints4);

            static const unsigned int Dimension = 2;

            using IntegrationPointType = IntegrationPoint<2>;

            using IntegrationPointsArrayType = std::array<IntegrationPointType, 16>;

            static std::size_t IntegrationPointsNumber(){
                return 16;
            }


            static const IntegrationPointsArrayType& IntegrationPoints(){
                static const IntegrationPointsArrayType s_integration_points = {{
                    IntegrationPointType( -std::sqrt( (3.0 + 2.0 * std::sqrt(6.0/5.0)) / 7.0 ), -std::sqrt( (3.0 + 2.0 * std::sqrt(6.0/5.0)) / 7.0 ), (0.5 - std::sqrt(5.0/6.0)/6.0)*(0.5 - std::sqrt(5.0/6.0)/6.0)),
                    IntegrationPointType( -std::sqrt( (3.0 + 2.0 * std::sqrt(6.0/5.0)) / 7.0 ), -std::sqrt( (3.0 - 2.0 * std::sqrt(6.0/5.0)) / 7.0 ), (0.5 - std::sqrt(5.0/6.0)/6.0)*(0.5 + std::sqrt(5.0/6.0)/6.0)),
                    IntegrationPointType( -std::sqrt( (3.0 + 2.0 * std::sqrt(6.0/5.0)) / 7.0 ),  std::sqrt( (3.0 - 2.0 * std::sqrt(6.0/5.0)) / 7.0 ), (0.5 - std::sqrt(5.0/6.0)/6.0)*(0.5 + std::sqrt(5.0/6.0)/6.0)),
                    IntegrationPointType( -std::sqrt( (3.0 + 2.0 * std::sqrt(6.0/5.0)) / 7.0 ),  std::sqrt( (3.0 + 2.0 * std::sqrt(6.0/5.0)) / 7.0 ), (0.5 - std::sqrt(5.0/6.0)/6.0)*(0.5 - std::sqrt(5.0/6.0)/6.0)),
                    IntegrationPointType( -std::sqrt( (3.0 - 2.0 * std::sqrt(6.0/5.0)) / 7.0 ), -std::sqrt( (3.0 + 2.0 * std::sqrt(6.0/5.0)) / 7.0 ), (0.5 + std::sqrt(5.0/6.0)/6.0)*(0.5 - std::sqrt(5.0/6.0)/6.0)),
                    IntegrationPointType( -std::sqrt( (3.0 - 2.0 * std::sqrt(6.0/5.0)) / 7.0 ), -std::sqrt( (3.0 - 2.0 * std::sqrt(6.0/5.0)) / 7.0 ), (0.5 + std::sqrt(5.0/6.0)/6.0)*(0.5 + std::sqrt(5.0/6.0)/6.0)),
                    IntegrationPointType( -std::sqrt( (3.0 - 2.0 * std::sqrt(6.0/5.0)) / 7.0 ),  std::sqrt( (3.0 - 2.0 * std::sqrt(6.0/5.0)) / 7.0 ), (0.5 + std::sqrt(5.0/6.0)/6.0)*(0.5 + std::sqrt(5.0/6.0)/6.0)),
                    IntegrationPointType( -std::sqrt( (3.0 - 2.0 * std::sqrt(6.0/5.0)) / 7.0 ),  std::sqrt( (3.0 + 2.0 * std::sqrt(6.0/5.0)) / 7.0 ), (0.5 + std::sqrt(5.0/6.0)/6.0)*(0.5 - std::sqrt(5.0/6.0)/6.0)),
                    IntegrationPointType(  std::sqrt( (3.0 - 2.0 * std::sqrt(6.0/5.0)) / 7.0 ), -std::sqrt( (3.0 + 2.0 * std::sqrt(6.0/5.0)) / 7.0 ), (0.5 + std::sqrt(5.0/6.0)/6.0)*(0.5 - std::sqrt(5.0/6.0)/6.0)),
                    IntegrationPointType(  std::sqrt( (3.0 - 2.0 * std::sqrt(6.0/5.0)) / 7.0 ), -std::sqrt( (3.0 - 2.0 * std::sqrt(6.0/5.0)) / 7.0 ), (0.5 + std::sqrt(5.0/6.0)/6.0)*(0.5 + std::sqrt(5.0/6.0)/6.0)),
                    IntegrationPointType(  std::sqrt( (3.0 - 2.0 * std::sqrt(6.0/5.0)) / 7.0 ),  std::sqrt( (3.0 - 2.0 * std::sqrt(6.0/5.0)) / 7.0 ), (0.5 + std::sqrt(5.0/6.0)/6.0)*(0.5 + std::sqrt(5.0/6.0)/6.0)),
                    IntegrationPointType(  std::sqrt( (3.0 - 2.0 * std::sqrt(6.0/5.0)) / 7.0 ),  std::sqrt( (3.0 + 2.0 * std::sqrt(6.0/5.0)) / 7.0 ), (0.5 + std::sqrt(5.0/6.0)/6.0)*(0.5 - std::sqrt(5.0/6.0)/6.0)),
                    IntegrationPointType(  std::sqrt( (3.0 + 2.0 * std::sqrt(6.0/5.0)) / 7.0 ), -std::sqrt( (3.0 + 2.0 * std::sqrt(6.0/5.0)) / 7.0 ), (0.5 - std::sqrt(5.0/6.0)/6.0)*(0.5 - std::sqrt(5.0/6.0)/6.0)),
                    IntegrationPointType(  std::sqrt( (3.0 + 2.0 * std::sqrt(6.0/5.0)) / 7.0 ), -std::sqrt( (3.0 - 2.0 * std::sqrt(6.0/5.0)) / 7.0 ), (0.5 - std::sqrt(5.0/6.0)/6.0)*(0.5 + std::sqrt(5.0/6.0)/6.0)),
                    IntegrationPointType(  std::sqrt( (3.0 + 2.0 * std::sqrt(6.0/5.0)) / 7.0 ),  std::sqrt( (3.0 - 2.0 * std::sqrt(6.0/5.0)) / 7.0 ), (0.5 - std::sqrt(5.0/6.0)/6.0)*(0.5 + std::sqrt(5.0/6.0)/6.0)),
                    IntegrationPointType(  std::sqrt( (3.0 + 2.0 * std::sqrt(6.0/5.0)) / 7.0 ),  std::sqrt( (3.0 + 2.0 * std::sqrt(6.0/5.0)) / 7.0 ), (0.5 - std::sqrt(5.0/6.0)/6.0)*(0.5 - std::sqrt(5.0/6.0)/6.0))
                }};
                return s_integration_points;
            }


            std::string Info() const{
                std::stringstream buffer;
                buffer << "Quadrilateral Gauss-Legendre quadrature 4 ";
                return buffer.str();
            }
    };


    /**
     * @class QuadrilateralGaussLegendreIntegrationPoints5
     * @brief 管理二维正方形的Gauss-Legendre等效积分点（五次）
     */
    class QuadrilateralGaussLegendreIntegrationPoints5{
        public:
            QUEST_CLASS_POINTER_DEFINITION(QuadrilateralGaussLegendreIntegrationPoints5);

            static const unsigned int Dimension = 2;

            using IntegrationPointType = IntegrationPoint<2>;

            using IntegrationPointsArrayType = std::array<IntegrationPointType, 25>;

            static std::size_t IntegrationPointsNumber(){
                return 25;
            }


            static const IntegrationPointsArrayType& IntegrationPoints(){
                static IntegrationPointsArrayType s_integration_points;
                const double a[] = {-0.906179845938664, -0.538469310105683, 0.000000000000000, 0.538469310105683, 0.906179845938664};
                const double w[] = {0.236926885056189, 0.478628670499366, 0.568888888888889, 0.478628670499366, 0.236926885056189};

                for(unsigned int i = 0; i < 5; ++i) {
                    for(unsigned int j = 0; j < 5; ++j) {
                        s_integration_points[5*i + j] = IntegrationPointType( a[i], a[j], w[i] * w[j]);
                    }
                }

                return s_integration_points;
            }


            std::string Info() const{
                std::stringstream buffer;
                buffer << "Quadrilateral Gauss-Legendre quadrature 5 ";
                return buffer.str();
            }
    };

} // namespace Quest

#endif //QUEST_QUADRILATERAL_GAUSS_LEGENDRE_INTEGRATION_POINTS_HPP