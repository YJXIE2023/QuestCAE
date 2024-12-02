#ifndef QUEST_DATA_VALUE_CONTAINER_HPP
#define QUEST_DATA_VALUE_CONTAINER_HPP

// 系统头文件
#include <string>
#include <iostream>
#include <cstddef>
#include <vector>

// 项目头文件
#include "includes/define.hpp"
#include "container/variable.hpp"
#include "includes/quest_components.hpp"
#include "includes/exceptions.hpp"

#ifdef QUEST_DEBUG
    #include "utilities/openmp_utils.hpp"
#endif

namespace Quest{

    /**
     * @class DataValueContainer
     * @brief 用于存储与变量关联的数据值容器
     * @details 该类提供了一个容器，用于存储存储与变量关联的数据值
     */
    class QUEST_API(QUEST_CORE) DataValueContainer{
        public:
            QUEST_DEFINE_LOCAL_FLAG(OVERWRITE_OLD_VALUES);
            QUEST_CLASS_POINTER_DEFINITION(DataValueContainer);

            using ValueType = std::pair<const VariableData*, void*>;
            using ContainerType = std::vector<ValueType>;
            using iterator = ContainerType::iterator;
            using const_iterator = ContainerType::const_iterator;
            using SizeType = ContainerType::size_type;


            /**
             * @brief 默认构造函数
             */
            DataValueContainer() {}


            /**
             * @brief 复制构造函数
             */
            DataValueContainer(const DataValueContainer& rOther){
                for(const_iterator i = rOther.mData.begin(); i!= rOther.mData.end(); ++i)
                    mData.push_back(*i);
            }


            /**
             * @brief 析构函数
             */
            virtual ~DataValueContainer() {
                for(iterator i = mData.begin(); i!= mData.end(); ++i)
                    i->first->Delete(i->second);
            }


            /**
             * @brief 函数调用运算符重载
             * @param rThisVariable 类型为VariableData的变量
             */
            template<typename TDataType>
            const TDataType& operator()(const VariableData& rThisVariable) const{
                return GetValue<TDataType>(rThisVariable);
            }


            /**
             * @brief 函数调用运算符重载
             * @param rThisVariable 类型为Variable<TDataType>的变量
             */
            template<typename TDataType>
            TDataType& operator()(const Variable<TDataType>& rThisVariable){
                return GetValue<TDataType>(rThisVariable);
            }


            /**
             * @brief 函数调用运算符重载
             * @param rThisVariable 类型为Variable<TDataType>的变量
             */
            template<typename TDataType>
            const TDataType& operator() (const Variable<TDataType>& rThisVariable) const{
                return GetValue<TDataType>(rThisVariable);
            }


            /**
             * @brief 下标运算符重载
             * @param rThisVariable 类型为VariableData的变量
             */
            template<typename TDataType>
            TDataType& operator[](const VariableData& rThisVariable){
                return GetValue<TDataType>(rThisVariable);
            }


            /**
             * @brief 下标运算符重载
             * @param rThisVariable 类型为VariableData的变量
             */
            template<typename TDataType>
            const TDataType& operator[] (const VariableData& rThisVariable) const{
                return GetValue<TDataType>(rThisVariable);
            }

            
            /**
             * @brief 下标运算符重载
             * @param rThisVariable 类型为Variable<TDataType>的变量
             */
            template<typename TDataType>
            TDataType& operator[](const Variable<TDataType>& rThisVariable){
                return GetValue<TDataType>(rThisVariable);
            }


            /**
             * @brief 下标运算符重载
             * @param rThisVariable 类型为Variable<TDataType>的变量
             */
            template<typename TDataType>
            const TDataType& operator[] (const Variable<TDataType>& rThisVariable) const{
                return GetValue<TDataType>(rThisVariable);
            }


            /**
             * @brief 返回容器的第一个元素的迭代器
             */
            iterator begin(){
                return mData.begin();
            }


            /**
             * @brief 返回容器的第一个元素的常量迭代器
             */
            const_iterator begin() const{   
                return mData.begin();
            }


            /**
             * @brief 返回容器的最后一个元素的迭代器
             */
            iterator end(){
                return mData.end();
            }


            /**
             * @brief 返回容器的最后一个元素的常量迭代器
             */
            const_iterator end() const{
                return mData.end();
            }


            /**
             * @brief 赋值运算符重载
             */
            DataValueContainer& operator=(const DataValueContainer& rOther){
                Clear();

                for(const_iterator i = rOther.mData.begin(); i!= rOther.mData.end(); ++i)
                    mData.push_back(ValueType(i->first, i->first->Clone(i->second)));

                return *this;
            }


            /**
             * @brief 获取变量对应的数据值
             */
            template<typename TDataType>
            TDataType& GetValue(const Variable<TDataType>& rThisVariable){
                typename ContainerType::iterator i;

                if((i = std::find_if(mData.begin(), mData.end(), IndexCheck(rThisVariable.SourceKey()))) != mData.end())
                    return *(static_cast<TDataType*>(i->second)+rThisVariable.GetComponentIndex());
                
                #ifdef QUEST_DEBUG
                    if(OpenMputils::IsInParallel() != 0)
                        QUEST_ERROR << "attempting to do a GetValue for: " << rThisVariable << " unfortunately the variable is not in the database and the operations is not threadsafe (this function is being called from within a parallel region)" << std::endl;
                #endif

                auto p_source_variable = &rThisVariable.GetSourceVariable();
                mData.push_back(ValueType(p_source_variable,p_source_variable->Clone(p_source_variable->pZero())));
                
                return *(static_cast<TDataType*>(mData.back().second)+rThisVariable.GetComponentIndex());
            }


            /**
             * @brief 获取变量对应的数据值
             */
            template<typename TDataType>
            const TDataType& GetValue(const Variable<TDataType>& rThisVariable) const{
                typename ContainerType::const_iterator i;

                if((i = std::find_if(mData.begin(), mData.end(), IndexCheck(rThisVariable.SourceKey()))) != mData.end())
                    return *(static_cast<TDataType*>(i->second)+rThisVariable.GetComponentIndex());
                
                return rThisVariable.Zero();
            }


            /**
             * @brief 返回容器的大小
             */
            SizeType Size(){
                return mData.size();
            }


            /**
             * @brief 设置变量的值
             * @details 如果变量不存在于容器中，则创建变量及其关联的数据值
             */
            template<typename TDataType>
            void SetValue(const Variable<TDataType>& rThisVariable, const TDataType& rValue){
                typename ContainerType::iterator i;

                if((i = std::find_if(mData.begin(), mData.end(), IndexCheck(rThisVariable.SourceKey()))) != mData.end()){
                    *(static_cast<TDataType*>(i->second)+rThisVariable.GetComponentIndex()) = rValue;
                } else {
                    auto p_source_variable = &rThisVariable.GetSourceVariable();
                    mData.push_back(ValueType(p_source_variable,p_source_variable->Clone(p_source_variable->pZero())));
                    *(static_cast<TDataType*>(mData.back().second)+rThisVariable.GetComponentIndex()) = rValue;
                }
            }


            /**
             * @brief 删除变量及其关联的数据值
             */
            template<typename TDataType>
            void Erase(const Variable<TDataType>& rThisVariable){
                typename ContainerType::iterator i;

                if((i = std::find_if(mData.begin(), mData.end(), IndexCheck(rThisVariable.SourceKey()))) != mData.end()){
                    i->first->Delete(i->second);
                    mData.erase(i);
                }
            }


            /**
             * @brief 清空容器
             */
            void Clear(){
                for(ContainerType::iterator i = mData.begin(); i != mData.end(); ++i)
                    i->first->Delete(i->second);

                mData.clear();
            }


            /**
             * @brief 合并两个容器
             */
            void Merge(const DataValueContainer& rOther, const Flags Options);


            /**
             * @brief 检查变量是否存在于容器中
             */
            template<typename TDataType>
            bool Has(const Variable<TDataType>& rThisVariable) const{
                return std::find_if(mData.begin(), mData.end(), IndexCheck(rThisVariable.SourceKey())) != mData.end();
            }


            /**
             * @brief 检查容器是否为空
             */
            bool IsEmpty() const{
                return mData.empty();
            }


            virtual std::string Info() const{
                return std::string("data value container");
            }


            virtual void PrintInfo(std::ostream& rOstream) const{
                rOstream << "data value container";
            }


            virtual void PrintData(std::ostream& rOstream) const{
                for(const_iterator i = mData.begin(); i != mData.end(); ++i){
                    rOstream << "   ";
                    i->first->Print(i->second, rOstream);
                    rOstream << std::endl;
                }
            }

        protected:

        private:
            /**
             * @brief 用于检查变量索引是否匹配的函数对象
             */
            class IndexCheck{
                public:
                    explicit IndexCheck(std::size_t I):mI(I){}

                    bool operator()(const ValueType& I){
                        return I.first->SourceKey() == mI;
                    }

                private:
                    std::size_t mI;
            };

            friend class Serializer;

            virtual void save(Serializer& rSerializer) const;

            virtual void load(Serializer& rSerializer);

        private:
            /**
             * @brief 存储变量及其关联的数据值的容器
             * @details std::vector< std::pair<const VariableData*, void*> >
             */
            ContainerType mData;

    };

    inline std::istream& operator >> (std::istream& rIstream, DataValueContainer& rThis){}

    inline std::ostream& operator << (std::ostream& rOstream, const DataValueContainer& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }

}

#endif //QUEST_DATA_VALUE_CONTAINER_HPP