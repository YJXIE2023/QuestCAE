#ifndef QUEST_SKYLINE_LU_CUSTOM_SCALAR_SOLVER_HPP
#define QUEST_SKYLINE_LU_CUSTOM_SCALAR_SOLVER_HPP

// 系统头文件
#include <iostream>
#include <complex>
#include <vector>
#include <algorithm>

// 第三方头文件
#include <memory>
#include <amgcl/backend/builtin.hpp>
#include <amgcl/adapter/zero_copy.hpp>
#include <amgcl/value_type/complex.hpp>
#include <amgcl/solver/skyline_lu.hpp>

// 项目头文件
#include "includes/define.hpp"
#include "includes/quest_parameters.hpp"
#include "linear_solvers/linear_solver.hpp"
#include "linear_solvers/direct_solver.hpp"

namespace Quest{

    template<typename TSparseSpaceType, typename TDenseSpaceType, typename TReordererType = Reorderer<TSparseSpaceType, TDenseSpaceType>>
    class SkylineLUCustomScalarSolver : public DirectSolver<TSparseSpaceType, TDenseSpaceType, TReordererType>{
        public:
            QUEST_CLASS_POINTER_DEFINITION(SkylineLUCustomScalarSolver);

            using SparseMatrixType = typename TSparseSpaceType::MatrixType;
            using VectorType = typename TSparseSpaceType::VectorType;
            using DenseMatrixType = typename TDenseSpaceType::MatrixType;
            using DataType = typename TSparseSpaceType::DataType;
            using BuiltinMatrixType = typename amgcl::backend::builtin<DataType>::matrix;
            using SolverType = amgcl::solver::skyline_lu<DataType>;

        public:
            /**
             * @brief 构造函数
             */
            SkylineLUCustomScalarSolver(): DirectSolver<TSparseSpaceType, TDenseSpaceType, TReordererType>(){}


            /**
             * @brief 构造函数
             */
            SkylineLUCustomScalarSolver(Parameters& rParam):
                DirectSolver<TSparseSpaceType, TDenseSpaceType, TReordererType>(rParam)
            {}


            /**
             * @brief 析构函数
             */
            ~SkylineLUCustomScalarSolver() override{
                Clear();
            }

            /**
             * @brief 初始化求解步
             */
            void InitializeSolutionStep(SparseMatrixType& rA, VectorType& rX, VectorType& rB) override{
                Clear();

                pBuiltinMatrix = amgcl::adapter::zero_copy(
                    rA.size1(),
                    rA.index1_data().begin(),
                    rA.index2_data().begin(),
                    rA.value_data().begin()
                );

                pSolver = std::make_shared<SolverType>(*pBuiltinMatrix);
            }


            /**
             * @brief 执行当前求解步
             */
            void PerformSolutionStep(SparseMatrixType& rA, VectorType& rX, VectorType& rB) override{
                std::vector<DataType> x(rX.size());
                std::vector<DataType> b(rB.size());

                std::copy(std::begin(rB), std::end(rB), std::begin(b));

                (*pSolver)(b, x);

                std::copy(std::begin(x), std::end(x), std::begin(rX));
            }


            /**
             * @brief 求解
             */
            bool Solve(SparseMatrixType& rA, VectorType& rX, VectorType& rB) override{
                InitializeSolutionStep(rA, rX, rB);
                PerformSolutionStep(rA, rX, rB);
                FinalizeSolutionStep(rA, rX, rB);

                return true;
            }


            /**
             * @brief 结束求解步
             */
            void FinalizeSolutionStep(SparseMatrixType& rA, VectorType& rX, VectorType& rB) override{
                Clear();
            }


            /**
             * @brief 清空求解信息
             */
            void Clear() override{
                pSolver.reset();
                pBuiltinMatrix.reset();
            }


            void PrintInfo(std::ostream& rOstream) const override{
                rOstream << "Skyline LU Custom Scalar Solver";
            }

        protected:

        private:

        private:
            /**
             * @brief 稀疏矩阵
             */
            Quest::shared_ptr<BuiltinMatrixType> pBuiltinMatrix;

            /**
             * @brief 求解器指针
             */
            Quest::shared_ptr<SolverType> pSolver;

    };


    template<typename TSparseSpaceType, typename TDenseSpaceType, typename TReordererType>
    inline std::istream& operator >> (std::istream& rIstream, SkylineLUCustomScalarSolver<TSparseSpaceType, TDenseSpaceType, TReordererType>& rThis){
        return rIstream;
    }


    template<typename TSparseSpaceType, typename TDenseSpaceType, typename TReordererType>
    inline std::ostream& operator << (std::ostream& rOstream, const SkylineLUCustomScalarSolver<TSparseSpaceType, TDenseSpaceType, TReordererType>& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }

}


#endif //QUEST_SKYLINE_LU_CUSTOM_SCALAR_SOLVER_HPP