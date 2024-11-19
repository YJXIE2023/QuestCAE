#ifndef QUEST_POINT_HPP
#define QUEST_POINT_HPP

// 项目头文件
#include "includes/define.hpp"
#include "container/array_1d.hpp"
#include "includes/serializer.hpp"

namespace Quest{

    /**
     * @class Point
     * @brief 点类
     * @details 点类，用于表示三维空间中的点，其坐标由三维数组表示。
     */
    class Point: public Array1d<double, 3>{

            static constexpr std::size_t mDimension = 3;

        public:
            QUEST_CLASS_POINTER_DEFINITION(Point);

            using BaseType = Array1d<double, mDimension>;
            using CoordinatesArrayType = BaseType;
            using SizeType = std::size_t;
            using IndexType = std::size_t;


            /**
             * @brief 默认构造函数
             */
            Point(): BaseType(){
                SetAllCoordinates();
            }


            /**
             * @brief 构造函数
             */
            Point(double NewX, double NewY = 0, double NewZ = 0): BaseType(){
                this->operator()(0) = NewX;
                this->operator()(1) = NewY;
                this->operator()(2) = NewZ;
            }


            /**
             * @brief 复制构造函数
             */
            Point(const Point& rOther): BaseType(rOther){}


            /**
             * @brief 坐标数组构造函数
             */
            explicit Point(const CoordinatesArrayType& rOther): BaseType(rOther){}


            /**
             * @brief 向量表达式构造函数
             */
            template<typename TVectorType>
            explicit Point(const vector_expression<TVectorType>& rOther): BaseType(rOther){}


            /**
             * @brief 由std::vector构造函数
             */
            explicit Point(const std::vector<double>& rOther): BaseType(){
                SizeType size = rOther.size();
                size = (size > mDimension)? mDimension : size;
                for (IndexType i = 0; i < size; ++i){
                    this->operator()(i) = rOther[i];
                }
            }

            /**
             * @brief 析构函数
             */
            virtual ~Point(){}


            /**
             * @brief 赋值运算符
             */
            Point& operator = (const Point& rOther){
                CoordinatesArrayType::operator=(rOther);
                return *this;
            }


            /**
             * @brief 相等运算符
             */
            bool operator == (const Point& rOther) const{
                return std::equal(this->begin(), this->end(), rOther.begin());
            }


            /**
             * @brief 计算平方距离
             */
            double SquaredDistance(const Point& rOther) const{
                const Array1d<double, 3> diff_vector = this->Coordinates() - rOther.Coordinates();
                return (std::pow(diff_vector[0],2)+std::pow(diff_vector[1],2)+std::pow(diff_vector[2],2));
            }


            /**
             * @brief 计算当前点与另一个点的欧几里得距离
             */
            double Distance(const Point& rOther) const{
                return norm_2(this->Coordinates() - rOther.Coordinates());
            }


            /**
             * @brief 点的维度
             */
            static constexpr IndexType Dimension(){
                return mDimension;
            }


            /**
             * @brief 点的X坐标
             */
            double X() const{
                return this->operator[](0);
            }


            /**
             * @brief 点的Y坐标
             */
            double Y() const{
                return this->operator[](1);
            }


            /**
             * @brief 点的Z坐标
             */
            double Z() const{
                return this->operator[](2);
            }


            /**
             * @brief 点X坐标的引用
             */
            double& X(){
                return this->operator[](0);
            }


            /**
             * @brief 点Y坐标的引用
             */
            double& Y(){
                return this->operator[](1);
            }


            /**
             * @brief 点Z坐标的引用
             */
            double& Z(){
                return this->operator[](2);
            }


            /**
             * @brief 点的坐标数组
             */
            const CoordinatesArrayType& Coordinates() const{
                return *this;
            }


            /**
             * @brief 点的坐标数组
             */
            CoordinatesArrayType& Coordinates(){
                return *this;
            }

            virtual std::string Info() const{
                std::stringstream buffer;
                buffer << "Point";
                return buffer.str();
            }

            virtual void PrintInfo(std::ostream& rOstream) const{
                rOstream << Info();
            }

            virtual void PrintData(std::ostream& rOstream) const{
                rOstream << " (" << this->operator[](0)
                         << ", " << this->operator[](1)
                         << ", " << this->operator[](2) << ")";
            }

        protected:

        private:
            /**
             * @brief 设置所有坐标为默认值
             */
            void SetAllCoordinates(const double& Value = double()){
                for (IndexType i = 0; i < mDimension; ++i){
                    this->operator()(i) = Value;
                }
            }

            friend class Serializer;

            virtual void save(Serializer& rSerializer) const{
                rSerializer.save_base("BaseClass", *static_cast<const Array1d<double, mDimension>*>(this));
            }

            virtual void load(Serializer& rSerializer){
                rSerializer.load_base("BaseClass", *static_cast<Array1d<double, mDimension>*>(this));
            }

    };

    inline std::istream& operator >> (std::istream& rIstream, Point& rThis){
        return rIstream;
    }

    inline std::ostream& operator << (std::ostream& rOstream, const Point& rThis){
        rThis.PrintInfo(rOstream);
        rThis.PrintData(rOstream);
        
        return rOstream;
    }

} // namespace Quest

#endif //QUEST_POINT_HPP