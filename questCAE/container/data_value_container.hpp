/*------------------------------------------------------------
提供一个高效、灵活的数据存储解决方案，以支持各种变量的管理和操作
-------------------------------------------------------------*/

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

    class QUEST_API DataValueContainer{
        public:
            QUEST_DEFINE_LOCAL_FLAG(OVERWRITE_OLD_VALUES);
            QUEST_CLASS_POINTER_DEFINITION(DataValueContainer);

            using ValueType = std::pair<const VariableData*, void*>;
            using ContainerType = std::vector<ValueType>;
            using iterator = ContainerType::iterator;
            using const_iterator = ContainerType::const_iterator;
            using SizeType = ContainerType::size_type;

            DataValueContainer() {}

            DataValueContainer(const DataValueContainer& rOther){
                for(const_iterator i = rOther.mData.begin(); i!= rOther.mData.end(); ++i)
                    mData.push_back(*i);
            }

            virtual ~DataValueContainer() {
                for(iterator i = mData.begin(); i!= mData.end(); ++i)
                    i->first->Delete(i->second);
            }

            template<typename TDataType>
            const TDataType& operator()(const VariableData& rThisVariable) const{
                return GetValue<TDataType>(rThisVariable);
            }


            template<typename TDataType>
            TDataType& operator()(const Variable<TDataType>& rThisVariable){
                return GetValue<TDataType>(rThisVariable);
            }


            template<typename TDataType>
            const TDataType& operator() (const Variable<TDataType>& rThisVariable) const{
                return GetValue<TDataType>(rThisVariable);
            }


            template<typename TDataType>
            TDataType& operator[](const VariableData& rThisVariable){
                return GetValue<TDataType>(rThisVariable);
            }


            template<typename TDataType>
            const TDataType& operator[] (const VariableData& rThisVariable) const{
                return GetValue<TDataType>(rThisVariable);
            }

            
            template<typename TDataType>
            TDataType& operator[](const Variable<TDataType>& rThisVariable){
                return GetValue<TDataType>(rThisVariable);
            }


            template<typename TDataType>
            const TDataType& operator[] (const Variable<TDataType>& rThisVariable) const{
                return GetValue<TDataType>(rThisVariable);
            }


            iterator begin(){
                return mData.begin();
            }


            const_iterator begin() const{   
                return mData.begin();
            }


            iterator end(){
                return mData.end();
            }


            const_iterator end() const{
                return mData.end();
            }


            DataValueContainer& operator=(const DataValueContainer& rOther){
                clear();

                for(const_iterator i = rOther.mData.begin(); i!= rOther.mData.end(); ++i)
                    mData.push_back(ValueType(i->first, i->first->Clone(i->second)));

                return *this;
            }


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


            template<typename TDataType>
            const TDataType& GetValue(const Variable<TDataType>& rThisVariable) const{
                typename ContainerType::const_iterator i;

                if((i = std::find_if(mData.begin(), mData.end(), IndexCheck(rThisVariable.SourceKey()))) != mData.end())
                    return *(static_cast<TDataType*>(i->second)+rThisVariable.GetComponentIndex());
                
                return rThisVariable.Zero();
            }


            SizeType Size(){
                return mData.size();
            }


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


            template<typename TDataType>
            void Erase(const Variable<TDataType>& rThisVariable){
                typename ContainerType::iterator i;

                if((i = std::find_if(mData.begin(), mData.end(), IndexCheck(rThisVariable.SourceKey()))) != mData.end()){
                    i->first->Delete(i->second);
                    mData.erase(i);
                }
            }


            void clear(){
                for(ContainerType::iterator i = mData.begin(); i != mData.end(); ++i)
                    i->first->Delete(i->second);

                mData.clear();
            }


            void Merge(const DataValueContainer& rOther, const Flags Options);


            template<typename TDataType>
            bool Has(const Variable<TDataType>& rThisVariable) const{
                return std::find_if(mData.begin(), mData.end(), IndexCheck(rThisVariable.SourceKey())) != mData.end();
            }


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