#ifndef QUEST_ITERATIVE_SOLVER_HPP
#define QUEST_ITERATIVE_SOLVER_HPP

// 系统头文件
#include <string>
#include <iostream>

// 项目头文件
#include "includes/define.hpp"
#include "includes/quest_parameters.hpp"
#include "linear_solvers/linear_solver.hpp"
#include "linear_solvers/preconditioner/preconditioner.hpp"

namespace Quest{

    /**
     * @class IterativeSolver
     * @brief 迭代求解器基类
     * @tparam TSparseSpaceType 指定未知数、系数、稀疏矩阵、未知数的向量、右端项向量及其相应运算符的类型
     * @tparam TDenseSpaceType 指定用于临时矩阵或多解未知数和右端项的矩阵类型及其运算符
     * @tparam TPreconditionerType 指定预处理器类型
     * @tparam TStopCriteriaType 指定停止准则类型
     * @tparam TReodererType 指定重排序器类型
     */
    template<typename TSparseSpaceType,
             typename TDenseSpaceType,
             typename TPreconditionerType = Preconditioner<TSparseSpaceType, TDenseSpaceType>,
             typename TReordererType = Reorderer<TSparseSpaceType, TDenseSpaceType>>
    class IterativeSolver : public LinearSolver<TSparseSpaceType, TDenseSpaceType, TReordererType>{
        public:
            QUEST_CLASS_POINTER_DEFINITION(IterativeSolver);

            using BaseType = LinearSolver<TSparseSpaceType, TDenseSpaceType, TReordererType>;
            using SparseMatrixType = typename TSparseSpaceType::MatrixType;
            using VectorType = typename TSparseSpaceType::VectorType;
            using DenseMatrixType = typename TDenseSpaceType::MatrixType;
            using PreconditionerType = TPreconditionerType;
            using IndexType = typename TSparseSpaceType::IndexType;

        public:
            /**
             * @brief 构造函数
             */
            IterativeSolver():
                mResidualNorm(0),
                mIterationsNumber(0),
                mBNorm(0),
                mpPreconditioner(new TPreconditionerType()),
                mTolerance(0),
                mMaxIterationsNumber(0)
            {}

            /**
             * @brief 构造函数
             */
            IterativeSolver(double NewTolerance):
                mResidualNorm(0),
                mIterationsNumber(0),
                mBNorm(0),
                mpPreconditioner(new TPreconditionerType()),
                mTolerance(NewTolerance),
                mMaxIterationsNumber(0)
            {}

            
            /**
             * @brief 构造函数
             */
            IterativeSolver(double NewTolerance, unsigned int NewMaxIterationsNumber):
                mResidualNorm(0),
                mIterationsNumber(0),
                mBNorm(0),
                mpPreconditioner(new TPreconditionerType()),
                mTolerance(NewTolerance),
                mMaxIterationsNumber(NewMaxIterationsNumber)
            {}


            /**
             * @brief 构造函数
             */
            IterativeSolver(
                double NewTolerance,
                unsigned int NewMaxIterationsNumber,
                typename TPreconditionerType::Pointer pNewPreconditioner
            ):
                mResidualNorm(0),
                mIterationsNumber(0),
                mBNorm(0),
                mpPreconditioner(pNewPreconditioner),
                mTolerance(NewTolerance),
                mMaxIterationsNumber(NewMaxIterationsNumber)
            {}


            /**
             * @Brief 构造函数
             */
            IterativeSolver(
                Parameters& settings,
                typename TPreconditionerType::Pointer pNewPreconditioner = Quest::make_shared<TPreconditionerType>()
            ):
                mResidualNorm(0),
                mIterationsNumber(0),
                mBNorm(0),
                mpPreconditioner(pNewPreconditioner)
            {
                QUEST_TRY

                Parameters default_parameters(
                    R"({
                        "solver_type": "IterativeSolver",
                        "tolerance": 1e-6,
                        "max_iteration":200,
                        "preconditioner_type":"none",
                        "scaling": false
                    })"
                );

                settings.ValidateAndAssignDefaults(default_parameters);

                this->SetTolerance(settings["tolerance"].GetDouble());
                this->SetMaxIterationsNumber(settings["max_iteration"].GetInt());

                QUEST_CATCH("")
            }


            /**
             * @brief 复制构造函数
             */
            IterativeSolver(const IterativeSolver& Other):
                BaseType(Other),
                mResidualNorm(Other.mResidualNorm),
                mIterationsNumber(Other.mIterationsNumber),
                mBNorm(Other.mBNorm),
                mpPreconditioner(Other.mpPreconditioner),
                mTolerance(Other.mTolerance),
                mMaxIterationsNumber(Other.mMaxIterationsNumber)
            {}


            /**
             * @brief 析构函数
             */
            ~IterativeSolver() override {}


            /**
             * @brief 赋值运算符重载
             */
            IterativeSolver& operator=(const IterativeSolver& Other){
                BaseType::operator=(Other);
                mResidualNorm = Other.mResidualNorm;
                mFirstResidualNorm = Other.mFirstResidualNorm;
                mIterationsNumber = Other.mIterationsNumber;
                mBNorm = Other.mBNorm;
                return *this;
            }


            /**
             * @brief 在每次系统中的系数发生变化时调用
             * @param rA 系数矩阵
             * @param rX 解向量
             * @param rB 右端项向量
             */
            void InitializeSolutionStep(SparseMatrixType& rA, VectorType& rX, VectorType& rB) override{
                GetPreconditioner()->FinalizeSolutionStep(rA, rX, rB);
            }


            /**
             * @brief 清空所有求解器内部数据
             */
            void Clear() override{
                GetPreconditioner()->Clear();
            }


            /** 某些求解器可能需要一定程度的矩阵结构信息。举个例子，
             *  在求解混合 u-p 问题时，识别与 v 和 p 相关的行是非常重要的。
             *  另一个例子是平滑聚合求解器的旋转零空间的自动设置，
             *  这需要知道与特定自由度 (dof) 相关联节点的空间位置。
             *  此函数用于指示求解器是否需要此类数据。
             */
            bool AdditionalPhysicalDataIsNeeded() override{
                if (GetPreconditioner()->AdditionalPhysicalDataIsNeeded()){
                    return true;
                }
                else{
                    return false;
                }
            }


            /** 某些求解器可能需要一定程度的矩阵结构信息。举个例子，
             *  在求解混合 u-p 问题时，识别与 v 和 p 相关的行是非常重要的。
             *  另一个例子是平滑聚合求解器的旋转零空间的自动设置，
             *  这需要知道与特定自由度 (dof) 相关联节点的空间位置。
             *  此函数是最终提供此类数据的地方。
             */
            void ProvideAdditionalData(
                SparseMatrixType& rA,
                VectorType& rX,
                VectorType& rB,
                typename ModelPart::DofsArrayType& rdof_set,
                ModelPart& r_model_part
            ) override{
                if(GetPreconditioner()->AdditionalPhysicalDataIsNeeded()){
                    GetPreconditioner()->ProvideAdditionalData(rA, rX, rB, rdof_set, r_model_part);
                }
            }


            /**
             * @brief 获取预处理器
             */
            virtual typename TPreconditionerType::Pointer GetPreconditioner(void){
                return mpPreconditioner;
            }


            /**
             * @brief 获取预处理器
             */
            virtual const typename TPreconditionerType::Pointer GetPreconditioner(void) const{
                return mpPreconditioner;
            }

            /**
             * @brief 设置预处理器
             */
            virtual void SetPreconditioner(typename TPreconditionerType::Pointer pNewPreconditioner){
                mpPreconditioner = pNewPreconditioner;
            }

            /**
             * @brief 设置最大迭代次数
             */
            virtual void SetMaxIterationsNumber(unsigned int NewMaxIterationsNumber){
                mMaxIterationsNumber = NewMaxIterationsNumber;
            }


            /**
             * @brief 获取最大迭代次数
             */
            virtual IndexType GetMaxIterationsNumber(){
                return mMaxIterationsNumber;
            }


            /**
             * @brief 设置迭代次数
             */
            virtual void SetIterationsNumber(unsigned int NewIterationsNumber){
                mIterationsNumber = NewIterationsNumber;
            }


            /**
             * @brief 获取迭代次数
             */
            IndexType GetIterationsNumber() override{
                return mIterationsNumber;
            }


            /**
             * @brief 设置容许误差
             */
            void SetTolerance(double NewTolerance) override{
                mTolerance = NewTolerance;
            }


            /**
             * @brief 获取容许误差
             */
            double GetTolerance() override{
                return mTolerance;
            }


            /**
             * @brief 设置残差范数
             */
            virtual void SetResidualNorm(double NewResidualNorm){
                if(mIterationsNumber == 1){
                    mFirstResidualNorm = NewResidualNorm;
                }
                mResidualNorm = NewResidualNorm;
            }


            /**
             * @brief 获取残差范数
             */
            virtual double GetResidualNorm(){
                return mResidualNorm;
            }


            /**
             * @brief 判断是否需要继续迭代
             */
            virtual bool IterationNeeded(){
                return (mIterationsNumber < mMaxIterationsNumber) && (mResidualNorm > mTolerance*mBNorm);
            }


            /**
             * @brief 判断求解是否收敛
             */
            virtual bool IsConverged(){
                return (mResidualNorm <= mTolerance*mBNorm);
            }


            std::string Info() const override{
                std::stringstream buffer;
                buffer << "Iterative solver with " << GetPreconditioner()->Info();
                return buffer.str();
            }


            void PrintInfo(std::ostream& rOstream) const override{
                rOstream << Info();
            }


            void PrintData(std::ostream& rOStream) const override
            {
                if (mBNorm == 0.00)
                    if (mResidualNorm != 0.00)
                        rOStream << "    Residual ratio : infinite" << std::endl;
                    else
                        rOStream << "    Residual ratio : 0" << std::endl;
                else
                {
                    rOStream << "    Initial Residual ratio : " << mBNorm << std::endl;
                    rOStream << "    Final Residual ratio : " << mResidualNorm << std::endl;
                    rOStream << "    Residual ratio : " << mResidualNorm / mBNorm << std::endl;
                    rOStream << "    Slope : " << (mResidualNorm - mBNorm) / mIterationsNumber << std::endl;
                }

                rOStream << "    Tolerance : " << mTolerance << std::endl;
                rOStream << "    Number of iterations : " << mIterationsNumber << std::endl;
                rOStream << "    Maximum number of iterations : " << mMaxIterationsNumber;
                if (mMaxIterationsNumber == mIterationsNumber)
                    rOStream << std::endl << "!!!!!!!!!!!! ITERATIVE SOLVER NON CONVERGED !!!!!!!!!!!!" << mMaxIterationsNumber;
            }

        protected:
            /**
             * @brief 残差范数
             */
            double mResidualNorm;

            /**
             * @brief 初始残差范数
             */
            double mFirstResidualNorm;

            /**
             * @brief 迭代次数
             */
            IndexType mIterationsNumber;

            /**
             * @brief 右端项的范数
             */
            double mBNorm;


            /**
             * @brief 预处理器乘法
             */
            void PreconditionedMult(SparseMatrixType& rA, VectorType& rX, VectorType& rY){
                GetPreconditioner()->Mult(rA, rX, rY);
            }


            /**
             * @brief 预处理转置乘法
             */
            void PreconditionedTransposeMult(SparseMatrixType& rA, VectorType& rX, VectorType& rY){
                GetPreconditioner()->TransposeMult(rA, rX, rY);
            }

        private:
            /**
             * @brief 预处理器指针
             */
            typename TPreconditionerType::Pointer mpPreconditioner;

            /**
             * @brief 最大容许误差
             */
            double mTolerance;

            /**
             * @brief 最大迭代次数
             */
            IndexType mMaxIterationsNumber;

    };


    template<typename TSparseSpaceType,
             typename TDenseSpaceType,
             typename TPreconditionerType,
             typename TReordererType>
    inline std::istream& operator>>(std::istream& rIstream, IterativeSolver<TSparseSpaceType, TDenseSpaceType, TPreconditionerType, TReordererType>& rThis){
        return rIstream;
    }


    template<typename TSparseSpaceType,
             typename TDenseSpaceType,
             typename TPreconditionerType,
             typename TReordererType>
    inline std::ostream& operator<<(std::ostream& rOstream, const IterativeSolver<TSparseSpaceType, TDenseSpaceType, TPreconditionerType, TReordererType>& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }

}

#endif //QUEST_ITERATIVE_SOLVER_HPP