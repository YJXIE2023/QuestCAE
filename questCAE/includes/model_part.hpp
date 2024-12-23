#ifndef QUEST_MODEL_PART_HPP
#define QUEST_MODEL_PART_HPP

// 系统头文件
#include <string>
#include <iostream>
#include <sstream>
#include <cstddef>

// 项目头文件
#include "includes/define.hpp"
#include "includes/serializer.hpp"
#include "includes/process_info.hpp"
#include "container/data_value_container.hpp"
#include "includes/mesh.hpp"
#include "container/geometry_container.hpp"
#include "includes/element.hpp"
#include "includes/condition.hpp"
#include "includes/communicator.hpp"
#include "includes/table.hpp"
#include "container/pointer_vector_map.hpp"
#include "container/pointer_hash_map_set.hpp"
#include "IO/logger.hpp"
#include "includes/quest_flags.hpp"
#include "includes/master_slave_constraint.hpp"
#include "container/variable.hpp"
#include "container/variable_data.hpp"

namespace Quest{

    class Model;

    /**
     * @class ModelPart
     * @brief 管理多物理场仿真中的网格
     */
    class QUEST_API(QUEST_CORE) ModelPart final : public DataValueContainer, public Flags{
        private:
            class GetModelPartName{
                public:
                    const std::string& operator()(const ModelPart& rModelPart) const{
                        return rModelPart.Name();
                    }
            };


        public:
            enum OwnershipType{
                Quest_All,
                Quest_Local,
                Quest_Ghost,
                Quest_Ownership_Size
            };


            using IndexType = std::size_t;
            using SizeType = std::size_t;
            using DofType = Dof<double>;
            using DofsVectorType = std::vector<DofType::Pointer>;
            using DoubleVariableType = Variable<double>;
            using MatrixType = Matrix;
            using VectorType = Vector;

            using DofsArrayType = PointerVectorSet<DofType>;
            using NodeType = Node;
            using GeometryType = Geometry<NodeType>;
            using PropertiesType = Properties;
            using ElementType = Element;
            using ConditionType = Condition;

            using MeshType = Mesh<NodeType, PropertiesType, ElementType, ConditionType>;
            using MeshesContainerType = PointerVector<MeshType>;
            using NodesContainerType = MeshType::NodesContainerType;
            using NodeIterator = MeshType::NodeIterator;
            using NodeConstantIterator = MeshType::NodeConstantIterator;
            using PropertiesContainerType = MeshType::PropertiesContainerType;
            using PropertiesIterator = MeshType::PropertiesIterator;
            using PropertiesConstantIterator = MeshType::PropertiesConstantIterator;
            using ElementsContainerType = MeshType::ElementsContainerType;
            using ElementIterator = MeshType::ElementIterator;
            using ElementConstantIterator = MeshType::ElementConstantIterator;
            using ConditionsContainerType = MeshType::ConditionsContainerType;
            using ConditionIterator = MeshType::ConditionIterator;
            using ConditionConstantIterator = MeshType::ConditionConstantIterator;

            using TableType = Table<double, double>;
            using TablesContainerType = PointerVectorMap<SizeType, TableType>;
            using TableIterator = TablesContainerType::iterator;
            using TableConstantIterator = TablesContainerType::const_iterator;

            using MasterSlaveConstraintType = MeshType::MasterSlaveConstraintType;
            using MasterSlaveConstraintContainerType = MeshType::MasterSlaveConstraintContainerType;
            using MasterSlaveConstraintIteratorType = MeshType::MasterSlaveConstraintIteratorType;
            using MasterSlaveConstraintConstantIteratorType = MeshType::MasterSlaveConstraintConstantIteratorType;
            
            using GeometryContainerType = GeometryContainer<GeometryType>;
            using GeometryIterator = typename GeometryContainerType::GeometryIterator;
            using GeometryConstantIterator = typename GeometryContainerType::GeometryConstantIterator;
            using GeometriesMapType = typename GeometryContainerType::GeometriesMapType;

            using SubModelPartsContainerType = PointerHashMapSet<ModelPart, std::hash<std::string>, GetModelPartName, Quest::shared_ptr<ModelPart>>;
            using SubModelPartIterator = SubModelPartsContainerType::iterator;
            using SubModelPartConstantIterator = SubModelPartsContainerType::const_iterator;

            QUEST_DEFINE_LOCAL_FLAG(ALL_ENTITIES);
            QUEST_DEFINE_LOCAL_FLAG(OVERWRITE_ENTITIES);

        public:
            /**
             * @brief 析构函数
             */
            ~ModelPart() override;

            /**
             * @brief 赋值运算符
             */
            ModelPart& operator = (const ModelPart& rOther) = delete;

            /**
             * @brief 清空模型部件，变量列表、缓冲区大小和进程信息会保留
             */
            void Clear();

            /**
             * @brief 清空模型部件，变量列表和缓冲区大小不会被保留
             */
            void Reset();

            /**
             * @brief 创建分析步
             */
            IndexType CreateSolutionStep();

            /**
             * @brief 克隆分析步
             */
            IndexType CloneSolutionStep();

            /**
             * @brief 克隆时间步
             */
            IndexType CloneTimeStep();

            /**
             * @brief 创建时间步
             */
            IndexType CreateTimeStep(double NewTime);

            /**
             * @brief 克隆时间步
             */
            IndexType CloneTimeStep(double NewTime);

            /**
             * @brief 重写时间步数据
             */
            void OverwriteSolutionStepData(IndexType SourceSolutionStepIndex, IndexType DestinationSourceSolutionStepIndex);

            /**
             * @brief 返回包含该模型部件的模型
             */
            Model& GetModel(){
                return mrModel;
            }

            /**
             * @brief 返回包含该模型部件的模型
             */
            const Model& GetModel() const{
                return mrModel;
            }

            /**
             * @brief 将数据库的值重置为时间步开始时的值
             */
            void ReduceTimeStep(ModelPart& rModelPart, double NewTime);

            /**
             * @brief 返回指定索引的Mesh的节点数量
             */
            SizeType NumberOfNodes(IndexType ThisIndex = 0) const{
                return GetMesh(ThisIndex).NumberOfNodes();
            }

            /**
             * @brief 在已有Mesh中添加节点
             */
            void AddNode(NodeType::Pointer pNewNode, IndexType ThisIndex = 0);

            /**
             * @brief 在已有Mesh中添加节点
             */
            void AddNodes(const std::vector<IndexType>& NodeIds, IndexType ThisIndex = 0);

            /**
             * @brief 在已有Mesh中添加节点
             */
            template<typename TIteratorType>
            void AddNodes(TIteratorType nodes_begin, TIteratorType nodes_end, IndexType ThisIndex = 0){
                QUEST_TRY
                ModelPart::NodesContainerType aux;
                ModelPart::NodesContainerType aux_root;
                ModelPart* root_model_part = &this->GetRootModelPart();

                for(IIteratorType it = nodes_begin; it!= nodes_end; ++it){
                    auto it_found = root_model_part->Nodes().find(it->Id());
                    if(it_found == root_model_part->NodesEnd()){
                        aux_root.push_back(*(it.base()));
                        aux.push_back(*(it.base()));
                    } else {
                        if(&(*it_found) != &(*it)){
                            QUEST_ERROR << "aattempting to add a new node with Id: " << it_found->Id() << ", unfortunately a (different) node with the same Id already exists" << std::endl;
                        } else {
                            aux.push_back(*(it.base()));
                        }
                    }
                }

                for(auto it = aux_root.begin(); it!= aux_root.end(); ++it){
                    root_model_part->Nodes().push_back(*(it.base()));
                }
                root_model_part->Nodes().Unique();

                ModelPart* current_part = this;
                while(current_part->IsSubModelPart()){
                    for(auto it = aux.begin(); it!= aux.end(); ++it){
                        current_part->Nodes().push_back(*(it.base()));
                    }

                    current_part->Nodes().Unique();

                    current_part = &(current_part->GetParentModelPart());
                }

                QUEST_CATCH("")
            }

            /**
             * @brief 向当前Mesh中添加一个节点
             */
            NodeType::Pointer CreateNewNode(int Id, double x, double y, double z, VariablesList::Pointer pNewVariablesList, IndexType ThisIndex = 0);

            /**
             * @brief 向当前Mesh中添加一个节点
             */
            NodeType::Pointer CreateNewNode(IndexType Id, double x, double y, double z, IndexType ThisIndex = 0);

            /**
             * @brief 向当前Mesh中添加一个节点
             */
            NodeType::Pointer CreateNewNode(IndexType Id, double x, double y, double z, double* pThisData, IndexType ThisIndex = 0);

            /**
             * @brief 向当前Mesh中添加一个节点
             */
            NodeType::Pointer CreateNewNode(IndexType NodeId, const NodeType& rSourceNode, IndexType ThisIndex = 0);

            /**
             * @brief 将一个节点分配到ModelPart中，并为该节点指定一个索引
             */
            void AssignNode(NodeType::Pointer pThisNode, IndexType ThisIndex = 0);

            /**
             * @brief 判断指定ID的网格中是否有指定ID的节点
             */
            bool HasNode(IndexType NodeId, IndexType ThisIndex = 0) const{
                return GetMesh(ThisIndex).HasNode(NodeId);
            }

            /**
             * @brief 返回指定ID的网格中指定ID的节点指针
             */
            NodeType::Pointer pGetNode(IndexType NodeId, IndexType ThisIndex = 0){
                return GetMesh(ThisIndex).pGetNode(NodeId);
            }

            /**
             * @brief 返回指定ID的网格中指定ID的节点指针
             */
            const NodeType::Pointer pGetNode(const IndexType NodeId, const IndexType ThisIndex = 0) const{
                return GetMesh(ThisIndex).pGetNode(NodeId);
            }

            /**
             * @brief 返回指定ID的网格中指定ID的节点
             */
            NodeType& GetNode(IndexType NodeId, IndexType ThisIndex = 0){
                return GetMesh(ThisIndex).GetNode(NodeId);
            }

            /**
             * @brief 返回指定ID的网格中指定ID的节点
             */
            const NodeType& GetNode(IndexType NodeId, IndexType ThisIndex = 0) const{
                return GetMesh(ThisIndex).GetNode(NodeId);
            }

            /**
             * @brief 移除指定ID的网格中指定ID的节点（该层级和所有子层级）
             */
            void RemoveNode(IndexType NodeId, IndexType ThisIndex = 0);

            /**
             * @brief 移除指定ID的网格中指定节点对象（该层级和所有子层级）
             */
            void RemoveNode(NodeType& ThisNode, IndexType ThisIndex = 0);

            /**
             * @brief 移除指定ID的网格中指定节点对象指针（该层级和所有子层级）
             */
            void RemoveNode(NodeType::Pointer pThisNode, IndexType ThisIndex = 0);

            /**
             * @brief 移除所有层级中指定ID网格中指定ID的节点
             */
            void RemoveNodeFromAllLevels(IndexType NodeId, IndexType ThisIndex = 0);

            /**
             * @brief 移除所有层级中指定ID网格中指定节点对象
             */
            void RemoveNodeFromAllLevels(NodeType& ThisNode, IndexType ThisIndex = 0);

            /**
             * @brief 移除所有层级中指定ID网格中指定节点对象指针
             */
            void RemoveNodeFromAllLevels(NodeType::Pointer pThisNode, IndexType ThisIndex = 0);

            /**
             * @brief 删除所有由 "IdentifierFlag" 标识的节点，通过移除指针来实现（该层级和所有子层级）
             */
            void RemoveNodes(Flags IndentifierFlag = TO_ERASE);

            /**
             * @brief 删除所有层级中所有由 "IdentifierFlag" 标识的节点，通过移除指针来实现
             */
            void RemoveNodesFromAllLevels(Flags IndentifierFlag = TO_ERASE);

            /**
             * @brief 返回根模型部件
             */
            ModelPart& GetRootModelPart();

            /**
             * @brief 返回根模型部件
             */
            const ModelPart& GetRootModelPart() const;

            /**
             * @brief 返回指定ID网格的第一个节点的迭代器
             */
            NodeIterator NodesBegin(IndexType ThisIndex = 0){
                return GetMesh(ThisIndex).NodesBegin();
            }

            /**
             * @brief 返回指定ID网格的第一个节点的迭代器
             */
            NodeConstantIterator NodesBegin(IndexType ThisIndex = 0) const{
                return GetMesh(ThisIndex).NodesBegin();
            }

            /**
             * @brief 返回指定ID网格的最后一个节点的迭代器
             */
            NodeIterator NodesEnd(IndexType ThisIndex = 0){
                return GetMesh(ThisIndex).NodesEnd();
            }

            /**
             * @brief 返回指定ID网格的最后一个节点的迭代器
             */
            NodeConstantIterator NodesEnd(IndexType ThisIndex = 0) const{
                return GetMesh(ThisIndex).NodesEnd();
            }

            /**
             * @brief 返回指定网格的节点容器
             */
            NodesContainerType& Nodes(IndexType ThisIndex = 0){
                return GetMesh(ThisIndex).Nodes();
            }

            /**
             * @brief 返回指定网格的节点容器
             */
            const NodesContainerType& Nodes(IndexType ThisIndex = 0) const{
                return GetMesh(ThisIndex).Nodes();
            }

            /**
             * @brief 返回指定ID网格的节点容器指针
             */
            NodesContainerType::Pointer pNodes(IndexType ThisIndex = 0){
                return GetMesh(ThisIndex).pNodes();
            }

            /**
             * @brief 设置指定ID网格的节点容器指针
             */
            void SetNodes(NodesContainerType::Pointer pOtherNodes, IndexType ThisIndex = 0){
                GetMesh(ThisIndex).SetNodes(pOtherNodes);
            }

            /**
             * @brief 返回指定ID网格的节点容器的原始容器
             */
            NodesContainerType::ContainerType& NodesArray(IndexType ThisIndex = 0){
                return GetMesh(ThisIndex).NodesArray();
            }

            /**
             * @brief 添加一个求解步相关的变量
             */
            void AddNodalSolutionStepVariable(const VariableData& ThisVariable){
                if(!HasNodalSolutionStepVariable(ThisVariable)){
                    QUEST_ERROR_IF((this->GetRootModelPart()).Nodes().size() != 0)
                        << "Attempting to add the variable \"" << ThisVariable.Name()
                        << "\" to the model part with name \"" << this->Name() << "\" which is not empty" << std::endl;

                    mpVariablesList->Add(ThisVariable);
                }
            }

            /**
             * @brief 判断是否有指定名称的求解步相关的变量
             */
            bool HasNodalSolutionStepVariable(const VariableData& ThisVariable) const{
                return mpVariablesList->Has(ThisVariable);
            }

            /**
             * @brief 返回求解步相关的变量列表
             */
            VariablesList& GetNodalSolutionStepCariablesList(){
                return *mpVariablesList;
            }

            /**
             * @brief 返回求解步相关的变量列表
             */
            const VariablesList& GetNodalSolutionStepCariablesList() const{
                return *mpVariablesList;
            }

            /**
             * @brief 返回求解步相关的变量列表指针
             */
            VariablesList::Pointer pGetNodalSolutionStepCariablesList() const{
                return mpVariablesList;
            }

            /**
             * @brief 设置求解步相关的变量列表指针
             */
            void SetNodalSolutionStepVariablesList();

            /**
             * @brief 设置求解步相关的变量列表指针
             */
            void SetNodalSolutionStepVariablesList(VariablesList::Pointer pNewVariablesList){
                mpVariablesList = pNewVariablesList;
            }

            /**
             * @brief 获取求解步相关变量的数量
             */
            SizeType GetNodalSolutionStepDataSize(){
                return mpVariablesList->DataSize();
            }

            /**
             * @brief 获取求解步相关变量的内存总大小
             */
            SizeType GeiNodalSolutionStepTotalDataSize(){
                return mpVariablesList->DataSize() * mBufferSize;
            }

            /**
             * @brief 获取表格的数量
             */
            SizeType NumberOfTables() const{
                return mTables.size();
            }

            /**
             * @brief 添加一个表格
             */
            void AddTable(IndexType TableIs, TableType::Pointer pNewTable);

            /**
             * @brief 返回指定ID的表格指针
             */
            TableType::Pointer pGetTable(IndexType TableId){
                return mTables(TableId);
            }

            /**
             * @brief 返回指定ID的表格
             */
            TableType& GetTable(IndexType TableId){
                return mTables[TableId];
            }

            /**
             * @brief 从该层级和所有子层级中移除指定ID的表格
             */
            void RemoveTable(IndexType TableId);

            /**
             * @brief 从所有层级中移除指定ID的表格
             */
            void RemoveTableFromAllLevels(IndexType TableId);

            /**
             * @brief 返回指向表格容器头元素的迭代器
             */
            TableIterator TablesBegin(){
                return mTables.begin();
            }

            /**
             * @brief 返回指向表格容器头元素的迭代器
             */
            TableConstantIterator TablesBegin() const{
                return mTables.begin();
            }

            /**
             * @brief 返回指向表格容器尾元素的迭代器
             */
            TableIterator TablesEnd(){
                return mTables.end();
            }

            /**
             * @brief 返回指向表格容器尾元素的迭代器
             */
            TableConstantIterator TablesEnd() const{
                return mTables.end();
            }

            /**
             * @brief 返回指向表格容器的指针
             */
            TablesContainerType& Tables(){
                return mTables;
            }

            /**
             * @brief 返回表格容器的底层容器
             */
            TablesContainerType::ContainerType& TablesArray(){
                return mTables.GetContainer();
            }

            /**
             * @brief 返回指定ID网格中主从约束的数量
             */
            SizeType NumberOfMasterSlaveConstraints(IndexType ThisIndex = 0) const{
                return GetMesh(ThisIndex).NumberOfMasterSlaveConstraints();
            }

            /**
             * @brief 获取指定ID网格中主从约束的容器
             */
            MasterSlaveConstraintContainerType& MasterSlaveConstraints(IndexType ThisIndex = 0){
                return GetMesh(ThisIndex).MasterSlaveConstraints();
            }

            /**
             * @brief 获取指定ID网格中主从约束的容器
             */
            const MasterSlaveConstraintContainerType& MasterSlaveConstraints(IndexType ThisIndex = 0) const{
                return GetMesh(ThisIndex).MasterSlaveConstraints();
            }

            /**
             * @brief 获取指定ID网格中指向主从约束容器首元素的迭代器
             */
            MasterSlaveConstraintConstantIteratorType MasterSlaveConstraintsBegin(IndexType ThisIndex = 0) const{
                return GetMesh(ThisIndex).MasterSlaveConstraintsBegin();
            }

            /**
             * @brief 获取指定ID网格中指向主从约束容器尾元素的迭代器
             */
            MasterSlaveConstraintConstantIteratorType MasterSlaveConstraintsEnd(IndexType ThisIndex = 0) const{
                return GetMesh(ThisIndex).MasterSlaveConstraintsEnd();
            }

            /**
             * @brief 获取指定ID网格中指向主从约束容器首元素的迭代器
             */
            MasterSlaveConstraintIteratorType MasterSlaveConstraintsBegin(IndexType ThisIndex = 0){
                return GetMesh(ThisIndex).MasterSlaveConstraintsBegin();
            }

            /**
             * @brief 获取指定ID网格中指向主从约束容器尾元素的迭代器
             */
            MasterSlaveConstraintIteratorType MasterSlaveConstraintsEnd(IndexType ThisIndex = 0){
                return GetMesh(ThisIndex).MasterSlaveConstraintsEnd();
            }

            /**
             * @brief 向指定ID网格的主从约束容器中添加一个主从约束
             */
            void AddMasterSlaveConstraint(MasterSlaveConstraintType::Pointer pNewMasterSlaveConstraint, IndexType ThisIndex = 0);

            /**
             * @brief 向指定ID网格的主从约束容器中添加主从约束
             */
            void AddMasterSlaveConstraints(const std::vector<IndexType>& MasterSlaveConstraintIds, IndexType ThisIndex = 0);

            /**
             * @brief 向指定ID网格的主从约束容器中添加主从约束
             */
            template<typename TIteratorType>
            void AddMasterSlaveConstraints(TIteratorType constraints_begin, TIteratorType constraints_end, IndexType ThisIndex = 0){
                QUEST_TRY
                ModelPart::MasterSlaveConstraintContainerType aux;
                ModelPart::MasterSlaveConstraintContainerType aux_root;
                ModelPart* root_model_part = &this->GetRootModelPart();

                for(IIteratorType it = constraints_begin; it!= constraints_end; ++it){
                    auto it_found = root_model_part->MasterSlaveConstraints().find(it->Id());
                    if(it_found == root_model_part->MasterSlaveConstraintsEnd()){
                        aux_root.push_back(*(it.base()));
                        aux.push_back(*(it.base()));
                    } else {
                        if(&(*it_found) != &(*it)){
                            QUEST_ERROR << "aattempting to add a new master slave constraint with Id: " << it_found->Id() << ", unfortunately a (different) constraint with the same Id already exists" << std::endl;
                        } else {
                            aux.push_back(*(it.base()));
                        }
                    }
                }

                for(auto it = aux_root.begin(); it!= aux_root.end(); ++it){
                    root_model_part->MasterSlaveConstraints().push_back(*(it.base()));
                }
                root_model_part->MasterSlaveConstraints().Unique();

                ModelPart* current_part = this;
                while(current_part->IsSubModelPart()){
                    for(auto it = aux.begin(); it!= aux.end(); ++it){
                        current_part->MasterSlaveConstraints().push_back(*(it.base()));
                    }

                    current_part->MasterSlaveConstraints().Unique();

                    current_part = &(current_part->GetParentModelPart());
                }

                QUEST_CATCH("")
            }

            /**
             * @brief 在当前模型部分中创建一个新的主从约束
             */
            MasterSlaveConstraint::Pointer CreateNewMasterSlaveConstraint(
                const std::string& ConstriantName,
                IndexType Id,
                DofsVectorType& rMasterDofVector,
                DofsVectorType& rSlaveDofVector,
                const MatrixType& RelationMatrix,
                const VectorType& RelationVector,
                IndexType ThisIndex = 0
            );
            
            /**
             * @brief 在当前模型部分中创建一个新的主从约束
             */
            MasterSlaveConstraint::Pointer CreateNewMasterSlaveConstraint(
                const std::string& ConstriantName,
                IndexType Id,
                NodeType& rMasterNode,
                const DoubleVariableType& rMasterVariable,
                NodeType& rSlaveNode,
                const DoubleVariableType& rSlaveVariable,
                const double Weight,
                const double Constant,
                IndexType ThisIndex = 0
            );

            /**
             * @brief 移除指定ID的网格中指定ID的主从约束（该层级和所有子层级）
             */
            void RemoveMasterSlaveConstraint(IndexType MasterSlaveConstraintId, IndexType ThisIndex = 0);

            /**
             * @brief 移除指定ID的网格中的主从约束（该层级和所有子层级）
             */
            void RemoveMasterSlaveConstraint(MasterSlaveConstraintType& ThisMasterSlaveConstraint, IndexType ThisIndex = 0);

            /**
             * @brief 移除指定ID的网格中的主从约束（所有层级）
             */
            void RemoveMasterSlaveConstraintFromAllLevels(IndexType MasterSlaveConstraintId, IndexType ThisIndex = 0);

            /**
             * @brief 移除指定ID的网格中的主从约束（所有层级）
             */
            void RemoveMasterSlaveConstraintFromAllLevels(MasterSlaveConstraintType& ThisMasterSlaveConstraint, IndexType ThisIndex = 0);

            /**
             * @brief 删除所有由 "IdentifierFlag" 标识的主从约束，通过移除指针来实现（该层级和所有子层级）
             */
            void RemoveMasterSlaveConstraints(Flags IndentifierFlag = TO_ERASE);

            /**
             * @brief 删除所有层级中所有由 "IdentifierFlag" 标识的主从约束，通过移除指针来实现（所有层级）
             */
            void RemoveMasterSlaveConstraintsFromAllLevels(Flags IndentifierFlag = TO_ERASE);

            /**
             * @brief 判断指定ID的网格中是否有指定ID的主从约束
             */
            bool HasMasterSlaveConstraint(
                const IndexType MasterSlaveConstraintId,
                IndexType ThisIndex = 0
            ) const{
                return GetMesh(ThisIndex).HasMasterSlaveConstraint(MasterSlaveConstraintId);
            }

            /**
             * @brief 获取指定ID的网格中指定ID的主从约束的指针
             */
            MasterSlaveConstraintType::Pointer pGetMasterSlaveConstraint(IndexType ConstrintId, IndexType ThisIndex = 0);

            /**
             * @brief 获取指定ID的网格中指定ID的主从约束
             */
            MasterSlaveConstraintType& GetMasterSlaveConstraint(IndexType MasterSlaveConstraintId, IndexType ThisIndex = 0);

            /**
             * @brief 获取指定ID的网格中指定ID的主从约束
             */
            const MasterSlaveConstraintType& GetMasterSlaveConstraint(IndexType MasterSlaveConstraintId, IndexType ThisIndex = 0) const;

            /**
             * @brief 获取指定ID的网格中的属性的数量
             */
            SizeType NumberOfProperties(IndexType ThisIndex = 0) const;

            /**
             * @brief 向指定ID网格的属性容器中添加一个属性
             */
            void AddProperties(PropertiesType::Pointer pNewProperties, IndexType ThisIndex = 0);

            /**
             * @brief 判断指定ID的网格中是否有指定ID的属性
             */
            bool HasProperties(IndexType PropertiesId, IndexType ThisIndex = 0) const;

            /**
             * @brief 判断对应于其标识符的属性是否存在于任何模型部分中
             */
            bool RecursivelyHasProperties(IndexType PropertiesId, IndexType ThisIndex = 0) const;

            /**
             * @brief 在指定ID的网格中创建新的属性
             */
            PropertiesType::Pointer CreateNewProperties(IndexType PropertiesId, IndexType ThisIndex = 0);

            /**
             * @brief 获取指定ID的网格中指定属性的指针
             */
            PropertiesType::Pointer pGetProperties(IndexType PropertiesId, IndexType ThisIndex = 0);

            /**
             * @brief 获取指定ID的网格中指定属性的指针
             */
            const PropertiesType::Pointer pGetProperties(IndexType PropertiesId, IndexType ThisIndex = 0) const;

            /**
             * @brief 获取指定ID的网格中指定属性
             */
            PropertiesType& GetProperties(IndexType PropertiesId, IndexType ThisIndex = 0);

            /**
             * @brief 获取指定ID的网格中指定属性
             */
            const PropertiesType& GetProperties(IndexType PropertiesId, IndexType ThisIndex = 0) const;

            /**
             * @brief 判断对应于其地址的子属性是否存在
             */
            bool HasProperties(const std::string& rAddress, IndexType ThisIndex = 0) const;

            /**
             * @brief 获取指定ID的网格中指定地址的子属性的指针
             */
            PropertiesType::Pointer pGetProperties(const std::string& rAddress, IndexType ThisIndex = 0);

            /**
             * @brief 获取指定ID的网格中指定地址的子属性的指针
             */
            const PropertiesType::Pointer pGetProperties(const std::string& rAddress, IndexType ThisIndex = 0) const;

            /**
             * @brief 获取指定ID的网格中指定地址的子属性
             */
            PropertiesType& GetProperties(const std::string& rAddress, IndexType ThisIndex = 0);

            /**
             * @brief 获取指定ID的网格中指定地址的子属性
             */
            const PropertiesType& GetProperties(const std::string& rAddress, IndexType ThisIndex = 0) const;

            /**
             * @brief 移除指定ID的网格中的属性（该层级和所有子层级）
             */
            void RemoveProperties(IndexType PropertiesId, IndexType ThisIndex = 0);

            /**
             * @brief 移除指定ID的网格中的属性（该层级和所有子层级）
             */
            void RemoveProperties(PropertiesType& ThisProperties, IndexType ThisIndex = 0);

            /**
             * @brief 移除指定ID的网格中的属性（该层级和所有子层级）
             */
            void RemoveProperties(PropertiesType::Pointer pThisProperties, IndexType ThisIndex = 0);

            /**
             * @brief 移除指定ID的网格中的属性（所有层级）
             */
            void RemovePropertiesFromAllLevels(IndexType PropertiesId, IndexType ThisIndex = 0);

            /**
             * @brief 移除指定ID的网格中的属性（所有层级）
             */
            void RemovePropertiesFromAllLevels(PropertiesType& ThisProperties, IndexType ThisIndex = 0);

            /**
             * @brief 移除指定ID的网格中的属性（所有层级）
             */
            void RemovePropertiesFromAllLevels(PropertiesType::Pointer pThisProperties, IndexType ThisIndex = 0);

            /**
             * @brief 返回指定ID的网格的属性容器中头元素的迭代器
             */
            PropertiesIterator PropertiesBegin(IndexType ThisIndex = 0){
                return GetMesh(ThisIndex).PropertiesBegin();
            }

            /**
             * @brief 返回指定ID的网格的属性容器中头元素的迭代器
             */
            PropertiesConstantIterator PropertiesBegin(IndexType ThisIndex = 0) const{
                return GetMesh(ThisIndex).PropertiesBegin();
            }

            /**
             * @brief 返回指定ID的网格的属性容器中尾元素的迭代器
             */
            PropertiesIterator PropertiesEnd(IndexType ThisIndex = 0){
                return GetMesh(ThisIndex).PropertiesEnd();
            }

            /**
             * @brief 返回指定ID的网格的属性容器中尾元素的迭代器
             */
            PropertiesConstantIterator PropertiesEnd(IndexType ThisIndex = 0) const{
                return GetMesh(ThisIndex).PropertiesEnd();
            }

            /**
             * @brief 返回指定ID的网格的属性容器
             */
            PropertiesContainerType& rProperties(IndexType ThisIndex = 0){
                return GetMesh(ThisIndex).Properties();
            }

            /**
             * @brief 返回指定ID的网格的属性容器指针
             */
            PropertiesContainerType::Pointer pProperties(IndexType ThisIndex = 0){
                return GetMesh(ThisIndex).pProperties();
            }

            /**
             * @brief 设置指定ID的网格的属性容器
             */
            void SetProperties(PropertiesContainerType::Pointer pOtherProperties, IndexType ThisIndex = 0){
                GetMesh(ThisIndex).SetProperties(pOtherProperties);
            }

            /**
             * @brief 返回指定ID的网格的属性容器的底层容器
             */
            PropertiesContainerType::ContainerType& PropertiesArray(IndexType ThisIndex = 0){
                return GetMesh(ThisIndex).PropertiesArray();
            }

            /**
             * @brief 返回指定ID网格中单元的数量
             */
            SizeType NumberOfElements(IndexType ThisIndex = 0) const{
                return GetMesh(ThisIndex).NumberOfElements();
            }

            /**
             * @brief 向指定ID网格的单元容器中添加一个单元
             */
            void AddElement(ElementType::Pointer pNewElement, IndexType ThisIndex = 0);

            /**
             * @brief 向指定ID网格的单元容器中添加一系列单元
             */
            void AddElements(const std::vector<IndexType>& ElementIds, IndexType ThisIndex = 0);

            /**
             * @brief 向指定ID网格的单元容器中添加一系列单元
             */
            template<typename TIteratorType>
            void AddElements(TIteratorType elements_begin, TIteratorType elements_end, IndexType ThisIndex = 0){
                QUEST_TRY
                ModelPart::ElementsContainerType aux;
                ModelPart::ElementsContainerType aux_root;
                ModelPart* root_model_part = &this->GetRootModelPart();

                for(IIteratorType it = elements_begin; it!= elements_end; ++it){
                    auto it_found = root_model_part->Elements().find(it->Id());
                    if(it_found == root_model_part->ElementsEnd()){
                        aux_root.push_back(*(it.base()));
                        aux.push_back(*(it.base()));
                    } else {
                        if(&(*it_found) != &(*it)){
                            QUEST_ERROR << "aattempting to add a new element with Id: " << it_found->Id() << ", unfortunately an element with the same Id already exists" << std::endl;
                        } else {
                            aux.push_back(*(it.base()));
                        }
                    }
                }

                for(auto it = aux_root.begin(); it!= aux_root.end(); ++it){
                    root_model_part->Elements().push_back(*(it.base()));
                }
                root_model_part->Elements().Unique();

                ModelPart* current_part = this;
                while(current_part->IsSubModelPart()){
                    for(auto it = aux.begin(); it!= aux.end(); ++it){
                        current_part->Elements().push_back(*(it.base()));
                    }

                    current_part->Elements().Unique();

                    current_part = &(current_part->GetParentModelPart());
                }

                QUEST_CATCH("")
            }

            /**
             * @brief 在当前模型部分中创建一个新的单元
             */
            ElementType::Pointer CreateNewElement(
                std::string ElementName,
                IndexType Id,
                std::vector<IndexType> ElementNodeIds,
                PropertiesType::Pointer pProperties,
                IndexType ThisIndex = 0
            );
            

            /**
             * @brief 在当前模型部分中创建一个新的单元
             */
            ElementType::Pointer CreateNewElement(
                std::string ElementName,
                IndexType Id,
                Geometry<Node>::PointsArrayType pElementNodes,
                PropertiesType::Pointer pProperties,
                IndexType ThisIndex = 0
            );

            /**
             * @brief 在当前模型部分中创建一个新的单元
             */
            ElementType::Pointer CreateNewElement(
                std::string ElementName,
                IndexType Id,
                typename GeometryType::Pointer pGeometry,
                PropertiesType::Pointer pProperties,
                IndexType ThisIndex = 0
            );
            

            /**
             * @brief 判断指定ID的网格中是否有指定ID的单元
             */
            bool HasElement(IndexType ElementId, IndexType ThisIndex = 0) const{
                return GetMesh(ThisIndex).HasElement(ElementId);
            }

            /**
             * @brief 获取指定ID的网格中指定ID的单元的指针
             */
            ElementType::Pointer pGetElement(IndexType ElementId, IndexType ThisIndex = 0){
                return GetMesh(ThisIndex).pGetElement(ElementId);
            }

            /**
             * @brief 获取指定ID的网格中指定ID的单元的指针
             */
            const ElementType::Pointer pGetElement(const IndexType ElementId, const IndexType ThisIndex = 0) const{
                return GetMesh(ThisIndex).pGetElement(ElementId);
            }

            /**
             * @brief 获取指定ID的网格中指定ID的单元
             */
            ElementType& GetElement(IndexType ElementId, IndexType ThisIndex = 0){
                return GetMesh(ThisIndex).GetElement(ElementId);
            }

            /**
             * @brief 获取指定ID的网格中指定ID的单元
             */
            const ElementType& GetElement(IndexType ElementId, IndexType ThisIndex = 0) const{
                return GetMesh(ThisIndex).GetElement(ElementId);
            }

            /**
             * @brief 移除指定ID的网格中的单元（该层级和所有子层级）
             */
            void RemoveElement(IndexType ElementId, IndexType ThisIndex = 0);

            /**
             * @brief 移除指定ID的网格中的单元（该层级和所有子层级）
             */
            void RemoveElement(ElementType& ThisElement, IndexType ThisIndex = 0);

            /**
             * @brief 移除指定ID的网格中的单元（该层级和所有子层级）
             */
            void RemoveElement(ElementType::Pointer pThisElement, IndexType ThisIndex = 0);

            /**
             * @brief 移除指定ID的网格中的单元（所有层级）
             */
            void RemoveElementFromAllLevels(IndexType ElementId, IndexType ThisIndex = 0);

            /**
             * @brief 移除指定ID的网格中的单元（所有层级）
             */
            void RemoveElementFromAllLevels(ElementType& ThisElement, IndexType ThisIndex = 0);

            /**
             * @brief 移除指定ID的网格中的单元（所有层级）
             */
            void RemoveElementFromAllLevels(ElementType::Pointer pThisElement, IndexType ThisIndex = 0);

            /**
             * @brief 移除指定标识符的单元（该层级和所有子层级）
             */
            void RemoveElements(Flags IndentifierFlag = TO_ERASE);

            /**
             * @brief 移除指定标识符的单元（所有层级）
             */
            void RemoveElementsFromAllLevels(Flags IndentifierFlag = TO_ERASE);

            /**
             * @brief 返回指定ID的网格的单元容器中头元素的迭代器
             */
            ElementIterator ElementsBegin(IndexType ThisIndex = 0){
                return GetMesh(ThisIndex).ElementsBegin();
            }

            /**
             * @brief 返回指定ID的网格的单元容器中头元素的迭代器
             */
            ElementConstantIterator ElementsBegin(IndexType ThisIndex = 0) const{
                return GetMesh(ThisIndex).ElementsBegin();
            }

            /**
             * @brief 返回指定ID的网格的单元容器中尾元素的迭代器
             */
            ElementIterator ElementsEnd(IndexType ThisIndex = 0){
                return GetMesh(ThisIndex).ElementsEnd();
            }

            /**
             * @brief 返回指定ID的网格的单元容器中尾元素的迭代器
             */
            ElementConstantIterator ElementsEnd(IndexType ThisIndex = 0) const{
                return GetMesh(ThisIndex).ElementsEnd();
            }

            /**
             * @brief 返回指定ID的网格的单元容器
             */
            ElementsContainerType& Elements(IndexType ThisIndex = 0){
                return GetMesh(ThisIndex).Elements();
            }

            /**
             * @brief 返回指定ID的网格的单元容器
             */
            const ElementsContainerType& Elements(IndexType ThisIndex = 0) const{
                return GetMesh(ThisIndex).Elements();
            }

            /**
             * @brief 返回指定ID的网格的单元容器指针
             */
            ElementsContainerType::Pointer pElements(IndexType ThisIndex = 0){
                return GetMesh(ThisIndex).pElements();
            }

            /**
             * @brief 设置指定ID的网格的单元容器
             */
            void SetElements(ElementsContainerType::Pointer pOtherElements, IndexType ThisIndex = 0){
                GetMesh(ThisIndex).SetElements(pOtherElements);
            }

            /**
             * @brief 返回指定ID的网格的单元容器的底层容器
             */
            ElementsContainerType::ContainerType& ElementsArray(IndexType ThisIndex = 0){
                return GetMesh(ThisIndex).ElementsArray();
            }

            /**
             * @brief 获取指定ID的网格的条件的数量
             */
            SizeType NumberOfConditions(IndexType ThisIndex = 0) const{
                return GetMesh(ThisIndex).NumberOfConditions();
            }

            /**
             * @brief 向指定ID网格的条件容器中添加一个条件
             */
            void AddCondition(ConditionType::Pointer pNewCondition, IndexType ThisIndex = 0);

            /**
             * @brief 向指定ID网格的条件容器中添加一系列条件
             */
            void AddConditions(const std::vector<IndexType>& ConditionIds, IndexType ThisIndex = 0);

            /**
             * @brief 向指定ID网格的条件容器中添加一系列条件
             */
            template<typename TIteratorType>
            void AddConditions(TIteratorType conditions_begin, TIteratorType conditions_end, IndexType ThisIndex = 0){
                QUEST_TRY
                ModelPart::ConditionsContainerType aux;
                ModelPart::ConditionsContainerType aux_root;
                ModelPart* root_model_part = &this->GetRootModelPart();

                for(IIteratorType it = conditions_begin; it!= conditions_end; ++it){
                    auto it_found = root_model_part->Conditions().find(it->Id());
                    if(it_found == root_model_part->ConditionsEnd()){
                        aux_root.push_back(*(it.base()));
                        aux.push_back(*(it.base()));
                    } else {
                        if(&(*it_found) != &(*it)){
                            QUEST_ERROR << "aattempting to add a new condition with Id: " << it_found->Id() << ", unfortunately a condition with the same Id already exists" << std::endl;
                        } else {
                            aux.push_back(*(it.base()));
                        }
                    }
                }

                for(auto it = aux_root.begin(); it!= aux_root.end(); ++it){
                    root_model_part->Conditions().push_back(*(it.base()));
                }
                root_model_part->Conditions().Unique();

                ModelPart* current_part = this;
                while(current_part->IsSubModelPart()){
                    for(auto it = aux.begin(); it!= aux.end(); ++it){
                        current_part->Conditions().push_back(*(it.base()));
                    }

                    current_part->Conditions().Unique();

                    current_part = &(current_part->GetParentModelPart());
                }

                QUEST_CATCH("")
            }

            /**
             * @brief 在当前模型部分中创建一个新的条件
             */
            ConditionType::Pointer CreateNewCondition(
                std::string ConditionName,
                IndexType Id,
                std::vector<IndexType> ConditionNodeIds,
                PropertiesType::Pointer pProperties,
                IndexType ThisIndex = 0
            );
            

            /**
             * @brief 在当前模型部分中创建一个新的条件
             */
            ConditionType::Pointer CreateNewCondition(
                std::string ConditionName,
                IndexType Id,
                Geometry<Node>::PointsArrayType pConditionNodes,
                PropertiesType::Pointer pProperties,
                IndexType ThisIndex = 0
            );

            /**
             * @brief 在当前模型部分中创建一个新的条件
             */
            ConditionType::Pointer CreateNewCondition(
                std::string ConditionName,
                IndexType Id,
                typename GeometryType::Pointer pGeometry,
                PropertiesType::Pointer pProperties,
                IndexType ThisIndex = 0
            );
            

            /**
             * @brief 判断指定ID的网格中是否有指定ID的条件
             */
            bool HasCondition(IndexType ConditionId, IndexType ThisIndex = 0) const{
                return GetMesh(ThisIndex).HasCondition(ConditionId);
            }

            /**
             * @brief 获取指定ID的网格中指定ID的条件的指针
             */
            ConditionType::Pointer pGetCondition(IndexType ConditionId, IndexType ThisIndex = 0){
                return GetMesh(ThisIndex).pGetCondition(ConditionId);
            }

            /**
             * @brief 获取指定ID的网格中指定ID的条件的指针
             */
            const ConditionType::Pointer pGetCondition(const IndexType ConditionId, const IndexType ThisIndex = 0) const{
                return GetMesh(ThisIndex).pGetCondition(ConditionId);
            }

            /**
             * @brief 获取指定ID的网格中指定ID的条件
             */
            ConditionType& GetCondition(IndexType ConditionId, IndexType ThisIndex = 0){
                return GetMesh(ThisIndex).GetCondition(ConditionId);
            }

            /**
             * @brief 获取指定ID的网格中指定ID的条件
             */
            const ConditionType& GetCondition(IndexType ConditionId, IndexType ThisIndex = 0) const{
                return GetMesh(ThisIndex).GetCondition(ConditionId);
            }

            /**
             * @brief 移除指定ID的网格中的条件（该层级和所有子层级）
             */
            void RemoveCondition(IndexType ConditionId, IndexType ThisIndex = 0);

            /**
             * @brief 移除指定ID的网格中的条件（该层级和所有子层级）
             */
            void RemoveCondition(ConditionType& ThisCondition, IndexType ThisIndex = 0);

            /**
             * @brief 移除指定ID的网格中的条件（该层级和所有子层级）
             */
            void RemoveCondition(ConditionType::Pointer pThisCondition, IndexType ThisIndex = 0);

            /**
             * @brief 移除指定ID的网格中的条件（所有层级）
             */
            void RemoveConditionFromAllLevels(IndexType ConditionId, IndexType ThisIndex = 0);

            /**
             * @brief 移除指定ID的网格中的条件（所有层级）
             */
            void RemoveConditionFromAllLevels(ConditionType& ThisCondition, IndexType ThisIndex = 0);

            /**
             * @brief 移除指定ID的网格中的条件（所有层级）
             */
            void RemoveConditionFromAllLevels(ConditionType::Pointer pThisCondition, IndexType ThisIndex = 0);

            /**
             * @brief 移除所有指定标志的条件（该层级和所有子层级）
             */
            void RemoveConditions(Flags IndentifierFlag = TO_ERASE);

            /**
             * @brief 移除所有指定标志的条件（所有层级）
             */
            void RemoveConditionsFromAllLevels(Flags IndentifierFlag = TO_ERASE);

            /**
             * @brief 返回指定ID的网格的条件容器中头元素的迭代器
             */
            ConditionIterator ConditionsBegin(IndexType ThisIndex = 0){
                return GetMesh(ThisIndex).ConditionsBegin();
            }

            /**
             * @brief 返回指定ID的网格的条件容器中头元素的迭代器
             */
            ConditionConstantIterator ConditionsBegin(IndexType ThisIndex = 0) const{
                return GetMesh(ThisIndex).ConditionsBegin();
            }

            /**
             * @brief 返回指定ID的网格的条件容器中尾元素的迭代器
             */
            ConditionIterator ConditionsEnd(IndexType ThisIndex = 0){
                return GetMesh(ThisIndex).ConditionsEnd();
            }

            /**
             * @brief 返回指定ID的网格的条件容器中尾元素的迭代器
             */
            ConditionConstantIterator ConditionsEnd(IndexType ThisIndex = 0) const{
                return GetMesh(ThisIndex).ConditionsEnd();
            }

            /**
             * @brief 返回指定ID的网格的条件容器
             */
            ConditionsContainerType& Conditions(IndexType ThisIndex = 0){
                return GetMesh(ThisIndex).Conditions();
            }

            /**
             * @brief 返回指定ID的网格的条件容器
             */
            const ConditionsContainerType& Conditions(IndexType ThisIndex = 0) const{
                return GetMesh(ThisIndex).Conditions();
            }

            /**
             * @brief 返回指定ID的网格的条件容器指针
             */
            ConditionsContainerType::Pointer pConditions(IndexType ThisIndex = 0){
                return GetMesh(ThisIndex).pConditions();
            }

            /**
             * @brief 设置指定ID的网格的条件容器
             */
            void SetConditions(ConditionsContainerType::Pointer pOtherConditions, IndexType ThisIndex = 0){
                GetMesh(ThisIndex).SetConditions(pOtherConditions);
            }

            /**
             * @brief 设置指定ID的网格的条件容器
             */
            ConditionsContainerType::ContainerType& ConditionsArray(IndexType ThisIndex = 0){
                return GetMesh(ThisIndex).ConditionsArray();
            }

            /**
             * @brief 获取指定ID的网格的几何对象的数量
             */
            SizeType NumberOfGeometries() const{
                return mGeometries.NumberOfGeometries();
            }

            /**
             * @brief 向当前模型部件中插入一个几何对象
             */
            GeometryType::Pointer CreateNewGeometry(
                const std::string& rGeometryTypeName,
                const std::vector<IndexType>& rGeometryNodeIds
            );

            /**
             * @brief 向当前模型部件中插入一个几何对象
             */
            GeometryType::Pointer CreateNewGeometry(
                const std::string& rGeometryTypeName,
                GeometryType::PointsArrayType pGeometryNodes
            );

            /**
             * @brief 向当前模型部件中插入一个几何对象
             */
            GeometryType::Pointer CreateNewGeometry(
                const std::string& rGeometryTypeName,
                GeometryType::Pointer pGeometry
            );

            /**
             * @brief 向当前模型部件中插入一个几何对象
             */
            GeometryType::Pointer CreateNewGeometry(
                const std::string& rGeometryTypeName,
                const IndexType GeometryId,
                const std::vector<IndexType>& rGeometryNodeIds
            );
            

            /**
             * @brief 向当前模型部件中插入一个几何对象
             */
            GeometryType::Pointer CreateNewGeometry(
                const std::string& rGeometryTypeName,
                const IndexType GeometryId,
                GeometryType::PointsArrayType pGeometryNodes
            );
            

            /**
             * @brief 向当前模型部件中插入一个几何对象
             */
            GeometryType::Pointer CreateNewGeometry(
                const std::string& rGeometryTypeName,
                const IndexType GeometryId,
                GeometryType::Pointer pGeometry
            );
            

            /**
             * @brief 向当前模型部件中插入一个几何对象
             */
            GeometryType::Pointer CreateNewGeometry(
                const std::string& rGeometryTypeName,
                const std::string& rGeometryIdentifierName,
                const std::vector<IndexType>& rGeometryNodeIds
            );

            /**
             * @brief 向当前模型部件中插入一个几何对象
             */
            GeometryType::Pointer CreateNewGeometry(
                const std::string& rGeometryTypeName,
                const std::string& rGeometryIdentifierName,
                GeometryType::PointsArrayType pGeometryNodes
            );

            /**
             * @brief 向当前模型部件中插入一个几何对象
             */
            GeometryType::Pointer CreateNewGeometry(
                const std::string& rGeometryTypeName,
                const std::string& rGeometryIdentifierName,
                GeometryType::Pointer pGeometry
            );

            /**
             * @brief 向当前模型部件中添加一个几何对象
             */
            void AddGeometry(typename GeometryType::Pointer pNewGeometry);

            /**
             * @brief 向当前模型部件中添加一系列几何对象
             */
            void AddGeometries(const std::vector<IndexType>& GeometriesIds);

            /**
             * @brief 向当前模型部件中添加一系列几何对象
             */
            template<typename TIteratorType>
            void AddGeometries(TIteratorType geometries_begin, TIteratorType geometries_end){
                QUEST_TRY
                std::vector<GeometryType::Pointer> aux, aux_root;
                ModelPart* p_root_model_part = &this->GetRootModelPart();

                for(IIteratorType it = geometries_begin; it!= geometries_end; ++it){                    
                    auto it_found = p_root_model_part->Geometries().find(it->Id());
                    if(it_found == p_root_model_part->GeometriesEnd()){
                        aux_root.push_back( it.operator->() );
                        aux.push_back( it.operator->() );
                    } else {
                        if(&(*it_found) != &(*it)){
                            QUEST_ERROR << "aattempting to add a new geometry with Id: " << it_found->Id() << ", unfortunately a geometry with the same Id already exists" << std::endl;
                        } else {
                            aux.push_back( it.operator->() );
                        }
                    }
                }

                for(auto& p_geom: aux_root){
                    p_root_model_part->AddGeometry(p_geom);
                }

                ModelPart* p_current_part = this;
                while(p_current_part->IsSubModelPart()){
                    for(auto& p_geom: aux){
                        p_current_part->AddGeometry(p_geom);
                    }

                    p_current_part = &(p_current_part->GetParentModelPart());
                }

                QUEST_CATCH("")
            }

            /**
             * @brief 获取当前模型部件中指定ID的几何对象指针
             */
            typename GeometryType::Pointer pGetGeometry(IndexType GeometryId){
                return mGeometries.pGetGeometry(GeometryId);
            }

            /**
             * @brief 获取当前模型部件中指定ID的几何对象指针
             */
            const typename GeometryType::Pointer pGetGeometry(IndexType GeometryId) const{
                return mGeometries.pGetGeometry(GeometryId);
            }

            /**
             * @brief 获取当前模型部件中指定名称的几何对象指针
             */
            typename GeometryType::Pointer pGetGeometry(std::string GeometryName){
                return mGeometries.pGetGeometry(GeometryName);
            }

            /**
             * @brief 获取当前模型部件中指定名称的几何对象指针
             */
            const typename GeometryType::Pointer pGetGeometry(std::string GeometryName) const{
                return mGeometries.pGetGeometry(GeometryName);
            }

            /**
             * @brief 获取当前模型部件中指定ID的几何对象
             */
            GeometryType& GetGeometry(IndexType GeometryId){
                return mGeometries.GetGeometry(GeometryId);
            }

            /**
             * @brief 获取当前模型部件中指定ID的几何对象
             */
            const GeometryType& GetGeometry(IndexType GeometryId) const{
                return mGeometries.GetGeometry(GeometryId);
            }

            /**
             * @brief 获取当前模型部件中指定名称的几何对象
             */
            GeometryType& GetGeometry(std::string GeometryName){
                return mGeometries.GetGeometry(GeometryName);
            }

            /**
             * @brief 获取当前模型部件中指定名称的几何对象
             */
            const GeometryType& GetGeometry(std::string GeometryName) const{
                return mGeometries.GetGeometry(GeometryName);
            }

            /**
             * @brief 判断当前模型部件中是否有指定ID的几何对象
             */
            bool HasGeometry(IndexType GeometryId) const{
                return mGeometries.HasGeometry(GeometryId);
            }

            /**
             * @brief 判断当前模型部件中是否有指定名称的几何对象
             */
            bool HasGeometry(std::string GeometryName) const{
                return mGeometries.HasGeometry(GeometryName);
            }

            /**
             * @brief 移除当前模型部件中的几何对象（该层级和所有子层级）
             */
            void RemoveGeometry(IndexType GeometryId);

            /**
             * @brief 移除当前模型部件中的几何对象（该层级和所有子层级）
             */
            void RemoveGeometry(std::string GeometryName);

            /**
             * @brief 移除当前模型部件中的几何对象（所有层级）
             */
            void RemoveGeometryFromAllLevels(IndexType GeometryId);

            /**
             * @brief 移除当前模型部件中的几何对象（所有层级）
             */
            void RemoveGeometryFromAllLevels(std::string GeometryName);

            /**
             * @brief 获取当前模型部件中几何对象容器的头元素的迭代器
             */
            GeometryIterator GeometriesBegin(){
                return mGeometries.GeometriesBegin();
            }

            /**
             * @brief 获取当前模型部件中几何对象容器的头元素的迭代器
             */
            GeometryConstantIterator GeometriesBegin() const{
                return mGeometries.GeometriesBegin();
            }

            /**
             * @brief 获取当前模型部件中几何对象容器的尾元素的迭代器
             */
            GeometryIterator GeometriesEnd(){
                return mGeometries.GeometriesEnd();
            }

            /**
             * @brief 获取当前模型部件中几何对象容器的尾元素的迭代器
             */
            GeometryConstantIterator GeometriesEnd() const{
                return mGeometries.GeometriesEnd();
            }

            /**
             * @brief 获取当前模型部件中几何对象容器
             */
            GeometriesMapType& Geometries(){
                return mGeometries.Geometries();
            }

            /**
             * @brief 获取当前模型部件中几何对象容器
             */
            const GeometriesMapType& Geometries() const{
                return mGeometries.Geometries();
            }

            /**
             * @brief 当前模型部件中子模型部件的数量
             */
            SizeType NumberOfSubModelParts() const{
                return mSubModelParts.size();
            }

            /**
             * @brief 在当前模型部件中创建子模型部件
             */
            ModelPart& CreateSubModelPart(const std::string& NewSubModelPartName);

            /**
             * @brief 获取当前模型部件中指定名称的子模型部件
             */
            ModelPart& GetSubModelPart(const std::string& SubModelPartName);

            /**
             * @brief 获取当前模型部件中指定名称的子模型部件
             */
            const ModelPart& GetSubModelPart(const std::string& SubModelPartName) const;

            /**
             * @brief 获取当前模型部件中指定名称的子模型部件指针
             */
            ModelPart* pGetSubModelPart(const std::string& SubModelPartName);

            /**
             * @brief 移除当前模型部件中指定名称的子模型部件
             */
            void RemoveSubModelPart(const std::string& ThisSubModelPartName);

            /**
             * @brief 移出当前模型部件中指定对象的子模型部件
             */
            void RemoveSubModelPart(ModelPart& ThisSubModelPart);

            /**
             * @brief 返回当前模型部件中子模型部件容器的头元素的迭代器
             */
            SubModelPartIterator SubModelPartsBegin(){
                return mSubModelParts.begin();
            }

            /**
             * @brief 返回当前模型部件中子模型部件容器的尾元素的迭代器
             */
            SubModelPartIterator SubModelPartsEnd(){
                return mSubModelParts.end();
            }

            /**
             * @brief 返回当前模型部件中子模型部件容器的头元素的迭代器
             */
            SubModelPartConstantIterator SubModelPartsBegin() const{
                return mSubModelParts.begin();
            }

            /**
             * @brief 返回当前模型部件中子模型部件容器的尾元素的迭代器
             */
            SubModelPartConstantIterator SubModelPartsEnd() const{
                return mSubModelParts.end();
            }

            /**
             * @brief 返回当前模型部件中子模型部件容器
             */
            SubModelPartsContainerType& SubModelParts(){
                return mSubModelParts;
            }

            /**
             * @brief 返回当前模型部件中子模型部件容器
             */
            const SubModelPartsContainerType& SubModelParts() const{
                return mSubModelParts;
            }

            /**
             * @brief 获取当前模型部件的父模型部件
             */
            ModelPart& GetParentModelPart();

            /**
             * @brief 获取当前模型部件的父模型部件
             */
            const ModelPart& GetParentModelPart() const;

            /**
             * @brief 判断当前模型部件中是否有指定名称的子模型部件
             */
            bool HasSubModelPart(const std::string& ThisSubModelPartName) const;

            /**
             * @brief 返回当前模型部件中进程信息对象
             */
            ProcessInfo& GetProcessInfo(){
                return *mpProcessInfo;
            }

            /**
             * @brief 返回当前模型部件中进程信息对象
             */
            const ProcessInfo& GetProcessInfo() const{
                return *mpProcessInfo;
            }

            /**
             * @brief 返回当前模型部件中进程信息对象指针
             */
            ProcessInfo::Pointer pGetProcessInfo(){
                return mpProcessInfo;
            }

            /**
             * @brief 返回当前模型部件中进程信息对象指针
             */
            const ProcessInfo::Pointer pGetProcessInfo() const{
                return mpProcessInfo;
            }

            /**
             * @brief 设置当前模型部件中进程信息对象
             */
            void SetProcessInfo(ProcessInfo::Pointer pNewProcessInfo){
                mpProcessInfo = pNewProcessInfo;
            }

            /**
             * @brief 设置当前模型部件中进程信息对象
             */
            void SetProcessInfo(ProcessInfo& NewProcessInfo){
                *mpProcessInfo = NewProcessInfo;
            }

            /**
             * @brief 获取当前模型部件中的网格数量
             */
            SizeType NumberOfMeshes(){
                return mMeshes.size();
            }

            /**
             * @brief 获取当前模型部件中指定ID的网格指针
             */
            MeshType::Pointer pGetMesh(IndexType ThisIndex = 0){
                return mMeshes(ThisIndex);
            }

            /**
             * @brief 获取当前模型部件中指定ID的网格指针
             */
            const MeshType::Pointer pGetMesh(IndexType ThisIndex = 0) const{
                return mMeshes(ThisIndex);
            }

            /**
             * @brief 获取当前模型部件中指定ID的网格
             */
            MeshType& GetMesh(IndexType ThisIndex = 0){
                return mMeshes[ThisIndex];
            }

            /**
             * @brief 获取当前模型部件中指定ID的网格
             */
            const MeshType& GetMesh(IndexType ThisIndex = 0) const{
                return mMeshes[ThisIndex];
            }

            /**
             * @brief 获取当前模型部件中所有网格的容器
             */
            MeshesContainerType& GetMeshes(){
                return mMeshes;
            }

            /**
             * @brief 获取当前模型部件中所有网格的容器
             */
            const MeshesContainerType& GetMeshes() const{
                return mMeshes;
            }

            /**
             * @brief 获取当前模型部件的名称
             */
            std::string Name(){
                return mName;
            }

            /**
             * @brief 获取当前模型部件的名称
             */
            const std::string& Name() const{
                return mName;
            }

            /**
             * @brief 获取通讯器
             */
            Communicator& GetCommunicator(){
                return *mpCommunicator;
            }

            /**
             * @brief 获取通讯器
             */
            const Communicator& GetCommunicator() const{
                return *mpCommunicator;
            }

            /**
             * @brief 获取通讯器指针
             */
            Communicator::Pointer pGetCommunicator(){
                return mpCommunicator;
            }

            /**
             * @brief 获取通讯器指针
             */
            void SetCommunicator(Communicator::Pointer pNewCommunicator){
                mpCommunicator = pNewCommunicator;
            }

            /**
             * @brief 获取当前模型部件的完整名称
             */
            std::string FullName() const{
                std::string full_name = this->Name();
                if(this->IsSubModelPart()){
                    full_name = this->GetParentModelPart().FullName() + "." + full_name;
                }
                return full_name;
            }

            /**
             * @brief 获取当前模型部件所有子模型部件的名称
             */
            std::vector<std::string> GetSubModelPartNames() const;

            /**
             * @brief 设置数据库缓冲区的大小
             */
            void SetBufferSize(IndexType NewBufferSize);

            /**
             * @brief 获取数据库缓冲区的大小
             */
            IndexType GetBufferSize() const{
                return mBufferSize;
            }

            /**
             * @brief 该方法检查当前模型部件的状态，并返回错误代码
             */
            virtual int Check() const;

            /**
             * @brief 判断是否为子模型部件
             */
            bool IsSubModelPart() const{
                return mpParentModelPart != nullptr;
            }

            /**
             * @brief 判断是否为分布式模型部件
             */
            bool IsDistributed() const{
                return mpCommunicator->IsDistributed();
            }


            std::string Info() const override;


            void PrintInfo(std::ostream& rOstream) const override;


            void PrintData(std::ostream& rOstream) const override;


            virtual void PrintInfo(std::ostream& rOstream, const std::string& rPrefixString) const;


            virtual void PrintData(std::ostream& rOstream, const std::string& rPrefixString) const;

        protected:

        private:
            friend class Model;

            /**
             * @brief 构造函数
             */
            ModelPart(VariablesList::Pointer pVariableList, Model& rOwnerModel);

            /**
             * @Brief 构造函数
             */
            ModelPart(const std::string& NewName, VariablesList::Pointer pVariableList, Model& rOwnerModel);

            /**
             * @brief 构造函数
             */
            ModelPart(const std::string& NewName, IndexType NewBufferSize, VariablesList::Pointer pVariableList, Model& rOwnerModel);

            /**
             * @brief 拷贝构造函数
             */
            ModelPart(const ModelPart& rOther) = delete;

            /**
             * @brief 该方法修剪字符串的不同部分，以便递归地访问任何子属性
             * @param rStringName 要修剪的给定名称
             * @return 索引列表
             */
            std::vector<IndexType> TrimComponentName(const std::string& rStringName) const{
                std::vector<IndexType> list_indexes;

                std::stringstream ss(rStringName);
                for(std::string index_string; std::getline(ss, index_string, '.');){
                    list_indexes.push_back(std::stoi(index_string));
                }

                QUEST_ERROR_IF(list_indexes.size() == 0) << "Properties:: Empty list of indexes when reading suproperties" << std::endl;

                return list_indexes;
            }

            /**
             * @brief 该方法设置当前模型部分所属子模型部分的缓冲区大小（递归地）
             * @param NewBufferSize 要设置的新缓冲区大小
             */
            void SetBufferSizeSubModelParts(IndexType NewBufferSize);

            /**
             * @brief 设置当前模型部分的父模型
             */
            void SetParentModelPart(ModelPart* pParentModelPart){
                mpParentModelPart = pParentModelPart;
            }

            /**
             * @brief 将源容器中的实体（如节点、单元、条件等）添加到目标容器中
             */
            template<typename TEntitiesContainerType>
            void AddEntities(const TEntitiesContainerType& Source, TEntitiesContainerType& rDestination, Flags Options){

            }

            /**
             * @brief 如果一个子模型部分不存在，则发出适当的错误信息
             * @param rSubModelPartName 不存在的子模型部分的名称
             */
            [[ noreturn ]] void ErrorNonExistingSubModelPart(const std::string& rSubModelPartName) const;


            friend class Serializer;


            void save(Serializer& rSerializer) const override;


            void load(Serializer& rSerializer) override;

        private:
            /**
             * @brief 存储模型部件的名称
             */
            std::string mName;

            /**
             * @brief 数据库的缓存区的大小
             */
            IndexType mBufferSize;

            /**
             * @brief 指向存储当前模型部件进程信息的指针（如计算过程中的数值或参数设置）
             */
            ProcessInfo::Pointer mpProcessInfo;

            /**
             * @brief 包含该模型部件的所有表格的容器
             * @details PointerVectorMap<SizeType, TableType>
             */
            TablesContainerType mTables;

            /**
             * @brief 存储该模型部件中所有Mesh对象的容器
             * @details PointerVector<MeshType>
             */
            MeshesContainerType mMeshes;

            /**
             * @brief 存储该模型部件中所有几何体的容器
             * @details GeometryContainer<GeometryType>
             */
            GeometryContainerType mGeometries;

            /**
             * @brief 变量列表
             */
            VariablesList::Pointer mpVariablesList;

            /**
             * @brief 通信器
             */
            Communicator::Pointer mpCommunicator;

            /**
             * @brief 当前模型部件的父模型部件
             */
            ModelPart* mpParentModelPart = nullptr;

            /**
             * @brief 存储子模型部件的容器
             * @details PointerHashMapSet<ModelPart, std::hash< std::string >, GetModelPartName, Kratos::shared_ptr<ModelPart> >
             */
            SubModelPartsContainerType mSubModelParts;

            /**
             * @brief 包含该模型部件的模型
             */
            Model& mrModel;
    };


    QUEST_API(QUEST_CORE) inline std::istream& operator >> (std::istream& rIstream, ModelPart& rThis){
        return rIstream;
    }


    QUEST_API(QUEST_CORE) inline std::ostream& operator << (std::ostream& rOstream, const ModelPart& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }

} // namespace Quest

#endif //QUEST_MODEL_PART_HPP