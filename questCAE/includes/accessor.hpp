/*----------------------------------------------------
定义一种访问属性（property）的方式
用于管理和简化对 Properties 类中的属性的读取、写入和操作方式
----------------------------------------------------*/

#ifndef QUEST_ACCESSOR_HPP
#define QUEST_ACCESSOR_HPP

// 项目头文件
#include "geometries/geometry.hpp"
#include "includes/process_info.hpp"
#include "includes/node.hpp"
#include "includes/properties.hpp"

namespace Quest{

    class QUEST_API(QUEST_CORE) Accessor{
        public:
            QUEST_CLASS_POINTER_DEFINITION(Accessor);

            using GeometryType = Geometry<Node>;

        public:
            Accessor() = default;

            virtual ~Accessor() = default;


            Accessor(const Accessor& rOther) {}


            virtual double GetValue(
                const Variable<double>& rVariable,
                const Properties& rProperties,
                const GeometryType& rGeometry,
                const Vector& rShapeFunctionsValues,
                const ProcessInfo& rProcessInfo
            ) const;


            virtual Vector GetValue(
                const Variable<Vector>& rVariable,
                const Properties& rProperties,
                const GeometryType& rGeometry,
                const Vector& rShapeFunctionsValues,
                const ProcessInfo& rProcessInfo
            ) const;


            virtual bool GetValue(
                const Variable<bool>& rVariable,
                const Properties& rProperties,
                const GeometryType& rGeometry,
                const Vector& rShapeFunctionsValues,
                const ProcessInfo& rProcessInfo
            ) const;


            virtual int GetValue(
                const Variable<int>& rVariable,
                const Properties& rProperties,
                const GeometryType& rGeometry,
                const Vector& rShapeFunctionsValues,
                const ProcessInfo& rProcessInfo
            ) const;


            virtual Matrix GetValue(
                const Variable<Matrix>& rVariable,
                const Properties& rProperties,
                const GeometryType& rGeometry,
                const Vector& rShapeFunctionsValues,
                const ProcessInfo& rProcessInfo
            ) const;


            virtual Array1d<double,3> GetValue(
                const Variable<Array1d<double,3>>& rVariable,
                const Properties& rProperties,
                const GeometryType& rGeometry,
                const Vector& rShapeFunctionsValues,
                const ProcessInfo& rProcessInfo
            ) const;


            virtual Array1d<double,6> GetValue(
                const Variable<Array1d<double,6>>& rVariable,
                const Properties& rProperties,
                const GeometryType& rGeometry,
                const Vector& rShapeFunctionsValues,
                const ProcessInfo& rProcessInfo
            ) const;


            virtual Array1d<double, 4> GetValue(
                const Variable<Array1d<double, 4>>& rVariables, 
                const Properties& rProperties, 
                const GeometryType& rGeometry, 
                const Vector& rShapeFunctionsValues, 
                const ProcessInfo& rProcessInfo
            ) const;


            virtual Array1d<double,9> GetValue( 
                const Variable<Array1d<double,9>>& rVariable, 
                const Properties& rProperties, 
                const GeometryType& rGeometry, 
                const Vector& rShapeFunctionsValues, 
                const ProcessInfo& rProcessInfo
            ) const;


            virtual std::string GetValue(
                const Variable<std::string>& rVariable,
                const Properties& rProperties,
                const GeometryType& rGeometry,
                const Vector& rShapeFunctionsValues,
                const ProcessInfo& rProcessInfo
            ) const;


            virtual Accessor::UniquePointer Clone() const;


            virtual std::string Info() const{
                std::stringstream buffer;
                buffer << "Accessor";
                return buffer.str();
            }


            virtual void PrintInfo(std::ostream& rOstream) const{
                rOstream << Info();
            }


            virtual void PrintData(std::ostream& rOstream) const{
                rOstream << "Virtual method of the base Accessor class.";
            }

        protected:

        private:
            friend class Serializer;


            virtual void save(Serializer& rSerializer) const{
            }


            virtual void load(Serializer& rSerializer){
            }

        private:

    };

    std::istream& operator>>(std::istream& rIStream, Accessor& rThis);

    std::ostream& operator<<(std::ostream& rOStream, const Accessor& rThis);

} // namespace Quest

#endif //QUEST_ACCESSOR_HPP