#ifndef QUEST_MODEL_PART_IO_HPP
#define QUEST_MODEL_PART_IO_HPP

// 系统头文件
#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_set>

// 项目头文件
#include "includes/define.hpp"
#include "includes/io.hpp"
#include "container/flags.hpp"

namespace Quest{

    class QUEST_API(QUEST_CORE) ModelPartIO : public IO{
        public:
            QUEST_CLASS_POINTER_DEFINITION(ModelPartIO);

            using BaseType = IO;
            using NodeType = BaseType::NodeType;
            using MeshType = BaseType::MeshType;
            using NodesContainerType = BaseType::NodesContainerType;
            using PropertiesContainerType = BaseType::PropertiesContainerType;
            using ElementsContainerType = BaseType::ElementsContainerType;
            using ConditionsContainerType = BaseType::ConditionsContainerType;
            using ConnectivitiesContainerType = BaseType::ConnectivitiesContainerType;
            using OutputFilesContainerType = std::vector<std::ofstream>;
            using SizeType = std::size_t;

            using BaseType::WriteProperties;

        public:
            /**
             * @Brief 构造函数
             */
            ModelPartIO(
                const std::filesystem::path& Filename,
                const Flags Options = IO::READ | IO::IGNORE_VARIABLES_ERROR.AsFalse() | IO::SKIP_TIMER
            );


            /**
             * @brief 构造函数
             */
            ModelPartIO(
                Quest::shared_ptr<std::iostream> Stream,
                const Flags Options = IO::IGNORE_VARIABLES_ERROR.AsFalse() | IO::SKIP_TIMER
            );


            /**
             * @brief 析构函数
             */
            ~ModelPartIO() override;


            /**
             * @brief 读取一个节点
             */
            bool ReadNode(NodeType& rThisNode) override;


            /**
             * @brief 读取一组节点
             */
            bool ReadNodes(NodesContainerType& rThisNodes) override;


            /**
             * @brief 读取节点数量
             */
            std::size_t ReadNodesNumber() override;


            /**
             * @brief 写入一组节点
             */
            void WriteNodes(NodesContainerType const& rThisNodes) override;


            /**
             * @brief 读取一个属性
             */
            void ReadProperties(Properties& rThisProperties) override;


            /**
             * @brief 读取一组属性
             */
            void ReadProperties(PropertiesContainerType& rThisProperties) override;


            /**
             * @brief 写入一个属性
             */
            void WriteProperties(PropertiesContainerType const& rThisProperties) override;


            /**
             * @brief 读取一个几何对象
             */
            void ReadGeometry(
                NodesContainerType& rThisNodes,
                GeometryType::Pointer& pThisGeometry
            ) override;


            /**
             * @brief 读取一组几何对象
             */
            void ReadGeometries(
                NodesContainerType& rThisNodes,
                GeometryContainerType& rThisGeometries
            ) override;


            /**
             * @brief 读取几何体的相关关系
             */
            std::size_t ReadGeometriesConnectivities(ConnectivitiesContainerType& rGeometriesConnectivities) override;


            /**
             * @brief 写入一组几何对象
             */
            void WriteGeometries(GeometryContainerType const& rThisGeometries) override;


            /**
             * @brief 读取一个单元对象
             */
            void ReadElement(
                NodesContainerType& rThisNodes,
                PropertiesContainerType& rThisProperties,
                Element::Pointer& pThisElement
            ) override;


            /**
             * @brief 读取一组单元对象
             */
            void ReadElements(
                NodesContainerType& rThisNodes,
                PropertiesContainerType& rThisProperties,
                ElementsContainerType& rThisElements
            ) override;


            /**
             * @brief 读取单元关联关系
             */
            std::size_t  ReadElementsConnectivities(ConnectivitiesContainerType& rElementsConnectivities) override;


            /**
             * @brief 写入一组单元
             */
            void WriteElements(ElementsContainerType const& rThisElements) override;


            /**
             * @brief 读取一组条件
             */
            void ReadConditions(
                NodesContainerType& rThisNodes,
                PropertiesContainerType& rThisProperties,
                ConditionsContainerType& rThisConditions
            ) override;


            /**
             * @brief 读取条件关联关系
             */
            std::size_t ReadConditionsConnectivities(ConnectivitiesContainerType& rConditionsConnectivities) override;


            /**
             * @brief 写入一组条件
             */
            void WriteConditions(ConditionsContainerType const& rThisConditions) override;


            /**
             * @brief 读取模型部件初始值
             */
            void ReadInitialValues(ModelPart& rThisModelPart) override;


            /**
             * @brief 读取一个网格
             */
            void ReadMesh(MeshType & rThisMesh) override;


            /**
             * @brief 写入一个网格
             */
            void WriteMesh(const MeshType & rThisMesh) override;


            /**
             * @brief 读取模型部件
             */
            void ReadModelPart(ModelPart & rThisModelPart) override;


            /**
             * @brief 写入模型部件
             */
            void WriteModelPart(const ModelPart & rThisModelPart) override;


            /**
             * @brief 读取节点图
             */
            std::size_t ReadNodalGraph(ConnectivitiesContainerType& rAuxConnectivities) override;


            /**
             * @brief 将模型部件分区
             */
            void DivideInputToPartitions(
                SizeType NumberOfPartitions,
                const PartitioningInfo& rPartitioningInfo
            ) override;


            /**
             * @Brief 将模型部件分区
             */
            void DivideInputToPartitions(
                Quest::shared_ptr<std::iostream> * pStreams,
                SizeType NumberOfPartitions,
                const PartitioningInfo& rPartitioningInfo
            ) override;


            /**
             * @brief 交换流
             */
            void SwapStreamSource(Quest::shared_ptr<std::iostream> newStream);


            /**
             * @brief 读取子模型部件单元和条件ID
             */
            void ReadSubModelPartElementsAndConditionsIds(
                std::string const& rModelPartName,
                std::unordered_set<SizeType> &rElementsIds,
                std::unordered_set<SizeType> &rConditionsIds
            ) override;


            /**
             * @brief 读取节点图并从给定的实体列表中提取必要的连接信息
             */
            std::size_t ReadNodalGraphFromEntitiesList(
                ConnectivitiesContainerType& rAuxConnectivities,
                std::unordered_set<SizeType> &rElementsIds,
                std::unordered_set<SizeType> &rConditionsIds
            ) override;


            std::string Info() const override
            {
                return "ModelPartIO";
            }


            void PrintInfo(std::ostream& rOStream) const override
            {
                rOStream << "ModelPartIO";
            }


            void PrintData(std::ostream& rOStream) const override{}

        protected:
            /**
             * @brief 重排节点ID
             */
            virtual ModelPartIO::SizeType ReorderedNodeId(ModelPartIO::SizeType NodeId);

            /**
             * @brief 重排几何体ID
             */
            virtual ModelPartIO::SizeType ReorderedGeometryId(ModelPartIO::SizeType GeometryId);

            /**
             * @brief 重排单元ID
             */
            virtual ModelPartIO::SizeType ReorderedElementId(ModelPartIO::SizeType ElementId);

            /**
             * @brief 重排条件ID
             */
            virtual ModelPartIO::SizeType ReorderedConditionId(ModelPartIO::SizeType ConditionId);

        private:
            /**
             * @brief 读取块的名称
             */
            std::string& ReadBlockName(std::string& rBlockName);

            /**
             * @brief 跳过指定名称的块（Block）中的数据
             */
            void SkipBlock(std::string const& BlockName);

            /**
             * @brief 检查当前数据是否已达到指定块（Block）的结尾
             */
            bool CheckEndBlock(std::string const& BlockName, std::string& rWord);

            /**
             * @brief 读取一个模型部件（ModelPart）的数据块
             */
            void ReadModelPartDataBlock(ModelPart& rModelPart, const bool is_submodelpart=false);

            /**
             * @brief 写入一个模型部件（ModelPart）的数据块
             */
            void WriteModelPartDataBlock(ModelPart& rModelPart, const bool is_submodelpart=false);

            /**
             * @brief 读取一个包含表格数据的块（TableBlock）
             */
            template<class TablesContainerType>
            void ReadTableBlock(TablesContainerType& rTables);

            /**
             * @brief 读取一组表格
             */
            void ReadTableBlock(ModelPart::TablesContainerType& rTables);

            /**
             * @brief 将表格数据写入文件或数据流中的块（TableBlock）
             */
            template<class TablesContainerType>
            void WriteTableBlock(TablesContainerType& rTables);

            /**
             * @brief 将表格数据写入表格容器中
             */
            void WriteTableBlock(ModelPart::TablesContainerType& rTables);

            /**
             * @brief 读取包含节点数据的块（NodesBlock）
             */
            void ReadNodesBlock(NodesContainerType& rThisNodes);

            /**
             * @brief 读取包含节点数据的块，专门针对 ModelPart 对象
             */
            void ReadNodesBlock(ModelPart& rModelPart);

            /**
             * @brief 计算当前块（NodesBlock）中包含的节点数量
             */
            std::size_t CountNodesInBlock();

            /**
             * @brief 读取包含属性数据的块
             */
            void ReadPropertiesBlock(PropertiesContainerType& rThisProperties);

            /**
             * @brief 读取包含属性数据的块
             */
            void ReadGeometriesBlock(ModelPart& rModelPart);

            /**
             * @brief 读取几何体数据的块，存储到 NodesContainerType 和 GeometryContainerType 中
             */
            void ReadGeometriesBlock(NodesContainerType& rThisNodes, GeometryContainerType& rThisGeometries);

            /**
             * @brief 读取包含元素数据的块
             */
            void ReadElementsBlock(ModelPart& rModelPart);

            /**
             * @brief 读取包含元素数据的块，并将节点、属性和元素数据分别存储到各自的容器中
             */
            void ReadElementsBlock(NodesContainerType& rThisNodes, PropertiesContainerType& rThisProperties, ElementsContainerType& rThisElements);

            /**
             * @brief 读取包含条件数据的块
             */
            void ReadConditionsBlock(ModelPart& rModelPart);

            /**
             * @brief 读取条件数据的块，并将节点、属性和条件数据分别存储到容器中
             */
            void ReadConditionsBlock(NodesContainerType& rThisNodes, PropertiesContainerType& rThisProperties, ConditionsContainerType& rThisConditions);

            /**
             * @brief 读取节点数据的块
             */
            void ReadNodalDataBlock(ModelPart& rThisModelPart);

            /**
             * @brief 将节点数据写入文件或数据流中的块
             */
            void WriteNodalDataBlock(ModelPart& rThisModelPart);

            /**
             * @brief 读取节点的自由度（DOF）变量数据
             */
            template<class TVariableType>
            void ReadNodalDofVariableData(NodesContainerType& rThisNodes, const TVariableType& rVariable);

            /**
             * @brief 读取节点标志数据
             */
            void ReadNodalFlags(NodesContainerType& rThisNodes, Flags const& rFlags);

            /**
             * @brief 读取节点的标量变量数据
             */
            template<class TVariableType>
            void ReadNodalScalarVariableData(NodesContainerType& rThisNodes, const TVariableType& rVariable);

            /**
             * @brief 读取节点的矢量变量数据
             */
            template<class TVariableType, class TDataType>
            void ReadNodalVectorialVariableData(NodesContainerType& rThisNodes, const TVariableType& rVariable, TDataType Dummy);

            /**
             * @brief 读取包含单元数据的块
             */
            void ReadElementalDataBlock(ElementsContainerType& rThisElements);

            /**
             * @brief 将对象容器的数据写入数据块
             */
            template<class TObjectsContainerType>
            void WriteDataBlock(const TObjectsContainerType& rThisObjectContainer, const std::string& rObjectName);
            
            /**
             * @brief 将指定变量的数据写入对象容器
             */
            template<class TVariableType, class TObjectsContainerType>
            void WriteDataBlock(const TObjectsContainerType& rThisObjectContainer,const VariableData* rVariable, const std::string& rObjectName);

            /**
             * @brief 读取单元的标量变量数据
             */
            template<class TVariableType>
            void ReadElementalScalarVariableData(ElementsContainerType& rThisElements, const TVariableType& rVariable);

            /**
             * @brief 读取单元的矢量变量数据
             */
            template<class TVariableType, class TDataType>
            void ReadElementalVectorialVariableData(ElementsContainerType& rThisElements, const TVariableType& rVariable, TDataType Dummy);
            
            /**
             * @brief 读取包含条件数据的块
             */
            void ReadConditionalDataBlock(ConditionsContainerType& rThisConditions);

            /**
             * @brief 读取条件的标量变量数据
             */
            template<class TVariableType>
            void ReadConditionalScalarVariableData(ConditionsContainerType& rThisConditions, const TVariableType& rVariable);

            /**
             * @brief 读取条件的矢量变量数据
             */
            template<class TVariableType, class TDataType>
            void ReadConditionalVectorialVariableData(ConditionsContainerType& rThisConditions, const TVariableType& rVariable, TDataType Dummy);

            /**
             * @brief 读取单元的连接性数据块
             */
            SizeType ReadElementsConnectivitiesBlock(ConnectivitiesContainerType& rThisConnectivities);

            /**
             * @brief 读取条件的连接性数据块
             */
            SizeType ReadConditionsConnectivitiesBlock(ConnectivitiesContainerType& rThisConnectivities);

            /**
             * @brief 通过几何体块填充节点的连接性数据
             */
            void FillNodalConnectivitiesFromGeometryBlock(ConnectivitiesContainerType& rNodalConnectivities);

            /**
             * @brief 通过单元块填充节点的连接性数据
             */
            void FillNodalConnectivitiesFromElementBlock(ConnectivitiesContainerType& rNodalConnectivities);

            /**
             * @brief 通过条件块填充节点的连接性数据
             */
            void FillNodalConnectivitiesFromConditionBlock(ConnectivitiesContainerType& rNodalConnectivities);

            /**
             * @brief 通过几何体块中的几何体 ID 列表填充节点连接性数据
             */
            void FillNodalConnectivitiesFromGeometryBlockInList(
                ConnectivitiesContainerType& rNodalConnectivities,
                std::unordered_set<SizeType>& rGeometriesIds
            );

            /**
             * @brief 通过单元块中的单元 ID 列表填充节点连接性数据
             */
            void FillNodalConnectivitiesFromElementBlockInList(
                ConnectivitiesContainerType& rNodalConnectivities,
                std::unordered_set<SizeType>& rElementsIds
            );

            /**
             * @brief 通过条件块中的条件 ID 列表填充节点连接性数据
             */
            void FillNodalConnectivitiesFromConditionBlockInList(
                ConnectivitiesContainerType& rNodalConnectivities,
                std::unordered_set<SizeType>& rConditionsIds
            );

            /**
             * @brief 读取通信器的相关数据块
             */
            void ReadCommunicatorDataBlock(Communicator& rThisCommunicator, NodesContainerType& rThisNodes);

            /**
             * @brief 读取本地节点的通信数据块
             */
            void ReadCommunicatorLocalNodesBlock(Communicator& rThisCommunicator, NodesContainerType& rThisNodes);

            /**
             * @brief 读取幽灵节点（ghost nodes）的通信数据块
             */
            void ReadCommunicatorGhostNodesBlock(Communicator& rThisCommunicator, NodesContainerType& rThisNodes);

            /**
             * @brief 读取网格块
             */
            void ReadMeshBlock(ModelPart& rModelPart);

            /**
             * @brief 写入网格块
             */
            void WriteMeshBlock(ModelPart& rModelPart);

            /**
             * @brief 读取网格数据块
             */
            void ReadMeshDataBlock(MeshType& rMesh);

            /**
             * @brief 读取网格节点块
             */
            void ReadMeshNodesBlock(ModelPart& rModelPart, MeshType& rMesh);

            /**
             * @brief 读取网格单元块
             */
            void ReadMeshElementsBlock(ModelPart& rModelPart, MeshType& rMesh);

            /**
             * @brief 读取网格条件块
             */
            void ReadMeshConditionsBlock(ModelPart& rModelPart, MeshType& rMesh);

            /**
             * @brief 读取网格属性块
             */
            void ReadMeshPropertiesBlock(ModelPart& rModelPart, MeshType& rMesh);

            /**
             * @brief 读取子模型部件块
             */
            void ReadSubModelPartBlock(ModelPart& rMainModelPart, ModelPart& rParentModelPart);

            /**
             * @brief 写入子模型部件块
             */
            void WriteSubModelPartBlock(ModelPart& rMainModelPart, const std::string& InitialTabulation);

            /**
             * @brief 读取子模型部件的数据块
             */
            void ReadSubModelPartDataBlock(ModelPart& rModelPart);

            /**
             * @brief 读取子模型部件的表格数据块
             */
            void ReadSubModelPartTablesBlock(ModelPart& rMainModelPart, ModelPart& rSubModelPart);

            /**
             * @brief 读取子模型部件的属性数据块
             */
            void ReadSubModelPartPropertiesBlock(ModelPart& rMainModelPart, ModelPart& rSubModelPart);

            /**
             * @brief 读取子模型部件的节点块
             */
            void ReadSubModelPartNodesBlock(ModelPart& rMainModelPart, ModelPart& rSubModelPart);

            /**
             * @brief 读取子模型部件的单元块
             */
            void ReadSubModelPartElementsBlock(ModelPart& rMainModelPart, ModelPart& rSubModelPart);

            /**
             * @brief 读取子模型部件的条件块
             */
            void ReadSubModelPartConditionsBlock(ModelPart& rMainModelPart, ModelPart& rSubModelPart);

            /**
             * @brief 读取子模型部件的几何体块
             */
            void ReadSubModelPartGeometriesBlock(
                ModelPart &rMainModelPart,
                ModelPart &rSubModelPart
            );

            /**
             * @brief 根据分区信息将输入数据分割成多个部分
             */
            void DivideInputToPartitionsImpl(
                OutputFilesContainerType& rOutputFiles,
                SizeType NumberOfPartitions,
                const PartitioningInfo& rPartitioningInfo
            );

            /**
             * @brief 分割模型部件的数据块
             */
            void DivideModelPartDataBlock(OutputFilesContainerType& OutputFiles);

            /**
             * @brief 分割表格块
             */
            void DivideTableBlock(OutputFilesContainerType& OutputFiles);

            /**
             * @brief 分割属性块
             */
            void DividePropertiesBlock(OutputFilesContainerType& OutputFiles);

            /**
             * @brief 分割节点块
             */
            void DivideNodesBlock(
                OutputFilesContainerType& OutputFiles,
                const PartitionIndicesContainerType& NodesAllPartitions);

            /**
             * @brief 分割几何体块
             */
            void DivideGeometriesBlock(
                OutputFilesContainerType& OutputFiles,
                const PartitionIndicesContainerType& GeometriesAllPartitions
            );

            /**
             * @brief 分割单元块
             */
            void DivideElementsBlock(
                OutputFilesContainerType& OutputFiles,
                const PartitionIndicesContainerType& ElementsAllPartitions
            );

            /**
             * @brief 分割条件块
             */
            void DivideConditionsBlock(
                OutputFilesContainerType& OutputFiles,
                const PartitionIndicesContainerType & ConditionsAllPartitions
            );

            /**
             * @brief 分割节点的数据块
             */
            void DivideNodalDataBlock(
                OutputFilesContainerType& OutputFiles,
                const PartitionIndicesContainerType& NodesAllPartitions
            );

            /**
             * @brief 分割标志变量的数据块
             */
            void DivideFlagVariableData(
                OutputFilesContainerType& OutputFiles,
                const PartitionIndicesContainerType& NodesAllPartitions
            );

            /**
             * @brief 分割自由度变量的数据块
             */
            void DivideDofVariableData(
                OutputFilesContainerType& OutputFiles,
                const PartitionIndicesContainerType& NodesAllPartitions
            );

            /**
             * @brief 分割矢量变量数据块
             */
            template<class TValueType>
            void DivideVectorialVariableData(
                OutputFilesContainerType& OutputFiles,
                const PartitionIndicesContainerType& EntitiesPartitions,
                std::string BlockName
            );

            /**
             * @brief 分割单元的数据块
             */
            void DivideElementalDataBlock(
                OutputFilesContainerType& OutputFiles,
                const PartitionIndicesContainerType& ElementsAllPartitions
            );

            /**
             * @brief 分割单元的标量变量数据块
             */
            void DivideScalarVariableData(
                OutputFilesContainerType& OutputFiles,
                const PartitionIndicesContainerType& EntitiesPartitions,
                std::string BlockName
            );

            /**
             * @brief 分割条件的数据块
             */
            void DivideConditionalDataBlock(
                OutputFilesContainerType& OutputFiles,
                const PartitionIndicesContainerType& ConditionsAllPartitions
            );

            /**
             * @brief 分割网格块
             */
            void DivideMeshBlock(
                OutputFilesContainerType& OutputFiles,
                const PartitionIndicesContainerType& NodesAllPartitions,
                const PartitionIndicesContainerType& ElementsAllPartitions,
                const PartitionIndicesContainerType& ConditionsAllPartitions
            );

            /**
             * @brief 分割子模型部件块
             */
            void DivideSubModelPartBlock(
                OutputFilesContainerType& OutputFiles,
                const PartitionIndicesContainerType& NodesAllPartitions,
                const PartitionIndicesContainerType& ElementsAllPartitions,
                const PartitionIndicesContainerType& ConditionsAllPartitions
            );

            /**
             * @brief 分割网格数据块
             */
            void DivideMeshDataBlock(OutputFilesContainerType& OutputFiles);

            /**
             * @brief 分割网格节点块
             */
            void DivideMeshNodesBlock(
                OutputFilesContainerType& OutputFiles,
                const PartitionIndicesContainerType& NodesAllPartitions
            );

            /**
             * @brief 分割网格单元块
             */
            void DivideMeshElementsBlock(
                OutputFilesContainerType& OutputFiles,
                const PartitionIndicesContainerType& ElementsAllPartitions
            );

            /**
             * @brief 分割网格条件块
             */
            void DivideMeshConditionsBlock(
                OutputFilesContainerType& OutputFiles,
                const PartitionIndicesContainerType& ConditionsAllPartitions
            );

            /**
             * @brief 分割子模型部件的数据块
             */
            void DivideSubModelPartDataBlock(OutputFilesContainerType& OutputFiles);

            /**
             * @brief 分割子模型部件的表格块
             */
            void DivideSubModelPartTableBlock(OutputFilesContainerType& OutputFiles);

            /**
             * @brief 分割子模型部件的节点块
             */
            void DivideSubModelPartNodesBlock(
                OutputFilesContainerType& OutputFiles,
                const PartitionIndicesContainerType& NodesAllPartitions
            );

            /**
             * @brief 分割子模型部件的单元块
             */
            void DivideSubModelPartElementsBlock(
                OutputFilesContainerType& OutputFiles,
                const PartitionIndicesContainerType& ElementsAllPartitions
            );

            /**
             * @brief 分割子模型部件的条件块
             */
            void DivideSubModelPartConditionsBlock(
                OutputFilesContainerType& OutputFiles,
                const PartitionIndicesContainerType& ConditionsAllPartitions
            );

            /**
             * @brief 写入分区信息
             */
            void WritePartitionIndices(OutputFilesContainerType& OutputFiles, PartitionIndicesType const&  NodesPartitions, PartitionIndicesContainerType const& NodesAllPartitions);

            /**
             * @brief 写入通信器的相关数据
             */
            void WriteCommunicatorData(
                OutputFilesContainerType& OutputFiles, 
                SizeType NumberOfPartitions, 
                const GraphType& DomainsColoredGraph,
                const PartitionIndicesType& NodesPartitions,
                const PartitionIndicesType& ElementsPartitions,
                const PartitionIndicesType& ConditionsPartitions,
                const PartitionIndicesContainerType& NodesAllPartitions,
                const PartitionIndicesContainerType& ElementsAllPartitions,
                const PartitionIndicesContainerType& ConditionsAllPartitions
            );

            /**
             * @brief 写入通信器的本地节点信息
             */
            void WriteCommunicatorLocalNodes(OutputFilesContainerType& OutputFiles, SizeType NumberOfPartitions, PartitionIndicesType const& NodesPartitions, PartitionIndicesContainerType const& NodesAllPartitions);

            /**
             * @brief 将某个字符串写入到所有输出文件中
             */
            void WriteInAllFiles(OutputFilesContainerType& OutputFiles, std::string const& ThisWord);

            /**
             * @brief 在容器中查找指定的键
             */
            template<class TContainerType, class TKeyType>
            typename TContainerType::iterator FindKey(TContainerType& ThisContainer , TKeyType ThisKey, std::string ComponentName);

            /**
             * @brief 读取矢量值
             */
            template<class TValueType>
            TValueType& ReadVectorialValue(TValueType& rValue);

            /**
             * @brief 从输入的字符串（rWord）中提取值并存储到 rValue 中
             */
            template<class TValueType>
            TValueType& ExtractValue(std::string rWord, TValueType & rValue);

            /**
             * @brief 从输入的字符串 rWord 中提取布尔值，并存储到 rValue 中
             */
            bool& ExtractValue(std::string rWord, bool & rValue);

            /**
             * @brief 读取材料本构定律的值
             */
            void ReadConstitutiveLawValue(ConstitutiveLaw::Pointer& rValue);

            /**
             * @brief 读取一个单词并存储到 Word 中
             */
            ModelPartIO& ReadWord(std::string& Word);

            /**
             * @brief 读取一个数据块，并存储到 Block 中
             */
            ModelPartIO& ReadBlock(std::string& Block, std::string const& BlockName);

            /**
             * @brief 跳过输入流中的空白字符（如空格、制表符、换行符等）
             */
            char SkipWhiteSpaces();

            /**
             * @brief 获取输入流中的下一个字符
             */
            char GetCharacter();

            /**
             * @brief 检查语句是否匹配给定的单词
             */
            void CheckStatement(std::string const& rStatement, std::string const& rGivenWord) const;

            /**
             * @brief 重置输入流
             */
            void ResetInput();

            /**
             * @brief 创建分区
             */
            inline void CreatePartition(unsigned int NumberOfThreads,const int NumberOfRows, DenseVector<unsigned int>& partitions);

            /**
             * @brief 扫描节点块并对每个节点调用 ReorderedNodeId
             */
            void ScanNodeBlock();

        private:
            /**
             * @brief 存储文件或数据流中的行数
             */
            SizeType mNumberOfLines;

            /**
             * @brief 存储文件的基础路径
             */
            std::filesystem::path mBaseFilename;

            /**
             * @brief 标志对象
             */
            Flags mOptions;

            /**
             * @brief 输入输出流的指针
             */
            Quest::shared_ptr<std::iostream> mpStream;

    };

}


#endif //QUEST_MODEL_PART_IO_HPP