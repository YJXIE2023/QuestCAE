#ifndef QUEST_CG_SOLVER_HPP
#define QUEST_CG_SOLVER_HPP

// 系统头文件
#include <string>
#include <iostream>

// 项目头文件
#include "includes/define.hpp"
#include "linear_solvers/iterative_solver.hpp"
#include "factories/preconditioner_factory.hpp"

namespace Quest{

    /**
     * @brief 共轭梯度法求解器
     */
    template<typename TSparseSpaceType, typename TDenseSpaceType,
             typename TPreconditionerType = Preconditioner<TSparseSpaceType, TDenseSpaceType>,
             typename TReordererType = Reorderer<TSparseSpaceType, TDenseSpaceType>>
    class CGSolver : public IterativeSolver<TSparseSpaceType, TDenseSpaceType, TPreconditionerType, TReordererType>{
        public:
            QUEST_CLASS_POINTER_DEFINITION(CGSolver);

            using BaseType = IterativeSolver<TSparseSpaceType, TDenseSpaceType, TPreconditionerType, TReordererType>;
            using SparseMatrixType = typename TSparseSpaceType::MatrixType;
            using VectorType = typename TSparseSpaceType::VectorType;
            using DenseMatrixType = typename TDenseSpaceType::MatrixType;

        public:
            /**
             * @brief 默认构造函数
             */
            CGSolver() {}


            /**
             * @brief 构造函数
             */
            CGSolver(double NewMaxTolerance) : BaseType(NewMaxTolerance) {}


            /**
             * @brief 构造函数
             */
            CGSolver(double NewMaxTolerance, unsigned int NewMaxIterationsNumber) : BaseType(NewMaxTolerance, NewMaxIterationsNumber) {}


            /**
             * @brief 构造函数
             */
            CGSolver(
                double NewMaxTolerance,
                unsigned int NewMaxIterationsNumber,
                typename TPreconditionerType::Pointer pNewPreconditioner
            ) : BaseType(NewMaxTolerance, NewMaxIterationsNumber, pNewPreconditioner) {}


            /**
             * @brief 构造函数
             */
            CGSolver(
                Parameters settings,
                typename TPreconditionerType::Pointer pNewPreconditioner
            ) : BaseType(settings, pNewPreconditioner) {}


            /**
             * @brief 构造函数
             */
            CGSolver(Parameters settings) : BaseType(settings) {
                if(settings.Has("Preconditioner_type")){
                    BaseType::SetPreconditioner(PreconditionerFactory<TSparseSpaceType, TDenseSpaceType>::Create(settings["Preconditioner_type"].GetString()));
                }
            }


            /**
             * @brief 复制构造函数
             */
            CGSolver(const CGSolver& Other) : BaseType(Other) {}


            /**
             * @brief 析构函数
             */
            ~CGSolver() override {}


            /**
             * @brief 重载赋值运算符
             */
            CGSolver& operator = (const CGSolver& Other){
                BaseType::operator=(Other);
                return *this;
            }


            /**
             * @brief 求解线性系统Ax=b，并将结果存储在系统向量 rX 中
             * @param rA 系统矩阵
             * @param rX 解向量
             * @param rB 右端常数向量
             */
            bool Solve(SparseMatrixType& rA, VectorType& rX, VectorType& rB) override{
                if(this->IsNotConsistent(rA, rX, rB))
                    return false;

                BaseType::GetPreconditioner()->Initialize(rA, rX, rB);
                BaseType::GetPreconditioner()->ApplyInverseRight(rX);
                BaseType::GetPreconditioner()->ApplyLeft(rB);

                bool is_solved = IterativeSolve(rA, rX, rB);

                QUEST_WARNING_IF("CG Linear Solver", !is_solved)<<"Non converged linear solution. ["<< BaseType::GetResidualNorm()/BaseType::mBNorm << " > "<<  BaseType::GetTolerance() << "]" << std::endl;

                BaseType::GetPreconditioner()->Finalize(rX);

                return is_solved;
            }


            /**
             * @brief 求解一组具有相同系数矩阵的线性系统的多重求解方法
             * @param rA 系数矩阵
             * @param rX 解向量
             * @param rB 右端常数向量
             */
            bool Solve(SparseMatrixType& rA, DenseMatrixType& rX, DenseMatrixType& rB) override{
                BaseType::GetPreconditioner()->Initialize(rA, rX, rB);

                bool is_solved = true;
                VectorType x(TDenseSpaceType::Size1(rX));
                VectorType b(TDenseSpaceType::Size1(rB));
                for(unsigned int i = 0; i < TDenseSpaceType::Size2(rX); i++){
                    TDenseSpaceType::GetColumn(i,rX,x);
                    TDenseSpaceType::GetColumn(i,rB,b);

                    BaseType::GetPreconditioner()->ApplyInverseRight(x);
                    BaseType::GetPreconditioner()->ApplyLeft(b);

                    is_solved &= IterativeSolve(rA, x, b);

                    BaseType::GetPreconditioner()->Finalize(x);
                }
                return is_solved;
            }


            stdd::string Info() const override{
                std::stringstream buffer;
                buffer << "Conjugate gradient linear solver with " << BaseType::GetPreconditioner()->Info();
                return  buffer.str();
            }


            void PrintInfo(std::ostream& rOstream) const override{
                rOstream << Info();
            }


            void PrintData(std::ostream& rOstream) const override{
                BaseType::PrintData(rOstream);
            }

        protected:

        private:
            /**
             * @brief 迭代求解线性系统Ax=b
             */
            bool IterativeSolve(SparseMatrixType& rA, VectorType& rX, VectorType& rB){
                const int size = TSparseSpaceType::Size(rA);

                BaseType::mIterationsNumber = 0;

                VectorType r(size);

                this->PreconditionedMult(rA, rX, r);
                TSparseSpaceType::ScaleAndAdd(1.0, rB, -1.0, r);

                BaseType::mBNorm = TSparseSpaceType::TwoNorm(rB);

                VectorType p(r);
                VectorType q(size);

                double roh0 = TSparseSpaceType::Dot(r, r);
                double roh1 = roh0;
                double beta = 0;

                if(fabs(roh0) < 1.0e-30)
                    return false;

                do{
                    this->PreconditionedMult(rA, p, q);

                    double pq = TSparseSpaceType::Dot(p, q);

                    if(fabs(pq) < 1.0e-30)
                        break;
                    
                    double alpha = roh0 / pq;

                    TSparseSpaceType::ScaleAndAdd(alpha, p, 1.00, rX);
                    TSparseSpaceType::ScaleAndAdd(-alpha, q, 1.00, r);

                    roh1 = TSparseSpaceType::Dot(r, r);

                    beta = roh1 / roh0;
                    TSparseSpaceType::ScaleAndAdd(1.0, r, beta, p);

                    roh0 = roh1;

                    BaseType::mResidualNorm = sqrt(roh1);
                    BaseType::mIterationsNumber++;
                }while(BaseType::IterationNeeded() && (fabs(roh0) > 1.0e-30));

                return BaseType::IsConverged();
            }


        private:

    };


    template<typename TSparseSpaceType, typename TDenseSpaceType, typename TPreconditionerType, typename TReordererType>
    inline std::istream& operator >> (std::istream& rIstream, CGSolver<TSparseSpaceType, TDenseSpaceType, TPreconditionerType, TReordererType>& rThis){
        return rIstream;
    }


    template<typename TSparseSpaceType, typename TDenseSpaceType, typename TPreconditionerType, typename TReordererType>
    inline std::ostream& operator << (std::ostream& rOstream, const CGSolver<TSparseSpaceType, TDenseSpaceType, TPreconditionerType, TReordererType>& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }

} // namespace Quest

#endif //QUEST_CG_SOLVER_HPP