#ifndef QUEST_ACCESSOR_HPP
#define QUEST_ACCESSOR_HPP

// 项目头文件
#include "geometries/geometry.hpp"
#include "includes/process_info.hpp"
#include "includes/node.hpp"
#include "includes/properties.hpp"

namespace Quest{

    /**
     * @class Accessor
     * @brief 定义了某个属性的访问方式
     * @details 描述了如何某问某一特定属性，为属性访问的方式提供和逻辑提供同一的接口和实现
     */
    class QUEST_API(QUEST_CORE) Accessor{
        public:
            QUEST_CLASS_POINTER_DEFINITION(Accessor);

            using GeometryType = Geometry<Node>;

        public:
            Accessor() = default;

            virtual ~Accessor() = default;


            Accessor(const Accessor& rOther) {}

            /**
             * @brief 获取double类型的属性值
             * @param rVariable 变量
             * @param rProperties 所属的属性集合
             * @param rGeometry 指定的几何体
             * @param rShapeFunctionsValues 所选高斯点的形函数向量
             * @param rProcessInfo 所使用的过程信息
             */
            virtual double GetValue(
                const Variable<double>& rVariable,
                const Properties& rProperties,
                const GeometryType& rGeometry,
                const Vector& rShapeFunctionsVector,
                const ProcessInfo& rProcessInfo
            ) const;

            /**
             * @brief 获取Vector类型的属性值
             * @param rVariable 变量
             * @param rProperties 所属的属性集合
             * @param rGeometry 指定的几何体
             * @param rShapeFunctionsValues 所选高斯点的形函数向量
             * @param rProcessInfo 所使用的过程信息
             */
            virtual Vector GetValue(
                const Variable<Vector>& rVariable,
                const Properties& rProperties,
                const GeometryType& rGeometry,
                const Vector& rShapeFunctionsVector,
                const ProcessInfo& rProcessInfo
            ) const;

            /**
             * @brief 获取bool类型的属性值
             * @param rVariable 变量
             * @param rProperties 所属的属性集合
             * @param rGeometry 指定的几何体
             * @param rShapeFunctionsValues 所选高斯点的形函数向量
             * @param rProcessInfo 所使用的过程信息
             */
            virtual bool GetValue(
                const Variable<bool>& rVariable,
                const Properties& rProperties,
                const GeometryType& rGeometry,
                const Vector& rShapeFunctionsVector,
                const ProcessInfo& rProcessInfo
            ) const;

            /**
             * @brief 获取int类型的属性值
             * @param rVariable 变量
             * @param rProperties 所属的属性集合
             * @param rGeometry 指定的几何体
             * @param rShapeFunctionsValues 所选高斯点的形函数向量
             * @param rProcessInfo 所使用的过程信息
             */
            virtual int GetValue(
                const Variable<int>& rVariable,
                const Properties& rProperties,
                const GeometryType& rGeometry,
                const Vector& rShapeFunctionsVector,
                const ProcessInfo& rProcessInfo
            ) const;

            /**
             * @brief 获取Matrix类型的属性值
             * @param rVariable 变量
             * @param rProperties 所属的属性集合
             * @param rGeometry 指定的几何体
             * @param rShapeFunctionsValues 所选高斯点的形函数向量
             * @param rProcessInfo 所使用的过程信息
             */
            virtual Matrix GetValue(
                const Variable<Matrix>& rVariable,
                const Properties& rProperties,
                const GeometryType& rGeometry,
                const Vector& rShapeFunctionsVector,
                const ProcessInfo& rProcessInfo
            ) const;

            /**
             * @brief 获取Array1d<double,3>类型的属性值
             * @param rVariable 变量
             * @param rProperties 所属的属性集合
             * @param rGeometry 指定的几何体
             * @param rShapeFunctionsValues 所选高斯点的形函数向量
             * @param rProcessInfo 所使用的过程信息
             */
            virtual Array1d<double,3> GetValue(
                const Variable<Array1d<double,3>>& rVariable,
                const Properties& rProperties,
                const GeometryType& rGeometry,
                const Vector& rShapeFunctionsVector,
                const ProcessInfo& rProcessInfo
            ) const;

            /**
             * @brief 获取Array1d<double,6>类型的属性值
             * @param rVariable 变量
             * @param rProperties 所属的属性集合
             * @param rGeometry 指定的几何体
             * @param rShapeFunctionsValues 所选高斯点的形函数向量
             * @param rProcessInfo 所使用的过程信息
             */
            virtual Array1d<double,6> GetValue(
                const Variable<Array1d<double,6>>& rVariable,
                const Properties& rProperties,
                const GeometryType& rGeometry,
                const Vector& rShapeFunctionsVector,
                const ProcessInfo& rProcessInfo
            ) const;

            /**
             * @brief 获取Array1d<double,4>类型的属性值
             * @param rVariable 变量
             * @param rProperties 所属的属性集合
             * @param rGeometry 指定的几何体
             * @param rShapeFunctionsValues 所选高斯点的形函数向量
             * @param rProcessInfo 所使用的过程信息
             */
            virtual Array1d<double, 4> GetValue(
                const Variable<Array1d<double, 4>>& rVariables, 
                const Properties& rProperties, 
                const GeometryType& rGeometry, 
                const Vector& rShapeFunctionsVector, 
                const ProcessInfo& rProcessInfo
            ) const;

            /**
             * @brief 获取Array1d<double,9>类型的属性值
             * @param rVariable 变量
             * @param rProperties 所属的属性集合
             * @param rGeometry 指定的几何体
             * @param rShapeFunctionsValues 所选高斯点的形函数向量
             * @param rProcessInfo 所使用的过程信息
             */
            virtual Array1d<double,9> GetValue( 
                const Variable<Array1d<double,9>>& rVariable, 
                const Properties& rProperties, 
                const GeometryType& rGeometry, 
                const Vector& rShapeFunctionsVector, 
                const ProcessInfo& rProcessInfo
            ) const;

            /**
             * @brief 获取string类型的属性值
             * @param rVariable 变量
             * @param rProperties 所属的属性集合
             * @param rGeometry 指定的几何体
             * @param rShapeFunctionsValues 所选高斯点的形函数向量
             * @param rProcessInfo 所使用的过程信息
             */
            virtual std::string GetValue(
                const Variable<std::string>& rVariable,
                const Properties& rProperties,
                const GeometryType& rGeometry,
                const Vector& rShapeFunctionsVector,
                const ProcessInfo& rProcessInfo
            ) const;

            /**
             * @brief 克隆函数
             */
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