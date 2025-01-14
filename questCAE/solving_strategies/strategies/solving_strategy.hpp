#ifndef QUEST_SOLVING_STRATEGY_HPP
#define QUEST_SOLVING_STRATEGY_HPP

// 项目头文件
#include "includes/define.hpp"
#include "includes/model_part.hpp"
#include "solving_strategies/schemes/schemes.hpp"
#include "solving_strategies/builder_and_solvers/builder_and_solvers.hpp"
#include "includes/quest_parameters.hpp"
#include "utilities/parallel_utilities.hpp"
#include "utilities/variable_utils.hpp"

namespace Quest{

    /**
     * @brief 求解策略的基类
     */
    template<typename TSparseSpace, typename TDenseSpace>
    class SolvingStrategy{
        public:
            using TDataType = typename TSparseSpace::DataType;
            using TSystemMatrixType = typename TSparseSpace::MatrixType;
            using TSystemVectorType = typename TSparseSpace::VectorType;
            using TSystemMatrixPointerType = typename TSparseSpace::MatrixPointerType;
            using TSystemVectorPointerType = typename TSparseSpace::VectorPointerType;
            using LocalSystemMatrixType = typename TDenseSpace::MatrixType;
            using LocalSystemVectorType = typename TDenseSpace::VectorType;
            using ClassType = SolvingStrategy<TSparseSpace, TDenseSpace>;
            using TDofType = typename ModelPart::DofType;
            using DofsArrayType = typename ModelPart::DofsArrayType;
            using NodesArrayType = ModelPart::NodesContainerType;
            using ElementsArrayType = ModelPart::ElementsContainerType;
            using ConditionsArrayType = ModelPart::ConditionsContainerType;

            QUEST_CLASS_POINTER_DEFINITION(SolvingStrategy);

        public:
            /**
             * @brief 默认构造函数
             */
            explicit SolvingStrategy() {}


            /**
             * @brief 构造函数，基于传入参数
             */
            explicit SolvingStrategy(
                ModelPart& rModelPart,
                Parameters ThisParameters
            ): mpModelPart(&rModelPart)
            {
                ThisParameters = this->ValidateAndAssignParameters(ThisParameters, this->GetDefaultParameters());
                this->AssignSettings(ThisParameters);
            }


            /**
             * @brief 构造函数
             */
            explicit SolvingStrategy(
                ModelPart& rModelPart,
                bool MoveMeshFlag = false
            ): mpModelPart(&rModelPart)
            {
                SetMoveMeshFlag(MoveMeshFlag);
            }


            /**
             * @brief 析构函数
             */
            virtual ~SolvingStrategy(){}


            /**
             * @brief 创建并返回一个求解策略对象指针
             */
            virtual typename ClassType::Pointer Create(
                ModelPart& rModelPart,
                Parameters ThisParameters
            ) const{
                return Quest::make_shared<ClassType>(rModelPart, ThisParameters);
            }


            /**
             * @brief 预测解的操作……如果未调用此方法，则使用简单预测器，其中假设当前求解步的解值等于旧值
             */
            virtual void Predict(){}


            /**
             * @brief 成员变量的初始化和先前的操作
             */
            virtual void Initialize(){}


            /**
             * @brief 求解操作
             */
            virtual double Solve()
            {
                Initialize();
                InitializeSolutionStep();
                Predict();
                SolveSolutionStep();
                FinalizeSolutionStep();

                return 0.0;
            }


            /**
             * @brief 清除内部存储
             */
            virtual void Clear(){}


            /**
             * @brief 这应被视为“求解后”收敛性检查，适用于耦合分析
             * @details 使用的收敛准则是“求解”步骤中使用的准则
             */
            virtual bool IsConverged(){
                return true;
            }


            /**
             * @brief 当需要计算非普通结果时，应在打印结果前调用此操作
             */
            virtual void CalculateOutputData(){}


            /**
             * @brief 每一求解步前初始化操作
             */
            virtual void InitializeSolutionStep(){}


            /**
             * @brief 每一求解步后的操作
             */
            virtual void FinalizeSolutionStep(){}


            /**
             * @brief 求解当前步的解
             */
            virtual bool SolveSolutionStep()
            {
                return true;
            }


            /**
             * @brief 设置信息输出等级
             */
            virtual void SetEchoLevel(const int Level)
            {
                mEchoLevel = Level;
            }


            /**
             * @brief 获取信息输出等级
             */
            int GetEchoLevel()
            {
                return mEchoLevel;
            }


            /**
             * @brief 设置是否移动网格
             */
            void SetMoveMeshFlag(bool Flag)
            {
                mMoveMeshFlag = Flag;
            }


            /**
             * @brief 获取是否移动网格
             */
            bool GetMoveMeshFlag()
            {
                return mMoveMeshFlag;
            }


            /**
             * @brief 设置重建级别值
             * 仅用于隐式策略
             * @param Level 重建级别
             */
            virtual void SetRebuildLevel(int Level)
            {
                QUEST_ERROR << "Accessing the strategy base class \'SetRebuildLevel\'. Please implement it in your derived class." << std::endl;
            }


            /**
             * @brief 获取重建级别值
             * 仅用于隐式策略
             * @return 重建级别
             */
            virtual int GetRebuildLevel() const
            {
                QUEST_ERROR << "Accessing the strategy base class \'GetRebuildLevel\'. Please implement it in your derived class." << std::endl;
            }


            /**
             * @brief 移动网格
             */
            virtual void MoveMesh()
            {
                QUEST_TRY

                QUEST_ERROR_IF_NOT(GetModelPart().HasNodalSolutionStepVariable(DISPLACEMENT_X)) << "It is impossible to move the mesh since the DISPLACEMENT var is not in the Model Part. Either use SetMoveMeshFlag(False) or add DISPLACEMENT to the list of variables" << std::endl;

                block_for_each(GetModelPart().Nodes(), [](Node& rNode){
                    noalias(rNode.Coordinates()) = rNode.GetInitialPosition().Coordinates();
                    noalias(rNode.Coordinates()) += rNode.FastGetSolutionStepValue(DISPLACEMENT);
                });

                QUEST_INFO_IF("SolvingStrategy", this->GetEchoLevel() != 0) << " MESH MOVED " << std::endl;

                QUEST_CATCH("")
            }


            /**
             * @brief 获取模型部件
             */
            ModelPart& GetModelPart()
            {
                return *mpModelPart;
            };


            const ModelPart& GetModelPart() const
            {
                return *mpModelPart;
            };


            /**
             * @brief 获取残差范数的操作
             * @return 残差范数
             */
            virtual double GetResidualNorm()
            {
                return 0.0;
            }


            /**
             * @brief 执行检查的函数
             * @details 该函数设计为只调用一次，用于验证输入是否正确
             */
            virtual int Check()
            {
                QUEST_TRY

                if (mMoveMeshFlag) {
                    VariableUtils().CheckVariableExists<>(DISPLACEMENT, GetModelPart().Nodes());
                }

                GetModelPart().Check();

                return 0;

                QUEST_CATCH("")
            }


            /**
             * @brief 该方法提供默认参数，以避免不同构造函数之间的冲突
             * @return 默认参数
             */
            virtual Parameters GetDefaultParameters() const
            {
                const Parameters default_parameters = Parameters(R"(
                {
                    "name"                         : "solving_strategy",
                    "move_mesh_flag"               : false,
                    "echo_level"                   : 1
                })");
                return default_parameters;
            }


            /**
             * @brief 该方法返回系统的左侧矩阵（LHS）
             * @return 左侧矩阵（LHS）
             */
            virtual TSystemMatrixType& GetSystemMatrix()
            {
                QUEST_TRY

                QUEST_ERROR << "\'GetSystemMatrix\' not implemented in base \'SolvingStrategy\'" << std::endl;

                QUEST_CATCH("");
            }


            /**
             * @brief 该方法返回系统的右侧向量（RHS）
             * @return 右侧向量（RHS）
             */
            virtual TSystemVectorType& GetSystemVector()
            {
                QUEST_TRY

                QUEST_ERROR << "\'GetSystemVector\' not implemented in base \'SolvingStrategy\'" << std::endl;

                QUEST_CATCH("");
            }


            /**
             * @brief 获取解向量
             */
            virtual TSystemVectorType& GetSolutionVector()
            {
                QUEST_TRY

                QUEST_ERROR << "\'GetSolutionVector\' not implemented in base \'SolvingStrategy\'" << std::endl;

                QUEST_CATCH("");
            }


            /**
             * @brief 获取类在参数中的名称
             */
            static std::string Name()
            {
                return "solving_strategy";
            }


            virtual std::string Info() const
            {
                return "SolvingStrategy";
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
             * @brief 此方法用于验证并分配默认参数
             * @param rParameters 需要验证的参数
             * @param DefaultParameters 默认参数
             * @return 返回经过验证的参数
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
            virtual void AssignSettings(const Parameters ThisParameters){
                mMoveMeshFlag = ThisParameters["move_mesh_flag"].GetBool();

                mEchoLevel = ThisParameters["echo_level"].GetInt();
            }

        protected:
            /**
             * @brief 求解策略中信息输出等级
             */
            int mEchoLevel;

        private:
            /**
             * @brief 复制构造函数
             */
            SolvingStrategy(const SolvingStrategy& Other);

        private:
            /**
             * @brief 求解的模型部件
             */
            ModelPart* mpModelPart = nullptr;

            /**
             * @brief 是否移动网格的标记
             */
            bool mMoveMeshFlag;

    };

}

#endif //QUEST_SOLVING_STRATEGY_HPP