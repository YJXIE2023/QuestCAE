#ifndef QUEST_MESH_HPP
#define QUEST_MESH_HPP

// 系统头文件
#include <string>
#include <iostream>
#include <sstream>
#include <cstddef>

// 项目头文件
#include "includes/define.hpp"
#include "container/pointer_vector_set.hpp"
#include "container/pointer_vector_map.hpp"
#include "includes/indexed_object.hpp"
#include "geometries/geometry.hpp"
#include "container/flags.hpp"
#include "container/data_value_container.hpp"
#include "includes/master_slave_constraint.hpp"

namespace Quest{

    /**
     * @class Mesh
     * @brief Mesh 是数据结构中的第二级抽象，包含节点、单元、条件及其属性
     * @details Mesh 时所有类型实体的完整集合，程序可以在不适用任何额外数据的前提下进行网格细化、材料优化、网格变形的处理
     */
    template<typename TNodeType, typename TPropertiesType, typename TElemetType, typename TConditionType>
    class Mesh: public DataValueContainer,public Flags{
        public:
            QUEST_CLASS_POINTER_DEFINITION(Mesh);

            using IndexType = std::size_t;
            using SizeType = std::size_t;
            using NodeType = TNodeType;
            using PropertiesType = TPropertiesType;
            using GeometryType = Geometry<NodeType>;
            using ElementType = TElemetType;
            using ConditionType = TConditionType;
            using MasterSlaveConstraintType = MasterSlaveConstriant;
            using MeshType = Mesh<TNodeType, TPropertiesType, TElemetType, TConditionType>;
            using NodesContainerType = PointerVectorSet<
                NodeType, 
                IndexedObject, 
                std::less<typename IndexTypeObject::result_type>,
                std::equal_to<typename IndexTypeObject::result_type>,
                typename NodeType::Pointer,
                std::vector<typename NodeType::Pointer>
            >;

            using NodeIterator = typename NodesContainerType::iterator;
            using NodeConstantIterator = typename NodesContainerType::const_iterator;
            using PropertiesContainerType = PointerVectorSet<PropertiesType, IndexedObject>;
            using PropertiesIterator = typename PropertiesContainerType::iterator;
            using PropertiesConstantIterator = typename PropertiesContainerType::const_iterator;
            using ElementsContainerType = PointerVectorSet<
                ElementType,
                IndexedObject,
                std::less<typename IndexTypeObject::result_type>,
                std::equal_to<typename IndexTypeObject::result_type>,
                typename ElementType::Pointer,
                std::vector<typename ElementType::Pointer>
            >;
            using ElementIterator = typename ElementsContainerType::iterator;
            using ElementConstantIterator = typename ElementsContainerType::const_iterator;
            using ConditionsContainerType = PointerVectorSet<
                ConditionType,
                IndexedObject,
                std::less<typename IndexTypeObject::result_type>,
                std::equal_to<typename IndexTypeObject::result_type>,
                typename ConditionType::Pointer,
                std::vector<typename ConditionType::Pointer>
            >;
            using ConditionIterator = typename ConditionsContainerType::iterator;
            using ConditionConstantIterator = typename ConditionsContainerType::const_iterator;
            using MasterSlaveConstraintContainerType = PointerVectorSet<MasterSlaveConstraintType, IndexedObject>;
            using MasterSlaveConstraintIteratorType = typename MasterSlaveConstraintContainerType::iterator;
            using MasterSlaveConstraintConstantIteratorType = typename MasterSlaveConstraintContainerType::const_iterator;

        public:
            /**
             * @brief 构造函数
             */
            Mesh():
                Flags(),
                mpNodes(new NodesContainerType()),
                mpProperties(new PropertiesContainerType()),    
                mpElements(new ElementsContainerType()),
                mpConditions(new ConditionsContainerType()),
                mpMasterSlaveConstraints(new MasterSlaveConstraintContainerType()) {}

            /**
             * @brief 复制构造函数
             */
            Mesh(const Mesh& rOther):
                Flags(rOther),
                mpNodes(rOther.mpNodes),
                mpProperties(rOther.mpProperties),  
                mpElements(rOther.mpElements),
                mpConditions(rOther.mpConditions),
                mpMasterSlaveConstraints(rOther.mpMasterSlaveConstraints) {}

            /**
             * @brief 构造函数
             * @param NewNode 节点集合
             * @param NewProperties 单元属性集合
             * @param NewElements 单元集合
             * @param NewConditions 条件集合
             * @param NewMasterSlaveConstraints 主从约束集合
             */
            Mesh(
                typename NodesContainerType::Pointer NewNode,
                typename PropertiesContainerType::Pointer NewProperties,
                typename ElementsContainerType::Pointer NewElements,
                typename ConditionsContainerType::Pointer NewConditions,
                typename MasterSlaveConstraintContainerType::Pointer NewMasterSlaveConstraints
            ):  Flags(),
                mpNodes(NewNode),
                mpProperties(NewProperties),
                mpElements(NewElements),
                mpConditions(NewConditions),
                mpMasterSlaveConstraints(NewMasterSlaveConstraints) {}

            /**
             * @brief 析构函数
             */
            ~Mesh() override {}

            /**
             * @brief 创建一个新的 Mesh 指针并克隆之前 Mesh 的数据
             */
            Mesh Clone(){
                typename NodesContainerType::Pointer p_nodes(new NodesContainerType(*mpNodes));
                typename PropertiesContainerType::Pointer p_properties(new PropertiesContainerType(*mpProperties)); 
                typename ElementsContainerType::Pointer p_elements(new ElementsContainerType(*mpElements)); 
                typename ConditionsContainerType::Pointer p_conditions(new ConditionsContainerType(*mpConditions)); 
                typename MasterSlaveConstraintContainerType::Pointer p_constraints(new MasterSlaveConstraintContainerType(*mpMasterSlaveConstraints)); 

                return Mesh(p_nodes, p_properties, p_elements, p_conditions, p_constraints);
            }

            /**
             * @brief 清空 Mesh 数据
             */
            void Clear(){
                Flags::Clear();
                DataValueContainer::Clear();
                mpNodes->clear();
                mpProperties->clear();
                mpElements->clear();
                mpConditions->clear();
                mpMasterSlaveConstraints->clear();
            }

            /**
             * @brief 返回几何体的维度
             */
            SizeType WorkingSpaceDimension() const{
                SizeType dimension = 3;

                if(NumberOfElements() != 0){
                    dimension = (mpElements->begin())->GetGeometry().WorkingSpaceDimension();
                } else if(NumberOfConditions() != 0){
                    dimension = (mpConditions->begin())->GetGeometry().WorkingSpaceDimension();
                } else if(NumberOfNodes() != 0){
                    dimension = (mpNodes->begin())->Dimension();
                }

                return dimension;
            }

            /**
             * @brief 返回节点的数量
             */
            SizeType NumberOfNodes() const{
                return mpNodes->size();
            }

            /**
             * @brief 添加一个新节点
             */
            void AddNode(typename NodeType::Pointer pNewNode){
                mpNodes->insert(mpNodes->begin(), pNewNode);
            }

            /**
             * @brief 返回指定ID的节点指针
             * @param NodeId 节点ID
             */
            typename NodeType::Pointer pGetNode(IndexType NodeId){
                auto i = mpNodes->find(NodeId);
                QUEST_ERROR_IF(i == mpNodes->end()) << "Node index not found: " << NodeId << "." << std::endl;
                return *i.base();
            }

            /**
             * @brief 返回指定ID的节点指针
             * @param NodeId 节点ID
             */
            const typename NodeType::Pointer pGetNode(const IndexType NodeId) const{
                const auto& r_nodes = *mpNodes;
                auto i = r_nodes.find(NodeId);
                QUEST_ERROR_IF(i == r_nodes.end()) << "Node index not found: " << NodeId << "." << std::endl;
                return *i.base();
            }

            /**
             * @brief 返回指定ID的节点对象
             * @param NodeId 节点ID
             */
            NodeType& GetNode(IndexType NodeId){
                auto i = mpNodes->find(NodeId);
                QUEST_ERROR_IF(i == mpNodes->end()) << "Node index not found: " << NodeId << "." << std::endl;
                return *i;
            }

            /**
             * @brief 返回指定ID的节点对象
             * @param NodeId 节点ID
             */
            const NodeType& GetNode(IndexType NodeId) const{
                const auto& r_nodes = *mpNodes;
                auto i = r_nodes.find(NodeId);
                QUEST_ERROR_IF(i == r_nodes.end()) << "Node index not found: " << NodeId << "." << std::endl;
                return *i;
            }

            /**
             * @brief 删除指定ID的节点
             * @param NodeId 节点ID
             */
            void RemoveNode(IndexType NodeId){
                mpNodes->erase(NodeId);
            }

            /**
             * @brief 删除指定节点对象
             */
            void RemoveNode(NodeType& ThisNode){
                mpNodes->erase(ThisNode.Id());
            }

            /**
             * @brief 删除指定节点指针
             */
            void RemoveNode(typename NodeType::Pointer pThisNode){
                mpNodes->erase(pThisNode->Id());
            }

            /**
             * @brief 返回指向节点容器首元素的迭代器
             */
            NodeIterator NodesBegin(){
                return mpNodes->begin();
            }

            /**
             * @brief 返回指向节点容器首元素的迭代器
             */
            NodeConstantIterator NodesBegin() const{
                return mpNodes->begin();
            }

            /**
             * @brief 返回指向节点容器尾元素的迭代器
             */
            NodeIterator NodesEnd(){
                return mpNodes->end();
            }

            /**
             * @brief 返回指向节点容器尾元素的迭代器
             */
            NodeConstantIterator NodesEnd() const{
                return mpNodes->end();
            }

            /**
             * @brief 返回节点容器
             */
            NodesContainerType& Nodes(){
                return *mpNodes;
            }

            /**
             * @brief 返回节点容器
             */
            const NodesContainerType& Nodes() const{
                return *mpNodes;
            }

            /**
             * @brief 返回指向节点容器的指针
             */
            typename NodesContainerType::Pointer pNodes(){
                return mpNodes;
            }

            /**
             * @brief 设置节点容器指针
             */
            void SetNodes(typename NodesContainerType::Pointer pOtherNodes){
                mpNodes = pOtherNodes;
            }

            /**
             * @brief 返回实际存储节点的容器（std::vector）
             */
            typename NodesContainerType::ContainerType& NodesArray(){
                return mpNodes->GetContainer();
            }

            /**
             * @brief 判断是否存在指定ID的节点
             */
            bool HasNode(IndexType NodeId) const{
                const auto& r_nodes = *mpNodes;
                return r_nodes.find(NodeId)!= r_nodes.end();
            }

            /**
             * @brief 返回单元属性的数量
             */
            SizeType NumberOfProperties() const{
                return mpProperties->size();
            }

            /**
             * @brief 添加一个新单元属性
             */
            void AddProperties(typename PropertiesType::Pointer pNewProperties){
                mpProperties->insert(mpProperties->begin(), pNewProperties);
            }

            /**
             * @brief 获取指定ID的单元属性指针
             */
            typename PropertiesType::Pointer pGetProperties(IndexType PropertiesId){
                return (*mpProperties)(PropertiesId);
            }

            /**
             * @brief 获取指定ID的单元属性对象
             */
            PropertiesType& GetProperties(IndexType PropertiesId){
                return (*mpProperties)[PropertiesId];
            }

            /**
             * @brief 移出指定ID的单元属性
             */
            void RemoveProperties(IndexType PropertiesId){
                mpProperties->erase(PropertiesId);
            }

            /**
             * @brief 移出指定单元属性对象
             */
            void RemoveProperties(PropertiesType& ThisProperties){
                mpProperties->erase(ThisProperties.Id());
            }

            /**
             * @brief 移出指定单元属性指针
             */
            void RemoveProperties(typename PropertiesType::Pointer pThisProperties){
                mpProperties->erase(pThisProperties->Id());
            }

            /**
             * @brief 返回指向单元属性容器首元素的迭代器
             */
            PropertiesIterator PropertiesBegin(){
                return mpProperties->begin();
            }

            /**
             * @brief 返回指向单元属性容器首元素的迭代器
             */
            PropertiesConstantIterator PropertiesBegin() const{
                return mpProperties->begin();
            }

            /**
             * @brief 返回指向单元属性容器尾元素的迭代器
             */
            PropertiesIterator PropertiesEnd(){
                return mpProperties->end();
            }

            /**
             * @brief 返回指向单元属性容器尾元素的迭代器
             */
            PropertiesConstantIterator PropertiesEnd() const{
                return mpProperties->end();
            }

            /**
             * @brief 返回单元属性容器
             */
            PropertiesContainerType& Properties(){
                return *mpProperties;
            }

            /**
             * @brief 返回单元属性容器
             */
            const PropertiesContainerType& Properties() const{
                return *mpProperties;
            }

            /**
             * @brief 返回指向单元属性容器的指针
             */
            typename PropertiesContainerType::Pointer pProperties(){
                return mpProperties;
            }

            /**
             * @brief 设置单元属性容器指针
             */
            void SetProperties(typename PropertiesContainerType::Pointer pOtherProperties){
                mpProperties = pOtherProperties;
            }

            /**
             * @brief 返回实际存储单元属性的容器（std::vector）
             */
            typename PropertiesContainerType::ContainerType& PropertiesArray(){
                return mpProperties->GetContainer();
            }

            /**
             * @brief 判断是否存在指定ID的单元属性
             */
            bool HasProperties(IndexType PropertiesId) const{
                const auto& r_properties = *mpProperties;
                return r_properties.find(PropertiesId)!= r_properties.end();
            }

            /**
             * @brief 返回单元的数量
             */
            SizeType NumberOfElements() const{
                return mpElements->size();
            }

            /**
             * @brief 添加一个新单元
             */
            void AddElement(typename ElementType::Pointer pNewElement){
                mpElements->insert(mpElements->begin(), pNewElement);
            }

            /**
             * @brief 获取指定ID的单元指针
             */
            typename ElementType::Pointer pGetElement(IndexType ElementId){
                auto i = mpElements->find(ElementId);
                QUEST_ERROR_IF(i == mpElements->end()) << "Element index not found: " << ElementId << "." << std::endl;
                return *i.base();
            }

            /**
             * @brief 获取指定ID的单元指针
             */
            const typename ElementType::Pointer pGetElement(const IndexType ElementId) const{
                const auto& r_elements = *mpElements;
                auto i = r_elements.find(ElementId);
                QUEST_ERROR_IF(i == r_elements.end()) << "Element index not found: " << ElementId << "." << std::endl;
                return *i.base();
            }

            /**
             * @brief 获取指定ID的单元对象
             */
            ElementType& GetElement(IndexType ElementId){
                auto i = mpElements->find(ElementId);
                QUEST_ERROR_IF(i == mpElements->end()) << "Element index not found: " << ElementId << "." << std::endl;
                return *i;
            }

            /**
             * @brief 获取指定ID的单元对象
             */
            const ElementType& GetElement(IndexType ElementId) const{
                const auto& r_elements = *mpElements;
                auto i = r_elements.find(ElementId);
                QUEST_ERROR_IF(i == r_elements.end()) << "Element index not found: " << ElementId << "." << std::endl;
                return *i;
            }

            /**
             * @brief 删除指定ID的单元
             */
            void RemoveElement(IndexType ElementId){
                mpElements->erase(ElementId);
            }

            /**
             * @brief 删除指定单元对象
             */
            void RemoveElement(ElementType& ThisElement){
                mpElements->erase(ThisElement.Id());
            }

            /**
             * @brief 删除指定单元指针
             */
            void RemoveElement(typename ElementType::Pointer pThisElement){
                mpElements->erase(pThisElement->Id());
            }

            /**
             * @brief 返回指向单元容器首元素的迭代器
             */
            ElementIterator ElementsBegin(){
                return mpElements->begin();
            }

            /**
             * @brief 返回指向单元容器首元素的迭代器
             */
            ElementConstantIterator ElementsBegin() const{
                return mpElements->begin();
            }

            /**
             * @brief 返回指向单元容器尾元素的迭代器
             */
            ElementIterator ElementsEnd(){
                return mpElements->end();
            }

            /**
             * @brief 返回指向单元容器尾元素的迭代器
             */
            ElementConstantIterator ElementsEnd() const{
                return mpElements->end();
            }

            /**
             * @brief 返回单元容器
             */
            ElementsContainerType& Elements(){
                return *mpElements;
            }

            /**
             * @brief 返回单元容器
             */
            const ElementsContainerType& Elements() const{
                return *mpElements;
            }

            /**
             * @brief 返回单元容器指针
             */
            typename ElementsContainerType::Pointer pElements(){
                return mpElements;
            }

            /**
             * @brief 设置单元容器指针
             */
            void SetElements(typename ElementsContainerType::Pointer pOtherElements){
                mpElements = pOtherElements;
            }

            /**
             * @brief 返回实际存储单元的容器（std::vector）
             */
            typename ElementsContainerType::ContainerType& ElementsArray(){
                return mpElements->GetContainer();
            }

            /**
             * @brief 判断是否存在指定ID的单元
             */
            bool HasElement(IndexType ElementId) const{
                const auto& r_elements = *mpElements;
                return r_elements.find(ElementId)!= r_elements.end();
            }

            /**
             * @brief 返回条件的数量
             */
            SizeType NumberOfConditions() const{
                return mpConditions->size();
            }

            /**
             * @brief 添加一个新条件
             */
            void AddCondition(typename ConditionType::Pointer pNewCondition){
                mpConditions->insert(mpConditions->begin(), pNewCondition);
            }

            /**
             * @brief 获取指定ID的条件指针
             */
            typename ConditionType::Pointer pGetCondition(IndexType ConditionId){
                auto i = mpConditions->find(ConditionId);
                QUEST_ERROR_IF(i == mpConditions->end()) << "Condition index not found: " << ConditionId << "." << std::endl;
                return *i.base();
            }

            /**
             * @brief 获取指定ID的条件指针
             */
            const typename ConditionType::Pointer pGetCondition(const IndexType ConditionId) const{
                const auto& r_conditions = *mpConditions;
                auto i = r_conditions.find(ConditionId);
                QUEST_ERROR_IF(i == r_conditions.end()) << "Condition index not found: " << ConditionId << "." << std::endl;
                return *i.base();
            }

            /**
             * @brief 获取指定ID的条件对象
             */
            ConditionType& GetCondition(IndexType ConditionId){
                auto i = mpConditions->find(ConditionId);
                QUEST_ERROR_IF(i == mpConditions->end()) << "Condition index not found: " << ConditionId << "." << std::endl;
                return *i;
            }

            /**
             * @brief 获取指定ID的条件对象
             */
            const ConditionType& GetCondition(IndexType ConditionId) const{
                const auto& r_conditions = *mpConditions;
                auto i = r_conditions.find(ConditionId);
                QUEST_ERROR_IF(i == r_conditions.end()) << "Condition index not found: " << ConditionId << "." << std::endl;
                return *i;
            }

            /**
             * @brief 删除指定ID的条件
             */
            void RemoveCondition(IndexType ConditionId){
                mpConditions->erase(ConditionId);
            }

            /**
             * @brief 删除指定条件对象
             */
            void RemoveCondition(ConditionType& ThisCondition){
                mpConditions->erase(ThisCondition.Id());
            }

            /**
             * @brief 删除指定条件指针
             */
            void RemoveCondition(typename ConditionType::Pointer pThisCondition){
                mpConditions->erase(pThisCondition->Id());
            }

            /**
             * @brief 返回指向条件容器首元素的迭代器
             */
            ConditionIterator ConditionsBegin(){
                return mpConditions->begin();
            }

            /**
             * @brief 返回指向条件容器首元素的迭代器
             */
            ConditionConstantIterator ConditionsBegin() const{
                return mpConditions->begin();
            }

            /**
             * @brief 返回指向条件容器尾元素的迭代器
             */
            ConditionIterator ConditionsEnd(){
                return mpConditions->end();
            }

            /**
             * @brief 返回指向条件容器尾元素的迭代器
             */
            ConditionConstantIterator ConditionsEnd() const{
                return mpConditions->end();
            }

            /**
             * @brief 返回条件容器
             */
            ConditionsContainerType& Conditions(){
                return *mpConditions;
            }

            /**
             * @brief 返回条件容器
             */
            const ConditionsContainerType& Conditions() const{
                return *mpConditions;
            }

            /**
             * @brief 返回条件容器指针
             */
            typename ConditionsContainerType::Pointer pConditions(){
                return mpConditions;
            }

            /**
             * @brief 返回条件容器指针
             */
            void SetConditions(typename ConditionsContainerType::Pointer pOtherConditions){
                mpConditions = pOtherConditions;
            }

            /**
             * @brief 返回实际存储条件的容器（std::vector）
             */
            typename ConditionsContainerType::ContainerType& ConditionsArray(){
                return mpConditions->GetContainer();
            }

            /**
             * @brief 判断是否存在指定ID的条件
             */
            bool HasCondition(IndexType ConditionId) const{
                const auto& r_conditions = *mpConditions;
                return r_conditions.find(ConditionId)!= r_conditions.end();
            }

            /**
             * @brief 返回主从约束对的数量
             */
            SizeType NumberOfMasterSlaveConstraints() const{
                return mpMasterSlaveConstraints->size();
            }

            /**
             * @brief 添加一个新主从约束对
             */
            bool AddMasterSlaveConstraint(typename MasterSlaveConstraintType::Pointer pNewMasterSlaveConstraint){
                const auto it_existing_constraint = mpMasterSlaveConstraints->find(pNewMasterSlaveConstraint->Id());
                if(it_existing_constraint == mpMasterSlaverConstraints->end()){
                    const auto it_insert_position = mpMasterSlaveConstraints->begin();
                    mpMasterSlaveConstraints->insert(it_insert_position, pNewMasterSlaveConstraint);
                    return true;
                }
                return false;
            }

            /**
             * @brief 获取指定ID的主从约束指针
             */
            typename MasterSlaveConstraintType::Pointer pGetMasterSlaveConstraint(IndexType MasterSlaveConstraintId){
                auto i = mpMasterSlaveConstraints->find(MasterSlaveConstraintId);
                QUEST_ERROR_IF(i == mpMasterSlaveConstraints->end()) << "MasterSlaveConstraint index not found: " << MasterSlaveConstraintId << "." << std::endl;
                return *i.base();
            }

            /**
             * @brief 获取指定ID的主从约束
             */
            MasterSlaveConstraintType& GetMasterSlaveConstraint(IndexType MasterSlaveConstraintId){
                auto i = mpMasterSlaveConstraints->find(MasterSlaveConstraintId);
                QUEST_ERROR_IF(i == mpMasterSlaveConstraints->end()) << "MasterSlaveConstraint index not found: " << MasterSlaveConstraintId << "." << std::endl;
                return *i;
            }

            /**
             * @brief 获取指定ID的主从约束
             */
            const MasterSlaveConstraintType& GetMasterSlaveConstraint(IndexType MasterSlaveConstraintId) const{
                auto i = mpMasterSlaveConstraints->find(MasterSlaveConstraintId);
                QUEST_ERROR_IF(i == mpMasterSlaveConstraints->end()) << "MasterSlaveConstraint index not found: " << MasterSlaveConstraintId << "." << std::endl;
                return *i;
            }

            /**
             * @brief 删除指定ID的主从约束
             */
            void RemoveMasterSlaveConstraint(IndexType MasterSlaveConstraintId){
                mpMasterSlaveConstraints->erase(MasterSlaveConstraintId);
            }

            /**
             * @brief 删除指定主从约束对象
             */
            void RemoveMasterSlaveConstraint(MasterSlaveConstraintType& ThisMasterSlaveConstraint){
                mpMasterSlaveConstraints->erase(ThisMasterSlaveConstraint.Id());
            }

            /**
             * @brief 删除指定主从约束指针
             */
            void RemoveMasterSlaveConstraint(typename MasterSlaveConstraintType::Pointer pThisMasterSlaveConstraint){
                mpMasterSlaveConstraints->erase(pThisMasterSlaveConstraint->Id());
            }

            /**
             * @brief 返回指向主从约束容器首元素的迭代器
             */
            MasterSlaveConstraintIterator MasterSlaveConstraintsBegin(){
                return mpMasterSlaveConstraints->begin();
            }

            /**
             * @brief 返回指向主从约束容器首元素的迭代器
             */
            MasterSlaveConstraintConstantIterator MasterSlaveConstraintsBegin() const{
                return mpMasterSlaveConstraints->begin();
            }

            /**
             * @brief 返回指向主从约束容器尾元素的迭代器
             */
            MasterSlaveConstraintIterator MasterSlaveConstraintsEnd(){
                return mpMasterSlaveConstraints->end();
            }

            /**
             * @brief 返回指向主从约束容器尾元素的迭代器
             */
            MasterSlaveConstraintConstantIterator MasterSlaveConstraintsEnd() const{
                return mpMasterSlaveConstraints->end();
            }

            /**
             * @brief 返回主从约束容器
             */
            MasterSlaveConstraintContainerType& MasterSlaveConstraints(){
                return *mpMasterSlaveConstraints;
            }

            /**
             * @brief 返回主从约束容器
             */
            const MasterSlaveConstraintContainerType& MasterSlaveConstraints() const{
                return *mpMasterSlaveConstraints;
            }

            /**
             * @brief 返回主从约束容器指针
             */
            typename MasterSlaveConstraintContainerType::Pointer pMasterSlaveConstraints(){
                return mpMasterSlaveConstraints;
            }

            /**
             * @brief 返回主从约束容器实际存储的容器（std::vector）
             */
            typename MasterSlaveConstraintContainerType::ContainerType& MasterSlaveConstraintsArray(){
                return mpMasterSlaveConstraints->GetContainer();
            }

            /**
             * @brief 判断是否存在指定ID的主从约束
             */
            bool HasMasterSlaveConstraint(IndexType MasterSlaveConstraintId) const{
                return mpMasterSlaveConstraints->find(MasterSlaveConstraintId) != mpMasterSlaveConstraints->end();
            }


            std::string Info() const override{
                return "Mesh";
            }


            void PrintInfo(std::ostream& rOstream) const override{
                rOstream << Info();
            }


            void PrintData(std::ostream& rOstream) const override{
                rOstream << "    Number of Nodes       : " << mpNodes->size() << std::endl;
                rOstream << "    Number of Properties  : " << mpProperties->size() << std::endl;
                rOstream << "    Number of Elements    : " << mpElements->size() << std::endl;
                rOstream << "    Number of Conditions  : " << mpConditions->size() << std::endl;
                rOstream << "    Number of Constraints : " << mpMasterSlaveConstraints->size() << std::endl;
            }


            virtual void PrintInfo(std::ostream& rOstream, const std::string& PrefixString) const{
                rOstream << PrefixString << Info();
            }


            virtual void PrintData(std::ostream& rOstream, const std::string& PrefixString) const{
                rOstream << PrefixString << "    Number of Nodes       : " << mpNodes->size() << std::endl;
                rOstream << PrefixString << "    Number of Properties  : " << mpProperties->size() << std::endl;
                rOstream << PrefixString << "    Number of Elements    : " << mpElements->size() << std::endl;
                rOstream << PrefixString << "    Number of Conditions  : " << mpConditions->size() << std::endl;
                rOstream << PrefixString << "    Number of Constraints : " << mpMasterSlaveConstraints->size() << std::endl;
            }

        protected:

        private:
            friend class Serializer;

            void save(Serializer& rSerializer) const override{
                QUEST_SERIALIZE_SAVE_BASE_CLASS(rSerializer, DataValueContainer);
                QUEST_SERIALIZE_SAVE_BASE_CLASS(rSerializer, Flags);
                rSerializer.save("Nodes",mpNodes);
                rSerializer.save("Properties",mpProperties);
                rSerializer.save("Elements",mpElements);
                rSerializer.save("Conditions",mpConditions);
                rSerializer.save("Constraints",mpMasterSlaveConstraints);
            }


            void load(Serializer& rSerializer) override{
                QUEST_SERIALIZE_LOAD_BASE_CLASS(rSerializer, DataValueContainer);
                QUEST_SERIALIZE_LOAD_BASE_CLASS(rSerializer, Flags);
                rSerializer.load("Nodes",mpNodes);
                rSerializer.load("Properties",mpProperties);
                rSerializer.load("Elements",mpElements);
                rSerializer.load("Conditions",mpConditions);
                rSerializer.load("Constraints",mpMasterSlaveConstraints);
            }

            Mesh& operator=(const Mesh& rOther){
                Flags::operator=(rOther);
                mpNodes = rOther.mpNodes;
                mpProperties = rOther.mpProperties;
                mpElements = rOther.mpElements;
                mpConditions = rOther.mpConditions;
                mpMasterSlaveConstraints = rOther.mpMasterSlaveConstraints;

                return *this;
            }

        private:
            /**
             * @brief 指向存储所有节点指针的容器
             * @details PointerVectorSet
             */
            typename NodesContainerType::Pointer mpNodes;

            /**
             * @brief 指向存储所有属性指针的容器
             * @details PointerVectorSet
             * 
             */
            typename PropertiesContainerType::Pointer mpProperties;

            /**
             * @brief 指向存储所有单元指针的容器
             * @details PointerVectorSet
             */
            typename ElementsContainerType::Pointer mpElements;

            /**
             * @brief 指向存储所有条件指针的容器
             * @details PointerVectorSet
             */
            typename ConditionsContainerType::Pointer mpConditions;

            /**
             * @brief 指向存储所有主从约束指针的容器
             * @details PointerVectorSet
             */
            typename MasterSlaveConstraintContainerType::Poienter mpMasterSlaveConstraints;

    };

    template<typename TNodeType, typename TPropertiesType, typename TElemetType, typename TConditionType>
    inline std::istream& operator >> (std::istream& rIstream, Mesh<TNodeType, TPropertiesType, TElemetType, TConditionType>& rThis);

    template<typename TNodeType, typename TPropertiesType, typename TElemetType, typename TConditionType>
    inline std::ostream& operator << (std::ostream& rOstream, const Mesh<TNodeType, TPropertiesType, TElemetType, TConditionType>& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }

} // namespace Quest

#endif //QUEST_MESH_HPP