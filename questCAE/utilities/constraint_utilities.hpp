#ifndef QUEST_CONSTRAINT_UTILITIES_HPP
#define QUEST_CONSTRAINT_UTILITIES_HPP


// 项目头文件
#include "includes/model_part.hpp"

namespace Quest{

    /**
     * @brief 该命名空间中包含了多点约束（MPC）所需的多个工具
     */
    namespace ConstraintUtilities{

        /**
         * @brief 该方法计算活跃的自由度（DOFs）
         * @param rModelPart 包含问题的模型部分的引用
         * @param rActiveDofs 包含活跃自由度的向量
         * @param rDofSet 所有自由度的集合
         */
        void QUEST_API(QUEST_CORE) ComputeActiveDofs(
            ModelPart& rModelPart,
            std::vector<int>& rActiveDofs,
            const ModelPart::DofsArrayType& rDofSet
        ){}


        /**
         * @brief 该方法计算活跃的自由度（DOFs）
         * @param rModelPart 引用，包含问题的模型部分
         * @param rActiveDofs 包含活跃自由度的向量
         * @param rDofSet 所有自由度的集合
         */
        void QUEST_API(QUEST_CORE) DistributedComputeActiveDofs(
            ModelPart& rModelPart,
            std::vector<int>& rActiveDofs,
            const ModelPart::DofsArrayType& rDofSet,
            const std::size_t InitialDofId
        ){}


        /**
         * @brief 该方法重置从属自由度的值
         * @param rModelPart 需要解决问题的模型
         */
        void QUEST_API(QUEST_CORE) ResetSlaveDofs(ModelPart& rModelPart){}


        /**
         * @brief 该方法重置从属自由度的值
         * @param rModelPart 要求解问题的模型
         */
        void QUEST_API(QUEST_CORE) ApplyConstraints(ModelPart& rModelPart){}


        /**
         * @brief 该方法预计算显式MPC对节点残差力的贡献
         * @param rModelPart 要求解问题的模型
         * @param rDofVariableNames 要检查的自由度变量名称
         * @param rResidualDofVariableNames 相应残差变量的名称
         */
        void QUEST_API(QUEST_CORE) PreComputeExplicitConstraintConstribution(
            ModelPart& rModelPart,
            const std::vector<std::string>& rDofVariableNames,
            const std::vector<std::string>& rResidualDofVariableNames
        ){}


        /**
         * @brief 该方法预计算显式MPC对节点质量和转动惯量的贡献
         * @todo 惯性必须使用Steiner定理计算
         * @param rModelPart 要求解问题的模型
         * @param DofDisplacementVariableName 要检查的自由度变量名称
         * @param MassVariableName 节点质量变量的名称
         * @param DofRotationVariableName 转动变量的名称
         * @param InertiaVariableName 要考虑的惯性变量
         */
        void QUEST_API(QUEST_CORE) PreComputeExplicitConstraintMassAndInertia(
            ModelPart& rModelPart,
            const std::string& DofDisplacementVariableName = "DISPLACEMENT",
            const std::string& MassVariableName = "NODAL_MASS",
            const std::string& DofRotationVariableName = "ROTATION",
            const std::string& InertiaVariableName = "NODAL_INERTIA_TENSOR"
        ){}

    };

}

#endif //QUEST_CONSTRAINT_UTILITIES_HPP