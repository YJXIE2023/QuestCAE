/*--------------------------------
三位空间中的点
--------------------------------*/

#ifndef QUEST_POINT_HPP
#define QUEST_POINT_HPP

// 项目头文件
#include "includes/define.hpp"
#include "container/array_1d.hpp"
#include "includes/serializer.hpp"

namespace Quest{

    class Point: public Array1d<double, 3>{

            static constexpr std::size_t mDimension = 3;

        public:
            QUEST_CLASS_POINTER_DEFINITION(Point);

            using BaseType = Array1d<double, mDimension>;
            using CoordinatesArrayType = BaseType;
            using SizeType = std::size_t;
            using IndexType = std::size_t;


            Point(): BaseType(){
                SetAllCoordinates();
            }

            Point(double NewX, double NewY = 0, double NewZ = 0): BaseType(){
                this->operator()(0) = NewX;
                this->operator()(1) = NewY;
                this->operator()(2) = NewZ;
            }

            Point(const Point& rOther): BaseType(rOther){}

            explicit Point(const CoordinatesArrayType& rOther): BaseType(rOther){}

            template<typename TVectorType>
            explicit Point(const vector_expression<TVectorType>& rOther): BaseType(rOther){}

            explicit Point(const std::vector<double>& rOther): BaseType(){
                SizeType size = rOther.size();
                size = (size > mDimension)? mDimension : size;
                for (IndexType i = 0; i < size; ++i){
                    this->operator()(i) = rOther[i];
                }
            }

            virtual ~Point(){}

            Point& operator = (const Point& rOther){
                CoordinatesArrayType::operator=(rOther);
                return *this;
            }

            bool operator == (const Point& rOther) const{
                return std::equal(this->begin(), this->end(), rOther.begin());
            }

            double SquaredDistance(const Point& rOther) const{
                const Array1d<double, 3> diff_vector = this->Coordiantes() - rOther.Coordiantes();
                return (std::pow(diff_vector[0],2)+std::pow(diff_vector[1],2)+std::pow(diff_vector[2],2));
            }

            double Distance(const Point& rOther) const{
                return norm_2(this->Coordiantes() - rOther.Coordiantes());
            }

            static constexpr IndexType Dimension(){
                return mDimension;
            }

            double X() const{
                return this->operator[](0);
            }

            double Y() const{
                return this->operator[](1);
            }

            double Z() const{
                return this->operator[](2);
            }

            double& X(){
                return this->operator[](0);
            }

            double& Y(){
                return this->operator[](1);
            }

            double& Z(){
                return this->operator[](2);
            }

            const CoordinatesArrayType& Coordiantes() const{
                return *this;
            }

            CoordinatesArrayType& Coordiantes(){
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