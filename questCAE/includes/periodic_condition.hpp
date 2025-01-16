#ifndef QUEST_PERIODIC_CONDITION_HPP
#define QUEST_PERIODIC_CONDITION_HPP

// 系统头文件
#include <string>
#include <iostream>
#include <sstream>
#include <cstddef>

// 项目头文件
#include "includes/define.hpp"
#include "includes/node.hpp"
#include "geometries/geometry.hpp"
#include "includes/properties.hpp"
#include "includes/process_info.hpp"
#include "includes/indexed_object.hpp"
#include "includes/condition.hpp"
#include "includes/serializer.hpp"
#include "includes/variables.hpp"
#include "container/periodic_variable_container.hpp"

namespace Quest{

    /**
     * @brief 周期性边界条件类
     * @details 此条件连接两个节点，分别位于两个边界上，这两个节点在两个周期性边界上被视为彼此的镜像
     */
    class QUEST_API(QUEST_CORE) PeriodicCondition : public Condition{
        public:
            QUEST_CLASS_POINTER_DEFINITION(PeriodicCondition);

            using IndexedObjectType = IndexedObject;
            using BaseType = Condition;
            using NodeType = Node;
            using PropertiesType = Properties;
            using GeometryType = Geometry<NodeType>;
            using NodesArrayType = Geometry<NodeType>::PointsArrayType;
            using VectorType = Vector;
            using MatrixType = Matrix;
            using IndexType = BaseType::IndexType;
            using SizeType = BaseType::SizeType;
            using DofType = BaseType::DofType;
            using EquationIdVectorType = BaseType::EquationIdVectorType;
            using DofsVectorType = BaseType::DofsVectorType;
            using DofsArrayType = BaseType::DofsArrayType;

        public:
            /**
             * @brief 默认构造函数
             */
            explicit PeriodicCondition(IndexType NewId = 0);


            /**
             * @brief 构造函数
             */
            PeriodicCondition(IndexType NewId, const NodesArrayType& ThisNodes);


            /**
             * @brief 构造函数
             */
            PeriodicCondition(IndexType NewId, GeometryType::Pointer pGeometry);


            /**
             * @brief 构造函数
             */
            PeriodicCondition(
                IndexType NewId,
                GeometryType::Pointer pGeometry,
                PropertiesType::Pointer pProperties
            );


            /**
             * @brief 复制构造函数
             */
            PeriodicCondition(const PeriodicCondition& rOther);


            /**
             * @brief 析构函数
             */
            ~PeriodicCondition() override;


            /**
             * @brief 赋值运算符重载
             */
            PeriodicCondition & operator=(const PeriodicCondition& rOther);


            /**
             * @brief 创建并返回一个新的对象指针
             */
            Condition::Pointer Create(
                IndexType NewId,
                const NodesArrayType& ThisNodes,
                PropertiesType::Pointer pProperties
            ) const override;


            /**
             * @brief 检查输入
             */
            int Check(const ProcessInfo& rCurrentProcessInfo) const override;


            /**
             * 惩罚项的权重由成员变量 mWeight 给出，
             * 使用 SetValuesOnIntegrationPoints 进行设置。周期性变量从存储在
             * rCurrentProcessInfo 中的 PERIODIC_VARIABLES 的值中读取。
             * @param rLeftHandSideMatrix 本地左侧矩阵（输出）
             * @param rRightHandSideVector 本地右侧向量（输出）
             * @param rCurrentProcessInfo ProcessInfo 实例（未使用）
             */
            void CalculateLocalSystem(
                MatrixType& rLeftHandSideMatrix,
                VectorType& rRightHandSideVector,
                const ProcessInfo& rCurrentProcessInfo
            ) override;


            /**
             * @brief 返回用于周期性变量的惩罚项矩阵
             */
            void CalculateLeftHandSide(
                MatrixType& rLeftHandSideMatrix,
                const ProcessInfo& rCurrentProcessInfo
            ) override;


            /**
             * @brief 返回被惩罚自由度的右侧向量值
             */
            void CalculateRightHandSide(
                VectorType& rRightHandSideVector,
                const ProcessInfo& rCurrentProcessInfo
            ) override;


            /**
             * @brief 确定该单元所有局部自由度对应的全局方程编号
             */
            void EquationIdVector(
                EquationIdVectorType& rResult,
                const ProcessInfo& rCurrentProcessInfo
            ) const override;


            /**
             * @brief 返回单元的自由度
             */
            void GetDofList(
                DofsVectorType& ElementalDofList,
                const ProcessInfo& CurrentProcessInfo
            ) const override;


            /**
             * @brief 返回每个节点的未知值
             */
            void GetValuesVector(Vector& Values, int Step = 0) const override;


            std::string Info() const override
            {
                std::stringstream buffer;
                buffer << "PeriodicCondition #" << Id();
                return buffer.str();
            }


            void PrintInfo(std::ostream& rOStream) const override
            {
                rOStream << "PeriodicCondition #" << Id();
            }


            void PrintData(std::ostream& rOStream) const override
            {
                Condition::PrintData(rOStream);
            }

        protected:

        private:
            friend class Serializer;

            void save(Serializer& rSerializer) const override;

            void load(Serializer& rSerializer) override;

        private:

    };

    inline std::istream& operator >> (std::istream& rIstream, PeriodicCondition& rThis){
        return rIstream;
    }


    inline std::ostream& operator << (std::ostream& rOstream, const PeriodicCondition& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);
        return rOstream;
    }

}

#endif //QUEST_PERIODIC_CONDITION_HPP