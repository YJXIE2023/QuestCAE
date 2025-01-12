#ifndef QUEST_VARIABLE_UTILS_HPP
#define QUEST_VARIABLE_UTILS_HPP

// 系统头文件
#include <type_traits>

// 项目头文件
#include "includes/define.hpp"
#include "includes/model_part.hpp"
#include "includes/check.hpp"
#include "includes/global_variables.hpp"
#include "utilities/parallel_utilities.hpp"
#include "utilities/atomic_utilities.hpp"
#include "utilities/reduction_utilities.hpp"

namespace Quest{

    /**
     * @brief 用于执行与变量值和特性相关的一些常见操作（已经并行化处理）
     * @details 这些方法被导出到 Python 接口，以便将这些改进添加到 Python 接口中
     */
    class QUEST_API(QUEST_CORE) VariableUtils{
        public:
            QUEST_CLASS_POINTER_DEFINITION(VariableUtils);

            using NodeType = ModelPart::NodeType;
            using ConditionType = ModelPart::ConditionType;
            using ElementType = ModelPart::ElementType;
            using NodesContainerType = ModelPart::NodesContainerType;
            using ConditionsContainerType = ModelPart::ConditionsContainerType;
            using ElementsContainerType = ModelPart::ElementsContainerType;
            using DoubleVarType = Variable<double>;
            using ArrayVarType = Variable<Array1d<double,3>>;

        public:
            /**
             * @brief 将一个变量的节点值从源模型部分的节点复制到目标模型部分的节点。假定源模型部分和目标模型部分具有相同数量的节点。
             * @param rVariable 要获取值的变量的引用
             * @param rDestinationVariable 要设置的变量的引用
             * @param rOriginModelPart 获取值的源模型部分
             * @param rDestinationModelPart 将值复制到的目标模型部分
             * @param ReadBufferStep 源缓冲区步骤
             * @param WriteBufferStep 目标缓冲区步骤
             */
            template <class TVarType>
            void CopyModelPartNodalVar(
                const TVarType &rVariable,
                const TVarType &rDestinationVariable,
                const ModelPart &rOriginModelPart,
                ModelPart &rDestinationModelPart,
                const unsigned int ReadBufferStep,
                const unsigned int WriteBufferStep )
            {
                const int n_orig_nodes = rOriginModelPart.NumberOfNodes();
                const int n_dest_nodes = rDestinationModelPart.NumberOfNodes();

                QUEST_ERROR_IF_NOT(n_orig_nodes == n_dest_nodes)
                    << "Origin and destination model parts have different number of nodes."
                    << "\n\t- Number of origin nodes: " << n_orig_nodes
                    << "\n\t- Number of destination nodes: " << n_dest_nodes << std::endl;

                IndexPartition<std::size_t>(n_orig_nodes).for_each([&](std::size_t index)
                                                                {
                    auto it_dest_node = rDestinationModelPart.NodesBegin() + index;
                    const auto it_orig_node = rOriginModelPart.NodesBegin() + index;
                    const auto &r_value = it_orig_node->GetSolutionStepValue(rVariable, ReadBufferStep);
                    it_dest_node->FastGetSolutionStepValue(rDestinationVariable, WriteBufferStep) = r_value; });

                rDestinationModelPart.GetCommunicator().SynchronizeVariable(rDestinationVariable);
            }


            /**
             * @brief 将一个变量的节点值从源模型部分的节点复制到目标模型部分的节点。假定源模型部分和目标模型部分具有相同数量的节点。
             * @param rVariable 要获取值的变量的引用
             * @param rDestinationVariable 要设置的变量的引用
             * @param rOriginModelPart 获取值的源模型部分
             * @param rDestinationModelPart 将值复制到的目标模型部分
             * @param BuffStep 缓冲区步骤
             */
            template <class TVarType>
            void CopyModelPartNodalVar(
                const TVarType &rVariable,
                const TVarType &rDestinationVariable,
                const ModelPart &rOriginModelPart,
                ModelPart &rDestinationModelPart,
                const unsigned int BuffStep = 0)
            {
                this->CopyModelPartNodalVar(rVariable, rDestinationVariable, rOriginModelPart, rDestinationModelPart, BuffStep, BuffStep);
            }


            /**
             * @brief 将一个变量的节点值从源模型部分的节点复制到目标模型部分的节点。假定源模型部分和目标模型部分具有相同数量的节点。
             * @param rVariable 要获取值并保存的变量的引用
             * @param rOriginModelPart 获取值的源模型部分
             * @param rDestinationModelPart 将值复制到的目标模型部分
             * @param BuffStep 缓冲区步骤
             */
            template< class TVarType >
            void CopyModelPartNodalVar(
                const TVarType& rVariable,
                const ModelPart& rOriginModelPart,
                ModelPart& rDestinationModelPart,
                const unsigned int BuffStep = 0)
            {
                this->CopyModelPartNodalVar(rVariable, rVariable, rOriginModelPart, rDestinationModelPart, BuffStep);
            }


            /**
             * @brief 将一个模型部分中的节点变量值复制到另一个模型部分中，并确保源模型部分和目标模型部分的节点数量一致
             * @param rVariable 源模型部分中的变量
             * @param rDestinationVariable 目标模型部分中的目标变量
             * @param rOriginModelPart 源模型部分（从中复制节点值）
             * @param rDestinationModelPart 目标模型部分（将节点值复制到这里）
             * @param BuffStep 缓冲区步骤
             */
            template< class TVarType >
            void CopyModelPartNodalVarToNonHistoricalVar(
                const TVarType &rVariable,
                const TVarType &rDestinationVariable,
                const ModelPart &rOriginModelPart,
                ModelPart &rDestinationModelPart,
                const unsigned int BuffStep = 0)
            {
                const int n_orig_nodes = rOriginModelPart.NumberOfNodes();
                const int n_dest_nodes = rDestinationModelPart.NumberOfNodes();

                QUEST_ERROR_IF_NOT(n_orig_nodes == n_dest_nodes) <<
                    "Origin and destination model parts have different number of nodes." <<
                    "\n\t- Number of origin nodes: " << n_orig_nodes <<
                    "\n\t- Number of destination nodes: " << n_dest_nodes << std::endl;

                IndexPartition<std::size_t>(n_orig_nodes).for_each([&](std::size_t index){
                    auto it_dest_node = rDestinationModelPart.NodesBegin() + index;
                    const auto it_orig_node = rOriginModelPart.NodesBegin() + index;
                    const auto& r_value = it_orig_node->GetSolutionStepValue(rVariable, BuffStep);
                    it_dest_node->GetValue(rDestinationVariable) = r_value;
                });

                rDestinationModelPart.GetCommunicator().SynchronizeNonHistoricalVariable(rDestinationVariable);
            }


            /**
             * @brief 将一个模型部分中的节点变量值复制到另一个模型部分中，并确保源模型部分和目标模型部分的节点数量一致
             * @param rVariable 源模型部分中的变量
             * @param rOriginModelPart 源模型部分（从中复制节点值）
             * @param rDestinationModelPart 目标模型部分（将节点值复制到这里）
             * @param BuffStep 缓冲区步骤
             */
            template< class TVarType >
            void CopyModelPartNodalVarToNonHistoricalVar(
                const TVarType &rVariable,
                const ModelPart &rOriginModelPart,
                ModelPart &rDestinationModelPart,
                const unsigned int BuffStep = 0)
            {
                this->CopyModelPartNodalVarToNonHistoricalVar(rVariable, rVariable, rOriginModelPart, rDestinationModelPart, BuffStep);
            }


            /**
             * @brief 将源模型部分中标记节点（符合给定条件的节点）上的历史变量值复制到目标模型部分中的历史变量值
             * @param rOriginVariable 源模型部分中的历史变量（如温度、应力等）
             * @param rDestinationVariable 目标模型部分中的历史变量
             * @param rOriginModelPart 源模型部分（从中复制节点值）
             * @param rDestinationModelPart 目标模型部分（将节点值复制到这里）
             * @param rFlag 标记条件，只有满足此条件的节点才会进行变量值的复制
             * @param CheckValue 是否检查变量值的有效性，默认为 true
             * @param ReadBufferStep 读取历史变量时的缓冲区步骤（从源模型部分读取）
             * @param WriteBufferStep 写入历史变量时的缓冲区步骤（写入目标模型部分）
             */
            template <class TDataType>
            void CopyModelPartFlaggedNodalHistoricalVarToHistoricalVar(
                const Variable<TDataType>& rOriginVariable,
                const Variable<TDataType>& rDestinationVariable,
                const ModelPart& rOriginModelPart,
                ModelPart& rDestinationModelPart,
                const Flags& rFlag,
                const bool CheckValue = true,
                const unsigned int ReadBufferStep = 0,
                const unsigned int WriteBufferStep = 0)
            {
                QUEST_TRY

                QUEST_ERROR_IF(
                    rOriginModelPart.FullName() == rDestinationModelPart.FullName() &&
                    rOriginVariable == rDestinationVariable &&
                    ReadBufferStep == WriteBufferStep)
                    << "Trying to copy flagged nodal solution step values with the same origin and destination model parts/variables/buffer steps. This is not permitted ( Origin model part: "
                    << rOriginModelPart.Name() << ", destination model part: " << rDestinationModelPart.Name()
                    << ", variable: " << rOriginVariable.Name() << ", buffer step: " << ReadBufferStep << " ) !";

                QUEST_ERROR_IF_NOT(rOriginModelPart.HasNodalSolutionStepVariable(rOriginVariable))
                    << rOriginVariable.Name() << " is not found in nodal solution step variables list in origin model part ( "
                    << rOriginModelPart.Name() << " ).";

                QUEST_ERROR_IF_NOT(rDestinationModelPart.HasNodalSolutionStepVariable(rDestinationVariable))
                    << rDestinationVariable.Name() << " is not found in nodal solution step variables list in destination model part ( "
                    << rDestinationModelPart.Name() << " ).";

                QUEST_ERROR_IF(ReadBufferStep >= rOriginModelPart.GetBufferSize())
                    << "Origin model part ( " << rOriginModelPart.Name()
                    << " ) buffer size is smaller or equal than read buffer size [ "
                    << rOriginModelPart.GetBufferSize() << " <= " << ReadBufferStep << " ].";

                QUEST_ERROR_IF(WriteBufferStep >= rDestinationModelPart.GetBufferSize())
                    << "Destination model part ( " << rDestinationModelPart.Name()
                    << " ) buffer size is smaller or equal than read buffer size [ "
                    << rDestinationModelPart.GetBufferSize() << " <= " << WriteBufferStep << " ].";

                CopyModelPartFlaggedVariable<NodesContainerType>(
                    rOriginModelPart, rDestinationModelPart, rFlag, CheckValue,
                    [&](NodeType& rDestNode, const TDataType& rValue) {
                        rDestNode.FastGetSolutionStepValue(
                            rDestinationVariable, WriteBufferStep) = rValue;
                    },
                    [&](const NodeType& rOriginNode) -> const TDataType& {
                        return rOriginNode.FastGetSolutionStepValue(rOriginVariable, ReadBufferStep);
                    });

                rDestinationModelPart.GetCommunicator().SynchronizeVariable(rDestinationVariable);

                QUEST_CATCH("");
            }


            /**
             * @brief 将源模型部分中标记节点（符合给定条件的节点）上的历史变量值复制到目标模型部分中的历史变量值
             * @param rOriginVariable 源模型部分中的历史变量（如温度、应力等）
             * @param rDestinationVariable 目标模型部分中的历史变量
             * @param rModelPart 源模型部分（从中复制节点值）
             * @param rFlag 标记条件，只有满足此条件的节点才会进行变量值的复制
             * @param CheckValue 是否检查变量值的有效性，默认为 true
             * @param ReadBufferStep 读取历史变量时的缓冲区步骤（从源模型部分读取）
             * @param WriteBufferStep 写入历史变量时的缓冲区步骤（写入目标模型部分）
             */
            template <class TDataType>
            void CopyModelPartFlaggedNodalHistoricalVarToHistoricalVar(
                const Variable<TDataType>& rOriginVariable,
                const Variable<TDataType>& rDestinationVariable,
                ModelPart& rModelPart,
                const Flags& rFlag,
                const bool CheckValue = true,
                const unsigned int ReadBufferStep = 0,
                const unsigned int WriteBufferStep = 0)
            {
                QUEST_TRY

                CopyModelPartFlaggedNodalHistoricalVarToHistoricalVar(
                    rOriginVariable, rDestinationVariable, rModelPart, rModelPart,
                    rFlag, CheckValue, ReadBufferStep, WriteBufferStep);

                QUEST_CATCH("");
            }


            /**
             * @brief 将源模型部分中的标记节点的历史变量值复制到目标模型部分中的历史变量值
             * @param rVariable 要获取值的变量的引用
             * @param rOriginModelPart 源模型部分（从中复制节点值）
             * @param rDestinationModelPart 目标模型部分（将节点值复制到这里）
             * @param rFlag 标记条件，只有满足此条件的节点才会进行变量值的复制
             * @param CheckValue 是否检查变量值的有效性，默认为 true
             * @param ReadBufferStep 读取历史变量时的缓冲区步骤（从源模型部分读取）
             * @param WriteBufferStep 写入历史变量时的缓冲区步骤（写入目标模型部分）
             */
            template <class TDataType>
            void CopyModelPartFlaggedNodalHistoricalVarToHistoricalVar(
                const Variable<TDataType>& rVariable,
                const ModelPart& rOriginModelPart,
                ModelPart& rDestinationModelPart,
                const Flags& rFlag,
                const bool CheckValue = true,
                const unsigned int ReadBufferStep = 0,
                const unsigned int WriteBufferStep = 0)
            {
                QUEST_TRY

                CopyModelPartFlaggedNodalHistoricalVarToHistoricalVar(
                    rVariable, rVariable, rOriginModelPart, rDestinationModelPart,
                    rFlag, CheckValue, ReadBufferStep, WriteBufferStep);

                QUEST_CATCH("");
            }


            /**
             * @brief 将源模型部分的标记节点的历史变量值复制到目标模型部分中的非历史变量
             * @param rOriginVariable 源变量（历史变量），从源模型部分的节点读取
             * @param rDestinationVariable 目标变量（非历史变量），将值设置到目标模型部分的节点
             * @param rOriginModelPart 源模型部分，包含要复制的变量的节点
             * @param rDestinationModelPart 目标模型部分（将节点值复制到这里）
             * @param rFlag 标记条件，只有满足此条件的节点才会进行变量值的复制
             * @param CheckValue 是否检查变量值的有效性，默认为 true
             * @param ReadBufferStep 读取历史变量时的缓冲区步骤
             */
            template <class TDataType>
            void CopyModelPartFlaggedNodalHistoricalVarToNonHistoricalVar(
                const Variable<TDataType>& rOriginVariable,
                const Variable<TDataType>& rDestinationVariable,
                const ModelPart& rOriginModelPart,
                ModelPart& rDestinationModelPart,
                const Flags& rFlag,
                const bool CheckValue = true,
                const unsigned int ReadBufferStep = 0)
            {
                QUEST_TRY

                QUEST_ERROR_IF_NOT(rOriginModelPart.HasNodalSolutionStepVariable(rOriginVariable))
                    << rOriginVariable.Name() << " is not found in nodal solution step variables list in origin model part ( "
                    << rOriginModelPart.Name() << " ).";

                QUEST_ERROR_IF(ReadBufferStep >= rOriginModelPart.GetBufferSize())
                    << "Origin model part ( " << rOriginModelPart.Name()
                    << " ) buffer size is smaller or equal than read buffer size [ "
                    << rOriginModelPart.GetBufferSize() << " <= " << ReadBufferStep << " ].";


                CopyModelPartFlaggedVariable<NodesContainerType>(
                    rOriginModelPart, rDestinationModelPart, rFlag, CheckValue,
                    [&](NodeType& rDestNode, const TDataType& rValue) {
                        rDestNode.SetValue(rDestinationVariable, rValue);
                    },
                    [&](const NodeType& rOriginNode) -> const TDataType& {
                        return rOriginNode.FastGetSolutionStepValue(rOriginVariable, ReadBufferStep);
                    });

                rDestinationModelPart.GetCommunicator().SynchronizeNonHistoricalVariable(rDestinationVariable);

                QUEST_CATCH("");
            }


            /**
             * @brief 将源模型部分的标记节点的历史变量值复制到目标模型部分中的非历史变量
             */
            template <class TDataType>
            void CopyModelPartFlaggedNodalHistoricalVarToNonHistoricalVar(
                const Variable<TDataType>& rOriginVariable,
                const Variable<TDataType>& rDestinationVariable,
                ModelPart& rModelPart,
                const Flags& rFlag,
                const bool CheckValue = true,
                const unsigned int ReadBufferStep = 0)
            {
                CopyModelPartFlaggedNodalHistoricalVarToNonHistoricalVar(
                    rOriginVariable, rDestinationVariable, rModelPart, rModelPart,
                    rFlag, CheckValue, ReadBufferStep);
            }


            /**
             * @brief 将源模型部分的标记节点的历史变量值复制到目标模型部分中的非历史变量
             */
            template <class TDataType>
            void CopyModelPartFlaggedNodalHistoricalVarToNonHistoricalVar(
                const Variable<TDataType>& rVariable,
                const ModelPart& rOriginModelPart,
                ModelPart& rDestinationModelPart,
                const Flags& rFlag,
                const bool CheckValue = true,
                const unsigned int ReadBufferStep = 0)
            {
                CopyModelPartFlaggedNodalHistoricalVarToNonHistoricalVar(
                    rVariable, rVariable, rOriginModelPart, rDestinationModelPart,
                    rFlag, CheckValue, ReadBufferStep);
            }


            /**
             * @brief 将源模型部分的标记节点的历史变量值复制到目标模型部分中的非历史变量
             */
            template <class TDataType>
            void CopyModelPartFlaggedNodalHistoricalVarToNonHistoricalVar(
                const Variable<TDataType>& rVariable,
                ModelPart& rModelPart,
                const Flags& rFlag,
                const bool CheckValue = true,
                const unsigned int ReadBufferStep = 0)
            {
                CopyModelPartFlaggedNodalHistoricalVarToNonHistoricalVar(
                    rVariable, rVariable, rModelPart, rModelPart,
                    rFlag, CheckValue, ReadBufferStep);
            }


            /**
             * @brief 将源模型部分中的非历史变量值复制到目标模型部分的历史变量
             * @param rOriginVariable 源变量（非历史变量），从源模型部分的节点读取
             * @param rDestinationVariable 目标变量（历史变量），将值设置到目标模型部分的节点
             * @param rOriginModelPart 源模型部分，包含要复制的变量的节点
             * @param rDestinationModelPart 目标模型部分（将节点值复制到这里）
             * @param rFlag 标记条件，只有满足此条件的节点才会进行变量值的复制
             * @param CheckValue 是否检查变量值的有效性，默认为 true
             * @param WriteBufferStep 写入历史变量时的缓冲区步骤
             */
            template <class TDataType>
            void CopyModelPartFlaggedNodalNonHistoricalVarToHistoricalVar(
                const Variable<TDataType>& rOriginVariable,
                const Variable<TDataType>& rDestinationVariable,
                const ModelPart& rOriginModelPart,
                ModelPart& rDestinationModelPart,
                const Flags& rFlag,
                const bool CheckValue = true,
                const unsigned int WriteBufferStep = 0)
            {
                QUEST_TRY

                QUEST_ERROR_IF_NOT(rDestinationModelPart.HasNodalSolutionStepVariable(rDestinationVariable))
                    << rDestinationVariable.Name() << " is not found in nodal solution step variables list in destination model part ( "
                    << rDestinationModelPart.Name() << " ).";

                QUEST_ERROR_IF(WriteBufferStep >= rDestinationModelPart.GetBufferSize())
                    << "Destination model part ( " << rDestinationModelPart.Name()
                    << " ) buffer size is smaller or equal than read buffer size [ "
                    << rDestinationModelPart.GetBufferSize() << " <= " << WriteBufferStep << " ].";

                CopyModelPartFlaggedVariable<NodesContainerType>(
                    rOriginModelPart, rDestinationModelPart, rFlag, CheckValue,
                    [&](NodeType& rDestNode, const TDataType& rValue) {
                        rDestNode.FastGetSolutionStepValue(
                            rDestinationVariable, WriteBufferStep) = rValue;
                    },
                    [&](const NodeType& rOriginNode) -> const TDataType& {
                        return rOriginNode.GetValue(rOriginVariable);
                    });

                rDestinationModelPart.GetCommunicator().SynchronizeVariable(rDestinationVariable);

                QUEST_CATCH("");
            }


            /**
             * @brief 将源模型部分中的非历史变量值复制到目标模型部分的历史变量
             */
            template <class TDataType>
            void CopyModelPartFlaggedNodalNonHistoricalVarToHistoricalVar(
                const Variable<TDataType>& rOriginVariable,
                const Variable<TDataType>& rDestinationVariable,
                ModelPart& rModelPart,
                const Flags& rFlag,
                const bool CheckValue = true,
                const unsigned int WriteBufferStep = 0)
            {
                CopyModelPartFlaggedNodalNonHistoricalVarToHistoricalVar(
                    rOriginVariable, rDestinationVariable, rModelPart, rModelPart,
                    rFlag, CheckValue, WriteBufferStep);
            }


            /**
             * @brief 将源模型部分中的非历史变量值复制到目标模型部分的历史变量
             */
            template <class TDataType>
            void CopyModelPartFlaggedNodalNonHistoricalVarToHistoricalVar(
                const Variable<TDataType>& rVariable,
                const ModelPart& rOriginModelPart,
                ModelPart& rDestinationModelPart,
                const Flags& rFlag,
                const bool CheckValue = true,
                const unsigned int WriteBufferStep = 0)
            {
                CopyModelPartFlaggedNodalNonHistoricalVarToHistoricalVar(
                    rVariable, rVariable, rOriginModelPart, rDestinationModelPart,
                    rFlag, CheckValue, WriteBufferStep);
            }


            /**
             * @brief 将源模型部分中的非历史变量值复制到目标模型部分的历史变量
             */
            template <class TDataType>
            void CopyModelPartFlaggedNodalNonHistoricalVarToHistoricalVar(
                const Variable<TDataType>& rVariable,
                ModelPart& rModelPart,
                const Flags& rFlag,
                const bool CheckValue = true,
                const unsigned int WriteBufferStep = 0)
            {
                CopyModelPartFlaggedNodalNonHistoricalVarToHistoricalVar(
                    rVariable, rVariable, rModelPart, rModelPart,
                    rFlag, CheckValue, WriteBufferStep);
            }


            /**
             * @brief 将源模型部分中的非历史变量值复制到目标模型部分的非历史变量
             * @param rOriginVariable 源变量（非历史变量），从源模型部分的节点读取
             * @param rDestinationVariable 目标变量（非历史变量），将值设置到目标模型部分的节点
             * @param rOriginModelPart 源模型部分，包含要复制的变量的节点
             * @param rDestinationModelPart 目标模型部分（将节点值复制到这里）
             * @param rFlag 标记条件，只有满足此条件的节点才会进行变量值的复制
             * @param CheckValue 是否检查变量值的有效性，默认为 true
             */
            template <class TDataType>
            void CopyModelPartFlaggedNodalNonHistoricalVarToNonHistoricalVar(
                const Variable<TDataType>& rOriginVariable,
                const Variable<TDataType>& rDestinationVariable,
                const ModelPart& rOriginModelPart,
                ModelPart& rDestinationModelPart,
                const Flags& rFlag,
                const bool CheckValue = true)
            {
                QUEST_TRY

                QUEST_ERROR_IF(
                    rOriginModelPart.FullName() == rDestinationModelPart.FullName() &&
                    rOriginVariable == rDestinationVariable
                ) << "Trying to copy flagged nodal non-historical values with the same model parts/variables. This is not permitted ( Origin model part: "
                    << rOriginModelPart.Name() << ", destination model part: " << rDestinationModelPart.Name()
                << ", variable: " << rOriginVariable.Name() << " ) !";

                CopyModelPartFlaggedVariable<NodesContainerType>(
                    rOriginModelPart, rDestinationModelPart, rFlag, CheckValue,
                    [&](NodeType& rDestNode, const TDataType& rValue) {
                        rDestNode.SetValue(rDestinationVariable, rValue);
                    },
                    [&](const NodeType& rOriginNode) -> const TDataType& {
                        return rOriginNode.GetValue(rOriginVariable);
                    });

                rDestinationModelPart.GetCommunicator().SynchronizeNonHistoricalVariable(rDestinationVariable);

                QUEST_CATCH("");
            }


            /**
             * @brief 将源模型部分中的非历史变量值复制到目标模型部分的非历史变量
             */
            template <class TDataType>
            void CopyModelPartFlaggedNodalNonHistoricalVarToNonHistoricalVar(
                const Variable<TDataType>& rOriginVariable,
                const Variable<TDataType>& rDestinationVariable,
                ModelPart& rModelPart,
                const Flags& rFlag,
                const bool CheckValue = true)
            {
                CopyModelPartFlaggedNodalNonHistoricalVarToNonHistoricalVar(
                    rOriginVariable, rDestinationVariable, rModelPart, rModelPart, rFlag, CheckValue);
            }


            /**
             * @brief 将源模型部分中的非历史变量值复制到目标模型部分的非历史变量
             */
            template <class TDataType>
            void CopyModelPartFlaggedNodalNonHistoricalVarToNonHistoricalVar(
                const Variable<TDataType>& rVariable,
                const ModelPart& rOriginModelPart,
                ModelPart& rDestinationModelPart,
                const Flags& rFlag,
                const bool CheckValue = true)
            {
                CopyModelPartFlaggedNodalNonHistoricalVarToNonHistoricalVar(
                    rVariable, rVariable, rOriginModelPart, rDestinationModelPart, rFlag, CheckValue);
            }


            /**
             * @brief 将源模型部分中的单元变量值复制到目标模型部分的单元变量，只复制满足指定标记条件的单元
             * @param rOriginVariable 源变量（单元变量），从源模型部分的单元读取
             * @param rDestinationVariable 目标变量（单元变量），将值设置到目标模型部分的单元
             * @param rOriginModelPart 源模型部分，包含要复制的变量的单元
             * @param rDestinationModelPart 目标模型部分（将单元值复制到这里）
             * @param rFlag 标记条件，只有满足此条件的单元才会进行变量值的复制
             * @param CheckValue 是否检查变量值的有效性，默认为 true
             */
            template <class TDataType>
            void CopyModelPartFlaggedElementVar(
                const Variable<TDataType>& rOriginVariable,
                const Variable<TDataType>& rDestinationVariable,
                const ModelPart& rOriginModelPart,
                ModelPart& rDestinationModelPart,
                const Flags& rFlag,
                const bool CheckValue = true)
            {
                QUEST_TRY

                QUEST_ERROR_IF(rOriginModelPart.FullName() == rDestinationModelPart.FullName() && rOriginVariable == rDestinationVariable)
                    << "Trying to copy flagged elemental variable data with the same model "
                    "parts/variables. This is not permitted ( Origin model part: "
                    << rOriginModelPart.Name() << ", destination model part: " << rDestinationModelPart.Name()
                    << ", variable: " << rOriginVariable.Name() << " ) !";

                CopyModelPartFlaggedVariable<ElementsContainerType>(
                    rOriginModelPart, rDestinationModelPart, rFlag, CheckValue,
                    [&](ElementType& rDestElement, const TDataType& rValue) {
                        rDestElement.SetValue(rDestinationVariable, rValue);
                    },
                    [&](const ElementType& rOriginElement) -> const TDataType& {
                        return rOriginElement.GetValue(rOriginVariable);
                    });

                QUEST_CATCH("");
            }


            /**
             * @brief 将源模型部分中的单元变量值复制到目标模型部分的单元变量，只复制满足指定标记条件的单元
             */
            template <class TDataType>
            void CopyModelPartFlaggedElementVar(
                const Variable<TDataType>& rOriginVariable,
                const Variable<TDataType>& rDestinationVariable,
                ModelPart& rModelPart,
                const Flags& rFlag,
                const bool CheckValue = true)
            {
                CopyModelPartFlaggedElementVar(
                    rOriginVariable, rDestinationVariable, rModelPart, rModelPart, rFlag, CheckValue);
            }


            /**
             * @brief 将源模型部分中的单元变量值复制到目标模型部分的单元变量，只复制满足指定标记条件的单元
             */
            template <class TDataType>
            void CopyModelPartFlaggedElementVar(
                const Variable<TDataType>& rVariable,
                const ModelPart& rOriginModelPart,
                ModelPart& rDestinationModelPart,
                const Flags& rFlag,
                const bool CheckValue = true)
            {
                CopyModelPartFlaggedElementVar(
                    rVariable, rVariable, rOriginModelPart, rDestinationModelPart, rFlag, CheckValue);
            }


            /**
             * @brief 将源模型部分中的单元变量值复制到目标模型部分的单元变量，只复制满足指定标记条件的单元
             * @param rOriginVariable 源变量（单元变量），从源模型部分的单元读取
             * @param rDestinationVariable 目标变量（单元变量），将值设置到目标模型部分的单元
             * @param rOriginModelPart 源模型部分，包含要复制的变量的单元
             * @param rDestinationModelPart 目标模型部分（将单元值复制到这里）
             * @param rFlag 标记条件，只有满足此条件的单元才会进行变量值的复制
             * @param CheckValue 是否检查变量值的有效性，默认为 true
             */
            template <class TDataType>
            void CopyModelPartFlaggedConditionVar(
                const Variable<TDataType>& rOriginVariable,
                const Variable<TDataType>& rDestinationVariable,
                const ModelPart& rOriginModelPart,
                ModelPart& rDestinationModelPart,
                const Flags& rFlag,
                const bool CheckValue = true)
            {
                QUEST_TRY

                QUEST_ERROR_IF(rOriginModelPart.FullName() == rDestinationModelPart.FullName() && rOriginVariable == rDestinationVariable)
                    << "Trying to copy flagged condition variable data with the same model "
                    "parts/variables. This is not permitted ( Origin model part: "
                    << rOriginModelPart.Name() << ", destination model part: " << rDestinationModelPart.Name()
                    << ", variable: " << rOriginVariable.Name() << " ) !";

                CopyModelPartFlaggedVariable<ConditionsContainerType>(
                    rOriginModelPart, rDestinationModelPart, rFlag, CheckValue,
                    [&](ConditionType& rDestCondition, const TDataType& rValue) {
                        rDestCondition.SetValue(rDestinationVariable, rValue);
                    },
                    [&](const ConditionType& rOriginCondition) -> const TDataType& {
                        return rOriginCondition.GetValue(rOriginVariable);
                    });

                QUEST_CATCH("");
            }


            /**
             * @brief 将源模型部分中的单元变量值复制到目标模型部分的单元变量，只复制满足指定标记条件的单元
             */
            template <class TDataType>
            void CopyModelPartFlaggedConditionVar(
                const Variable<TDataType>& rOriginVariable,
                const Variable<TDataType>& rDestinationVariable,
                ModelPart& rModelPart,
                const Flags& rFlag,
                const bool CheckValue = true)
            {
                CopyModelPartFlaggedConditionVar(
                    rOriginVariable, rDestinationVariable, rModelPart, rModelPart, rFlag, CheckValue);
            }


            /**
             * @brief 将源模型部分中的单元变量值复制到目标模型部分的单元变量，只复制满足指定标记条件的单元
             */
            template <class TDataType>
            void CopyModelPartFlaggedConditionVar(
                const Variable<TDataType>& rVariable,
                const ModelPart& rOriginModelPart,
                ModelPart& rDestinationModelPart,
                const Flags& rFlag,
                const bool CheckValue = true)
            {
                CopyModelPartFlaggedConditionVar(
                    rVariable, rVariable, rOriginModelPart, rDestinationModelPart, rFlag, CheckValue);
            }


            /**
             * @brief 将一个变量的单元值从源模型部分的单元复制到目标模型部分的单元中。假定源模型部分和目标模型部分具有相同数量的单元
             * @param rVariable 要设置的变量的引用
             * @param rOriginModelPart 从中获取值的源模型部分
             * @param rDestinationModelPart 值被复制到的目标模型部分
             */
            template< class TVarType >
            void CopyModelPartElementalVar(
                const TVarType& rVariable,
                const ModelPart& rOriginModelPart,
                ModelPart& rDestinationModelPart){

                const int n_orig_elems = rOriginModelPart.NumberOfElements();
                const int n_dest_elems = rDestinationModelPart.NumberOfElements();

                QUEST_ERROR_IF_NOT(n_orig_elems == n_dest_elems) << "Origin and destination model parts have different number of elements."
                                                                << "\n\t- Number of origin elements: " << n_orig_elems
                                                                << "\n\t- Number of destination elements: " << n_dest_elems << std::endl;

                IndexPartition<std::size_t>(n_orig_elems).for_each([&](std::size_t index){
                auto it_dest_elems = rDestinationModelPart.ElementsBegin() + index;
                const auto it_orig_elems = rOriginModelPart.ElementsBegin() + index;
                const auto& r_value = it_orig_elems->GetValue(rVariable);
                it_dest_elems->SetValue(rVariable,r_value);
                });
            }


            /**
             * @brief 设置标量变量的节点值
             */
            template<class TDataType, class TVarType = Variable<TDataType> >
            void SetVariable(
                const TVarType& rVariable,
                const TDataType& rValue,
                NodesContainerType& rNodes,
                const unsigned int Step = 0)
            {
                QUEST_TRY

                block_for_each(rNodes, [&](Node& rNode) {
                    rNode.FastGetSolutionStepValue(rVariable, Step) = rValue;
                });

                QUEST_CATCH("")
            }


            /**
             * @brief 设置标量变量的节点值（考虑标志）
             */
            template <class TDataType, class TVarType = Variable<TDataType>>
            void SetVariable(
                const TVarType &rVariable,
                const TDataType &rValue,
                NodesContainerType &rNodes,
                const Flags Flag,
                const bool CheckValue = true)
            {
                QUEST_TRY

                block_for_each(rNodes, [&](Node& rNode){
                    if(rNode.Is(Flag) == CheckValue){
                        rNode.FastGetSolutionStepValue(rVariable) = rValue;}
                });

                QUEST_CATCH("")
            }


            /**
             * @brief 将任意变量的节点值设置为零
             */
            template< class TType , class TContainerType>
            void SetNonHistoricalVariableToZero(
                const Variable< TType >& rVariable,
                TContainerType& rContainer)
            {
                QUEST_TRY
                this->SetNonHistoricalVariable(rVariable, rVariable.Zero(), rContainer);
                QUEST_CATCH("")
            }


            /**
             * @brief 将提供的变量列表在非历史数据库中设置为零
             */
            template<class TContainerType, class... TVariableArgs>
            static void SetNonHistoricalVariablesToZero(
                TContainerType& rContainer,
                const TVariableArgs&... rVariableArgs)
            {
                block_for_each(rContainer, [&](auto& rEntity){
                    (rEntity.SetValue(rVariableArgs, rVariableArgs.Zero()), ...);
                });
            }


            /**
             * @brief 将任何变量的节点值设置为零
             */
            template< class TType >
            void SetHistoricalVariableToZero(
                const Variable< TType >& rVariable,
                NodesContainerType& rNodes)
            {
                QUEST_TRY
                this->SetVariable(rVariable, rVariable.Zero(), rNodes);
                QUEST_CATCH("")
            }


            /**
             * @brief 将提供的变量列表在节点历史数据库中设置为零
             */
            template<class... TVariableArgs>
            static void SetHistoricalVariablesToZero(
                NodesContainerType& rNodes,
                const TVariableArgs&... rVariableArgs)
            {
                block_for_each(rNodes, [&](NodeType& rNode){(
                    AuxiliaryHistoricalValueSetter<typename TVariableArgs::Type>(rVariableArgs, rVariableArgs.Zero(), rNode), ...);
                });
            }


            /**
             * @brief 设置任意类型的非历史变量的容器值
             */
            template< class TType, class TContainerType, class TVarType = Variable< TType >>
            void SetNonHistoricalVariable(
                const TVarType& rVariable,
                const TType& Value,
                TContainerType& rContainer
                )
            {
                QUEST_TRY

                block_for_each(rContainer, [&](typename TContainerType::value_type& rEntity){
                    rEntity.SetValue(rVariable, Value);
                });

                QUEST_CATCH("")
            }


            /**
             * @brief 设置考虑标志的任何类型非历史变量的容器值
             */
            template< class TType, class TContainerType, class TVarType = Variable< TType >>
            void SetNonHistoricalVariable(
                const TVarType& rVariable,
                const TType& rValue,
                TContainerType& rContainer,
                const Flags Flag,
                const bool Check = true
                )
            {
                QUEST_TRY

                block_for_each(rContainer, [&](typename TContainerType::value_type& rEntity){
                    if(rEntity.Is(Flag) == Check){
                        rEntity.SetValue(rVariable, rValue);}
                });

                QUEST_CATCH("")
            }


            /**
             * @brief 删除容器中的非历史变量
             */
            template< class TContainerType, class TVarType>
            void EraseNonHistoricalVariable(
                const TVarType& rVariable,
                TContainerType& rContainer
                )
            {
                QUEST_TRY

                block_for_each(rContainer, [&rVariable](auto& rEntity){
                        rEntity.GetData().Erase(rVariable);
                });

                QUEST_CATCH("")
            }


            /**
             * @brief 清空容器中的数据值
             */
            template< class TContainerType>
            void ClearNonHistoricalData(TContainerType& rContainer)
            {
                QUEST_TRY

                block_for_each(rContainer, [&](typename TContainerType::value_type& rEntity){
                        rEntity.GetData().Clear();
                });

                QUEST_CATCH("")
            }


            /**
             * @brief 将变量值从 TContainerType 容器分配到节点
             * @details 该方法将存储在 rModelPart 中 TContainerType 数据值容器中的变量值分配到节点。
             * 根据 rWeightVariable 的值，对每个节点使用常数加权。结果将存储在节点的非历史数据值容器中，使用相同的 rVariable。
             * 如果 IsInverseWeightProvided 为 true，则反转 rWeightVariable 提供的权重以获取节点权重。
             * 否则，将使用 rWeightVariable 给出的值作为权重
             */
            template <class TDataType, class TContainerType, class TWeightDataType>
            void WeightedAccumulateVariableOnNodes(
                ModelPart& rModelPart,
                const Variable<TDataType>& rVariable,
                const Variable<TWeightDataType>& rWeightVariable,
                const bool IsInverseWeightProvided = false
            );


            /**
             * @brief 根据给定的状态在指定的容器上设置一个标志
             */
            template< class TContainerType >
            void SetFlag(
                const Flags& rFlag,
                const bool FlagValue,
                TContainerType& rContainer
                )
            {
                QUEST_TRY

                block_for_each(rContainer, [&](typename TContainerType::value_type& rEntity){
                        rEntity.Set(rFlag, FlagValue);
                });

                QUEST_CATCH("")

            }


            /**
             * @brief 重置给定状态的标志
             */
            template< class TContainerType >
            void ResetFlag(
                const Flags& rFlag,
                TContainerType& rContainer
                )
            {
                QUEST_TRY

                block_for_each(rContainer, [&](typename TContainerType::value_type& rEntity){
                        rEntity.Reset(rFlag);
                });

                QUEST_CATCH("")
            }


            /**
             * @brief 反转给定状态的标志
             */
            template< class TContainerType >
            void FlipFlag(
                const Flags& rFlag,
                TContainerType& rContainer
                )
            {
                QUEST_TRY

                block_for_each(rContainer, [&](typename TContainerType::value_type& rEntity){
                        rEntity.Flip(rFlag);
                });

                QUEST_CATCH("")
            }


            /**
             * @brief 获取非历史变量的值，并将其保存到另一个变量中
             */
            template< class TDataType, class TVariableType = Variable<TDataType> >
            void SaveVariable(
                const TVariableType &rOriginVariable,
                const TVariableType &rSavedVariable,
                NodesContainerType &rNodesContainer)
            {
                QUEST_TRY

                block_for_each(rNodesContainer, [&](Node& rNode){
                    rNode.SetValue(rSavedVariable, rNode.FastGetSolutionStepValue(rOriginVariable));
                });

                QUEST_CATCH("")
            }


            /**
             * @brief 获取非历史变量的值，并将其保存到另一个历史变量中
             */
            template< class TDataType, class TContainerType, class TVariableType = Variable<TDataType> >
            void SaveNonHistoricalVariable(
                const TVariableType &rOriginVariable,
                const TVariableType &rSavedVariable,
                TContainerType &rContainer
                )
            {
                QUEST_TRY

                block_for_each(rContainer, [&](typename TContainerType::value_type& rEntity){
                    rEntity.SetValue(rSavedVariable, rEntity.GetValue(rOriginVariable));
                });

                QUEST_CATCH("")
            }


            /**
             * @brief 获取一个历史变量的值，并将其设置到提供的容器中所有节点的另一个变量中
             */
            template< class TDataType, class TVariableType = Variable<TDataType> >
            void CopyVariable(
                const TVariableType &rOriginVariable,
                const TVariableType &rDestinationVariable,
                NodesContainerType &rNodesContainer)
            {
                QUEST_TRY

                block_for_each(rNodesContainer, [&](Node& rNode){
                    rNode.FastGetSolutionStepValue(rDestinationVariable) = rNode.FastGetSolutionStepValue(rOriginVariable);
                });

                QUEST_CATCH("")
            }


            /**
             * @brief 返回通过给定的双精度变量和值过滤后的节点列表
             */
            [[nodiscard]] NodesContainerType SelectNodeList(
                const DoubleVarType& Variable,
                const double Value,
                const NodesContainerType& rOriginNodes
            ){}


            /**
             * @brief 检查节点映射中所有节点是否具有给定的变量
             */
            template<class TVarType>
            int CheckVariableExists(
                const TVarType& rVariable,
                const NodesContainerType& rNodes
                )
            {
                QUEST_TRY

                for (auto& i_node : rNodes)
                    QUEST_CHECK_VARIABLE_IN_NODAL_DATA(rVariable, i_node);

                return 0;

                QUEST_CATCH("");
            }


            /**
             * @brief 固定或释放列表中所有节点的变量，自由度必须存在
             */
            template< class TVarType >
            void ApplyFixity(
                const TVarType& rVar,
                const bool IsFixed,
                NodesContainerType& rNodes
                )
            {
                QUEST_TRY

                if (rNodes.size() != 0) {
                    QUEST_ERROR_IF_NOT(rNodes.begin()->HasDofFor(rVar)) << "Trying to fix/free dof of variable " << rVar.Name() << " but this dof does not exist in node #" << rNodes.begin()->Id() << "!" << std::endl;

                #ifdef QUEST_DEBUG
                    for (const auto& r_node : rNodes) {
                        QUEST_ERROR_IF_NOT(r_node.HasDofFor(rVar)) << "Trying to fix/free dof of variable " << rVar.Name() << " but this dof does not exist in node #" << r_node.Id() << "!" << std::endl;
                    }
                #endif

                    CheckVariableExists(rVar, rNodes);

                    if (IsFixed) {
                        block_for_each(rNodes,[&](Node& rNode){
                            rNode.pGetDof(rVar)->FixDof();
                        });
                    } else {
                        block_for_each(rNodes,[&](Node& rNode){
                            rNode.pGetDof(rVar)->FreeDof();
                        });
                    }
                }

                QUEST_CATCH("")
            }


            /**
             * @brief 如果 rFlag 与该特定节点提供的 CheckValue 匹配，则此方法将固定/释放给定的 rVariable
             */
            template< class TVarType >
            void ApplyFixity(
                const TVarType& rVariable,
                const bool IsFixed,
                NodesContainerType& rNodes,
                const Flags& rFlag,
                const bool CheckValue = true)
            {
                QUEST_TRY

                if (rNodes.size() != 0) {
                    QUEST_ERROR_IF_NOT(rNodes.begin()->HasDofFor(rVariable))
                        << "Trying to fix/free dof of variable " << rVariable.Name()
                        << " but this dof does not exist in node #"
                        << rNodes.begin()->Id() << "!" << std::endl;

                #ifdef QUEST_DEBUG
                    for (const auto& r_node : rNodes) {
                        QUEST_ERROR_IF_NOT(r_node.HasDofFor(rVariable))
                            << "Trying to fix/free dof of variable " << rVariable.Name()
                            << " but this dof does not exist in node #" << r_node.Id()
                            << "!" << std::endl;
                    }
                #endif

                    CheckVariableExists(rVariable, rNodes);

                    if (IsFixed) {
                        block_for_each(rNodes, [&rVariable, &rFlag, CheckValue](NodeType& rNode) {
                            if (rNode.Is(rFlag) == CheckValue) {
                                rNode.pGetDof(rVariable)->FixDof();
                            }
                        });
                    }
                    else {
                        block_for_each(rNodes, [&rVariable, &rFlag, CheckValue](NodeType& rNode) {
                            if (rNode.Is(rFlag) == CheckValue) {
                                rNode.pGetDof(rVariable)->FreeDof();
                            }
                        });
                    }
                }

                QUEST_CATCH("");
            }


            /**
             * @brief  遍历向量数据，将其值设置到节点集合中的节点
             * @details 该函数适用于标量历史变量，因为数据向量中的每个值都会设置到对应的节点。此外，这些值必须与节点的顺序一致（第 i 个值对应第 i 个节点）
             */
            template< class TVarType >
            void ApplyVector(
                const TVarType& rVar,
                const Vector& rData,
                NodesContainerType& rNodes
                )
            {
                QUEST_TRY

                if(rNodes.size() != 0 && rNodes.size() == rData.size()) {
                    CheckVariableExists(rVar, rNodes);

                    IndexPartition<std::size_t>(rNodes.size()).for_each([&](std::size_t index){
                        NodesContainerType::iterator it_node = rNodes.begin() + index;
                        it_node->FastGetSolutionStepValue(rVar) = rData[index];
                    });
                } else
                    QUEST_ERROR  << "There is a mismatch between the size of data array and the number of nodes ";

                QUEST_CATCH("")
            }


            /**
             * @brief 返回非历史向量变量的节点值求和
             */
            [[nodiscard]] Array1d<double, 3> SumNonHistoricalNodeVectorVariable(
                const ArrayVarType& rVar,
                const ModelPart& rModelPart
            ){}


            /**
             * @brief 返回非历史标量变量的节点值求和
             */
            template< class TVarType >
            [[nodiscard]] double SumNonHistoricalNodeScalarVariable(
                const TVarType& rVar,
                const ModelPart& rModelPart
                )
            {
                QUEST_TRY

                double sum_value = 0.0;

                // Getting info
                const auto& r_communicator = rModelPart.GetCommunicator();
                const auto& r_local_mesh = r_communicator.LocalMesh();
                const auto& r_nodes_array = r_local_mesh.Nodes();

                sum_value = block_for_each<SumReduction<double>>(r_nodes_array, [&](Node& rNode){
                    return rNode.GetValue(rVar);
                });

                return r_communicator.GetDataCommunicator().SumAll(sum_value);

                QUEST_CATCH("")
            }


            /**
             * @brief 此方法累加并返回变量值
             */
            template< class TDataType, class TVarType = Variable<TDataType> >
            [[nodiscard]] TDataType SumHistoricalVariable(
                const TVarType &rVariable,
                const ModelPart &rModelPart,
                const unsigned int BuffStep = 0
                )
            {
                QUEST_TRY

                const auto &r_communicator = rModelPart.GetCommunicator();

                TDataType sum_value = block_for_each<SumReduction<TDataType>>(r_communicator.LocalMesh().Nodes(),[&](Node& rNode){
                    return rNode.GetSolutionStepValue(rVariable, BuffStep);
                });

                return r_communicator.GetDataCommunicator().SumAll(sum_value);

                QUEST_CATCH("")
            }


            /**
             * @brief 返回历史向量变量的条件值求和
             */
            [[nodiscard]] Array1d<double, 3> SumConditionVectorVariable(
                const ArrayVarType& rVar,
                const ModelPart& rModelPart
            ){}


            /**
             * @brief 返回历史标量变量的条件值求和
             */
            template< class TVarType >
            [[nodiscard]] double SumConditionScalarVariable(
                const TVarType& rVar,
                const ModelPart& rModelPart
                )
            {
                QUEST_TRY

                double sum_value = 0.0;

                const auto& r_communicator = rModelPart.GetCommunicator();
                const auto& r_local_mesh = r_communicator.LocalMesh();
                const auto& r_conditions_array = r_local_mesh.Conditions();

                sum_value = block_for_each<SumReduction<double>>(r_conditions_array, [&](ConditionType& rCond){
                    return rCond.GetValue(rVar);
                });

                return r_communicator.GetDataCommunicator().SumAll(sum_value);

                QUEST_CATCH("")
            }


            /**
             * @brief 返回历史向量变量的单元值求和
             */
            Array1d<double, 3> SumElementVectorVariable(
                const ArrayVarType& rVar,
                const ModelPart& rModelPart
            );


            /**
             * @brief 返回历史标量变量的单元值求和
             */
            template< class TVarType >
            [[nodiscard]] double SumElementScalarVariable(
                const TVarType& rVar,
                const ModelPart& rModelPart
                )
            {
                QUEST_TRY

                double sum_value = 0.0;

                // Getting info
                const auto& r_communicator = rModelPart.GetCommunicator();
                const auto& r_local_mesh = r_communicator.LocalMesh();
                const auto& r_elements_array = r_local_mesh.Elements();

                sum_value = block_for_each<SumReduction<double>>(r_elements_array, [&](ElementType& rElem){
                    return rElem.GetValue(rVar);
                });

                return r_communicator.GetDataCommunicator().SumAll(sum_value);

                QUEST_CATCH("")
            }


            /**
             * @brief 将自由度（DoF）添加到模型部件中的节点，且为并行进行的
             */
            template< class TVarType >
            void AddDof(
                const TVarType& rVar,
                ModelPart& rModelPart
                )
            {
                QUEST_TRY

                if(rModelPart.NumberOfNodes() != 0)
                    QUEST_ERROR_IF_NOT(rModelPart.NodesBegin()->SolutionStepsDataHas(rVar)) << "ERROR:: Variable : " << rVar << "not included in the Solution step data ";

                rModelPart.GetNodalSolutionStepVariablesList().AddDof(&rVar);

                block_for_each(rModelPart.Nodes(),[&](Node& rNode){
                    rNode.AddDof(rVar);
                });

                QUEST_CATCH("")
            }


            /**
             * @brief 该函数将自由度（DoF）添加到模型部件中的节点（并行进行）
             */
            template< class TVarType >
            void AddDofWithReaction(
                const TVarType& rVar,
                const TVarType& rReactionVar,
                ModelPart& rModelPart
                )
            {
                QUEST_TRY

                if(rModelPart.NumberOfNodes() != 0) {
                    QUEST_ERROR_IF_NOT(rModelPart.NodesBegin()->SolutionStepsDataHas(rVar)) << "ERROR:: DoF Variable : " << rVar << "not included in the Soluttion step data ";
                    QUEST_ERROR_IF_NOT(rModelPart.NodesBegin()->SolutionStepsDataHas(rReactionVar)) << "ERROR:: Reaction Variable : " << rReactionVar << "not included in the Soluttion step data ";
                }

            #ifdef QUEST_DEBUG
                CheckVariableExists(rVar, rModelPart.Nodes());
                CheckVariableExists(rReactionVar, rModelPart.Nodes());
            #endif

                rModelPart.GetNodalSolutionStepVariablesList().AddDof(&rVar, &rReactionVar);

                block_for_each(rModelPart.Nodes(),[&](Node& rNode){
                    rNode.AddDof(rVar,rReactionVar);
                });

                QUEST_CATCH("")
            }


            /**
             * @brief 将一组自由度（DOF）添加到节点
             * @details 提供一个包含自由度变量名称的列表，使用此方法可以将这些变量作为自由度添加到给定模型部件的节点中
             */
            static void AddDofsList(
                const std::vector<std::string>& rDofsVarNamesList,
                ModelPart& rModelPart
            ){}


            /**
             * @brief 将一组自由度（DOF）添加到节点
             * @details 提供一个包含自由度和反作用力变量名称的列表，使用此方法可以将这些变量作为自由度和反作用力添加到给定模型部件的节点中
             */
            static void AddDofsWithReactionsList(
                const std::vector<std::array<std::string,2>>& rDofsAndReactionsNamesList,
                ModelPart& rModelPart
            ){}


            /**
             * @brief 检查变量键值
             */
            bool CheckVariableKeys(){}


            /**
             * @brief 将当前的节点坐标更新回初始坐标
             */
            void UpdateCurrentToInitialConfiguration(const ModelPart::NodesContainerType& rNodes){}


            /**
             * @brief 将初始节点坐标更新为当前坐标
             */
            void UpdateInitialToCurrentConfiguration(const ModelPart::NodesContainerType& rNodes){}


            /**
             * @brief 对于每个节点，此方法获取提供的变量的值，并将当前坐标更新为初始位置（X0, Y0, Z0）加上该变量的值
             */
            void UpdateCurrentPosition(
                const ModelPart::NodesContainerType& rNodes,
                const ArrayVarType& rUpdateVariable = DISPLACEMENT,
                const IndexType BufferPosition = 0
            ){}


            /**
             * @brief 此函数返回所有节点的当前坐标，以连续的向量形式。
             * 它支持1D、2D或3D的操作。
             * 对于每个节点，此方法获取提供的变量的值，并将当前坐标更新为初始位置（X0, Y0, Z0）加上该变量的值。
             * 如果 Dimension == 1，则向量的形式为 (X X X ....)
             * 如果 Dimension == 2，则向量的形式为 (X Y X Y X Y ....)
             * 如果 Dimension == 3，则向量的形式为 (X Y Z X Y Z ....)
             * @param rNodes 节点数组，从中提取坐标
             * @param Dimension 所需组件的数量
             */
            template<class TVectorType=Vector>
            [[nodiscard]] TVectorType GetCurrentPositionsVector(
                const ModelPart::NodesContainerType& rNodes,
                const unsigned int Dimension
            ){}


            /**
             * @brief 此函数返回所有节点的初始坐标，以连续的向量形式。
             * 它支持1D、2D或3D的操作。
             * 对于每个节点，此方法获取提供的变量的值，并将当前坐标更新为初始位置（X0, Y0, Z0）加上该变量的值。
             * 如果 Dimension == 1，则向量的形式为 (X0 X0 X0 ....)
             * 如果 Dimension == 2，则向量的形式为 (X0 Y0 X0 Y0 X0 Y0 ....)
             * 如果 Dimension == 3，则向量的形式为 (X0 Y0 Z0 X0 Y0 Z0 ....)
             * @param rNodes 节点数组，从中提取坐标
             * @param Dimension 所需组件的数量
             */
            template<class TVectorType=Vector>
            [[nodiscard]] TVectorType GetInitialPositionsVector(
                const ModelPart::NodesContainerType& rNodes,
                const unsigned int Dimension
            ){}


            /**
             * @brief 此函数是 GetCurrentPositionsVector 的 "set" 对应函数，允许
             * 设置所有节点的当前坐标，并假设它们存储在一个连续的 1D 向量中。
             * 它支持1D、2D或3D的操作。
             * 对于每个节点，此方法获取提供的变量的值，并将当前坐标更新为初始位置（X, Y, Z）加上该变量的值。
             * 如果 Dimension == 1，则预期的输入向量形式为 (X X X ....)
             * 如果 Dimension == 2，则预期的输入向量形式为 (X Y X Y X Y ....)
             * 如果 Dimension == 3，则预期的输入向量形式为 (X Y Z X Y Z ....)
             * @param rNodes 节点数组，从中提取坐标
             * @param rPositions 含有当前坐标的向量
             */
            void SetCurrentPositionsVector(
                ModelPart::NodesContainerType& rNodes,
                const Vector& rPositions
            ){}


            /**
             * @brief 此函数是 GetInitialPositionsVector 的 "set" 对应函数，允许
             * 设置所有节点的初始坐标，并假设它们存储在一个连续的 1D 向量中。
             * 它支持1D、2D或3D的操作。
             * 对于每个节点，此方法获取提供的变量的值，并将当前位置更新为初始位置（X0, Y0, Z0）加上该变量的值。
             * 如果 Dimension == 1，则预期的输入向量形式为 (X0 X0 X0 ....)
             * 如果 Dimension == 2，则预期的输入向量形式为 (X0 Y0 X0 Y0 X0 Y0 ....)
             * 如果 Dimension == 3，则预期的输入向量形式为 (X0 Y0 Z0 X0 Y0 Z0 ....)
             * @param rNodes 节点数组，从中提取坐标
             * @param rPositions 含有初始坐标的向量
             */
            void SetInitialPositionsVector(
                ModelPart::NodesContainerType& rNodes,
                const Vector& rPositions
            );


            /**
             * @brief 判断实体中是否存在变量的值
             */
            template <Globals::DataLocation TLocation, class TEntity, class TValue>
            static bool HasValue(const TEntity& rEntity, const Variable<TValue>& rVariable)
            {
                if constexpr (TLocation == Globals::DataLocation::NodeHistorical) {
                    static_assert(std::is_same_v<TEntity,Node>);
                    return rEntity.SolutionStepsDataHas(rVariable);
                } else {
                    static_assert(std::is_same_v<TEntity,Node>
                                || std::is_same_v<TEntity,Element>
                                || std::is_same_v<TEntity,Condition>
                                || std::is_same_v<TEntity,ProcessInfo>
                                || std::is_same_v<TEntity,ModelPart>);
                    return rEntity.Has(rVariable);
                }
            }


            /**
             * @brief 获取存储在实体中的变量值
             */
            template <Globals::DataLocation TLocation, class TEntity, class TValue>
            static std::conditional_t<std::is_arithmetic_v<TValue>,
                                    TValue,             // <== return by value if scalar type
                                    const TValue&>      // <== return by reference if non-scalar type
            GetValue(const TEntity& rEntity, const Variable<TValue>& rVariable)
            {
                if constexpr (TLocation == Globals::DataLocation::NodeHistorical) {
                    static_assert(std::is_same_v<TEntity,Node>);
                    return rEntity.FastGetSolutionStepValue(rVariable);
                } else if constexpr (TLocation == Globals::DataLocation::NodeNonHistorical) {
                    static_assert(std::is_same_v<TEntity,Node>);
                    return rEntity.GetValue(rVariable);
                } else if constexpr (TLocation == Globals::DataLocation::Element) {
                    static_assert(std::is_same_v<TEntity,Element>);
                    return rEntity.GetValue(rVariable);
                } else if constexpr (TLocation == Globals::DataLocation::Condition) {
                    static_assert(std::is_same_v<TEntity,Condition>);
                    return rEntity.GetValue(rVariable);
                } else if constexpr (TLocation == Globals::DataLocation::ProcessInfo) {
                    static_assert(std::is_same_v<TEntity,ProcessInfo>);
                    return rEntity.GetValue(rVariable);
                } else if constexpr (TLocation == Globals::DataLocation::ModelPart) {
                    static_assert(std::is_same_v<TEntity,ModelPart>);
                    return rEntity.GetValue(rVariable);
                } else {
                    static_assert(std::is_same_v<TEntity,void>, "Unsupported DataLocation");
                }
            }


            /**
             * @brief 获取存储在实体中的变量值
             */
            template <Globals::DataLocation TLocation, class TEntity, class TValue>
            static TValue& GetValue(TEntity& rEntity, const Variable<TValue>& rVariable)
            {
                if constexpr (TLocation == Globals::DataLocation::NodeHistorical) {
                    static_assert(std::is_same_v<TEntity,Node>);
                    return rEntity.FastGetSolutionStepValue(rVariable);
                } else if constexpr (TLocation == Globals::DataLocation::NodeNonHistorical) {
                    static_assert(std::is_same_v<TEntity,Node>);
                    return rEntity.GetValue(rVariable);
                } else if constexpr (TLocation == Globals::DataLocation::Element) {
                    static_assert(std::is_same_v<TEntity,Element>);
                    return rEntity.GetValue(rVariable);
                } else if constexpr (TLocation == Globals::DataLocation::Condition) {
                    static_assert(std::is_same_v<TEntity,Condition>);
                    return rEntity.GetValue(rVariable);
                } else if constexpr (TLocation == Globals::DataLocation::ProcessInfo) {
                    static_assert(std::is_same_v<TEntity,ProcessInfo>);
                    return rEntity.GetValue(rVariable);
                } else if constexpr (TLocation == Globals::DataLocation::ModelPart) {
                    static_assert(std::is_same_v<TEntity,ModelPart>);
                    return rEntity.GetValue(rVariable);
                } else {
                    static_assert(std::is_same_v<TEntity,void>, "Unsupported DataLocation");
                }
            }


            /**
             * @brief 覆盖存储在实体中的变量值
             */
            template <Globals::DataLocation TLocation, class TEntity, class TValue>
            static void SetValue(TEntity& rEntity,
                                const Variable<TValue>& rVariable,
                                std::conditional_t<std::is_arithmetic_v<TValue>,
                                                    TValue,         /*pass scalar types by value*/
                                                    const TValue&>  /*pass non-scalar types by reference*/ Value)
            {
                if constexpr (TLocation == Globals::DataLocation::NodeHistorical) {
                    static_assert(std::is_same_v<TEntity,Node>);
                    rEntity.FastGetSolutionStepValue(rVariable) = Value;
                } else if constexpr (TLocation == Globals::DataLocation::NodeNonHistorical) {
                    static_assert(std::is_same_v<TEntity,Node>);
                    rEntity.SetValue(rVariable, Value);
                } else if constexpr (TLocation == Globals::DataLocation::Element) {
                    static_assert(std::is_same_v<TEntity,Element>);
                    rEntity.SetValue(rVariable, Value);
                } else if constexpr (TLocation == Globals::DataLocation::Condition) {
                    static_assert(std::is_same_v<TEntity,Condition>);
                    rEntity.SetValue(rVariable, Value);
                } else if constexpr (TLocation == Globals::DataLocation::ProcessInfo) {
                    static_assert(std::is_same_v<TEntity,ProcessInfo>);
                    rEntity.SetValue(rVariable, Value);
                } else if constexpr (TLocation == Globals::DataLocation::ModelPart) {
                    static_assert(std::is_same_v<TEntity,ModelPart>);
                    rEntity.SetValue(rVariable, Value);
                } else {
                    static_assert(std::is_same_v<TEntity,void>, "Unsupported DataLocation");
                }
            }


            /**
             * @brief 此函数允许获取所有 rNodes 中与 rVar 对应的数据库条目，并将其展平，以便相关的分量出现在输出向量中。
             * 此版本适用于向量变量（类型为 Variable<Array1d<double,3>>）。
             * 如果 Dimension 为 1，则只获取第一个分量；如果 Dimension 为 2，则获取 x 和 y 分量；如果 Dimension 为 3，则一次获取 3 个分量。
             */
            [[nodiscard]] Vector GetSolutionStepValuesVector(
                const ModelPart::NodesContainerType& rNodes,
                const Variable<Array1d<double,3>>& rVar,
                const unsigned int Step,
                const unsigned int Dimension=3
            ){}


            /**
             * @brief 此函数允许获取所有 rNodes 中与 rVar 对应的数据库条目，并将其展平，以便相关的分量出现在输出向量中。
             * 此版本适用于标量变量（类型为 Variable<double>）。
             * 如果 Dimension 为 1，则只获取第一个分量；如果 Dimension 为 2，则获取 x 和 y 分量；如果 Dimension 为 3，则一次获取 3 个分量。
             */
            [[nodiscard]] Vector GetSolutionStepValuesVector(
                const ModelPart::NodesContainerType& rNodes,
                const Variable<double>& rVar,
                const unsigned int Step
            ){}


            /**
             * @brief 此函数允许设置所有 rNodes 中与 rVar 对应的数据库条目，
             * 给定一个展平的数组，其中包含所有变量分量并按顺序排列。
             * 此版本适用于向量变量（类型为 Variable<Array1d<double,3>>）
             */
            void SetSolutionStepValuesVector(
                ModelPart::NodesContainerType& rNodes,
                const Variable<Array1d<double,3>>& rVar,
                const Vector& rData,
                const unsigned int Step
            ){}


            /**
             * @brief 此函数允许设置所有 rNodes 中与 rVar 对应的数据库条目，
             * 给定一个展平的数组，其中包含所有变量分量并按顺序排列。
             * 此版本适用于标量变量（类型为 Variable<double>）
             */
            void SetSolutionStepValuesVector(
                ModelPart::NodesContainerType& rNodes,
                const Variable<double>& rVar,
                const Vector& rData,
                const unsigned int Step
            ){}


            /**
             * @brief 此函数允许获取所有 rNodes 中与 rVar 对应的数据库条目，
             * 这些条目会被展平，以便相关组件出现在输出向量中。
             * 此版本适用于向量变量（类型为 Variable<Array1d<double,3>>）。
             * 如果 Dimension 为 1，则只会获得第一个组件；如果 Dimension 为 2，则获得 x 和 y 组件；
             * 如果 Dimension 为 3，则同时获得三个组件。
             * 此版本访问的是非历史数据。
             */
            [[nodiscard]] Vector GetValuesVector(
                const ModelPart::NodesContainerType& rNodes,
                const Variable<Array1d<double,3>>& rVar,
                const unsigned int Dimension=3
            ){}


            /**
             * @brief 此函数允许获取所有 rNodes 中与 rVar 对应的数据库条目，
             * 这些条目会被展平，以便相关组件出现在输出向量中。
             * 此版本适用于标量变量（类型为 Variable<double>）。
             * 如果 Dimension 为 1，则只会获得第一个组件；如果 Dimension 为 2，则获得 x 和 y 组件；
             * 如果 Dimension 为 3，则同时获得三个组件。
             * 此版本访问的是非历史数据。
             */
            [[nodiscard]] Vector GetValuesVector(
                const ModelPart::NodesContainerType& rNodes,
                const Variable<double>& rVar
            ){}


            /**
             * @brief 此函数允许设置所有 rNodes 中与 rVar 对应的数据库条目，
             * 通过给定一个展平的数组，其中所有变量组件依次排列。
             * 此版本适用于向量变量（类型为 Variable<Array1d<double,3>>）。
             * 此版本访问的是非历史数据
             */
            void SetValuesVector(
                ModelPart::NodesContainerType& rNodes,
                const Variable<Array1d<double,3>>& rVar,
                const Vector& rData
            ){}


            /**
             * @brief 此函数允许设置所有 rNodes 中与 rVar 对应的数据库条目，
             * 通过给定一个展平的数组，其中所有变量组件依次排列。
             * 此版本适用于标量变量（类型为 Variable<double>）。
             * 此版本访问的是非历史数据
             */
            void SetValuesVector(
                ModelPart::NodesContainerType& rNodes,
                const Variable<double>& rVar,
                const Vector& rData
            ){}

        protected:

        private:
            /**
             * @brief 这是一个辅助方法，用于检查键值
             */
            template< class TVarType >
            bool CheckVariableKeysHelper()
            {
                QUEST_TRY

                for (const auto& var : QuestComponents< TVarType >::GetComponents()) {
                    if (var.first == "NONE" || var.first == "")
                        std::cout << " var first is NONE or empty " << var.first << var.second << std::endl;
                    if (var.second->Name() == "NONE" || var.second->Name() == "")
                        std::cout << var.first << var.second << std::endl;
                    if (var.first != var.second->Name()) 
                        std::cout << "Registration Name = " << var.first << " Variable Name = " << std::endl;
                }

                return true;
                QUEST_CATCH("")
            }


            /**
             * @brief 返回模型部件的容器
             */
            template <class TContainerType>
            [[nodiscard]] TContainerType& GetContainer(ModelPart& rModelPart){}


            /**
             * @brief 返回模型部件的容器
             */
            template <class TContainerType>
            [[nodiscard]] const TContainerType& GetContainer(const ModelPart& rModelPart){}


            /**
             * @brief 将给定的历史变量的值设置到指定的节点上
             */
            template<class TDataType>
            static void AuxiliaryHistoricalValueSetter(
                const Variable<TDataType>& rVariable,
                const TDataType& rValue,
                NodeType& rNode
            ){}


            /**
             * @brief 从源 ModelPart 中的容器复制符合特定标志（rFlag）条件的变量到目标 ModelPart 的相应容器中
             */
            template <class TContainerType, class TSetterFunction, class TGetterFunction>
            void CopyModelPartFlaggedVariable(
                const ModelPart& rOriginModelPart,
                ModelPart& rDestinationModelPart,
                const Flags& rFlag,
                const bool CheckValue,
                TSetterFunction&& rSetterFunction,
                TGetterFunction&& rGetterFunction)
            {
                QUEST_TRY

                const auto& r_origin_container = GetContainer<TContainerType>(rOriginModelPart);
                auto& r_destination_container = GetContainer<TContainerType>(rDestinationModelPart);

                const int number_of_origin_items = r_origin_container.size();
                const int number_of_destination_items = r_destination_container.size();

                QUEST_ERROR_IF_NOT(number_of_origin_items == number_of_destination_items)
                    << "Origin ( " << rOriginModelPart.Name() << " ) and destination ( "
                    << rDestinationModelPart.Name() << " ) model parts have different number of items."
                    << "\n\t- Number of origin items: " << number_of_origin_items
                    << "\n\t- Number of destination items: " << number_of_destination_items
                    << std::endl;

                IndexPartition<int>(number_of_origin_items).for_each([&](int i_node) {
                    const auto& r_orig_item = *(r_origin_container.begin() + i_node);
                    auto& r_dest_item = *(r_destination_container.begin() + i_node);
                    if (r_orig_item.Is(rFlag) == CheckValue) {
                        rSetterFunction(r_dest_item, rGetterFunction(r_orig_item));
                    }
                });

                QUEST_CATCH("");
            }

        private:

    };

}


#endif //QUEST_VARIABLE_UTILS_HPP