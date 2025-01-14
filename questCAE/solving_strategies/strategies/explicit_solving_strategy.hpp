#ifndef QUEST_EXPLICIT_SOLVING_STRATEGY_HPP
#define QUEST_EXPLICIT_SOLVING_STRATEGY_HPP

// 项目头文件
#include "solving_strategies/strategies/solving_strategy.hpp"
#include "includes/define.hpp"
#include "includes/model_part.hpp"
#include "utilities/variable_utils.hpp"
#include "utilities/parallel_utilities.hpp"
#include "solving_strategies/builder_and_solvers/explicit_builder.HPP"
#include "includes/quest_parameters.hpp"
#include "utilities/entities_utilities.hpp"

namespace Quest{

    /**
     * @brief 显式求解策略类
     */
    template <class TSparseSpace, class TDenseSpace>
    class ExplicitSolvingStrategy : public SolvingStrategy<TSparseSpace, TDenseSpace>{
        public:
            using ExplicitBuilderType = ExplicitBuilder<TSparseSpace, TDenseSpace>;
            using ExplicitBuilderPointerType = typename ExplicitBuilderType::Pointer;
            using DofType = typename ExplicitBuilderType::DofType;
            using BaseType = SolvingStrategy<TSparseSpace, TDenseSpace>;
            using ClassType = ExplicitSolvingStrategy<TSparseSpace, TDenseSpace>;

            QUEST_CLASS_POINTER_DEFINITION(ExplicitSolvingStrategy);

        public:
            /**
             * @brief 默认构造函数
             */
            explicit ExplicitSolvingStrategy(){}


            /**
             * @brief 构造函数，基于输入参数构造
             */
            explicit ExplicitSolvingStrategy(
                ModelPart &rModelPart,
                Parameters ThisParameters
            ): BaseType(rModelPart, ThisParameters)
            {
                mpExplicitBuilder = Quest::make_unique<ExplicitBuilder<TSparseSpace, TDenseSpace>>();
            }


            /**
             * @brief 构造函数
             */
            explicit ExplicitSolvingStrategy(
                ModelPart &rModelPart,
                typename ExplicitBuilderType::Pointer pExplicitBuilder,
                bool MoveMeshFlag = false,
                int RebuildLevel = 0
            ): BaseType(rModelPart, MoveMeshFlag), 
                mpExplicitBuilder(pExplicitBuilder)
            {
                SetRebuildLevel(RebuildLevel);
            }


            /**
             * @brief 构造函数
             */
            explicit ExplicitSolvingStrategy(
                ModelPart &rModelPart,
                bool MoveMeshFlag = false,
                int RebuildLevel = 0
            ): BaseType(rModelPart, MoveMeshFlag)
            {
                SetRebuildLevel(RebuildLevel);
                mpExplicitBuilder = Quest::make_unique<ExplicitBuilder<TSparseSpace, TDenseSpace>>();
            }


            ExplicitSolvingStrategy(const ExplicitSolvingStrategy &Other) = delete;


            /**
             * @brief 析构函数
             */
            virtual ~ExplicitSolvingStrategy() {}


            /**
             * @brief 创建并返回一个显式求解策略类的指针
             */
            typename BaseType::Pointer Create(
                ModelPart& rModelPart,
                Parameters ThisParameters
            ) const override
            {
                return Quest::make_shared<ClassType>(rModelPart, ThisParameters);
            }


            /**
             * @brief 成员变量的初始化及先前操作
             */
            void Initialize() override
            {
                EntitiesUtilities::InitializeAllEntities(BaseType::GetModelPart());

                if (mRebuildLevel != 0) {
                    mpExplicitBuilder->SetResetDofSetFlag(true);
                }

                if (BaseType::GetMoveMeshFlag()) {
                    mpExplicitBuilder->SetResetLumpedMassVectorFlag(true);
                }

                mpExplicitBuilder->Initialize(BaseType::GetModelPart());
            }


            /**
             * @brief 清除内部存储
             */
            void Clear() override
            {
                mpExplicitBuilder->Clear();
            }


            /**
             * @brief 执行在求解步骤之前应该完成的所有必要操作。
             * @details 应使用一个成员变量作为标志，确保每个步骤仅调用一次此函数。
             */
            void InitializeSolutionStep() override
            {
                EntitiesUtilities::InitializeSolutionStepAllEntities(BaseType::GetModelPart());

                mpExplicitBuilder->InitializeSolutionStep(BaseType::GetModelPart());
            }


            /**
             * @brief 执行在求解步骤之后应该完成的所有必要操作。
             * @details 应使用一个成员变量作为标志，确保每个步骤仅调用一次此函数。
             */
            void FinalizeSolutionStep() override
            {
                EntitiesUtilities::FinalizeSolutionStepAllEntities(BaseType::GetModelPart());

                mpExplicitBuilder->FinalizeSolutionStep(BaseType::GetModelPart());
            }


            /**
             * @brief 求解当前步
             */
            bool SolveSolutionStep() override
            {
                EntitiesUtilities::InitializeNonLinearIterationAllEntities(BaseType::GetModelPart());

                if(BaseType::GetModelPart().MasterSlaveConstraints().size() != 0) {
                    mpExplicitBuilder->ApplyConstraints(BaseType::GetModelPart());
                }

                SolveWithLumpedMassMatrix();

                if (BaseType::GetMoveMeshFlag()) {
                    BaseType::MoveMesh();
                }

                EntitiesUtilities::FinalizeNonLinearIterationAllEntities(BaseType::GetModelPart());

                return true;
            }


            /**
             * @brief 设置自由度集合的重建等级
             */
            void SetRebuildLevel(int Level) override
            {
                mRebuildLevel = Level;
            }


            /**
             * @brief 获取自由度集合的重建等级
             */
            int GetRebuildLevel() const override
            {
                return mRebuildLevel;
            }


            /**
             * @brief 返回显示构建器的指针
             */
            ExplicitBuilderPointerType pGetExplicitBuilder()
            {
                return mpExplicitBuilder;
            };


            /**
             * @brief 获取显示构建器对象
             */
            ExplicitBuilderType& GetExplicitBuilder()
            {
                QUEST_ERROR_IF(mpExplicitBuilder == nullptr) << "Asking for builder and solver when it is empty" << std::endl;
                return *mpExplicitBuilder;
            };

            const ExplicitBuilderType& GetExplicitBuilder() const
            {
                QUEST_ERROR_IF(mpExplicitBuilder == nullptr) << "Asking for builder and solver when it is empty" << std::endl;
                return *mpExplicitBuilder;
            };


            /**
             * @brief 获取残差范数
             */
            double GetResidualNorm() override
            {
                auto& r_explicit_bs = GetExplicitBuilder();
                auto& r_dof_set = r_explicit_bs.GetDofSet();

                r_explicit_bs.BuildRHS(BaseType::GetModelPart());

                double res_norm = 0.0;
                res_norm = block_for_each<SumReduction<double>>(
                    r_dof_set,
                    [](DofType &rDof){return rDof.GetSolutionStepReactionValue();}
                );

                return res_norm;
            }


            /**
             * @brief 该方法提供默认参数，以避免不同构造函数之间的冲突
             * @return 默认参数
             */
            Parameters GetDefaultParameters() const override
            {
                Parameters default_parameters = Parameters(R"(
                {
                    "explicit_solving_strategy" : "explicit_solving_strategy",
                    "rebuild_level"             : 0,
                    "explicit_builder_settings" : {
                        "name": "explicit_builder"
                    }
                })");

                const Parameters base_default_parameters = BaseType::GetDefaultParameters();
                default_parameters.RecursivelyAddMissingParameters(base_default_parameters);

                return default_parameters;
            }


            /**
             * @brief 返回当前类在参数中设置的名称
             */
            static std::string Name()
            {
                return "explicit_solving_strategy";
            }


            std::string Info() const override
            {
                return "ExplicitSolvingStrategy";
            }

        protected:
            /**
             * @brief 自由度集合的重建等级
             * 0 -> 仅在一次设置 DOF 集合
             * 1 -> 在每个求解步骤开始时设置 DOF 集合
             */
            int mRebuildLevel;


            /**
             * @brief 计算显式更新
             */
            virtual void SolveWithLumpedMassMatrix()
            {
                QUEST_ERROR << "Calling the base ExplicitSolvingStrategy SolveWithLumpedMassMatrix(). Implement the specific explicit scheme solution update in a derived class" << std::endl;
            }


            /**
             * @brief 获取时间增量
             */
            virtual inline double GetDeltaTime()
            {
                return BaseType::GetModelPart().GetProcessInfo().GetValue(DELTA_TIME);
            }


            /**
             * @brief 该方法将设置分配给成员变量
             * @param ThisParameters 要分配给成员变量的参数
             */
            void AssignSettings(const Parameters ThisParameters) override
            {
                BaseType::AssignSettings(ThisParameters);

                const bool rebuild_level = ThisParameters["rebuild_level"].GetInt();
                SetRebuildLevel(rebuild_level);
            }

        private:
            /**
             * @brief 显示构建器指针
             */
            ExplicitBuilderPointerType mpExplicitBuilder = nullptr;

    };

}

#endif //QUEST_EXPLICIT_SOLVING_STRATEGY_HPP