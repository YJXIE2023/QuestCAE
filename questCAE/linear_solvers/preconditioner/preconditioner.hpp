#ifndef QUEST_PRECONDITIONER_HPP
#define QUEST_PRECONDITIONER_HPP

// 系统头文件
#include <string>
#include <iostream>

// 项目头文件
#include "includes/define.hpp"
#include "includes/model_part.hpp"

namespace Quest{

    /**
     * @class Preconditioner
     * @brief 预处理器基类
     * @details 为迭代求解器提供预处理功能（如左预处理、右预处理），通常在每次迭代前后对解进行预处理来加速收敛
     */
    template<typename TSparseSpaceType, typename TDenseSpaceType>
    class Preconditioner{
        public:
            QUEST_CLASS_POINTER_DEFINITION(Preconditioner);

            using SparseMatrixType = typename TSparseSpaceType::MatrixType;
            using VectorType = typename TDenseSpaceType::VectorType;
            using DenseMatrixType = typename TDenseSpaceType::MatrixType;

        public:
            /**
             * @brief 默认构造函数
             */
            Preconditioner() {}


            /**
             * @brief 复制构造函数
             */
            Preconditioner(const Preconditioner& Other) {}


            /**
             * @brief 析构函数
             */
            virtual ~Preconditioner() {}


            /**
             * @brief 赋值运算符重载
             */
            Preconditioner& operator = (const Preconditioner& Other) { 
                return *this; 
            }


            /**
             * @brief 预处理器初始化函数
             * @details 常规求解方法的预处理器初始化函数
             */
            virtual void Initialize(SparseMatrixType& rA, VectorType& rX, VectorType& rB){}


            /**
             * @brief 预处理器初始化函数
             * @details 多重求解方法的预处理器初始化函数
             */
            virtual void Initialize(SparseMatrixType& rA, DenseMatrixType& rX, DenseMatrixType& rB){
                VectorType x(TDenseSpaceType::Size1(rX));
                VectorType b(TDenseSpaceType::Size1(rB));

                TDenseSpaceType::GetColumn(0, rX, x);
                TDenseSpaceType::GetColumn(0, rB, b);

                Initialize(rA, x, b);
            }


            /**
             * @brief 矩阵求解前的预处理操作
             * @details 该函数在求解器开始求解之前调用，可以用于执行一些必要的预处理操作，如直接求解之前进行矩阵分解等
             */
            virtual void InitializeSolutionStep(SparseMatrixType& rA, VectorType& rX, VectorType& rB){
            }


            /**
             * @brief 求解结束后移出不再需要的数据资源
             * @param rA 系数矩阵
             * @param rX 求解向量
             * @param rB 常数项
             */
            virtual void FinalizeSolutionStep(SparseMatrixType& rA, VectorType& rX, VectorType& rB){
            }


            /**
             * @brief 清除求解器的所有内部数据，将求解器对象恢复到初始创建时的状态
             */
            virtual void Clear(){
            }


            /**
             * @brief 检查是否需要额外的物理数据
             * @details 该函数用于检查是否需要额外的物理数据，如：混合u-p问题，平滑聚合求解器等
             */
            virtual bool AdditionalPhysicalDataIsNeeded(){
                return false;
            }


            /**
             * @brief 提供额外的物理数据
             * @details 该函数用于提供额外的物理数据，如：混合u-p问题，平滑聚合求解器等
             * @param rA 系数矩阵
             * @param rX 求解向量
             * @param rB 常数项
             * @param rDofSet 自由度集
             * @param rModelPart 模型部分
             */
            virtual void ProvideAdditionalData(
                SparseMatrixType& rA,
                VectorType& rX,
                VectorType& rB,
                typename ModelPart::DofsArrayType& rDofSet,
                ModelPart& rModelPart
            ){}


            

        protected:

        private:

    };


    template<typename TSparseSpaceType, typename TDenseSpaceType>
    inline std::istream& operator >> (std::istream& rIstream, Preconditioner<TSparseSpaceType, TDenseSpaceType>& rThis){
        return rIstream;
    }


    template<typename TSparseSpaceType, typename TDenseSpaceType>
    inline std::ostream& operator << (std::ostream& rOstream, const Preconditioner<TSparseSpaceType, TDenseSpaceType>& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }

} // namespace Quest

#endif //QUEST_PRECONDITIONER_HPP