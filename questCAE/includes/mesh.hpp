/*------------------------------------------------------------
用于组织和管理计算网格的核心组件
它提供了将节点、单元、条件及其属性结合在一起的抽象层，便于处理和操作
------------------------------------------------------------*/

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
            Mesh():
                Flags(),
                mpNodes(new NodesContainerType()),
                mpProperties(new PropertiesContainerType()),    
                mpElements(new ElementsContainerType()),
                mpConditions(new ConditionsContainerType()),
                mpMasterSlaveConstraints(new MasterSlaveConstraintContainerType()) {}


            Mesh(const Mesh& rOther):
                Flags(rOther),
                mpNodes(rOther.mpNodes),
                mpProperties(rOther.mpProperties),  
                mpElements(rOther.mpElements),
                mpConditions(rOther.mpConditions),
                mpMasterSlaveConstraints(rOther.mpMasterSlaveConstraints) {}


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


            ~Mesh() override {}


            Mesh Clone(){
                typename NodesContainerType::Pointer p_nodes(new NodesContainerType(*mpNodes));
                typename PropertiesContainerType::Pointer p_properties(new PropertiesContainerType(*mpProperties)); 
                typename ElementsContainerType::Pointer p_elements(new ElementsContainerType(*mpElements)); 
                typename ConditionsContainerType::Pointer p_conditions(new ConditionsContainerType(*mpConditions)); 
                typename MasterSlaveConstraintContainerType::Pointer p_constraints(new MasterSlaveConstraintContainerType(*mpMasterSlaveConstraints)); 

                return Mesh(p_nodes, p_properties, p_elements, p_conditions, p_constraints);
            }


            void Clear(){
                Flags::Clear();
                DataValueContainer::Clear();
                mpNodes->clear();
                mpProperties->clear();
                mpElements->clear();
                mpConditions->clear();
                mpMasterSlaveConstraints->clear();
            }


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


            SizeType NumberOfNodes() const{
                return mpNodes->size();
            }


            void AddNode(typename NodeType::Pointer pNewNode){
                mpNodes->insert(mpNodes->begin(), pNewNode);
            }


            typename NodeType::Pointer pGetNode(IndexType NodeId){
                auto i = mpNodes->find(NodeId);
                QUEST_ERROR_IF(i == mpNodes->end()) << "Node index not found: " << NodeId << "." << std::endl;
                return *i.base();
            }


            const typename NodeType::Pointer pGetNode(const IndexType NodeId) const{
                const auto& r_nodes = *mpNodes;
                auto i = r_nodes.find(NodeId);
                QUEST_ERROR_IF(i == r_nodes.end()) << "Node index not found: " << NodeId << "." << std::endl;
                return *i.base();
            }


            NodeType& GetNode(IndexType NodeId){
                auto i = mpNodes->find(NodeId);
                QUEST_ERROR_IF(i == mpNodes->end()) << "Node index not found: " << NodeId << "." << std::endl;
                return *i;
            }


            const NodeType& GetNode(IndexType NodeId) const{
                const auto& r_nodes = *mpNodes;
                auto i = r_nodes.find(NodeId);
                QUEST_ERROR_IF(i == r_nodes.end()) << "Node index not found: " << NodeId << "." << std::endl;
                return *i;
            }


            void RemoveNode(IndexType NodeId){
                mpNodes->erase(NodeId);
            }


            void RemoveNode(NodeType& ThisNode){
                mpNodes->erase(ThisNode.Id());
            }


            void RemoveNode(typename NodeType::Pointer pThisNode){
                mpNodes->erase(pThisNode->Id());
            }


            NodeIterator NodesBegin(){
                return mpNodes->begin();
            }


            NodeConstantIterator NodesBegin() const{
                return mpNodes->begin();
            }


            NodeIterator NodesEnd(){
                return mpNodes->end();
            }


            NodeConstantIterator NodesEnd() const{
                return mpNodes->end();
            }


            NodesContainerType& Nodes(){
                return *mpNodes;
            }


            const NodesContainerType& Nodes() const{
                return *mpNodes;
            }


            typename NodesContainerType::Pointer pNodes(){
                return mpNodes;
            }


            void SetNodes(typename NodesContainerType::Pointer pOtherNodes){
                mpNodes = pOtherNodes;
            }


            typename NodesContainerType::ContainerType& NodesArray(){
                return mpNodes->GetContainer();
            }


            bool HasNode(IndexType NodeId) const{
                const auto& r_nodes = *mpNodes;
                return r_nodes.find(NodeId)!= r_nodes.end();
            }


            SizeType NumberOfProperties() const{
                return mpProperties->size();
            }


            void AddProperties(typename PropertiesType::Pointer pNewProperties){
                mpProperties->insert(mpProperties->begin(), pNewProperties);
            }


            typename PropertiesType::Pointer pGetProperties(IndexType PropertiesId){
                return (*mpProperties)(PropertiesId);
            }


            PropertiesType& GetProperties(IndexType PropertiesId){
                return (*mpProperties)[PropertiesId];
            }


            void RemoveProperties(IndexType PropertiesId){
                mpProperties->erase(PropertiesId);
            }


            void RemoveProperties(PropertiesType& ThisProperties){
                mpProperties->erase(ThisProperties.Id());
            }


            void RemoveProperties(typename PropertiesType::Pointer pThisProperties){
                mpProperties->erase(pThisProperties->Id());
            }


            PropertiesIterator PropertiesBegin(){
                return mpProperties->begin();
            }


            PropertiesConstantIterator PropertiesBegin() const{
                return mpProperties->begin();
            }


            PropertiesIterator PropertiesEnd(){
                return mpProperties->end();
            }


            PropertiesConstantIterator PropertiesEnd() const{
                return mpProperties->end();
            }


            PropertiesContainerType& Properties(){
                return *mpProperties;
            }


            const PropertiesContainerType& Properties() const{
                return *mpProperties;
            }


            typename PropertiesContainerType::Pointer pProperties(){
                return mpProperties;
            }


            void SetProperties(typename PropertiesContainerType::Pointer pOtherProperties){
                mpProperties = pOtherProperties;
            }


            typename PropertiesContainerType::ContainerType& PropertiesArray(){
                return mpProperties->GetContainer();
            }


            bool HasProperties(IndexType PropertiesId) const{
                const auto& r_properties = *mpProperties;
                return r_properties.find(PropertiesId)!= r_properties.end();
            }


            SizeType NumberOfElements() const{
                return mpElements->size();
            }


            void AddElement(typename ElementType::Pointer pNewElement){
                mpElements->insert(mpElements->begin(), pNewElement);
            }


            typename ElementType::Pointer pGetElement(IndexType ElementId){
                auto i = mpElements->find(ElementId);
                QUEST_ERROR_IF(i == mpElements->end()) << "Element index not found: " << ElementId << "." << std::endl;
                return *i.base();
            }


            const typename ElementType::Pointer pGetElement(const IndexType ElementId) const{
                const auto& r_elements = *mpElements;
                auto i = r_elements.find(ElementId);
                QUEST_ERROR_IF(i == r_elements.end()) << "Element index not found: " << ElementId << "." << std::endl;
                return *i.base();
            }


            ElementType& GetElement(IndexType ElementId){
                auto i = mpElements->find(ElementId);
                QUEST_ERROR_IF(i == mpElements->end()) << "Element index not found: " << ElementId << "." << std::endl;
                return *i;
            }


            const ElementType& GetElement(IndexType ElementId) const{
                const auto& r_elements = *mpElements;
                auto i = r_elements.find(ElementId);
                QUEST_ERROR_IF(i == r_elements.end()) << "Element index not found: " << ElementId << "." << std::endl;
                return *i;
            }


            void RemoveElement(IndexType ElementId){
                mpElements->erase(ElementId);
            }


            void RemoveElement(ElementType& ThisElement){
                mpElements->erase(ThisElement.Id());
            }


            void RemoveElement(typename ElementType::Pointer pThisElement){
                mpElements->erase(pThisElement->Id());
            }


            ElementIterator ElementsBegin(){
                return mpElements->begin();
            }


            ElementConstantIterator ElementsBegin() const{
                return mpElements->begin();
            }


            ElementIterator ElementsEnd(){
                return mpElements->end();
            }


            ElementConstantIterator ElementsEnd() const{
                return mpElements->end();
            }


            ElementsContainerType& Elements(){
                return *mpElements;
            }


            const ElementsContainerType& Elements() const{
                return *mpElements;
            }


            typename ElementsContainerType::Pointer pElements(){
                return mpElements;
            }


            void SetElements(typename ElementsContainerType::Pointer pOtherElements){
                mpElements = pOtherElements;
            }


            typename ElementsContainerType::ContainerType& ElementsArray(){
                return mpElements->GetContainer();
            }


            bool HasElement(IndexType ElementId) const{
                const auto& r_elements = *mpElements;
                return r_elements.find(ElementId)!= r_elements.end();
            }


            SizeType NumberOfConditions() const{
                return mpConditions->size();
            }


            void AddCondition(typename ConditionType::Pointer pNewCondition){
                mpConditions->insert(mpConditions->begin(), pNewCondition);
            }


            typename ConditionType::Pointer pGetCondition(IndexType ConditionId){
                auto i = mpConditions->find(ConditionId);
                QUEST_ERROR_IF(i == mpConditions->end()) << "Condition index not found: " << ConditionId << "." << std::endl;
                return *i.base();
            }


            const typename ConditionType::Pointer pGetCondition(const IndexType ConditionId) const{
                const auto& r_conditions = *mpConditions;
                auto i = r_conditions.find(ConditionId);
                QUEST_ERROR_IF(i == r_conditions.end()) << "Condition index not found: " << ConditionId << "." << std::endl;
                return *i.base();
            }


            ConditionType& GetCondition(IndexType ConditionId){
                auto i = mpConditions->find(ConditionId);
                QUEST_ERROR_IF(i == mpConditions->end()) << "Condition index not found: " << ConditionId << "." << std::endl;
                return *i;
            }


            const ConditionType& GetCondition(IndexType ConditionId) const{
                const auto& r_conditions = *mpConditions;
                auto i = r_conditions.find(ConditionId);
                QUEST_ERROR_IF(i == r_conditions.end()) << "Condition index not found: " << ConditionId << "." << std::endl;
                return *i;
            }


            void RemoveCondition(IndexType ConditionId){
                mpConditions->erase(ConditionId);
            }


            void RemoveCondition(ConditionType& ThisCondition){
                mpConditions->erase(ThisCondition.Id());
            }


            void RemoveCondition(typename ConditionType::Pointer pThisCondition){
                mpConditions->erase(pThisCondition->Id());
            }


            ConditionIterator ConditionsBegin(){
                return mpConditions->begin();
            }


            ConditionConstantIterator ConditionsBegin() const{
                return mpConditions->begin();
            }


            ConditionIterator ConditionsEnd(){
                return mpConditions->end();
            }


            ConditionConstantIterator ConditionsEnd() const{
                return mpConditions->end();
            }


            ConditionsContainerType& Conditions(){
                return *mpConditions;
            }


            const ConditionsContainerType& Conditions() const{
                return *mpConditions;
            }


            typename ConditionsContainerType::Pointer pConditions(){
                return mpConditions;
            }


            void SetConditions(typename ConditionsContainerType::Pointer pOtherConditions){
                mpConditions = pOtherConditions;
            }


            typename ConditionsContainerType::ContainerType& ConditionsArray(){
                return mpConditions->GetContainer();
            }


            bool HasCondition(IndexType ConditionId) const{
                const auto& r_conditions = *mpConditions;
                return r_conditions.find(ConditionId)!= r_conditions.end();
            }


            SizeType NumberOfMasterSlaveConstraints() const{
                return mpMasterSlaveConstraints->size();
            }


            bool AddMasterSlaveConstraint(typename MasterSlaveConstraintType::Pointer pNewMasterSlaveConstraint){
                const auto it_existing_constraint = mpMasterSlaveConstraints->find(pNewMasterSlaveConstraint->Id());
                if(it_existing_constraint == mpMasterSlaverConstraints->end()){
                    const auto it_insert_position = mpMasterSlaveConstraints->begin();
                    mpMasterSlaveConstraints->insert(it_insert_position, pNewMasterSlaveConstraint);
                    return true;
                }
                return false;
            }


            typename MasterSlaveConstraintType::Pointer pGetMasterSlaveConstraint(IndexType MasterSlaveConstraintId){
                auto i = mpMasterSlaveConstraints->find(MasterSlaveConstraintId);
                QUEST_ERROR_IF(i == mpMasterSlaveConstraints->end()) << "MasterSlaveConstraint index not found: " << MasterSlaveConstraintId << "." << std::endl;
                return *i.base();
            }


            MasterSlaveConstraintType& GetMasterSlaveConstraint(IndexType MasterSlaveConstraintId){
                auto i = mpMasterSlaveConstraints->find(MasterSlaveConstraintId);
                QUEST_ERROR_IF(i == mpMasterSlaveConstraints->end()) << "MasterSlaveConstraint index not found: " << MasterSlaveConstraintId << "." << std::endl;
                return *i;
            }


            const MasterSlaveConstraintType& GetMasterSlaveConstraint(IndexType MasterSlaveConstraintId) const{
                auto i = mpMasterSlaveConstraints->find(MasterSlaveConstraintId);
                QUEST_ERROR_IF(i == mpMasterSlaveConstraints->end()) << "MasterSlaveConstraint index not found: " << MasterSlaveConstraintId << "." << std::endl;
                return *i;
            }


            void RemoveMasterSlaveConstraint(IndexType MasterSlaveConstraintId){
                mpMasterSlaveConstraints->erase(MasterSlaveConstraintId);
            }


            void RemoveMasterSlaveConstraint(MasterSlaveConstraintType& ThisMasterSlaveConstraint){
                mpMasterSlaveConstraints->erase(ThisMasterSlaveConstraint.Id());
            }


            void RemoveMasterSlaveConstraint(typename MasterSlaveConstraintType::Pointer pThisMasterSlaveConstraint){
                mpMasterSlaveConstraints->erase(pThisMasterSlaveConstraint->Id());
            }


            MasterSlaveConstraintIterator MasterSlaveConstraintsBegin(){
                return mpMasterSlaveConstraints->begin();
            }


            MasterSlaveConstraintConstantIterator MasterSlaveConstraintsBegin() const{
                return mpMasterSlaveConstraints->begin();
            }


            MasterSlaveConstraintIterator MasterSlaveConstraintsEnd(){
                return mpMasterSlaveConstraints->end();
            }


            MasterSlaveConstraintConstantIterator MasterSlaveConstraintsEnd() const{
                return mpMasterSlaveConstraints->end();
            }


            MasterSlaveConstraintContainerType& MasterSlaveConstraints(){
                return *mpMasterSlaveConstraints;
            }


            const MasterSlaveConstraintContainerType& MasterSlaveConstraints() const{
                return *mpMasterSlaveConstraints;
            }


            typename MasterSlaveConstraintContainerType::Pointer pMasterSlaveConstraints(){
                return mpMasterSlaveConstraints;
            }


            typename MasterSlaveConstraintContainerType::ContainerType& MasterSlaveConstraintsArray(){
                return mpMasterSlaveConstraints->GetContainer();
            }


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
            typename NodesContainerType::Pointer mpNodes;
            typename PropertiesContainerType::Pointer mpProperties;
            typename ElementsContainerType::Pointer mpElements;
            typename ConditionsContainerType::Pointer mpConditions;
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