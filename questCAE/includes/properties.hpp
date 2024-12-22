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

    /**
     * @class Properties
     * @brief 封装了由不同的Element或Condition共享的数据，通过基于变量的方式访问
     * @details 如一组单元的材料参数、一组边界条件/荷载条件的参数、一组单元的属性参数
     * 这些数据由Properties类持有，并在单元和条件之间共享。
     * Properties可以用于访问节点数据，其流程是先从Properties数据容器中查找该变量，如果没有找到，则从节点中查找。
     */
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
            /**
             * @brief 构造函数
             * @param NewId 对象的ID
             */
            explicit Properties(IndexType NewId = 0):
                BaseType(NewId),
                mData(),
                mTables(),
                mSubPropertiesList(),
                mAccessors() {}

            /**
             * @brief 构造函数
             * @param NewId 对象的ID
             * @param rSubPropertiesList 子Properties列表
             */
            explicit Properties(IndexType NewId, const SubPropertiesContainerType& rSubPropertiesList):
                BaseType(NewId),
                mData(),
                mTables(),
                mSubPropertiesList(rSubPropertiesList),
                mAccessors() {}

            /**
             * @brief 拷贝构造函数
             * @param rOther 待拷贝的对象
             */
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

            /**
             * @brief 析构函数
             */
            ~Properties() override {}

            /**
             * @brief 赋值运算符重载
             */
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

            /**
             * @brief 函数调用运算符重载，获取变量的值
             */
            template<typename TVariableType>
            typename TVariableType::Type& operator()(const TVariableType& rV){
                return GetValue(rV);
            }

            /**
             * @brief 函数调用运算符重载，获取变量的值
             */
            template<typename TVariableType>
            const typename TVariableType::Type& operator()(const TVariableType& rV) const{
                return GetValue(rV);
            }

            /**
             * @brief 下标运算符重载，获取变量的值
             */
            template<typename TVariableType>
            typename TVariableType::Type& operator[](const TVariableType& rV){
                return GetValue(rV);
            }

            /**
             * @brief 下标运算符重载，获取变量的值
             */
            template<typename TVariableType>
            const typename TVariableType::Type& operator[](const TVariableType& rV) const{
                return GetValue(rV);
            }

            /**
             * @brief 函数调用运算符重载，获取指定节点的变量的值
             */
            template<typename TVariableType>
            typename TVariableType::Type& operator()(const TVariableType& rV, Node& rThisNode){
                return GetValue(rV, rThisNode);
            }

            /**
             * @brief 函数调用运算符重载，获取指定节点的变量的值
             */
            template<typename TVariableType>
            const typename TVariableType::Type& operator()(const TVariableType& rV, const Node& rThisNode) const{
                return GetValue(rV, rThisNode);
            }

            /**
             * @brief 函数调用运算符重载，获取指定节点的变量的指定分析步的值
             */
            template<typename TVariableType>
            typename TVariableType::Type& operator()(const TVariableType& rV, Node& rThisNode, IndexType SolutionStepIndex){
                return GetValue(rV, rThisNode, SolutionStepIndex);
            }

            /**
             * @brief 函数调用运算符重载，获取指定节点的变量的指定分析步的值
             */
            template<typename TVariableType>
            const typename TVariableType::Type& operator()(const TVariableType& rV, const Node& rThisNode, IndexType SolutionStepIndex) const{
                return GetValue(rV, rThisNode, SolutionStepIndex);
            }

            /**
             * @brief 函数调用运算符重载，获取指定节点的变量的指定分析步的值
             * @param rCurrentProcessInfo 求解过程信息
             */
            template<typename TVariableType>
            typename TVariableType::Type& operator()(const TVariableType& rV, Node& rThisNode, const ProcessInfo& rCurrentProcessInfo){
                return GetValue(rV, rThisNode, rCurrentProcessInfo.GetSolutionStepIndex());
            }

            /**
             * @brief 函数调用运算符重载，获取指定节点的变量的指定分析步的值
             * @param rCurrentProcessInfo 求解过程信息
             */
            template<typename TVariableType>
            const typename TVariableType::Type& operator()(const TVariableType& rV, const Node& rThisNode, const ProcessInfo& rCurrentProcessInfo) const{
                return GetValue(rV, rThisNode, rCurrentProcessInfo.GetSolutionStepIndex());
            }

            /**
             * @brief 移除变量
             */
            template<typename TVariableType>
            void Erase(const TVariableType& rV){
                mData.Erase(rV);
            }

            /**
             * @brief 获取变量的值
             */
            template<typename TVariableType>
            typename TVariableType::Type& GetValue(const TVariableType& rV){
                return mData.GetValue(rV);
            }

            /**
             * @brief 获取变量的值
             */
            template<typename TVariableType>
            const typename TVariableType::Type& GetValue(const TVariableType& rV) const{
                return mData.GetValue(rV);
            }

            /**
             * @brief 获取指定节点的变量的值
             */
            template<typename TVariableType>
            typename TVariableType::Type& GetValue(const TVariableType& rV, Node& rThisNode){
                if(mData.Has(rV)){
                    return mData.GetValue(rV);
                }
                return rThisNode.GetValue(rV);
            }

            /**
             * @brief 获取指定节点的变量的值
             */
            template<typename TVariableType>
            const typename TVariableType::Type& GetValue(const TVariableType& rV, const Node& rThisNode) const{
                if(mData.Has(rV)){
                    return mData.GetValue(rV);
                }
                return rThisNode.GetValue(rV);
            }

            /**
             * @brief 获取指定节点的变量的指定分析步的值
             */
            template<typename TVariableType>
            typename TVariableType::Type& GetValue(const TVariableType& rV, Node& rThisNode, IndexType SolutionStepIndex){
                if(mData.Has(rV)){
                    return mData.GetValue(rV);
                }
                return rThisNode.GetValue(rV, SolutionStepIndex);
            }

            /**
             * @brief 获取指定节点的变量的指定分析步的值
             */
            template<typename TVariableType>
            const typename TVariableType::Type& GetValue(const TVariableType& rV, const Node& rThisNode, IndexType SolutionStepIndex) const{
                if(mData.Has(rV)){
                    return mData.GetValue(rV);
                }
                return rThisNode.GetValue(rV, SolutionStepIndex);
            }

            /**
             * @brief 获取变量的值
             */
            template<typename TVariableType>
            typename TVariableType::Type GetValue(const TVariableType& rV, const GeometryType& rGeometry, const Vector& rShapeFunctionVector, const ProcessInfo& rProcessInfo) const{
                auto it_value = mAccessors.find(rV.Key());
                if(it_value != mAccessors.end()){
                    return (*it_value->second)->GetValue(rV, *this, rGeometry, rShapeFunctionVector, rProcessInfo);
                } else {
                    return mData.GetValue(rV);
                }
            }

            /**
             * @brief 设置变量的值
             */
            template<typename TVariableType>
            void SetValue(const TVariableType& rV, const typename TVariableType::Type& rValue){
                mData.SetValue(rV, rValue);
            }

            /**
             * @brief 判断是否有变量
             */
            bool HasVariables() const{
                return !mData.IsEmpty();
            }

            /**
             * @brief 设置Accessor对象
             * @details 该方法在当前属性的accessor容器中设置一个变量-Accessor映射
             */
            template<typename TVariableType>
            void SetAccessor(const TVariableType& rV, AccessorPointerType pAccessor){
                mAccessors.emplace(rV.Key(), std::move(pAccessor));
            }

            /**
             * @brief 获取变量的Accessor对象
             */
            template<typename TVariableType>
            Accessor& GetAccessor(const TVariableType& rV){
                auto it_value = mAccessors.find(rV.Key());
                QUEST_ERROR_IF(it_value == mAccessor.end())
                    << "Trying to retrieve inexisting accessor for '" << rV.Name() << "' in properties " << Id() << "." << std::endl;
                return *(it_value->second);
            }

            /**
             * @brief 获取变量的Accessor对象
             */
            template<typename TVariableType>
            Accessor& GetAccessor(const TVariableType& rV) const{
                auto it_value = mAccessors.find(rV.Key());
                QUEST_ERROR_IF(it_value == mAccessor.end())
                    << "Trying to retrieve inexisting accessor for '" << rV.Name() << "' in properties " << Id() << "." << std::endl;
                return *(it_value->second);
            }

            /**
             * @brief 获取变量的Accessor对象指针
             */
            template<typename TVariableType>
            AccessorPointerType& pGetAccessor(const TVariableType& rV){
                const auto it_value = mAccessors.find(rV.Key());
                QUEST_ERROR_IF(it_value == mAccessor.end())
                    << "Trying to retrieve inexisting accessor for '" << rV.Name() << "' in properties " << Id() << "." << std::endl;
                return it_value->second;
            }

            /**
             * @brief 检查当前属性是否具有该变量的访问器
             */
            template<typename TVariableType>
            bool HasAccessor(const TVariableType& rV) const{
                return (mAccessors.find(rV.Key()) != mAccessors.end());
            }

            /**
             * @brief 获取指定的表格
             * @details 该方法通过XVariable和YVariable的Key值获取指定的表格
             */
            template<typename TXVariableType, typename TYVariableType>
            TableType& GetTable(const TXVariableType& XVariable, const TYVariableType& YVariable){
                return mTables[Key(XVariable.Key(), YVariable.Key())];
            }

            /**
             * @brief 获取指定的表格
             * @details 该方法通过XVariable和YVariable的Key值获取指定的表格
             */
            template<typename TXVariableType, typename TYVariableType>
            const TableType& GetTable(const TXVariableType& XVariable, const TYVariableType& YVariable) const{
                return mTables.at(Key(XVariable.Key(), YVariable.Key()));
            }

            /**
             * @brief 设置表格
             * @details 该方法通过XVariable和YVariable的Key值设置指定的表格
             */
            template<typename TXVariableType, typename TYVariableType>
            void SetTable(const TXVariableType& XVariable, const TYVariableType& YVariable, const TableType& rTable){
                mTables[Key(XVariable.Key(), YVariable.Key())] = rTable;
            }

            /**
             * @brief 判断是否有表格
             */
            bool HasTables() const{
                return !mTables.empty();
            }

            /**
             * @brief 判断properties是否为空
             */
            bool IsEmpty() const{
                return !(HasVariables() || HasTables());
            }

            /**
             * @brief 输入x变量和y变量的Key值，获取表格的Key值
             */
            int64_t Key(std::size_t XKey, std::size_t YKey) const{
                int64_t result_key = XKey;
                result_key = result_key << 32;
                result_key |= YKey;
                return result_key;
            }

            /**
             * @brief 获取子属性的数量
             */
            std::size_t NumberOfSubproperties() const{
                return mSubPropertiesList.size();
            }

            /**
             * @brief 添加子属性
             */
            void AddSubProperties(Properties::Pointer pSubProperties){
                QUEST_DEBUG_ERROR_IF(this->HasSubProperties(pSubProperties->Id())) << "Subproperties with ID: " << pSubProperties->Id() << " already exist in properties " << Id() << "." << std::endl;
                mSubPropertiesList.insert(mSubPropertiesList.begin(), *pSubProperties);
            }

            /**
             * @brief 判断某一子属性是否存在
             */
            bool HasSubProperties(const IndexType SubPropertyIndex) const{
                return (mSubPropertiesList.find(SubPropertyIndex) != mSubPropertiesList.end());
            }

            /**
             * @brief 通过子属性的ID获取子属性指针
             */
            Properties::Pointer pGetSubProperties(const IndexType SubPropertyIndex){
                auto property_iterator = mSubPropertiesList.find(SubPropertyIndex);
                if(property_iterator != mSubPropertiesList.end()){
                    return *(property_iterator.base());
                } else {
                    QUEST_ERROR << "Subproperty ID: " << SubPropertyIndex << " is not defined on current Properties ID: " << this->Id() << "." << std::endl;
                    return nullptr;
                }
            }

            /**
             * @brief 通过子属性的ID获取子属性指针
             */
            const Properties::Pointer pGetSubProperties(const IndexType SubPropertyIndex) const{
                auto property_iterator = mSubPropertiesList.find(SubPropertyIndex);
                if(property_iterator != mSubPropertiesList.end()){
                    return *(property_iterator.base());
                } else {
                    QUEST_ERROR << "Subproperty ID: " << SubPropertyIndex << " is not defined on current Properties ID: " << this->Id() << "." << std::endl;
                    return nullptr;
                }
            }

            /**
             * @brief 通过子属性的ID获取子属性
             */
            Properties& GetSubProperties(const IndexType SubPropertyIndex){
                auto property_iterator = mSubPropertiesList.find(SubPropertyIndex);
                if(property_iterator != mSubPropertiesList.end()){
                    return *(property_iterator);
                } else {
                    QUEST_ERROR << "Subproperty ID: " << SubPropertyIndex << " is not defined on current Properties ID: " << this->Id() << "." << std::endl;
                    return *this;
                }
            }

            /**
             * @brief 通过子属性的ID获取子属性
             */
            const Properties& GetSubProperties(const IndexType SubPropertyIndex) const{
                auto property_iterator = mSubPropertiesList.find(SubPropertyIndex);
                if(property_iterator != mSubPropertiesList.end()){
                    return *(property_iterator);
                } else {
                    QUEST_ERROR << "Subproperty ID: " << SubPropertyIndex << " is not defined on current Properties ID: " << this->Id() << "." << std::endl;
                    return *this;
                }
            }

            /**
             * @brief 获取子属性容器
             */
            SubPropertiesContainerType& GetSubProperties(){
                return mSubPropertiesList;
            }

            /**
             * @brief 获取子属性容器
             */
            const SubPropertiesContainerType& GetSubProperties() const{
                return mSubPropertiesList;
            }

            /**
             * @brief 设置子属性容器
             */
            void SetSubProperties(SubPropertiesContainerType& rSubPropertiesList){
                mSubPropertiesList = rSubPropertiesList;
            }

            /**
             * @brief 获取变量存储容器
             */
            ContainerType& Data(){
                return mData;
            }

            /**
             * @brief 获取变量存储容器
             */
            const ContainerType& Data() const{
                return mData;
            }

            /**
             * @brief 获取表格存储容器
             */
            TablesContainerType& Tables(){
                return mTables;
            }

            /**
             * @brief 获取表格存储容器
             */
            const TablesContainerType& Tables() const{
                return mTables;
            }

            /**
             * @brief 判断变量存储容器中是否有指定变量
             */
            template<typename TVariableType>
            bool Has(const TVariableType& rV) const{
                return mData.Has(rV);
            }

            /**
             * @brief 判断表格存储容器中是否有指定表格
             */
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
            /**
             * @brief 存储变量值的容器
             * @details DataValueContainer
             */
            ContainerType mData;

            /**
             * @brief 存储表格数据的容器
             * @details std::unordered_map<std::size_t, Table<double>>
             */
            TablesContainerType mTables;

            /**
             * @brief 存储子属性的容器
             * @details PointerVectorSet<Properties, IndexedObject>
             */
            SubPropertiesContainerType mSubPropertiesList;

            /**
             * @brief 与子属性相关的访问器对象，Properties的键值与访问器对象一一对应
             * @details std::unordered_map<KeyType, Accessor::UniquePointer>
             */
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