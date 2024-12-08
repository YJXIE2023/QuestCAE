#ifndef QUEST_GEOMETRY_DIMENSION_HPP
#define QUEST_GEOMETRY_DIMENSION_HPP

// 项目头文件
#include "includes/define.hpp"
#include "includes/serializer.hpp"

namespace Quest{

    class GeometryDimension{
        public:
            QUEST_CLASS_POINTER_DEFINITION(GeometryDimension);

            using IndexType = std::size_t;
            using SizeType = std::size_t;

            /**
             * @brief 构造函数
             * @param ThisWorkingSpaceDimension 整体坐标系维度
             * @param ThisLocalSpaceDimension 局部坐标系维度
             */
            GeometryDimension(SizeType ThisWorkingSpaceDimension, SizeType ThisLocalSpaceDimension):
                mWorkingSpaceDimension(ThisWorkingSpaceDimension),
                mLocalSpaceDimension(ThisLocalSpaceDimension){}


            /**
             * @brief 复制构造函数  
             * @param rOther 待复制的对象
             */
            GeometryDimension(const GeometryDimension& rOther):
                mWorkingSpaceDimension(rOther.mWorkingSpaceDimension),
                mLocalSpaceDimension(rOther.mLocalSpaceDimension){}

            virtual ~GeometryDimension(){}

            GeometryDimension& operator = (const GeometryDimension& rOther){
                mWorkingSpaceDimension = rOther.mWorkingSpaceDimension;
                mLocalSpaceDimension = rOther.mLocalSpaceDimension;

                return *this;
            }


            /**
             * @brief 获取整体坐标系维度
             */
            inline SizeType WorkingSpaceDimension() const{
                return mWorkingSpaceDimension;
            }


            /**
             * @brief 获取局部坐标系维度
             */
            inline SizeType LocalSpaceDimension() const{
                return mLocalSpaceDimension;
            }

            virtual std::string Info() const{
                std::stringstream buffer;
                buffer << "geometry dimension";
                return buffer.str();
            }

            virtual void PrintInfo(std::ostream& rOstream) const{
                rOstream << Info();
            }

            virtual void PrintData(std::ostream& rOstream) const{
                rOstream << "    Working space dimension: " << mWorkingSpaceDimension << std::endl;
                rOstream << "    Local space dimension: " << mLocalSpaceDimension << std::endl;
            }

        protected:

        private:
            friend class Serializer;

            virtual void save(Serializer& rSerializer) const{
                rSerializer.save("WorkingSpaceDimension", mWorkingSpaceDimension);
                rSerializer.save("LocalSpaceDimension", mLocalSpaceDimension);
            }

            virtual void load(Serializer& rSerializer){
                rSerializer.load("WorkingSpaceDimension", mWorkingSpaceDimension);
                rSerializer.load("LocalSpaceDimension", mLocalSpaceDimension);
            }

            GeometryDimension(){}

        private:
            /**
             * @brief 整体坐标系维度
             */
            SizeType mWorkingSpaceDimension;

            /**
             * @brief 局部坐标系维度
             */
            SizeType mLocalSpaceDimension;

    };

    inline std::istream& operator >> (std::istream& rIstream, GeometryDimension& rThis);

    inline std::ostream& operator << (std::ostream& rOstream, const GeometryDimension& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }

} // namespace Quest

#endif //QUEST_GEOMETRY_DIMENSION_HPP