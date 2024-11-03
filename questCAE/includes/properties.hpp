/*----------------------------------------------------
用于封装多个 Element 或 Condition 对象之间共享的数据
----------------------------------------------------*/

#ifndef QUEST_PROPERTOES_HPP
#define QUEST_PROPERTOES_HPP

// 系统头文件
#include <string>
#include <iostream>
#include <cstddef>
#include <unordered_map>

// 项目头文件
#include "includes/define.hpp"
#include "includes/accessor.hpp"
#include "includes/node.hpp"
#include "includes/indexed_object.hpp"
#include "container/data_value_container.hpp"
#include "includes/process_info.hpp"
#include "includes/table.hpp"
#include "utilities/string_utilities.hpp"

namespace Quest{

    class Properties: public IndexedObject{
        public:
            QUEST_CLASS_POINTER_DEFINITION(Properties);

            #ifdef _WIN32
                using int64_t = __int64;
            #endif

            using BaseType = IndexedObject;
            using ContainerType = DataValueContainer;
            using GeometryType = Geometry<Node>;
            using IndexType = std::size_t;
            using TableType = Table<double>;
            using KeyType = IndexType;
            using AccessorPointerType = Accessor::UniquePointer;
            using AccessorsContainerType = std::unordered_map<KeyType, AccessorPointerType>;
            using TablesContainerType = std::unordered_map<std::size_t, TableType>;
            using SubPropertiesContainerType = PointerVectorSet<Properties, IndexedObject>;

        public:
            explicit Properties(IndexType NewId = 0):
                BaseType(NewId),
                mData(),
                mTables(),
                mSubPropertiesList(),
                mAccessors() {}


            explicit Properties(IndexType NewId, const SubPropertiesContainerType& rSubPropertiesList):
                BaseType(NewId),
                mData(),
                mTables(),
                mSubPropertiesList(rSubPropertiesList),
                mAccessors() {}


            Properties(const Properties& rOther):
                BaseType(rOther),
                mData(rOther.mData),
                mTables(rOther.mTables),
                mSubPropertiesList(rOther.mSubPropertiesList)
            {
                for(auto& r_item: rOther.mAccessors){
                    const auto key = r_item.first;
                    const auto& rp_accessor = r_item.second;
                    mAccessors.emplace(key, rp_accessor->Clone());
                }
            }


            ~Properties() override {}


            Properties& operator = (const Properties& rOther){
                BaseType::operator=(rOther);
                mData = rOther.mData;
                mTables = rOther.mTables;
                mSubPropertiesList = rOther.mSubPropertiesList;
                for(auto& r_item: rOther.mAccessors){
                    const auto key = r_item.first;
                    const auto& rp_accessor = r_item.second;
                    mAccessors.emplace(key, rp_accessor->Clone());
                }
                return *this;
            }


            template<typename TVariableType>
            typename TVariableType::Type& operator()(const TVariableType& rV){
                return GetValue(rV);
            }


            template<typename TVariableType>
            const typename TVariableType::Type& operator()(const TVariableType& rV) const{
                return GetValue(rV);
            }


            template<typename TVariableType>
            typename TVariableType::Type& operator[](const TVariableType& rV){
                return GetValue(rV);
            }


            template<typename TVariableType>
            const typename TVariableType::Type& operator[](const TVariableType& rV) const{
                return GetValue(rV);
            }


            template<typename TVariableType>
            typename TVariableType::Type& operator()(const TVariableType& rV, Node& rThisNode){
                return GetValue(rV, rThisNode);
            }


            template<typename TVariableType>
            const typename TVariableType::Type& operator()(const TVariableType& rV, const Node& rThisNode) const{
                return GetValue(rV, rThisNode);
            }


            template<typename TVariableType>
            typename TVariableType::Type& operator()(const TVariableType& rV, Node& rThisNode, IndexType SolutionStepIndex){
                return GetValue(rV, rThisNode, SolutionStepIndex);
            }


            template<typename TVariableType>
            const typename TVariableType::Type& operator()(const TVariableType& rV, const Node& rThisNode, IndexType SolutionStepIndex) const{
                return GetValue(rV, rThisNode, SolutionStepIndex);
            }


            template<typename TVariableType>
            typename TVariableType::Type& operator()(const TVariableType& rV, Node& rThisNode, const ProcessInfo& rCurrentProcessInfo){
                return GetValue(rV, rThisNode, rCurrentProcessInfo.GetSolutionStepIndex());
            }


            template<typename TVariableType>
            const typename TVariableType::Type& operator()(const TVariableType& rV, const Node& rThisNode, const ProcessInfo& rCurrentProcessInfo) const{
                return GetValue(rV, rThisNode, rCurrentProcessInfo.GetSolutionStepIndex());
            }


            template<typename TVariableType>
            void Erase(const TVariableType& rV){
                mData.Erase(rV);
            }


            template<typename TVariableType>
            typename TVariableType::Type& GetValue(const TVariableType& rV){
                return mData.GetValue(rV);
            }


            template<typename TVariableType>
            const typename TVariableType::Type& GetValue(const TVariableType& rV) const{
                return mData.GetValue(rV);
            }


            template<typename TVariableType>
            typename TVariableType::Type& GetValue(const TVariableType& rV, Node& rThisNode){
                if(mData.Has(rV)){
                    return mData.GetValue(rV);
                }
                return rThisNode.GetValue(rV);
            }


            template<typename TVariableType>
            const typename TVariableType::Type& GetValue(const TVariableType& rV, const Node& rThisNode) const{
                if(mData.Has(rV)){
                    return mData.GetValue(rV);
                }
                return rThisNode.GetValue(rV);
            }


            template<typename TVariableType>
            typename TVariableType::Type& GetValue(const TVariableType& rV, Node& rThisNode, IndexType SolutionStepIndex){
                if(mData.Has(rV)){
                    return mData.GetValue(rV);
                }
                return rThisNode.GetValue(rV, SolutionStepIndex);
            }


            template<typename TVariableType>
            const typename TVariableType::Type& GetValue(const TVariableType& rV, const Node& rThisNode, IndexType SolutionStepIndex) const{
                if(mData.Has(rV)){
                    return mData.GetValue(rV);
                }
                return rThisNode.GetValue(rV, SolutionStepIndex);
            }


            template<typename TVariableType>
            typename TVariableType::Type GetValue(const TVariableType& rV, const GeometryType& rGeometry, const Vector& rShapeFunctionVector, const ProcessInfo& rProcessInfo) const{
                auto it_value = mAccessors.find(rV.Key());
                if(it_value != mAccessors.end()){
                    return (*it_value->second)->GetValue(rV, *this, rGeometry, rShapeFunctionVector, rProcessInfo);
                } else {
                    return mData.GetValue(rV);
                }
            }


            template<typename TVariableType>
            void SetValue(const TVariableType& rV, const typename TVariableType::Type& rValue){
                mData.SetValue(rV, rValue);
            }


            bool HasVariables() const{
                return !mData.IsEmpty();
            }


            template<typename TVariableType>
            void SetAccessor(const TVariableType& rV, AccessorPointerType pAccessor){
                mAccessors.emplace(rV.Key(), std::move(pAccessor));
            }


            template<typename TVariableType>
            Accessor& GetAccessor(const TVariableType& rV){
                auto it_value = mAccessors.find(rV.Key());
                QUEST_ERROR_IF(it_value == mAccessor.end())
                    << "Trying to retrieve inexisting accessor for '" << rV.Name() << "' in properties " << Id() << "." << std::endl;
                return *(it_value->second);
            }


            template<typename TVariableType>
            Accessor& GetAccessor(const TVariableType& rV) const{
                auto it_value = mAccessors.find(rV.Key());
                QUEST_ERROR_IF(it_value == mAccessor.end())
                    << "Trying to retrieve inexisting accessor for '" << rV.Name() << "' in properties " << Id() << "." << std::endl;
                return *(it_value->second);
            }


            template<typename TVariableType>
            AccessorPointerType& pGetAccessor(const TVariableType& rV){
                const auto it_value = mAccessors.find(rV.Key());
                QUEST_ERROR_IF(it_value == mAccessor.end())
                    << "Trying to retrieve inexisting accessor for '" << rV.Name() << "' in properties " << Id() << "." << std::endl;
                return it_value->second;
            }


            template<typename TVariableType>
            bool HasAccessor(const TVariableType& rV) const{
                return (mAccessors.find(rV.Key()) != mAccessors.end());
            }


            template<typename TXVariableType, typename TYVariableType>
            TableType& GetTable(const TXVariableType& XVariable, const TYVariableType& YVariable){
                return mTables[Key(XVariable.Key(), YVariable.Key())];
            }


            template<typename TXVariableType, typename TYVariableType>
            const TableType& GetTable(const TXVariableType& XVariable, const TYVariableType& YVariable) const{
                return mTables.at(Key(XVariable.Key(), YVariable.Key()));
            }


            template<typename TXVariableType, typename TYVariableType>
            void SetTable(const TXVariableType& XVariable, const TYVariableType& YVariable, const TableType& rTable){
                mTables[Key(XVariable.Key(), YVariable.Key())] = rTable;
            }


            bool HasTables() const{
                return !mTables.empty();
            }


            bool IsEmpty() const{
                return !(HasVariables() || HasTables());
            }


            int64_t Key(std::size_t XKey, std::size_t YKey) const{
                int64_t result_key = XKey;
                result_key = result_key << 32;
                result_key |= YKey;
                return result_key;
            }


            std::size_t NumberOfSubproperties() const{
                return mSubPropertiesList.size();
            }


            void AddSubProperties(Properties::Pointer pSubProperties){
                QUEST_DEBUG_ERROR_IF(this->HasSubProperties(pSubProperties->Id())) << "Subproperties with ID: " << pSubProperties->Id() << " already exist in properties " << Id() << "." << std::endl;
                mSubPropertiesList.insert(mSubPropertiesList.begin(), *pSubProperties);
            }


            bool HasSubProperties(const IndexType SubPropertyIndex) const{
                return (mSubPropertiesList.find(SubPropertyIndex) != mSubPropertiesList.end());
            }


            Properties::Pointer pGetSubProperties(const IndexType SubPropertyIndex){
                auto property_iterator = mSubPropertiesList.find(SubPropertyIndex);
                if(property_iterator != mSubPropertiesList.end()){
                    return *(property_iterator.base());
                } else {
                    QUEST_ERROR << "Subproperty ID: " << SubPropertyIndex << " is not defined on current Properties ID: " << this->Id() << "." << std::endl;
                    return nullptr;
                }
            }


            const Properties::Pointer pGetSubProperties(const IndexType SubPropertyIndex) const{
                auto property_iterator = mSubPropertiesList.find(SubPropertyIndex);
                if(property_iterator != mSubPropertiesList.end()){
                    return *(property_iterator.base());
                } else {
                    QUEST_ERROR << "Subproperty ID: " << SubPropertyIndex << " is not defined on current Properties ID: " << this->Id() << "." << std::endl;
                    return nullptr;
                }
            }


            Properties& GetSubProperties(const IndexType SubPropertyIndex){
                auto property_iterator = mSubPropertiesList.find(SubPropertyIndex);
                if(property_iterator != mSubPropertiesList.end()){
                    return *(property_iterator);
                } else {
                    QUEST_ERROR << "Subproperty ID: " << SubPropertyIndex << " is not defined on current Properties ID: " << this->Id() << "." << std::endl;
                    return *this;
                }
            }


            const Properties& GetSubProperties(const IndexType SubPropertyIndex) const{
                auto property_iterator = mSubPropertiesList.find(SubPropertyIndex);
                if(property_iterator != mSubPropertiesList.end()){
                    return *(property_iterator);
                } else {
                    QUEST_ERROR << "Subproperty ID: " << SubPropertyIndex << " is not defined on current Properties ID: " << this->Id() << "." << std::endl;
                    return *this;
                }
            }


            SubPropertiesContainerType& GetSubProperties(){
                return mSubPropertiesList;
            }


            const SubPropertiesContainerType& GetSubProperties() const{
                return mSubPropertiesList;
            }


            void SetSubProperties(SubPropertiesContainerType& rSubPropertiesList){
                mSubPropertiesList = rSubPropertiesList;
            }


            ContainerType& Data(){
                return mData;
            }


            const ContainerType& Data() const{
                return mData;
            }


            TablesContainerType& Tables(){
                return mTables;
            }


            const TablesContainerType& Tables() const{
                return mTables;
            }


            template<typename TVariableType>
            bool Has(const TVariableType& rV) const{
                return mData.Has(rV);
            }


            template<typename TXVariableType, typename TYVariableType>
            bool HasTable(const TXVariableType& XVariable, const TYVariableType& YVariable) const{
                return (mTables.find(Key(XVariable.Key(), YVariable.Key())) != mTables.end());
            }


            std::string Info() const override{
                std::stringstream buffer;
                buffer << "Properties";
                return buffer.str();
            }


            void PrintInfo(std::ostream& rOstream) const override{
                rOstream << Info();
            }


            void PrintData(std::ostream& rOstream) const override{
                rOstream << "Id : " << this->Id() << std::endl;

                mData.PrintData(rOstream);

                if(mTables.size() > 0){
                    rOstream << "This properties contains " << mTables.size() << " tables" << std::endl;
                    for(auto& r_item: mTables){
                        rOstream << "Table key: " << r_item.first << std::endl;
                        StringUtilities::PrintDataWithIndentation(rOstream, r_item.second);
                    }
                }

                if(mSubPropertiesList.size() > 0){
                    rOstream << "\nThis properties contains " << mSubPropertiesList.size() << " subproperties" << std::endl;
                    for(auto& r_item: mSubPropertiesList){
                        StringUtilities::PrintDataWithIndentation(rOstream, r_item);
                    }
                }

                if(mAccessors.size() > 0){
                    rOstream << "\nThis properties contains " << mAccessors.size() << " accessors" << std::endl;
                    for(auto& r_item: mAccessors){
                        rOstream << "Accessor for variable key: " << r_item.first << std::endl;
                        StringUtilities::PrintDataWithIndentation(rOstream, *r_item.second);
                    }
                }
            }

        protected:

        private:
            friend class Serializer;


            void save(Serializer& rSerializer) const override{
                QUEST_SERIALIZE_SAVE_BASE_CLASS(rSerializer, IndexedObject);
                rSerializer.save("Data", mData);
                rSerializer.save("Tables", mTables);
                rSerializer.save("SubPropertiesList", mSubPropertiesList);
                std::vector<std::pair<const KeyType,Accessor*>> aux_accessors_contsiner;
                for(auto& r_item: mAccessors){
                    const auto key = r_item.first;
                    const auto& rp_accessor = r_item.second;
                    aux_accessors_contsiner.push_back(std::make_pair(key, &(*rp_accessor)));
                }
                rSerializer.save("Accessors", aux_accessors_contsiner);
            }


            void load(Serializer& rSerializer) override{
                QUEST_SERIALIZE_LOAD_BASE_CLASS(rSerializer, IndexedObject);
                rSerializer.load("Data", mData);    
                rSerializer.load("Tables", mTables);
                rSerializer.load("SubPropertiesList", mSubPropertiesList);
                std::vector<std::pair<const KeyType,Accessor*>> aux_accessors_contsiner;
                rSerializer.load("Accessors", aux_accessors_contsiner);
                for(auto& r_item: aux_accessors_contsiner){
                    const auto key = r_item.first;
                    const auto& rp_accessor = r_item.second;
                    mAccessors.emplace(key, rp_accessor->Clone());
                }
            }

        private:
            ContainerType mData;
            TablesContainerType mTables;
            SubPropertiesContainerType mSubPropertiesList;
            AccessorsContainerType mAccessors = {};

    };


    inline std::istream& operator >> (std::istream& rIstream, Properties& rThis){}


    inline std::ostream& operator << (std::ostream& rOstream, const Properties& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }

} // namespace Quest

#endif //QUEST_PROPERTOES_HPP