#ifndef QUEST_BUILDER_AND_SOLVERS_HPP
#define QUEST_BUILDER_AND_SOLVERS_HPP

// 系统头文件
#include <set>

// 项目头文件
#include "includes/define.hpp"
#include "includes/model_part.hpp"
#include "solving_strategies/schemes/schemes.hpp"
#include "includes/quest_parameters.hpp"

namespace Quest{

    /**
     * @brief 提供了基础的构建和求解操作的实现
     * @brief 右侧向量（RHS）由不平衡载荷（残差）构成
     * 自由度通过将约束自由度放置在系统的末尾并按反向顺序排序来重新排列，与DofSet的顺序相反
     */
    template<typename TSparseSpace, typename TDenseSpace, typename TLinearSolver>
    class BuilderAndSolver{
        public:
            QUEST_CLASS_POINTER_DEFINITION(BuilderAndSolver);

            using ClassType = BuilderAndSolver<TSparseSpace, TDenseSpace, TLinearSolver>;
            using SizeType = std::size_t;
            using IndexType = std::size_t;
            using TDataType = typename TSparseSpace::DataType;
            using TSystemMatrixType = typename TSparseSpace::MatrixType;
            using TSystemVectorType = typename TSparseSpace::VectorType;
            using TSystemMatrixPointerType = typename TSparseSpace::MatrixPointerType;
            using TSystemVectorPointerType = typename TSparseSpace::VectorPointerType;
            using LocalSystemMatrixType = typename TDenseSpace::MatrixType;
            using LocalSystemVectorType = typename TDenseSpace::VectorType;
            using TSchemeType = Scheme<TSparseSpace, TDenseSpace>;
            using TDofType = ModelPart::DofType;
            using DofsArrayType = ModelPart::DofsArrayType;
            using NodesArrayType = ModelPart::NodesContainerType;
            using ElementsArrayType =  ModelPart::ElementsContainerType;
            using ConditionsArrayType = ModelPart::ConditionsContainerType;
            using ElementsContainerType = PointerVectorSet<Element, IndexedObject>;

        public:
            /**
             * @brief 默认构造函数
             */
            explicit BuilderAndSolver(){}


            /**
             * @brief 构造函数，基于输入参数
             */
            explicit BuilderAndSolver(
                typename TLinearSolver::Pointer pNewLinearSystemSolver,
                Parameters ThisParameters
            ){
                ThisParameters = this->ValidateAndAssignParameters(ThisParameters, this->GetDefaultParameters());
                this->AssignSettings(ThisParameters);

                mpLinearSystemSolver = pNewLinearSystemSolver;
            }


            /**
             * @brief 构造函数
             */
            explicit BuilderAndSolver(typename TLinearSolver::Pointer pNewLinearSystemSolver)
            {
                mpLinearSystemSolver = pNewLinearSystemSolver;
            }


            /**
             * @brief 析构函数
             */
            virtual ~BuilderAndSolver(){}


            /**
             * @brief 创建并返回一个新的对象指针
             */
            virtual ClassType::Pointer Create(
                typename TLinearSolver::Pointer pNewLinearSystemSolver,
                Parameters ThisParameters
            ) const{
                return Quest::make_shared<ClassType>(pNewLinearSystemSolver,ThisParameters);
            }


            /**
             * @brief 返回是否计算反力的标志
             */
            bool GetCalculateReactionsFlag() const
            {
                return mCalculateReactionsFlag;
            }


            /**
             * @brief 设置是否计算反力的标志
             */
            void SetCalculateReactionsFlag(bool flag)
            {
                mCalculateReactionsFlag = flag;
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
             * @brief 返回LHS矩阵是否需要重构的标志
             */
            bool GetReshapeMatrixFlag() const
            {
                return mReshapeMatrixFlag;
            }


            /**
             * @brief 设置LHS矩阵是否需要重构的标志
             */
            void SetReshapeMatrixFlag(bool ReshapeMatrixFlag)
            {
                mReshapeMatrixFlag = ReshapeMatrixFlag;
            }


            /**
             * @brief 返回线性方程组大小
             */
            unsigned int GetEquationSystemSize() const
            {
                return mEquationSystemSize;
            }


            /**
             * @brief 返回线性求解器对象指针
             */
            typename TLinearSolver::Pointer GetLinearSystemSolver() const
            {
                return mpLinearSystemSolver;
            }


            /**
             * @brief 设置线性求解器对象指针
             */
            void SetLinearSystemSolver(typename TLinearSolver::Pointer pLinearSystemSolver)
            {
                mpLinearSystemSolver = pLinearSystemSolver;
            }


            /**
             * @brief 用于构建 LHS（左端矩阵）的函数。根据所选择的实现方式，矩阵的大小可以等于自由度总数或未约束自由度的数量。
             * @param pScheme 指向积分方案的指针
             * @param rModelPart 要计算的模型部分
             * @param rA 系统方程的 LHS 矩阵
             */
            virtual void BuildLHS(
                typename TSchemeType::Pointer pScheme,
                ModelPart& rModelPart,
                TSystemMatrixType& rA
            ){}


            /**
             * @brief 用于构建 RHS（右端向量）的函数。向量的大小可以是自由度总数，也可以是未约束自由度的数量。
             * @param pScheme 指向积分方案的指针
             * @param rModelPart 要计算的模型部分
             * @param rb 系统方程的 RHS 向量
             */
            virtual void BuildRHS(
                typename TSchemeType::Pointer pScheme,
                ModelPart& rModelPart,
                TSystemVectorType& rb
            ){}


            /**
             * @brief 功能等同于（但通常更快）分别执行 BuildLHS 和 BuildRHS。
             * @param pScheme 指向积分方案的指针
             * @param rModelPart 要计算的模型部分
             * @param rA 系统方程的 LHS 矩阵
             * @param rb 系统方程的 RHS 向量
             */
            virtual void Build(
                typename TSchemeType::Pointer pScheme,
                ModelPart& rModelPart,
                TSystemMatrixType& rA,
                TSystemVectorType& rb
            ){}


            /**
             * @brief 构建一个大小为 n×N 的矩形矩阵，其中 "n" 是未约束自由度的数量，"N" 是涉及的总自由度数量。
             * @details 该矩阵通过构建完整矩阵并移除对应于固定自由度的行（但保留列）获得。
             * @param pScheme 指向积分方案的指针
             * @param rModelPart 要计算的模型部分
             * @param rA 系统方程的 LHS 矩阵
             */
            virtual void BuildLHS_CompleteOnFreeRows(
                typename TSchemeType::Pointer pScheme,
                ModelPart& rModelPart,
                TSystemMatrixType& rA
            ){}


            /**
             * @brief 构建一个大小为 N×N 的矩阵，其中 "N" 是涉及的总自由度数量。
             * @details 该矩阵通过构建完整矩阵获得，包括对应于固定自由度的行和列。
             * @param pScheme 指向积分方案的指针
             * @param rModelPart 要计算的模型部分
             * @param rA 系统方程的 LHS 矩阵
             */
            virtual void BuildLHS_Complete(
                typename TSchemeType::Pointer pScheme,
                ModelPart& rModelPart,
                TSystemMatrixType& rA
            ){}


            /**
             * @brief 调用线性系统求解器
             * @param rA 系统方程的 LHS 矩阵
             * @param rDx 未知量向量
             * @param rb 系统方程的 RHS 向量
             */
            virtual void SystemSolve(
                TSystemMatrixType& rA,
                TSystemVectorType& rDx,
                TSystemVectorType& rb
            ){}


            /**
             * @brief 同时执行构建和求解阶段的函数。
             * @details 当能够在构建完成后立即求解时，这是理想中最快且最安全的函数。
             * @param pScheme 指向积分方案的指针
             * @param rModelPart 要计算的模型部分
             * @param rA 系统方程的 LHS 矩阵
             * @param rDx 未知量向量
             * @param rb 系统方程的 RHS 向量
             */
            virtual void BuildAndSolve(
                typename TSchemeType::Pointer pScheme,
                ModelPart& rModelPart,
                TSystemMatrixType& rA,
                TSystemVectorType& rDx,
                TSystemVectorType& rb
            ){}


            /**
             * @brief 同时执行构建和求解阶段的函数，使用旧迭代的数据库进行线性化。
             * @details 当能够在构建完成后立即求解时，这是理想中最快且最安全的函数。
             * @param pScheme 指向积分方案的指针
             * @param rModelPart 要计算的模型部分
             * @param rA 系统方程的 LHS 矩阵
             * @param rDx 未知量向量
             * @param rb 系统方程的 RHS 向量
             * @param MoveMesh 指示在调用方案的更新时是否需要进行网格更新
             */
            virtual void BuildAndSolveLinearizedOnPreviousIteration(
                typename TSchemeType::Pointer pScheme,
                ModelPart& rModelPart,
                TSystemMatrixType& rA,
                TSystemVectorType& rDx,
                TSystemVectorType& rb,
                const bool MoveMesh
            ){
                QUEST_ERROR << "No special implementation available for "
                    << "BuildAndSolveLinearizedOnPreviousIteration "
                    << " please use UseOldStiffnessInFirstIterationFlag=false in the settings of the strategy "
                    << std::endl;
            }


            /**
             * @brief 与前述相同，但系统的矩阵已经构建完成，只有 RHS 被重新构建。
             * @param pScheme 指向积分方案的指针
             * @param rModelPart 要计算的模型部分
             * @param rA 系统方程的 LHS 矩阵
             * @param rDx 未知量向量
             * @param rb 系统方程的 RHS 向量
             */
            virtual void BuildRHSAndSolve(
                typename TSchemeType::Pointer pScheme,
                ModelPart& rModelPart,
                TSystemMatrixType& rA,
                TSystemVectorType& rDx,
                TSystemVectorType& rb
            ){}


            /**
             * @brief 应用dirichlet 条件。根据所选实现和系统矩阵的构建方式，这个操作可能非常复杂，也可能完全不需要消耗资源。
             * @details 有关如何针对特定实现工作的解释，用户应参考所选的特定 Builder And Solver。
             * @param pScheme 指向积分方案的指针
             * @param rModelPart 要计算的模型部分
             * @param rA 系统方程的 LHS 矩阵
             * @param rDx 未知量向量
             * @param rb 系统方程的 RHS 向量
             */
            virtual void ApplyDirichletConditions(
                typename TSchemeType::Pointer pScheme,
                ModelPart& rModelPart,
                TSystemMatrixType& rA,
                TSystemVectorType& rDx,
                TSystemVectorType& rb
            ){}


            /**
             * @brief 与前面相同，但仅影响 LHS。
             * @param pScheme 指向积分方案的指针
             * @param rModelPart 要计算的模型部分
             * @param rA 系统方程的 LHS 矩阵
             * @param rDx 未知量向量
             */
            virtual void ApplyDirichletConditions_LHS(
                typename TSchemeType::Pointer pScheme,
                ModelPart& rModelPart,
                TSystemMatrixType& rA,
                TSystemVectorType& rDx
            ){}


            /**
             * @brief 与前面相同，但仅影响 RHS。
             * @param pScheme 指向积分方案的指针
             * @param rModelPart 要计算的模型部分
             * @param rA 系统方程的 LHS 矩阵
             * @param rb 系统方程的 RHS 向量
             */
            virtual void ApplyDirichletConditions_RHS(
                typename TSchemeType::Pointer pScheme,
                ModelPart& rModelPart,
                TSystemVectorType& rDx,
                TSystemVectorType& rb
            ){}


            /**
             * @brief 应用具有主从关系矩阵的约束（仅影响 RHS）。
             * @param pScheme 考虑的积分方案
             * @param rModelPart 要求解的问题的模型部分
             * @param rb RHS 向量
             */
            virtual void ApplyRHSConstraints(
                typename TSchemeType::Pointer pScheme,
                ModelPart& rModelPart,
                TSystemVectorType& rb
            ){}


            /**
             * @brief 应用具有主从关系矩阵的约束
             * @param pScheme 考虑的积分方案
             * @param rModelPart 要求解的问题的模型部分
             * @param rA LHS 矩阵
             * @param rb RHS 向量
             */
            virtual void ApplyConstraints(
                typename TSchemeType::Pointer pScheme,
                ModelPart& rModelPart,
                TSystemMatrixType& rA,
                TSystemVectorType& rb
            ){}


            /**
             * @brief 通过“询问”每个元素和条件的 Dof 来构建问题中涉及的 DofSets 列表。
             * @details Dof 列表存储在 BuilderAndSolver 中，因为它与矩阵和 RHS 的构建方式密切相关。
             * @param pScheme 积分方案的指针
             * @param rModelPart 要计算的模型部分
             */
            virtual void SetUpDofSet(
                typename TSchemeType::Pointer pScheme,
                ModelPart& rModelPart
            ){}


            /**
             * @brief 从单元中获取自由度列表
             */
            virtual DofsArrayType& GetDofSet()
            {
                return mDofSet;
            }


            virtual const DofsArrayType& GetDofSet() const
            {
                return mDofSet;
            }


            /**
             * @brief 它组织 DofSet 以加快构建阶段。
             * @param rModelPart 要计算的模型部分
             */
            virtual void SetUpSystem(ModelPart& rModelPart){}


            /**
             * @brief 此方法初始化并调整方程系统的大小
             * @param pScheme 指向积分方案的指针
             * @param rModelPart 要计算的模型部分
             * @param pA 方程系统的 LHS 矩阵的指针
             * @param pDx 未知数的向量的指针
             * @param pb 方程系统的 RHS 向量的指针
             */
            virtual void ResizeAndInitializeVectors(
                typename TSchemeType::Pointer pScheme,
                TSystemMatrixPointerType& pA,
                TSystemVectorPointerType& pDx,
                TSystemVectorPointerType& pb,
                ModelPart& rModelPart
            ){}


            /**
             * @brief 在解算步骤开始时对方程系统应用某些操作
             * @param rModelPart 要计算的模型部分
             * @param rA 方程系统的 LHS 矩阵
             * @param rDx 未知数的向量
             * @param rb 方程系统的 RHS 向量
             */
            virtual void InitializeSolutionStep(
                ModelPart& rModelPart,
                TSystemMatrixType& rA,
                TSystemVectorType& rDx,
                TSystemVectorType& rb
            ){}


            /**
             * @brief 在解算步骤结束时对方程系统应用某些操作
             * @param rModelPart 要计算的模型部分
             * @param rA 方程系统的 LHS 矩阵
             * @param rDx 未知数的向量
             * @param rb 方程系统的 RHS 向量
             */
            virtual void FinalizeSolutionStep(
                ModelPart& rModelPart,
                TSystemMatrixType& rA,
                TSystemVectorType& rDx,
                TSystemVectorType& rb
            ){}


            /**
             * @brief 计算系统的反力
             * @param pScheme 指向积分方案的指针
             * @param rModelPart 要计算的模型部分
             * @param rA 方程系统的 LHS 矩阵
             * @param rDx 未知数的向量
             * @param rb 方程系统的 RHS 向量
             */
            virtual void CalculateReactions(
                typename TSchemeType::Pointer pScheme,
                ModelPart& rModelPart,
                TSystemMatrixType& rA,
                TSystemVectorType& rDx,
                TSystemVectorType& rb
            ){}


            /**
             * @brief 求解步结束后用于清除内存
             */
            virtual void Clear()
            {
                this->mDofSet = DofsArrayType();
                this->mpReactionsVector.reset();
                if (this->mpLinearSystemSolver != nullptr) this->mpLinearSystemSolver->Clear();

                QUEST_INFO_IF("BuilderAndSolver", this->GetEchoLevel() > 0) << "Clear Function called" << std::endl;
            }


            /**
             * @brief 此函数设计为一次性执行所有必要的输入检查
             * @details 检查可能是“昂贵的”，因为该函数旨在捕获用户的错误。
             * @param rModelPart 要计算的模型部分
             * @return 0 表示所有检查通过
             */
            virtual int Check(ModelPart& rModelPart)
            {
                QUEST_TRY

                return 0;
                QUEST_CATCH("");
            }


            /**
             * @brief 此方法提供默认参数，以避免不同构造函数之间的冲突
             * @return 默认参数
             */
            virtual Parameters GetDefaultParameters() const
            {
                const Parameters default_parameters = Parameters(R"(
                {
                    "name"       : "builder_and_solver",
                    "echo_level" : 1
                })" );
                return default_parameters;
            }


            /**
             * @brief 返回类在参数中的名称
             */
            static std::string Name()
            {
                return "builder_and_solver";
            }


            /**
             * @brief 设置echo级别
             */
            void SetEchoLevel(int Level)
            {
                mEchoLevel = Level;
            }


            /**
             * @brief 返回echo级别
             */
            int GetEchoLevel() const
            {
                return mEchoLevel;
            }


            /**
             * @brief 此方法返回约束关系（T）矩阵
             * @return 约束关系（T）矩阵
             */
            virtual typename TSparseSpace::MatrixType& GetConstraintRelationMatrix()
            {
                QUEST_ERROR << "GetConstraintRelationMatrix is not implemented in base BuilderAndSolver" << std::endl;
            }


            /**
             * @brief 返回约束常量向量
             */
            virtual typename TSparseSpace::VectorType& GetConstraintConstantVector()
            {
                QUEST_ERROR << "GetConstraintConstantVector is not implemented in base BuilderAndSolver" << std::endl;
            }


            virtual std::string Info() const
            {
                return "BuilderAndSolver";
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
             * @brief 该方法用于验证并分配默认参数
             * @param rParameters 待验证的参数
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
             * @brief 该方法将设置分配给成员变量
             * @param ThisParameters 分配给成员变量的参数
             */
            virtual void AssignSettings(const Parameters ThisParameters){
                mEchoLevel = ThisParameters["echo_level"].GetInt();
            }


        protected:
            /**
             * @brief 指向线性求解器的指针
             */
            typename TLinearSolver::Pointer mpLinearSystemSolver = nullptr; 

            /**
             * @brief 包含系统自由度（DoF）的集合
             */
            DofsArrayType mDofSet; 

            /**
             * @brief 用于标记矩阵是否在每一步求解中重新调整形状
             */
            bool mReshapeMatrixFlag = false; 

            /**
             * @brief 用于标记自由度集合是否已经初始化
             */
            bool mDofSetIsInitialized = false;

            /**
             * @brief 标记是否需要计算反力
             */
            bool mCalculateReactionsFlag = false; 

            /**
             * @brief 问题中自由度的数量，也就是方程系统的大小
             */
            unsigned int mEquationSystemSize; 

            /**
             * @brief 控制输出的详细级别
             */
            int mEchoLevel = 0;

            /**
             * @brief 指向反力向量的指针
             */
            TSystemVectorPointerType mpReactionsVector;

        private:

    };

}

#endif //QUEST_BUILDER_AND_SOLVERS_HPP