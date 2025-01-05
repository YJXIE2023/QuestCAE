#ifndef QUEST_IO_HPP
#define QUEST_IO_HPP

// 系统头文件
#include <string>
#include <iostream>
#include <unordered_set>

// 项目头文件
#include "includes/model_part.hpp"

namespace Quest{

    /**
     * @brief 提供了不同实现的输入输出过程，可以用于以不同的格式和特性进行读写操作
     */
    class QUEST_API(QUEST_CORE) IO{
        public:
            QUEST_CLASS_POINTER_DEFINITION(IO);

            QUEST_DEFINE_LOCAL_FLAG( READ );
            QUEST_DEFINE_LOCAL_FLAG( WRITE );
            QUEST_DEFINE_LOCAL_FLAG( APPEND );
            QUEST_DEFINE_LOCAL_FLAG( IGNORE_VARIABLES_ERROR );
            QUEST_DEFINE_LOCAL_FLAG( SKIP_TIMER );
            QUEST_DEFINE_LOCAL_FLAG( MESH_ONLY );
            QUEST_DEFINE_LOCAL_FLAG( SCIENTIFIC_PRECISION );

            using NodeType = Node;
            using GeometryType = Geometry<NodeType>;
            using MeshType = Mesh<NodeType, Properties, Element, Condition>;
            using NodesContainerType = MeshType::NodesContainerType;
            using PropertiesContainerType = MeshType::PropertiesContainerType;
            using GeometryContainerType = ModelPart::GeometryContainerType;
            using ElementsContainerType = MeshType::ElementsContainerType;
            using ConditionsContainerType = MeshType::ConditionsContainerType;
            using ConnectivitiesContainerType = std::vector<std::vector<std::size_t>>;
            using PartitionIndicesContainerType = std::vector<std::vector<std::size_t>>;
            using PartitionIndicesType = std::vector<std::size_t>;
            using SizeType = std::size_t;
            using GraphType = DenseMatrix<int>;


            /**
             * @brief 辅助结构体，包含有关 ModelPart 中实体分区的信息
             */
            struct PartitioningInfo
            {
                GraphType Graph;                                         // 存储与模型分区相关的图结构
                PartitionIndicesType NodesPartitions;                    // 记录每个节点属于哪个分区的索引信息
                PartitionIndicesType ElementsPartitions;                 // 记录每个元素属于哪个分区的索引信息
                PartitionIndicesType ConditionsPartitions;               // 记录每个条件属于哪个分区的索引信息
                PartitionIndicesContainerType NodesAllPartitions;        // 存储每个节点所在的所有分区（包括本地分区和幽灵分区）
                PartitionIndicesContainerType ElementsAllPartitions;     // 存储每个元素所在的所有分区（包括本地分区和幽灵分区）
                PartitionIndicesContainerType ConditionsAllPartitions;   // 存储每个条件所在的所有分区（包括本地分区和幽灵分区）
            };


        public:

        protected:

        private:

        private:

    };

}

#endif //QUEST_IO_HPP