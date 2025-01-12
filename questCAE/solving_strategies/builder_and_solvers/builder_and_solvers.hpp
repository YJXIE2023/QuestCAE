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