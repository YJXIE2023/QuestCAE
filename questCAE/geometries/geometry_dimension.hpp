/*---------------------------------
与几何维度的定义和管理相关
---------------------------------*/

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

            GeometryDimension(SizeType ThisWorkingSpaceDimension, SizeType ThisLocalSpaceDimension):
                mWorkingSpaceDimension(ThisWorkingSpaceDimension),
                mLocalSpaceDimension(ThisLocalSpaceDimension){}

            GeometryDimension(const GeometryDimension& rOther):
                mWorkingSpaceDimension(rOther.mWorkingSpaceDimension),
                mLocalSpaceDimension(rOther.mLocalSpaceDimension){}

            virtual ~GeometryDimension(){}

            GeometryDimension& operator = (const GeometryDimension& rOther){
                mWorkingSpaceDimension = rOther.mWorkingSpaceDimension;
                mLocalSpaceDimension = rOther.mLocalSpaceDimension;

                return *this;
            }

            inline SizeType WorkingSpaceDimension() const{
                return mWorkingSpaceDimension;
            }

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
            SizeType mWorkingSpaceDimension;
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