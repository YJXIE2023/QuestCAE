#ifndef QUEST_IMPLICIT_SOLVING_STRATEGY_HPP
#define QUEST_IMPLICIT_SOLVING_STRATEGY_HPP

// 项目头文件
#include "includes/define.hpp"
#include "includes/model_part.hpp"
#include "solving_strategies/strategies/solving_strategy.hpp"
#include "solving_strategies/builder_and_solvers/builder_and_solvers.hpp"
#include "includes/quest_parameters.hpp"

namespace Quest{

    /**
     * @brief 隐式求解策略基类
     * 这是所有隐式策略（如线搜索、牛顿法等）的基类
     * @tparam TSparseSpace 线性代数稀疏空间
     * @tparam TDenseSpace 线性代数密集空间
     * @tparam TLinearSolver 线性求解器类型
     */
    template<class TSparseSpace, class TDenseSpace, class TLinearSolver>
    class ImplicitSolvingStrategy : public SolvingStrategy<TSparseSpace, TDenseSpace>{
        public:
            using BaseType = SolvingStrategy<TSparseSpace, TDenseSpace>;
            using TDataType = typename BaseType::TDataType;
            using TSystemMatrixType = typename BaseType::TSystemMatrixType;
            using TSystemVectorType = typename BaseType::TSystemVectorType;
            using TSystemMatrixPointerType = typename BaseType::TSystemMatrixPointerType;
            using TSystemVectorPointerType = typename BaseType::TSystemVectorPointerType;
            using LocalSystemMatrixType = typename BaseType::LocalSystemMatrixType;
            using LocalSystemVectorType = typename BaseType::LocalSystemVectorType;
            using TSchemeType = Scheme<TSparseSpace, TDenseSpace>;
            using TBuilderAndSolverType = BuilderAndSolver<TSparseSpace, TDenseSpace, TLinearSolver>;
            using ClassType = ImplicitSolvingStrategy<TSparseSpace, TDenseSpace, TLinearSolver>;
            using TDofType = typename BaseType::TDofType;
            using DofsArrayType = typename BaseType::DofsArrayType;
            using NodesArrayType = typename BaseType::NodesArrayType;
            using ElementsArrayType = typename BaseType::ElementsArrayType;
            using ConditionsArrayType = typename BaseType::ConditionsArrayType;

            QUEST_CLASS_POINTER_DEFINITION(ImplicitSolvingStrategy);

        public:
            /**
             * @brief 默认构造函数
             */
            explicit ImplicitSolvingStrategy() {}


            /**
             * @brief 构造函数，基于传入参数
             */
            explicit ImplicitSolvingStrategy(
                ModelPart& rModelPart,
                Parameters ThisParameters
            ): BaseType(rModelPart, ThisParameters)
            {
                ThisParameters = this->ValidateAndAssignParameters(ThisParameters, this->GetDefaultParameters());
                this->AssignSettings(ThisParameters);
            }


            /**
             * @brief 构造函数
             */
            explicit ImplicitSolvingStrategy(
                ModelPart& rModelPart,
                bool MoveMeshFlag = false
            ): BaseType(rModelPart, MoveMeshFlag){}


            /**
             * @brief 虚构函数
             */
            virtual ~ImplicitSolvingStrategy(){}


            /**
             * @brief 创建并返回一个隐式求解策略对象
             */
            typename BaseType::Pointer Create(
                ModelPart& rModelPart,
                Parameters ThisParameters
            ) const override{
                return Quest::make_shared<ClassType>(rModelPart, ThisParameters);
            }


            /**
             * @brief 该方法提供默认参数，以避免不同构造函数之间的冲突
             * @return 默认参数
             */
            Parameters GetDefaultParameters() const override
            {
                Parameters default_parameters = Parameters(R"(
                {
                    "name"                         : "implicit_solving_strategy",
                    "build_level"                  : 2
                })");

                const Parameters base_default_parameters = BaseType::GetDefaultParameters();
                default_parameters.RecursivelyAddMissingParameters(base_default_parameters);

                return default_parameters;
            }


            /**
             * @brief 返回当前类在参数中的名称
             */
            static std::string Name()
            {
                return "implicit_solving_strategy";
            }


            /**
             * @brief 设置构建级别
             */
            void SetRebuildLevel(int Level) override
            {
                mRebuildLevel = Level;
                mStiffnessMatrixIsBuilt = false;
            }


            /**
             * @brief 获取构建级别
             */
            int GetRebuildLevel() const override
            {
                return mRebuildLevel;
            }


            /**
             * @brief 设置刚度矩阵是否构建
             */
            void SetStiffnessMatrixIsBuilt(const bool StiffnessMatrixIsBuilt)
            {
                mStiffnessMatrixIsBuilt = StiffnessMatrixIsBuilt;
            }


            /**
             * @brief 获取刚度矩阵是否构建
             */
            bool GetStiffnessMatrixIsBuilt() const
            {
                return mStiffnessMatrixIsBuilt;
            }


            std::string Info() const override
            {
                return "ImplicitSolvingStrategy";
            }

        protected:
            /**
             * @brief 构建级别
             * @details
             * {
             * 0 -> 仅在第一次构建刚度矩阵时构建
             * 1 -> 在每个解算步骤开始时构建刚度矩阵
             * 2 -> 在每次迭代时构建刚度矩阵
             * }
             */
            int mRebuildLevel;

            /**
             * @brief 刚度矩阵是否创建的标志
             */
            bool mStiffnessMatrixIsBuilt;


            /**
             * @brief 该方法将设置分配给成员变量
             * @param ThisParameters 要分配给成员变量的参数
             */
            void AssignSettings(const Parameters ThisParameters) override
            {
                BaseType::AssignSettings(ThisParameters);

                mRebuildLevel = ThisParameters["build_level"].GetInt();
            }

        private:
            /**
             * @brief 复制构造函数
             */
            ImplicitSolvingStrategy(const ImplicitSolvingStrategy& Other);

    };

}

#endif //QUEST_IMPLICIT_SOLVING_STRATEGY_HPP