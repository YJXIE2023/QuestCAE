/*---------------------------------------------------------------
管理和操作变量列表及其在 VariablesListDataValueContainer 中的位置
---------------------------------------------------------------*/

#ifndef QUEST_VARIABLES_LIST_HPP
#define QUEST_VARIABLES_LIST_HPP

// 系统头文件
#include <string>
#include <iostream>
#include <vector>
#include <atomic>

// 第三方头文件
#include <boost/iterator/indirect_iterator.hpp>    // indirect_iterator为迭代器适配器，用于间接地访问容器或数据结构中存储的指针或智能指针指向的元素

// 项目头文件
#include "includes/define.hpp"
#include "container/variable.hpp"

#ifdef QUEST_DEBUG
    #include "utilities/openmp_utils.hpp"
#endif

namespace Quest{

    class QUEST_API VariablesList final{
        public:
            QUEST_CLASS_INTRUSIVE_POINTER_DEFINITION(VariablesList);

            using SizeType = std::size_t;
            using IndexType = std::size_t;
            using BlockType = double;
            using data_type = VariableData;
            using value_type = const VariableData*;
            using const_pointer = const VariableData*;
            using const_reference = VariableData const&;

            using KeysContainerType = std::vector<IndexType>;
            using PositionsContanerType = std::vector<IndexType>;
            using VariablesContainerType = std::vector<const VariableData*>;

            using const_iterator = boost::indirect_iterator<VariablesContainerType::const_iterator>;
            using const_reverse_iterator = boost::indirect_iterator<VariablesContainerType::const_reverse_iterator>;

            using size_type = VariablesContainerType::size_type;
            using ptr_const_iterator = VariablesContainerType::const_iterator;
            using ptr_const_reverse_iterator = VariablesContainerType::const_reverse_iterator;
            using difference_type = VariablesContainerType::difference_type;

            VariablesList(){}

            template<typename TInputIteratorType>
            VariablesList(TInputIteratorType first, TInputIteratorType last){
                for(; first!= last; ++first){
                    Add(*first);
                }
            }

            VariablesList(const VariablesList& rOther):
                mDataSize(rOther.mDataSize),
                mHashFunctionIndex(rOther.mHashFunctionIndex),
                mKeys(rOther.mKeys),
                mPositions(rOther.mPositions),
                mVariables(rOther.mVariables),
                mDofVariables(rOther.mDofVariables),
                mDofReactions(rOther.mDofReactions)
            {}

            ~VariablesList(){}

            VariablesList& operator = (const VariablesList& rOther){
                mDataSize = rOther.mDataSize;
                mHashFunctionIndex = rOther.mHashFunctionIndex;
                mKeys = rOther.mKeys;
                mPositions = rOther.mPositions;
                mVariables = rOther.mVariables;
                mDofVariables = rOther.mDofVariables;
                mDofReactions = rOther.mDofReactions;

                return *this;
            }

            IndexType operator()(IndexType VariableKey) const{
                return GetPosition(VariableKey);
            }

            template<typename TDataType> 
            IndexType operator()(const Variable<TDataType>& rVariable) const{
                return GetPosition(rVariable.SourceKey());
            }

            bool operator == (const VariablesList& rOther) const{
                if(size() != rOther.size()){
                    return false;
                } else {
                    return std::equal(mPositions.begin(), mPositions.end(), rOther.mPositions.begin()) &&
                            std::equal(mVariables.begin(), mVariables.end(), rOther.mVariables.begin());
                }
            }

            unsigned int use_count() const noexcept{
                return mReferenceCounter;
            }

            const_iterator begin() const{
                return const_iterator(mVariables.begin());
            }

            const_iterator end() const{
                return const_iterator(mVariables.end());
            }

            const_reverse_iterator rbegin() const{
                return const_reverse_iterator(mVariables.rbegin());
            }

            const_reverse_iterator rend() const{
                return const_reverse_iterator(mVariables.rend());
            }

            ptr_const_iterator ptr_begin() const{
                return mVariables.begin();
            }

            ptr_const_iterator ptr_end() const{
                return mVariables.end();
            }

            ptr_const_reverse_iterator ptr_rbegin() const{
                return mVariables.rbegin();
            }

            ptr_const_reverse_iterator ptr_rend() const{
                return mVariables.rend();
            }

            const_reference front() const{
                assert(!IsEmpty());
                return *mVariables.front();
            }

            const_reference back() const{
                assert(!IsEmpty());
                return *mVariables.back();
            }

            size_type size() const{
                return mVariables.size();
            }

            size_type max_size() const{
                return mVariables.max_size();
            }

            void swap(VariablesList& rOther){
                SizeType temp = mDataSize;
                mDataSize = rOther.mDataSize;
                rOther.mDataSize = temp;

                temp = mHashFunctionIndex;
                mHashFunctionIndex = rOther.mHashFunctionIndex;
                rOther.mHashFunctionIndex = temp;

                mVariables.swap(rOther.mVariables);

                mDofVariables.swap(rOther.mDofVariables);
                mDofReactions.swap(rOther.mDofReactions);

                mKeys.swap(rOther.mKeys);
                mPositions.swap(rOther.mPositions);
            }

            template<typename TOtherDataType>
            void push_back(const TOtherDataType& rOther){
                Add(rOther);
            }

            void clear(){
                mDataSize = 0;
                mHashFunctionIndex = 0;
                mVariables.clear();
                mDofVariables.clear();
                mDofReactions.clear();
                mKeys = {static_cast<IndexType>(-1)};
                mPositions = {static_cast<IndexType>(-1)};
            }

            void Add(const VariableData& ThisVariable){
                if (ThisVariable.SourceKey() == 0){
                    QUEST_THROW_ERROR(std::logic_error,"Adding uninitialize variable to this variable list","");
                }

                if (Has(ThisVariable)){
                    return;
                }

                if (ThisVariable.IsComponent()){
                    Add(ThisVariable.GetSourceVariable());
                    return;
                }

                mVariables.push_back(&ThisVariable);
                SetPosition(ThisVariable.SourceKey(),mDataSize);
                const SizeType block_size = sizeof(BlockType);
                mDataSize += static_cast<SizeType>(((block_size - 1)+ThisVariable.Size())/block_size);
            }

            int AddDog(const VariableData* pThisDofVariable){
                for(std::size_t dof_index = 0; dof_index < pThisDofVariable->Size(); ++dof_index){
                    if(*mDofVariables[dof_index] == *pThisDofVariable){
                        return static_cast<int>(dof_index);
                    }
                }

                #ifdef QUEST_DEBUG
                    if(OpenMPUtils::IsInParallel() != 0){
                        QUEST_ERROR << "attempting to call AddDof for: " << pThisDofVariable << ". Unfortunately the Dof was not added before and the operations is not threadsafe (this function is being called within a parallel region)" << std::endl;
                    }
                #endif

                mDofVariables.push_back(pThisDofVariable);
                mDofReactions.push_back(nullptr);

                QUEST_DEBUG_ERROR_IF(mDofVariables.size()>64) << "Adding too many dofs to the node. Each node only can store 64 Dofs." << std::endl;
                return mDofVariables.size()-1;
            }


            int AddDof(const VariableData* pThisDofVariable, const VariableData* pThisDofReaction){
                for(std::size_t dof_index = 0; dof_index < pThisDofVariable->Size(); ++dof_index){
                    if(*mDofVariables[dof_index] == *pThisDofVariable){
                        mDofReactions[dof_index] = pThisDofReaction;
                        return static_cast<int>(dof_index);
                    }
                }

                #ifdef QUEST_DEBUG
                    if(OpenMPUtils::IsInParallel() != 0){
                        QUEST_ERROR << "attempting to call AddDof for: " << pThisDofVariable << ". Unfortunately the Dof was not added before and the operations is not threadsafe (this function is being called within a parallel region)" << std::endl;
                    }
                #endif

                mDofVariables.push_back(pThisDofVariable);
                mDofReactions.push_back(pThisDofReaction);

                QUEST_DEBUG_ERROR_IF(mDofVariables.size()>64) << "Adding too many dofs to the node. Each node only can store 64 Dofs." << std::endl;
                return mDofVariables.size()-1;
            }


            const VariableData& GetDofVariable(int DofIndex) const{
                return *mDofVariables[DofIndex];
            }


            const VariableData* pGetDofReaction(int DofIndex) const{
                return mDofReactions[DofIndex];
            }


            void SetDofReaaction(const VariableData* pThisDofReaction, int DofIndex){
                QUEST_DEBUG_ERROR_IF(static_cast<std::size_t>(DofIndex) >= mDofReactions.size()) << "The given dof with index = " << DofIndex  << " is not stored in this variables list" << std::endl;
                mDofReactions[DofIndex] = pThisDofReaction;
            }


            IndexType Index(IndexType VariableKey) const{
                return GetPosition(VariableKey);
            }


            template<typename TDataType>
            IndexType Index(const Variable<TDataType>& ThisVariable) const{
                return GetPosition(ThisVariable.SourceKey());
            }


            IndexType Index(const VariableData* pThisVariable) const{
                return GetPosition(pThisVariable->SourceKey());
            }


            SizeType DataSize() const{
                return mDataSize;
            }


            const VariablesContainerType& Variables(){
                return mVariables;
            }


            bool Has(const VariableData& rThisVariable) const{
                if(rThisVariable.IsComponent()){
                    return Has(rThisVariable.GetSourceVariable());
                }

                if(mPositions.empty()){
                    return false;
                }

                if(rThisVariable.SourceKey() == 0){
                    return false;
                }

                return mKeys[GetHashIndex(rThisVariable.SourceKey(),mKeys.size(),mHashFunctionIndex)] == rThisVariable.SourceKey();
            }


            bool IsEmpty() const{
                return mVariables.empty();
            }

            std::string Info() const{
                return "Variables list";
            }


            void printInfo(std::ostream& rOstream) const{
                rOstream << Info();
            }


            void PrintData(std::ostream& rOstream) const{
                rOstream << " with " << size() << " variables";
                rOstream << " (size : " << mDataSize << " blocks of " << sizeof(BlockType) << " bytes)" << std::endl;
                for(IndexType i = 0; i < mVariables.size(); ++i){
                    rOstream << "  " << mVariables[i]->Name() << " \t-> " << GetPosition(mVariables[i]->Key()) << std::endl;
                }

                rOstream << " with " << mDofVariables.size() << " Dofs:";
                for(IndexType i = 0; i < mDofVariables.size(); ++i){
                    rOstream << "    [" << mDofVariables[i]->Name() << " , " << ((mDofReactions[i] == nullptr) ? "NONE" : mDofReactions[i]->Name()) << " ]" << std::endl;
                }
            }

        protected:

        private:
            friend void intrusive_ptr_add_ref(const VariablesList* p){
                p->mReferenceCounter.fetch_add(1, std::memory_order_relaxed);
            }

            friend void intrusive_ptr_release(const VariablesList* p){
                if(p->mReferenceCounter.fetch_sub(1, std::memory_order_release) == 1){
                    std::atomic_thread_fence(std::memory_order_acquire);
                    delete p;
                }
            }

            void SetPosition(IndexType Key,SizeType ThePosition){
                if(mPositions.empty()){
                    ResizePositions();
                }

                if(mPositions[GetHashIndex(Key,mPositions.size(),mHashFunctionIndex)] < mDataSize){
                    ResizePositions();
                }

                mKeys[GetHashIndex(Key,mPositions.size(),mHashFunctionIndex)] = Key;
                mPositions[GetHashIndex(Key,mPositions.size(),mHashFunctionIndex)] = ThePosition;
            }

            SizeType GetHashIndex(std::size_t Key, std::size_t TableSize, std::size_t HashFunctionIndex) const{
                return (Key >> HashFunctionIndex) & (TableSize - 1);
            }

            SizeType GetPosition(IndexType Key) const{
                SizeType index = GetHashIndex(Key,mPositions.size(),mHashFunctionIndex);
                return mPositions[index];
            }

            void ResizePositions(){
                bool size_is_ok = false;
                std::size_t new_size = mPositions.size();
                SizeType new_hash_function_index = 0;
                while(size_is_ok != true){
                    new_hash_function_index++;
                    if(new_hash_function_index > 31){
                        new_hash_function_index = 0;
                        new_size *= 2;
                    }
                    KeysContainerType new_keys(new_size, static_cast<IndexType>(-1));
                    PositionsContanerType new_positions(new_size, static_cast<IndexType>(-1));
                    size_is_ok = true;

                    for(auto i_variable = mVariables.begin(); i_variable != mVariables.end(); ++i_variable){
                        if(new_positions[GetHashIndex((*i_variable)->SourceKey(),new_size,new_hash_function_index)] > mDataSize){
                            new_positions[GetHashIndex((*i_variable)->SourceKey(),new_size,new_hash_function_index)] = mPositions[GetHashIndex((*i_variable)->SourceKey(),mPositions.size(),mHashFunctionIndex)];
                            new_keys[GetHashIndex((*i_variable)->SourceKey(),new_size,new_hash_function_index)] = (*i_variable)->SourceKey();
                        } else {
                            size_is_ok = false;
                            break;
                        }
                    }

                    if(size_is_ok){
                        mPositions.swap(new_positions);
                        mKeys.swap(new_keys);
                        mHashFunctionIndex = new_hash_function_index;
                    }
                }
            }

            friend class Serializer;

            virtual void save(Serializer& rSerializer) const;

            virtual void load(Serializer& rSerializer);

        private:
            SizeType mDataSize = 0;
            SizeType mHashFunctionIndex = 0;
            KeysContainerType mKeys = {static_cast<IndexType>(-1)};
            PositionsContanerType mPositions = {static_cast<IndexType>(-1)};
            VariablesContainerType mVariables;
            VariablesContainerType mDofVariables;
            VariablesContainerType mDofReactions;

            mutable std::atomic<int> mReferenceCounter{0};

    };

    inline std::istream& operator >> (std::istream& rIstream, VariablesList& rThis){}

    inline std::ostream& operator << (std::ostream& rOstream, const VariablesList& rThis){
        rThis.printInfo(rOstream);
        rThis.PrintData(rOstream);

        return rOstream;
    }

} // namespace Quest

#endif //QUEST_VARIABLES_LIST_HPP