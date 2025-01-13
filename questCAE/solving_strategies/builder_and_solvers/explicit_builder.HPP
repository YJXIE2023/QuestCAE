#ifndef QUEST_EXPLICIT_BUILDER_HPP
#define QUEST_EXPLICIT_BUILDER_HPP


// 系统头文件
#include <set>
#include <unordered_set>

// 项目头文件
#include "includes/define.hpp"
#include "includes/model_part.hpp"
#include "utilities/parallel_utilities.hpp"
#include "utilities/constraint_utilities.hpp"
#include "includes/quest_parameters.hpp"
#include "factories/factory.hpp"
#include "utilities/atomic_utilities.hpp"

namespace Quest{

    /**
     * @class ExplicitBuilder
     * @ingroup QuestCore
     * @brief 当前类提供了基本显式构建器和求解操作的实现。
     * @details 右手边（RHS）由不平衡的荷载（残差）组成
     * 自由度被重新排序，将受限的自由度放在系统的末尾，按与DofSet相反的顺序排列
     */
    template<class TSparseSpace, class TDenseSpace >
    class ExplicitBuilder{
        public:
            using SizeType = std::size_t;
            using IndexType = std::size_t;
            using TDataType = typename TSparseSpace::DataType;
            using TSystemMatrixType = typename TSparseSpace::MatrixType;
            using TSystemVectorType = typename TSparseSpace::VectorType;
            using TSystemMatrixPointerType = typename TSparseSpace::MatrixPointerType;
            using TSystemVectorPointerType = typename TSparseSpace::VectorPointerType;
            using LocalSystemMatrixType = typename TDenseSpace::MatrixType;
            using LocalSystemVectorType = typename TDenseSpace::VectorType;
            using DofType = ModelPart::DofType;
            using DofsArrayType = ModelPart::DofsArrayType;
            using DofsVectorType = ModelPart::DofsVectorType;
            using DofSetType = typename std::unordered_set<DofType::Pointer, DofPointerHasher>;
            using NodesArrayType = ModelPart::NodesContainerType;
            using ElementsArrayType = ModelPart::ElementsContainerType;
            using ConditionsArrayType = ModelPart::ConditionsContainerType;
            using ElementsContainerType = PointerVectorSet<Element, IndexedObject>;
            using ClassType = ExplicitBuilder<TSparseSpace, TDenseSpace>;

            QUEST_CLASS_POINTER_DEFINITION(ExplicitBuilder);

        public:
            /**
             * @brief 构造函数，基于输入参数
             */
            explicit ExplicitBuilder(Parameters ThisParameters)
            {
                ThisParameters = this->ValidateAndAssignParameters(ThisParameters, this->GetDefaultParameters());
                this->AssignSettings(ThisParameters);
            }


            /**
             * @brief 构造函数
             */
            explicit ExplicitBuilder() = default;


            /**
             * @brief 析构函数
             */
            virtual ~ExplicitBuilder() = default;


            /**
             * @brief 创建并返回一个新的ExplicitBuilder对象指针
             */
            virtual typename ClassType::Pointer Create(Parameters ThisParameters) const
            {
                return Quest::make_shared<ClassType>(ThisParameters);
            }


            /**
             * @brief 返回是否需要计算反力
             */
            bool GetCalculateReactionsFlag() const
            {
                return mCalculateReactionsFlag;
            }


            /**
             * @brief 设置是否需要计算反力
             */
            void SetCalculateReactionsFlag(bool CalculateReactionsFlag)
            {
                mCalculateReactionsFlag = CalculateReactionsFlag;
            }


            /**
             * @brief 返回自由度集合是否初始化的标志
             */
            bool GetDofSetIsInitializedFlag() const
            {
                return mDofSetIsInitialized;
            }


            /**
             * @brief 设置自由度集合是否初始化的标志
             */
            void SetDofSetIsInitializedFlag(bool DofSetIsInitialized)
            {
                mDofSetIsInitialized = DofSetIsInitialized;
            }


            /**
             * @brief 返回是否重新设置节点自由度几何的标志
             */
            bool GetResetDofSetFlag() const
            {
                return mResetDofSetFlag;
            }


            /**
             * @brief 设置是否重新设置节点自由度几何的标志
             */
            void SetResetDofSetFlag(bool ResetDofSetFlag)
            {
                mResetDofSetFlag = ResetDofSetFlag;
            }


            /**
             * @brief 返回是否需要重置集中质量向量
             */
            bool GetResetLumpedMassVectorFlag() const
            {
                return mResetLumpedMassVectorFlag;
            }


            /**
             * @brief 设置是否需要重置集中质量向量
             */
            void SetResetLumpedMassVectorFlag(bool ResetLumpedMassVectorFlag)
            {
                mResetLumpedMassVectorFlag = ResetLumpedMassVectorFlag;
            }


            /**
             * @brief 返回模型的总自由度数
             */
            unsigned int GetEquationSystemSize() const
            {
                return mEquationSystemSize;
            }


            /**
             * @brief 获取自由度集合
             */
            DofsArrayType& GetDofSet()
            {
                return mDofSet;
            }

            const DofsArrayType& GetDofSet() const
            {
                return mDofSet;
            }


            /**
             * @brief 获取集中质量矩阵向量指针
             * @return TSystemVectorPointerType& 集中质量矩阵向量指针
             */
            TSystemVectorPointerType& pGetLumpedMassMatrixVector()
            {
                return mpLumpedMassVector;
            }


            /**
             * @brief 获取集中质量向量
             */
            TSystemVectorType& GetLumpedMassMatrixVector()
            {
                QUEST_ERROR_IF_NOT(mpLumpedMassVector) << "Lumped mass matrix vector is not initialized!" << std::endl;
                return (*mpLumpedMassVector);
            }


            /**
             * @brief 构建右侧向量（RHS）的函数
             * 该向量的大小可以是总自由度数，也可以是未受限自由度的数量
             * @param rModelPart 要计算的模型部分
             */
            virtual void BuildRHS(ModelPart& rModelPart)
            {
                QUEST_TRY

                BuildRHSNoDirichlet(rModelPart);

                QUEST_CATCH("")
            }


            /**
             * @brief 构建右侧向量（RHS）的函数
             * 该向量的大小可以是总自由度数，也可以是未受限自由度的数量
             * @param rModelPart 要计算的模型部分
             */
            virtual void BuildRHSNoDirichlet(ModelPart& rModelPart)
            {
                QUEST_TRY

                InitializeDofSetReactions();

                const auto &r_elements_array = rModelPart.Elements();
                const auto &r_conditions_array = rModelPart.Conditions();
                const int n_elems = static_cast<int>(r_elements_array.size());
                const int n_conds = static_cast<int>(r_conditions_array.size());

                const auto& r_process_info = rModelPart.GetProcessInfo();

                #pragma omp parallel firstprivate(n_elems, n_conds)
                {
                    #pragma omp for schedule(guided, 512) nowait
                    for (int i_elem = 0; i_elem < n_elems; ++i_elem) {
                        auto it_elem = r_elements_array.begin() + i_elem;
                        if (it_elem->IsActive()) {
                            it_elem->AddExplicitContribution(r_process_info);
                        }
                    }

                    #pragma omp for schedule(guided, 512)
                    for (int i_cond = 0; i_cond < n_conds; ++i_cond) {
                        auto it_cond = r_conditions_array.begin() + i_cond;
                        if (it_cond->IsActive()) {
                            it_cond->AddExplicitContribution(r_process_info);
                        }
                    }
                }

                QUEST_CATCH("")
            }


            /**
             * @brief 应用约束条件
             * @param rModelPart 要计算的模型部分
             * @param rA 系统方程的左侧矩阵（LHS）
             * @param rb 系统方程的右侧向量（RHS）
             */
            virtual void ApplyConstraints(ModelPart& rModelPart)
            {
                ConstraintUtilities::ResetSlaveDofs(rModelPart);

                ConstraintUtilities::ApplyConstraints(rModelPart);
            }


            /**
             * @brief 执行那些预期只执行一次的操作
             * @param rModelPart 要计算的模型部分
             */
            virtual void Initialize(ModelPart& rModelPart)
            {
                if (!mDofSetIsInitialized) {
                    this->SetUpDofSet(rModelPart);
                    this->SetUpDofSetEquationIds();
                    this->SetUpLumpedMassVector(rModelPart);
                } else if (!mLumpedMassVectorIsInitialized) {
                    QUEST_WARNING("ExplicitBuilder") << "Calling Initialize() with already initialized DOF set. Initializing lumped mass vector." << std::endl;;
                    this->SetUpLumpedMassVector(rModelPart);
                } else {
                    QUEST_WARNING("ExplicitBuilder") << "Calling Initialize() with already initialized DOF set and lumped mass vector." << std::endl;;
                }
            }


            /**
             * @brief 在求解步骤开始时对方程系统应用某些操作
             * @param rModelPart 要计算的模型部分
             */
            virtual void InitializeSolutionStep(ModelPart& rModelPart)
            {
                if (mResetDofSetFlag) {
                    this->SetUpDofSet(rModelPart);
                    this->SetUpDofSetEquationIds();
                    this->SetUpLumpedMassVector(rModelPart);
                } else if (mResetLumpedMassVectorFlag) {
                    this->SetUpLumpedMassVector(rModelPart);
                }

                this->InitializeDofSetReactions();
            }


            /**
             * @brief 在求解步骤结束时对方程系统应用某些操作
             */
            virtual void FinalizeSolutionStep(ModelPart& rModelPart)
            {
                if (mCalculateReactionsFlag) {
                    this->CalculateReactions();
                }
            }


            /**
             * @brief 该函数旨在求解步骤结束时调用，用于清理不再需要的内存存储
             */
            virtual void Clear()
            {
                this->mDofSet = DofsArrayType();
                this->mpLumpedMassVector.reset();

                QUEST_INFO_IF("ExplicitBuilder", this->GetEchoLevel() > 0) << "Clear Function called" << std::endl;
            }


            /**
             * @brief 该函数旨在一次性执行所有必要的检查，以确保输入的有效性。由于该函数旨在捕捉用户的错误，检查可能是“昂贵”的
             * @param rModelPart 计算所需的模型部分
             * @return 0 表示一切正常
             */
            virtual int Check(const ModelPart& rModelPart) const
            {
                QUEST_TRY

                return 0;

                QUEST_CATCH("");
            }


            /**
             * @brief 该方法提供默认参数，以避免不同构造函数之间的冲突
             * @return 默认参数
             */
            virtual Parameters GetDefaultParameters() const
            {
                const Parameters default_parameters = Parameters(R"(
                {
                    "name" : "explicit_builder"
                })");
                return default_parameters;
            }


            /**
             * @brief 返回类在参数中的名称
             */
            static std::string Name()
            {
                return "explicit_builder";
            }


            /**
             * @brief 设置echo等级
             */
            void SetEchoLevel(int Level)
            {
                mEchoLevel = Level;
            }


            /**
             * @brief 获取echo等级
             */
            int GetEchoLevel() const
            {
                return mEchoLevel;
            }


            virtual std::string Info() const
            {
                return "ExplicitBuilder";
            }


            virtual void PrintInfo(std::ostream& rOStream) const
            {
                rOStream << Info();
            }


            virtual void PrintData(std::ostream& rOStream) const
            {
                rOStream << Info();
            }

        protected:
            /**
             * @brief 构建问题中涉及的所有DofSets的列表，通过向每个元素和条件请求其自由度（Dofs）。
             * @details 这些自由度列表存储在ExplicitBuilder中，因为它与矩阵和RHS的构建方式密切相关。
             * @param rModelPart 要计算的模型部分
             */
            virtual void SetUpDofSet(const ModelPart& rModelPart)
            {
                QUEST_TRY;

                QUEST_INFO_IF("ExplicitBuilder", this->GetEchoLevel() > 1) << "Setting up the dofs" << std::endl;

                // Gets the array of elements, conditions and constraints from the modeler
                const auto &r_elements_array = rModelPart.Elements();
                const auto &r_conditions_array = rModelPart.Conditions();
                const auto &r_constraints_array = rModelPart.MasterSlaveConstraints();
                const int n_elems = static_cast<int>(r_elements_array.size());
                const int n_conds = static_cast<int>(r_conditions_array.size());
                const int n_constraints = static_cast<int>(r_constraints_array.size());

                DofSetType dof_global_set;
                dof_global_set.reserve(n_elems*20);

                DofsVectorType dof_list;
                DofsVectorType second_dof_list; 

                #pragma omp parallel firstprivate(dof_list, second_dof_list)
                {
                    const auto& r_process_info = rModelPart.GetProcessInfo();

                    DofSetType dofs_tmp_set;
                    dofs_tmp_set.reserve(20000);

                    #pragma omp for schedule(guided, 512) nowait
                    for (int i_elem = 0; i_elem < n_elems; ++i_elem) {
                        const auto it_elem = r_elements_array.begin() + i_elem;
                        it_elem->GetDofList(dof_list, r_process_info);
                        dofs_tmp_set.insert(dof_list.begin(), dof_list.end());
                    }

                    #pragma omp for schedule(guided, 512) nowait
                    for (int i_cond = 0; i_cond < n_conds; ++i_cond) {
                        const auto it_cond = r_conditions_array.begin() + i_cond;
                        it_cond->GetDofList(dof_list, r_process_info);
                        dofs_tmp_set.insert(dof_list.begin(), dof_list.end());
                    }

                    #pragma omp for  schedule(guided, 512) nowait
                    for (int i_const = 0; i_const < n_constraints; ++i_const) {
                        auto it_const = r_constraints_array.begin() + i_const;
                        it_const->GetDofList(dof_list, second_dof_list, r_process_info);
                        dofs_tmp_set.insert(dof_list.begin(), dof_list.end());
                        dofs_tmp_set.insert(second_dof_list.begin(), second_dof_list.end());
                    }

                    #pragma omp critical
                    {
                        dof_global_set.insert(dofs_tmp_set.begin(), dofs_tmp_set.end());
                    }
                }

                QUEST_INFO_IF("ExplicitBuilder", ( this->GetEchoLevel() > 2)) << "Initializing ordered array filling\n" << std::endl;

                mDofSet = DofsArrayType();
                DofsArrayType temp_dof_set;
                temp_dof_set.reserve(dof_global_set.size());
                for (auto it_dof = dof_global_set.begin(); it_dof != dof_global_set.end(); ++it_dof) {
                    temp_dof_set.push_back(*it_dof);
                }
                temp_dof_set.Sort();
                mDofSet = temp_dof_set;
                mEquationSystemSize = mDofSet.size();

                QUEST_ERROR_IF(mDofSet.size() == 0) << "No degrees of freedom!" << std::endl;

                for (auto it_dof = mDofSet.begin(); it_dof != mDofSet.end(); ++it_dof) {
                    QUEST_ERROR_IF_NOT(it_dof->HasReaction()) << "Reaction variable not set for the following : " << std::endl
                        << "Node : " << it_dof->Id() << std::endl
                        << "Dof : " << (*it_dof) << std::endl << "Not possible to calculate reactions." << std::endl;
                }

                mDofSetIsInitialized = true;

                QUEST_INFO_IF("ExplicitBuilder", ( this->GetEchoLevel() > 2)) << "Number of degrees of freedom:" << mDofSet.size() << std::endl;
                QUEST_INFO_IF("ExplicitBuilder", ( this->GetEchoLevel() > 2 && rModelPart.GetCommunicator().MyPID() == 0)) << "Finished setting up the dofs" << std::endl;
                QUEST_INFO_IF("ExplicitBuilder", ( this->GetEchoLevel() > 2)) << "End of setup dof set\n" << std::endl;

                QUEST_CATCH("");
            }


            /**
             * @brief 设置DOF集合方程ID对象
             * 设置DOF集合的方程ID
             */
            virtual void SetUpDofSetEquationIds()
            {
                QUEST_ERROR_IF_NOT(mDofSetIsInitialized) << "Trying to set the equation ids. before initializing the DOF set. Please call the SetUpDofSet() before." << std::endl;
                QUEST_ERROR_IF(mEquationSystemSize == 0) << "Trying to set the equation ids. in an empty DOF set (equation system size is 0)." << std::endl;

                IndexPartition<int>(mEquationSystemSize).for_each(
                    [&](int i_dof){
                        auto it_dof = mDofSet.begin() + i_dof;
                        it_dof->SetEquationId(i_dof);
                    }
                );
            }


            /**
             * @brief 设置累积质量向量对象
             * 此方法设置在显式更新中使用的累积质量向量。
             * 请注意，它要求方程ID已经设置，并且质量贡献的实现
             * 必须在单元级别通过CalculateLumpedMassVector方法完成。
             * @param rModelPart 要计算的模型部件
             */
            virtual void SetUpLumpedMassVector(const ModelPart &rModelPart)
            {
                QUEST_TRY;

                QUEST_INFO_IF("ExplicitBuilder", this->GetEchoLevel() > 1) << "Setting up the lumped mass matrix vector" << std::endl;

                mpLumpedMassVector = TSystemVectorPointerType(new TSystemVectorType(GetDofSet().size()));
                TDenseSpace::SetToZero(*mpLumpedMassVector);

                LocalSystemVectorType elem_mass_vector;
                std::vector<std::size_t> elem_equation_id;
                const auto &r_elements_array = rModelPart.Elements();
                const auto &r_process_info = rModelPart.GetProcessInfo();
                const int n_elems = static_cast<int>(r_elements_array.size());

                #pragma omp for private(elem_mass_vector) schedule(guided, 512) nowait
                for (int i_elem = 0; i_elem < n_elems; ++i_elem) {
                    const auto it_elem = r_elements_array.begin() + i_elem;

                    it_elem->CalculateLumpedMassVector(elem_mass_vector, r_process_info);
                    it_elem->EquationIdVector(elem_equation_id, r_process_info);

                    for (IndexType i = 0; i < elem_equation_id.size(); ++i) {
                        AtomicAdd((*mpLumpedMassVector)[elem_equation_id[i]], elem_mass_vector(i));
                    }
                }

                mLumpedMassVectorIsInitialized = true;

                QUEST_CATCH("");
            }


            /**
             * @brief 初始化 DOF 集的反力
             * 对于已初始化的 DOF 集（mDofSet），此方法将相应的反应变量值设置为零。
             * 请注意，在显式构建中，反应作为残差容器使用。
             */
            virtual void InitializeDofSetReactions()
            {
                QUEST_ERROR_IF_NOT(mDofSetIsInitialized) << "Trying to initialize the explicit residual but the DOFs set is not initialized yet." << std::endl;
                QUEST_ERROR_IF(mEquationSystemSize == 0) << "Trying to set the equation ids. in an empty DOF set (equation system size is 0)." << std::endl;

                block_for_each(
                    mDofSet,
                    [](DofType& rDof){
                        rDof.GetSolutionStepReactionValue() = 0.0;
                    }
                );
            }


            /**
             * @brief 计算系统的反应
             * @param rModelPart 要计算的模型部分
             */
            virtual void CalculateReactions()
            {
                if (mCalculateReactionsFlag) {
                    QUEST_ERROR_IF_NOT(mDofSetIsInitialized) << "Trying to initialize the explicit residual but the DOFs set is not initialized yet." << std::endl;
                    QUEST_ERROR_IF(mEquationSystemSize == 0) << "Trying to set the equation ids. in an empty DOF set (equation system size is 0)." << std::endl;

                    block_for_each(
                        mDofSet,
                        [](DofType& rDof){
                            auto& r_reaction_value = rDof.GetSolutionStepReactionValue();
                            r_reaction_value *= -1.0;
                        }
                    );
                }
            }


            /**
             * @brief 此方法验证并分配默认参数
             * @param rParameters 要验证的参数
             * @param DefaultParameters 默认参数
             * @return 返回验证后的参数
             */
            virtual Parameters ValidateAndAssignParameters(
                Parameters ThisParameters,
                const Parameters DefaultParameters
            ) const{
                ThisParameters.ValidateAndAssignDefaults(DefaultParameters);
                return ThisParameters;
            }


            /**
             * @brief 此方法将设置分配给成员变量
             * @param ThisParameters 分配给成员变量的参数
             */
            virtual void AssignSettings(const Parameters ThisParameters){}


        protected:
            /**
             * @brief 系统自由度集合
             */
            DofsArrayType mDofSet; 

            /**
             * @brief 指向与DOF集合相关的集中质量向量
             */
            TSystemVectorPointerType mpLumpedMassVector; 

            /**
             * @brief 是否在每个时间步中需要重新设置DOF集合
             */
            bool mResetDofSetFlag = false;  

            /**
             * @brief 是否需要在每个时间步中重新设置集中质量向量
             */
            bool mResetLumpedMassVectorFlag = false;  

            /**
             * @brief 自由度集合是否已经初始化
             */
            bool mDofSetIsInitialized = false; 

            /**
             * @brief 集中质量向量是否已经初始化
             */
            bool mLumpedMassVectorIsInitialized = false; 

            /**
             * @brief 是否需要计算反力
             */
            bool mCalculateReactionsFlag = false; 

            /**
             * @brief 求解的方程系统中自由度的数量
             */
            unsigned int mEquationSystemSize; 

            /**
             * @brief 控制输出信息的详细级别
             */
            int mEchoLevel = 0;

        private:
            /**
             * @brief 存储注册的圆形对象的数组
             */
            static std::vector<Internals::RegisteredPrototypeBase<ClassType>> msPrototypes;

    };

}

#endif //QUEST_EXPLICIT_BUILDER_HPP