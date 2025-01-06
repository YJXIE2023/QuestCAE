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
            /**
             * @brief 构造函数
             */
            IO() = default;


            /**
             * @brief 析构函数
             */
            virtual ~IO() = default;


            /**
             * @brief 复制构造函数
             */
            IO(const IO& rOther) = delete;


            /**
             * @brief 赋值运算符重载
             */
            IO& operator = (const IO& rOther) = delete;


            /**
             * @brief 读取一个节点
             */
            virtual bool ReadNode(NodeType& rThisNode)
            {
                QUEST_ERROR << "Calling base class method (ReadNode). Please check the definition of derived class." << std::endl;
            }


            /**
             * @brief 读取一组节点
             */
            virtual bool ReadNodes(NodesContainerType& rThisNodes)
            {
                QUEST_ERROR << "Calling base class method (ReadNodes). Please check the definition of derived class" << std::endl;
            }


            /**
             * @brief 读取节点数量
             */
            virtual std::size_t ReadNodesNumber()
            {
                QUEST_ERROR << "Calling base class method (ReadNodesNumber). Please check the definition of derived class." << std::endl;;
            }


            /**
             * @brief 将一组节点写入
             */
            virtual void WriteNodes(NodesContainerType const& rThisNodes)
            {
                QUEST_ERROR << "Calling base class method (WriteNodes). Please check the definition of derived class" << std::endl;
            }


            /**
             * @brief 读取一个属性对象
             */
            virtual void ReadProperties(Properties& rThisProperties)
            {
                QUEST_ERROR << "Calling base class method (ReadProperties). Please check the definition of derived class" << std::endl;
            }


            /**
             * @brief 读取一组属性对象
             */
            virtual void ReadProperties(PropertiesContainerType& rThisProperties)
            {
                QUEST_ERROR << "Calling base class method (ReadProperties). Please check the definition of derived class" << std::endl;
            }


            /**
             * @brief 写入一个属性对象
             */
            virtual void WriteProperties(Properties const& rThisProperties)
            {
                QUEST_ERROR << "Calling base class method (WriteProperties). Please check the definition of derived class" << std::endl;
            }


            /**
             * @brief 写入一组属性对象
             */
            virtual void WriteProperties(PropertiesContainerType const& rThisProperties)
            {
                QUEST_ERROR << "Calling base class method (WriteProperties). Please check the definition of derived class" << std::endl;
            }


            /**
             * @brief 读取一个几何对象
             */
            virtual void ReadGeometry(NodesContainerType& rThisNodes,GeometryType::Pointer& pThisGeometry)
            {
                QUEST_ERROR << "Calling base class method (ReadGeometry). Please check the definition of derived class" << std::endl;
            }


            /**
             * @brief 读取一组几何对象
             */
            virtual void ReadGeometries(NodesContainerType& rThisNodes,GeometryContainerType& rThisGeometries)
            {
                QUEST_ERROR << "Calling base class method (ReadGeometries). Please check the definition of derived class" << std::endl;
            }


            /**
             * @brief 读取几何体的连接信息
             */
            virtual std::size_t ReadGeometriesConnectivities(ConnectivitiesContainerType& rGeometriesConnectivities)
            {
                QUEST_ERROR << "Calling base class method (ReadGeometriesConnectivities). Please check the definition of derived class" << std::endl;
            }


            /**
             * @brief 写入一组几何对象
             */
            virtual void WriteGeometries(GeometryContainerType const& rThisGeometries)
            {
                QUEST_ERROR << "Calling base class method (WriteGeometries). Please check the definition of derived class" << std::endl;
            }


            /**
             * @brief 读取一个单元
             */
            virtual void ReadElement(
                NodesContainerType& rThisNodes,
                PropertiesContainerType& rThisProperties,
                Element::Pointer& pThisElement
            ){
                QUEST_ERROR << "Calling base class method (ReadElement). Please check the definition of derived class" << std::endl;
            }


            /**
             * @brief 读取一组元素
             */
            virtual void ReadElements(
                NodesContainerType& rThisNodes,
                PropertiesContainerType& rThisProperties,
                ElementsContainerType& rThisElements
            ){
                QUEST_ERROR << "Calling base class method (ReadElements). Please check the definition of derived class" << std::endl;
            }


            /**
             * @brief 读取单元的连接信息
             */
            virtual std::size_t ReadElementsConnectivities(ConnectivitiesContainerType& rElementsConnectivities)
            {
                QUEST_ERROR << "Calling base class method (ReadElementsConnectivities). Please check the definition of derived class" << std::endl;
            }


            /**
             * @brief 写入一组单元
             */
            virtual void WriteElements(ElementsContainerType const& rThisElements)
            {
                QUEST_ERROR << "Calling base class method (WriteElements). Please check the definition of derived class" << std::endl;
            }


            /**
             * @brief 读取一个条件
             */
            virtual void ReadCondition(
                NodesContainerType& rThisNodes,
                PropertiesContainerType& rThisProperties,
                Condition::Pointer& pThisCondition
            ){
                QUEST_ERROR << "Calling base class method (ReadCondition). Please check the definition of derived class" << std::endl;
            }


            /**
             * @brief 读取一组条件
             */
            virtual void ReadConditions(
                NodesContainerType& rThisNodes,
                PropertiesContainerType& rThisProperties,
                ConditionsContainerType& rThisConditions
            ){
                QUEST_ERROR << "Calling base class method (ReadConditions). Please check the definition of derived class" << std::endl;
            }


            /**
             * @brief 读取条件的关联关系
             */
            virtual std::size_t ReadConditionsConnectivities(ConnectivitiesContainerType& rConditionsConnectivities)
            {
                QUEST_ERROR << "Calling base class method (ReadConditionsConnectivities). Please check the definition of derived class" << std::endl;
            }


            /**
             * @brief 写入一组条件
             */
            virtual void WriteConditions(ConditionsContainerType const& rThisConditions)
            {
                QUEST_ERROR << "Calling base class method (WriteConditions). Please check the definition of derived class" << std::endl;
            }


            /**
             * @brief 读取模型部件的初始值
             */
            virtual void ReadInitialValues(ModelPart& rThisModelPart)
            {
                QUEST_ERROR << "Calling base class method (ReadInitialValues). Please check the definition of derived class" << std::endl;
            }


            /**
             * @brief 读取节点、单元和条件的初始值
             */
            virtual void ReadInitialValues(NodesContainerType& rThisNodes, ElementsContainerType& rThisElements, ConditionsContainerType& rThisConditions)
            {
                QUEST_ERROR << "Calling base class method (ReadInitialValues). Please check the definition of derived class" << std::endl;
            }


            /**
             * @brief 读取一个网格对象
             */
            virtual void ReadMesh(MeshType & rThisMesh)
            {
                QUEST_ERROR << "Calling base class method (ReadMesh). Please check the definition of derived class" << std::endl;
            }


            /**
             * @brief 写入一个网格对象
             */
            virtual void WriteMesh(const MeshType& rThisMesh )
            {
                MeshType& non_const_mesh = const_cast<MeshType&>(rThisMesh);
                QUEST_START_IGNORING_DEPRECATED_FUNCTION_WARNING
                this->WriteMesh(non_const_mesh);
                QUEST_STOP_IGNORING_DEPRECATED_FUNCTION_WARNING
            }


            /**
             * @brief 读取模型对象
             */
            virtual void ReadModelPart(ModelPart & rThisModelPart)
            {
                QUEST_ERROR << "Calling base class method (ReadModelPart). Please check the definition of derived class" << std::endl;
            }


            /**
             * @brief 写入模型对象
             */
            virtual void WriteModelPart(const ModelPart& rThisModelPart)
            {
                ModelPart& non_const_model_part = const_cast<ModelPart&>(rThisModelPart);
                QUEST_START_IGNORING_DEPRECATED_FUNCTION_WARNING
                this->WriteModelPart(non_const_model_part);
                QUEST_STOP_IGNORING_DEPRECATED_FUNCTION_WARNING
            }


            /**
             * @brief 写入节点网格
             */
            virtual void WriteNodeMesh(const MeshType& rThisMesh )
            {
                MeshType& non_const_mesh = const_cast<MeshType&>(rThisMesh);
                QUEST_START_IGNORING_DEPRECATED_FUNCTION_WARNING
                this->WriteNodeMesh(non_const_mesh);
                QUEST_STOP_IGNORING_DEPRECATED_FUNCTION_WARNING
            }


            /**
             * @brief 读取节点图
             */
            virtual std::size_t ReadNodalGraph(ConnectivitiesContainerType& rAuxConnectivities)
            {
                QUEST_ERROR << "Calling base class method (ReadNodalGraph). Please check the definition of derived class" << std::endl;;
            }


            /**
             * @brief 将模型部件分区
             * @param NumberOfPartitions 要分区的数量
             * @param PartitioningType 要分区的类型
             */
            virtual void DivideInputToPartitions(
                SizeType NumberOfPartitions,
                const PartitioningInfo& rPartitioningInfo)
            {
                QUEST_START_IGNORING_DEPRECATED_FUNCTION_WARNING
                DivideInputToPartitions(NumberOfPartitions, rPartitioningInfo.Graph, rPartitioningInfo.NodesPartitions, rPartitioningInfo.ElementsPartitions, rPartitioningInfo.ConditionsPartitions, rPartitioningInfo.NodesAllPartitions, rPartitioningInfo.ElementsAllPartitions, rPartitioningInfo.ConditionsAllPartitions); // for backward compatibility
                QUEST_STOP_IGNORING_DEPRECATED_FUNCTION_WARNING
            }


            /**
             * @brief 将模型部件分区
             */
            QUEST_DEPRECATED_MESSAGE("'This version of \"DivideInputToPartitions\" is deprecated, please use the interface that accepts a \"PartitioningInfo\"")
            virtual void DivideInputToPartitions(
                SizeType NumberOfPartitions,
                GraphType const& rDomainsColoredGraph,
                PartitionIndicesType const& rNodesPartitions,
                PartitionIndicesType const& rElementsPartitions,
                PartitionIndicesType const& rConditionsPartitions,
                PartitionIndicesContainerType const& rNodesAllPartitions,
                PartitionIndicesContainerType const& rElementsAllPartitions,
                PartitionIndicesContainerType const& rConditionsAllPartitions)
            {
                QUEST_ERROR << "Calling base class method (DivideInputToPartitions). Please check the definition of derived class" << std::endl;
            }


            /**
             * @brief 将模型部件分区
             */
            virtual void DivideInputToPartitions(
                Quest::shared_ptr<std::iostream> * pStreams,
                SizeType NumberOfPartitions,
                const PartitioningInfo& rPartitioningInfo)
            {
                QUEST_START_IGNORING_DEPRECATED_FUNCTION_WARNING
                DivideInputToPartitions(pStreams, NumberOfPartitions, rPartitioningInfo.Graph, rPartitioningInfo.NodesPartitions, rPartitioningInfo.ElementsPartitions, rPartitioningInfo.ConditionsPartitions, rPartitioningInfo.NodesAllPartitions, rPartitioningInfo.ElementsAllPartitions, rPartitioningInfo.ConditionsAllPartitions); // for backward compatibility
                QUEST_STOP_IGNORING_DEPRECATED_FUNCTION_WARNING
            }


            /**
             * @brief 将模型部件分区
             */
            QUEST_DEPRECATED_MESSAGE("'This version of \"DivideInputToPartitions\" is deprecated, please use the interface that accepts a \"PartitioningInfo\"")
            virtual void DivideInputToPartitions(
                Quest::shared_ptr<std::iostream> * pStreams,
                SizeType NumberOfPartitions,
                GraphType const& rDomainsColoredGraph,
                PartitionIndicesType const& rNodesPartitions,
                PartitionIndicesType const& rElementsPartitions,
                PartitionIndicesType const& rConditionsPartitions,
                PartitionIndicesContainerType const& rNodesAllPartitions,
                PartitionIndicesContainerType const& rElementsAllPartitions,
                PartitionIndicesContainerType const& rConditionsAllPartitions)
            {
                QUEST_ERROR << "Calling base class method (DivideInputToPartitions). Please check the definition of derived class" << std::endl;
            }

            /**
             * @brief 读取子模型部件的单元和条件Id
             */
            virtual void ReadSubModelPartElementsAndConditionsIds(
                std::string const& rModelPartName,
                std::unordered_set<SizeType> &rElementsIds,
                std::unordered_set<SizeType> &rConditionsIds
            ){
                QUEST_ERROR << "Calling base class method (ReadSubModelPartElementsAndConditionsIds). Please check the definition of derived class" << std::endl;
            }


            /**
             * @brief 读取节点图并从给定的实体列表中提取必要的连接信息
             */
            virtual std::size_t ReadNodalGraphFromEntitiesList(
                ConnectivitiesContainerType& rAuxConnectivities,
                std::unordered_set<SizeType> &rElementsIds,
                std::unordered_set<SizeType> &rConditionsIds
            ){
                QUEST_ERROR << "Calling base class method (ReadNodalGraphFromEntitiesList). Please check the definition of derived class" << std::endl;
            }


            virtual std::string Info() const
            {
                return "IO";
            }


            virtual void PrintInfo(std::ostream& rOStream) const
            {
                rOStream << "IO";
            }


            virtual void PrintData(std::ostream& rOStream) const{}

        protected:

        private:

        private:

    };

    inline std::istream& operator >> (std::istream& rIstream, IO& rThis);


    inline std::ostream& operator << (std::ostream& rOstream, const IO& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }

}

#endif //QUEST_IO_HPP