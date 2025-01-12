#ifndef QUEST_SCHEMES_HPP
#define QUEST_SCHEMES_HPP

// 项目头文件
#include "includes/model_part.hpp"
#include "utilities/openmp_utils.hpp"
#include "includes/quest_parameters.hpp"
#include "utilities/entities_utilities.hpp"
#include "utilities/parallel_utilities.hpp"

namespace Quest{

    /**
     * @brief 作为求解策略的基类，提供了求解策略所需的基本接口
     */
    template<typename TSparseSpace, typename TDenseSpace>
    class Scheme{
        public:
            QUEST_CLASS_POINTER_DEFINITION(Scheme);

            using ClassType = Scheme<TSparseSpace, TDenseSpace>;
            using TDataType = typename TSparseSpace::DataType;
            using TSystemMatrixType = typename TSparseSpace::MatrixType;
            using TSystemVectorType = typename TSparseSpace::VectorType;
            using LocalSystemMatrixType = typename TDenseSpace::MatrixType;
            using LocalSystemVectorType = typename TDenseSpace::VectorType;
            using TDofType = Dof<double>;
            using DofsArrayType = ModelPart::DofsArrayType;
            using ElementsArrayType = ModelPart::ElementsContainerType;
            using ConditionsArrayType = ModelPart::ConditionsContainerType;

        public:
            /**
             * @brief 构造函数
             */
            explicit Scheme()
            {
                mSchemeIsInitialized = false;
                mElementsAreInitialized = false;
                mConditionsAreInitialized = false;
            }


            /**
             * @brief 构造函数，基于给定的参数构造
             */
            explicit Scheme(Parameters ThisParameters)
            {
                ThisParameters = this->ValidateAndAssignParameters(ThisParameters, this->GetDefaultParameters());
                this->AssignSettings(ThisParameters);

                mSchemeIsInitialized = false;
                mElementsAreInitialized = false;
                mConditionsAreInitialized = false;
            }


            /**
             * @brief 复制构造函数
             */
            explicit Scheme(Scheme& rOther):
                mSchemeIsInitialized(rOther.mSchemeIsInitialized),
                mElementsAreInitialized(rOther.mElementsAreInitialized),
                mConditionsAreInitialized(rOther.mConditionsAreInitialized)
            {}


            /**
             * @brief 析构函数
             */
            virtual ~Scheme(){}


            /**
             * @brief 创建并返回一个求解方案指针
             */
            virtual typename ClassType::Pointer Create(Parameters ThisParameters) const
            {
                return Quest::make_shared<ClassType>(ThisParameters);
            }


            /**
             * @brief 克隆方法
             */
            virtual Pointer Clone()
            {
                return Quest::make_shared<Scheme>(*this) ;
            }


            /**
             * @brief 用于初始化方案（Scheme）
             */
            virtual void Initialize(ModelPart& rModelPart)
            {
                QUEST_TRY
                mSchemeIsInitialized = true;
                QUEST_CATCH("")
            }


            /**
             * @brief 判断方案（Scheme）是否已初始化
             */
            bool SchemeIsInitialized()
            {
                return mSchemeIsInitialized;
            }


            /**
             * @brief 用于设置方案（Scheme）是否初始化变量的状态
             */
            void SetSchemeIsInitialized(bool SchemeIsInitializedFlag = true)
            {
                mSchemeIsInitialized = SchemeIsInitializedFlag;
            }


            /**
             * @brief 判断单元（Elements）是否已初始化
             */
            bool ElementsAreInitialized()
            {
                return mElementsAreInitialized;
            }


            /**
             * @brief 用于设置单元（Elements）是否初始化的状态
             */
            void SetElementsAreInitialized(bool ElementsAreInitializedFlag = true)
            {
                mElementsAreInitialized = ElementsAreInitializedFlag;
            }


            /**
             * @brief 判断条件（Conditions）是否已初始化
             */
            bool ConditionsAreInitialized()
            {
                return mConditionsAreInitialized;
            }


            /**
             * @brief 用于设置条件（Conditions）是否初始化的状态
             */
            void SetConditionsAreInitialized(bool ConditionsAreInitializedFlag = true)
            {
                mConditionsAreInitialized = ConditionsAreInitializedFlag;
            }


            /**
             * @brief 初始化单元（Elements）的方法
             */
            virtual void InitializeElements( ModelPart& rModelPart)
            {
                QUEST_TRY

                EntitiesUtilities::InitializeEntities<Element>(rModelPart);

                SetElementsAreInitialized();

                QUEST_CATCH("")
            }


            /**
             * @brief 初始化条件（Conditions）的方法
             */
            virtual void InitializeConditions(ModelPart& rModelPart)
            {
                QUEST_TRY

                QUEST_ERROR_IF_NOT(mElementsAreInitialized) << "Before initializing Conditions, initialize Elements FIRST" << std::endl;

                EntitiesUtilities::InitializeEntities<Condition>(rModelPart);

                SetConditionsAreInitialized();

                QUEST_CATCH("")
            }


            /**
             * @brief 在每个求解步开始时调用的函数。
             * @details 在此函数中应执行的基本操作包括：
             * - 管理在时间步内保持恒定的变量（例如，依赖于当前时间步的时间方案常数）
             * @param rModelPart 问题求解的模型部件
             * @param A 左手边矩阵（LHS矩阵）
             * @param Dx 主变量的增量更新
             * @param b 右手边向量（RHS向量）
             */
            virtual void InitializeSolutionStep(
                ModelPart& rModelPart,
                TSystemMatrixType& A,
                TSystemVectorType& Dx,
                TSystemVectorType& b
            ){
                QUEST_TRY

                EntitiesUtilities::InitializeSolutionStepAllEntities(rModelPart);

                QUEST_CATCH("")
            }


            /**
             * @brief 在求解步结束时调用的函数（若为迭代过程，则在收敛后调用）。
             * @param rModelPart 问题求解的模型部件
             * @param A 左手边矩阵（LHS矩阵）
             * @param Dx 主变量的增量更新
             * @param b 右手边向量（RHS向量）
             */
            virtual void FinalizeSolutionStep(
                ModelPart& rModelPart,
                TSystemMatrixType& A,
                TSystemVectorType& Dx,
                TSystemVectorType& b
            ){
                QUEST_TRY

                EntitiesUtilities::FinalizeSolutionStepAllEntities(rModelPart);

                QUEST_CATCH("")
            }


            /**
             * @brief 每一次迭代时调用的函数，设计为在每次非线性迭代开始时调用。
             * @note 注意：不会在此处调用具有相同名称的单元函数。
             * @warning 必须在派生类中定义。
             * @details 该函数在生成器中调用，以提高内存效率。
             * @param rModelPart 问题求解的模型部件
             * @param A 左手边矩阵（LHS矩阵）
             * @param Dx 主变量的增量更新
             * @param b 右手边向量（RHS向量）
             */
            virtual void InitializeNonLinIteration(
                ModelPart& rModelPart,
                TSystemMatrixType& A,
                TSystemVectorType& Dx,
                TSystemVectorType& b
            ){
                QUEST_TRY

                EntitiesUtilities::InitializeNonLinearIterationAllEntities(rModelPart);

                QUEST_CATCH("")
            }


            /**
             * @brief 在需要结束一次迭代时调用的函数。设计为在每次非线性迭代结束时调用。
             * @param rModelPart 问题求解的模型部件
             * @param A 左手边矩阵（LHS矩阵）
             * @param Dx 主变量的增量更新
             * @param b 右手边向量（RHS向量）
             */
            virtual void FinalizeNonLinIteration(
                ModelPart& rModelPart,
                TSystemMatrixType& A,
                TSystemVectorType& Dx,
                TSystemVectorType& b
            ){
                QUEST_TRY

                EntitiesUtilities::FinalizeNonLinearIterationAllEntities(rModelPart);

                QUEST_CATCH("")
            }


            /**
             * @brief 预测函数，用于预测下一时间步的解
             * @warning 必须在派生类中定义
             * @param rModelPart 问题求解的模型部件
             * @param A 左手边矩阵（LHS矩阵）
             * @param Dx 主变量的增量更新
             * @param b 右手边向量（RHS向量）
             */
            virtual void Predict(
                ModelPart& rModelPart,
                DofsArrayType& rDofSet,
                TSystemMatrixType& A,
                TSystemVectorType& Dx,
                TSystemVectorType& b
            ){
                QUEST_TRY
                QUEST_CATCH("")
            }


            /**
             * @brief 更新解
             * @warning 必须在派生类中定义
             * @param rModelPart 问题求解的模型部件
             * @param rDofSet 所有主变量的集合
             * @param A 左手边矩阵（LHS矩阵）
             * @param Dx 主变量的增量更新
             * @param b 右手边向量（RHS向量）
             */
            virtual void Update(
                ModelPart& rModelPart,
                DofsArrayType& rDofSet,
                TSystemMatrixType& A,
                TSystemVectorType& Dx,
                TSystemVectorType& b
            ){
                QUEST_TRY
                QUEST_CATCH("")
            }


            /**
             * @brief 准备输出结果所需数据的函数
             * @warning 必须在派生类中定义
             * @param rModelPart 问题求解的模型部件
             * @param rDofSet 所有主变量的集合
             * @param A 左手边矩阵（LHS矩阵）
             * @param Dx 主变量的增量更新
             * @param b 右手边向量（RHS向量）
             */
            virtual void CalculateOutputData(
                ModelPart& rModelPart,
                DofsArrayType& rDofSet,
                TSystemMatrixType& A,
                TSystemVectorType& Dx,
                TSystemVectorType& b
            ){
                QUEST_TRY
                QUEST_CATCH("")
            }


            /**
             * @brief 清除输出结果
             */
            virtual void CleanOutputData()
            {
                QUEST_TRY
                QUEST_CATCH("")
            }


            /**
             * @brief 该函数旨在求解步骤结束时调用，以清理在求解步骤结束后不再需要的内存存储。
             * @warning 必须在派生类中实现
             */
            virtual void Clean()
            {
                QUEST_TRY
                QUEST_CATCH("")
            }


            /**
             * @brief 释放内部存储。
             * @warning 必须在派生类中实现
             */
            virtual void Clear()
            {
                QUEST_TRY
                QUEST_CATCH("")
            }


            /**
             * @brief 这个函数设计为在一次调用中执行所有必要的检查
             * 对提供的输入进行检查。由于该函数旨在捕捉用户错误，因此检查可能会比较“昂贵”。
             * @details 由于该函数旨在捕捉用户错误，检查可能会比较“昂贵”。
             * @param rModelPart 要求解的模型部分
             * @return 0 表示一切正常，1 表示存在问题
             */
            virtual int Check(const ModelPart& rModelPart) const
            {
                QUEST_TRY
                return 0;
                QUEST_CATCH("");
            }

            virtual int Check(ModelPart& rModelPart)
            {
                const Scheme& r_const_this = *this;
                const ModelPart& r_const_model_part = rModelPart;
                return r_const_this.Check(r_const_model_part);
            }


            /**
             * @brief 这个函数旨在构建器和求解器中调用，用于引入选择的时间积分方案。
             * @details 它“请求”单元所需的矩阵，并执行引入所选时间积分方案所需的操作。该函数同时计算对系统的LHS和RHS的贡献。
             * @param rElement 要计算的单元
             * @param LHS_Contribution LHS矩阵的贡献
             * @param RHS_Contribution RHS向量的贡献
             * @param rEquationIdVector 单元自由度的ID
             * @param rCurrentProcessInfo 当前进程信息实例
             */
            virtual void CalculateSystemContributions(
                Element& rElement,
                LocalSystemMatrixType& LHS_Contribution,
                LocalSystemVectorType& RHS_Contribution,
                Element::EquationIdVectorType& rEquationIdVector,
                const ProcessInfo& rCurrentProcessInfo
            ){
                rElement.CalculateLocalSystem(LHS_Contribution, RHS_Contribution, rCurrentProcessInfo);
            }


            /**
             * @brief 这个函数旨在构建器和求解器中调用，用于引入选择的时间积分方案。
             * @details 它“请求”条件所需的矩阵，并执行引入所选时间积分方案所需的操作。该函数同时计算对系统的LHS和RHS的贡献。
             * @param rCondition 要计算的条件
             * @param LHS_Contribution LHS矩阵的贡献
             * @param RHS_Contribution RHS向量的贡献
             * @param rEquationIdVector 单元自由度的ID
             * @param rCurrentProcessInfo 当前进程信息实例
             */
            virtual void CalculateSystemContributions(
                Condition& rCondition,
                LocalSystemMatrixType& LHS_Contribution,
                LocalSystemVectorType& RHS_Contribution,
                Element::EquationIdVectorType& rEquationIdVector,
                const ProcessInfo& rCurrentProcessInfo
            ){
                rCondition.CalculateLocalSystem(LHS_Contribution, RHS_Contribution, rCurrentProcessInfo);
            }


            /**
             * @brief 此函数旨在计算RHS贡献
             * @param rElement 要计算的单元
             * @param RHS_Contribution RHS向量的贡献
             * @param rEquationIdVector 单元自由度的ID
             * @param rCurrentProcessInfo 当前进程信息实例
             */
            virtual void CalculateRHSContribution(
                Element& rElement,
                LocalSystemVectorType& RHS_Contribution,
                Element::EquationIdVectorType& rEquationIdVector,
                const ProcessInfo& rCurrentProcessInfo
            ){
                rElement.CalculateRightHandSide(RHS_Contribution, rCurrentProcessInfo);
            }


            /**
             * @brief 此函数旨在计算RHS贡献
             * @param rCondition 要计算的条件
             * @param RHS_Contribution RHS向量的贡献
             * @param rEquationIdVector 条件自由度的ID
             * @param rCurrentProcessInfo 当前进程信息实例
             */
            virtual void CalculateRHSContribution(
                Condition& rCondition,
                LocalSystemVectorType& RHS_Contribution,
                Element::EquationIdVectorType& rEquationIdVector,
                const ProcessInfo& rCurrentProcessInfo
            ){
                rCondition.CalculateRightHandSide(RHS_Contribution, rCurrentProcessInfo);
            }


            /**
             * @brief 此函数旨在计算LHS贡献
             * @param rElement 要计算的单元
             * @param RHS_Contribution LHS向量的贡献
             * @param rEquationIdVector 单元自由度的ID
             * @param rCurrentProcessInfo 当前进程信息实例
             */
            virtual void CalculateLHSContribution(
                Element& rElement,
                LocalSystemMatrixType& LHS_Contribution,
                Element::EquationIdVectorType& rEquationIdVector,
                const ProcessInfo& rCurrentProcessInfo
            ){
                rElement.CalculateLeftHandSide(LHS_Contribution, rCurrentProcessInfo);
            }


            /**
             * @brief 此函数旨在计算LHS贡献
             * @param rCondition 要计算的条件
             * @param RHS_Contribution LHS向量的贡献
             * @param rEquationIdVector 条件自由度的ID
             * @param rCurrentProcessInfo 当前进程信息实例
             */
            virtual void CalculateLHSContribution(
                Condition& rCondition,
                LocalSystemMatrixType& LHS_Contribution,
                Element::EquationIdVectorType& rEquationIdVector,
                const ProcessInfo& rCurrentProcessInfo
            ){
                rCondition.CalculateLeftHandSide(LHS_Contribution, rCurrentProcessInfo);
            }


            /**
             * @brief 该方法获取与当前单元对应的方程ID
             * @param rElement 要计算的单元
             * @param rEquationId 单元自由度的ID
             * @param rCurrentProcessInfo 当前进程信息实例
             */
            virtual void EquationId(
                const Element& rElement,
                Element::EquationIdVectorType& rEquationId,
                const ProcessInfo& rCurrentProcessInfo
            ){
                rElement.EquationIdVector(rEquationId, rCurrentProcessInfo);
            }


            /**
             * @brief 该方法获取与当前条件对应的方程ID
             * @param rCondition 要计算的条件
             * @param rEquationId 条件自由度的ID
             * @param rCurrentProcessInfo 当前进程信息实例
             */
            virtual void EquationId(
                const Condition& rCondition,
                Element::EquationIdVectorType& rEquationId,
                const ProcessInfo& rCurrentProcessInfo
            ){
                rCondition.EquationIdVector(rEquationId, rCurrentProcessInfo);
            }


            /**
             * @brief 返回要在系统中组装的自由度列表，用于给定单元
             * @param pCurrentElement 要计算的单元
             * @param rDofList 包含单元自由度的列表
             * @param rCurrentProcessInfo 当前进程信息实例
             */
            virtual void GetDofList(
                const Element& rElement,
                Element::DofsVectorType& rDofList,
                const ProcessInfo& rCurrentProcessInfo
            ){
                rElement.GetDofList(rDofList, rCurrentProcessInfo);
            }


            /**
             * @brief 返回要在系统中组装的自由度列表，用于给定条件
             * @param rCondition 要计算的条件
             * @param rDofList 包含条件自由度的列表
             * @param rCurrentProcessInfo 当前进程信息实例
             */
            virtual void GetDofList(
                const Condition& rCondition,
                Element::DofsVectorType& rDofList,
                const ProcessInfo& rCurrentProcessInfo
            ){
                rCondition.GetDofList(rDofList, rCurrentProcessInfo);
            }


            /**
             * @brief 获取默认参数
             */
            virtual Parameters GetDefaultParameters() const{
                const Parameters default_parameters = Parameters(R"(
                {
                    "name" : "scheme"
                })" );
                return default_parameters;
            }


            /**
             * @brief 返回在设置中使用的类的名称
             */
            static std::string Name()
            {
                return "scheme";
            }


            virtual std::string Info() const{
                return "Scheme";
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

        private:
            /**
             * @brief 用于指示方案（Scheme）是否已初始化的标志 
             */
            bool mSchemeIsInitialized; 

            /**
             * @brief 用于指示单元（Elements）是否已初始化的标志
             */
            bool mElementsAreInitialized;

            /**
             * @brief 用于指示条件（Conditions）是否已初始化的标志
             */
            bool mConditionsAreInitialized; 


            /**
             * @brief 此方法用于验证并分配默认参数
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


        private:

    };

}


#endif //QUEST_SCHEMES_HPP