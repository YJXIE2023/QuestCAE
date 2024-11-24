#ifndef QUEST_DIRECT_SOLVER_HPP
#define QUEST_DIRECT_SOLVER_HPP

// 项目头文件
#include "includes/define.hpp"
#include "includes/quest_parameters.hpp"
#include "linear_solvers/linear_solver.hpp"

namespace Quest{

    /**
     * @class DirectSolver
     * @brief 直接求解器基类
     * @details 继承自LinearSolver，直接求解器的抽象层，无额外成员变量和函数，直接继承LinearSolver的接口。
     * @tparam TSparseSpaceType 稀疏存储类型
     * @tparam TDenseSpaceType  密集存储类型
     * @tparam TReordererType   重排器
     */
    template<typename TSparseSpaceType, typename TDenseSpaceType, typename TReordererType = Reorderer<TSparseSpaceType, TDenseSpaceType> >
    class DirectSolver : public LinearSolver<TSparseSpaceType, TDenseSpaceType, TReordererType>{
        public:
            QUEST_CLASS_POINTER_DEFINITION(DirectSolver);

            using BaseType = LinearSolver<TSparseSpaceType, TDenseSpaceType, TReordererType>;
            using SparseMatrixType = typename TSparseSpaceType::MatrixType;
            using VectorType = typename TDenseSpaceType::VectorType;
            using DenseMatrixType = typename TDenseSpaceType::MatrixType;

        public:
            /**
             * @brief 默认构造函数
             */
            DirectSolver() : BaseType(){}


            /**
             * @brief 构造函数
             */
            DirectSolver(Parameters settings){}


            /**
             * @brief 析构函数
             */
            ~DirectSolver() override {}


            /**
             * @brief 复制构造函数
             */
            DirectSolver(const DirectSolver& Other) : BaseType(Other){}


            void PrintInfo(std::ostream& rOStream) const override{
                rOStream << "Direct Solver";
            }


            void PrintData(std::ostream& rOStream) const override{
            }

        protected:

        private:
            /**
             * @brief 赋值运算符重载
             */
            DirectSolver& operator=(const DirectSolver& Other);
    };


    template<typename TSparseSpaceType, typename TDenseSpaceType, typename TReordererType>
    inline std::istream& operator >> (std::istream& rIStream, DirectSolver<TSparseSpaceType, TDenseSpaceType, TReordererType>& rThis){
        return rIStream;
    }


    template<typename TSparseSpaceType, typename TDenseSpaceType, typename TReordererType>
    inline std::ostream& operator << (std::ostream& rOStream, const DirectSolver<TSparseSpaceType, TDenseSpaceType, TReordererType>& rThis){
        rThis.PrintInfo(rOStream);
        rOStream << std::endl;
        rThis.PrintData(rOStream);

        return rOStream;
    }

} // namespace Quest

#endif //QUEST_DIRECT_SOLVER_HPP