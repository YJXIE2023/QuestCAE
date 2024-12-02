#ifndef QUEST_VARIABLES_LIST_HPP
#define QUEST_VARIABLES_LIST_HPP

// 系统头文件
#include <string>
#include <iostream>
#include <vector>
#include <atomic>

// 第三方头文件
/**
 * @brief 简介迭代器
 * @details boost::indirect_iterator<std::vector<std::shared_ptr<MyClass>>::iterator>：
 * 这是一个间接迭代器，它包装了一个普通的迭代器（指向存储智能指针的容器），并允许你访问智能指针指向的 MyClass 对象
 */
#include <boost/iterator/indirect_iterator.hpp> 

// 项目头文件
#include "includes/define.hpp"
#include "container/variable.hpp"

#ifdef QUEST_DEBUG
    #include "utilities/openmp_utils.hpp"
#endif

namespace Quest{

    /**
     * @class VariablesList
     * @brief 存储变量列表及其在 VariablesListDataValueContainer 中的位置
     */
    class QUEST_API(QUEST_CORE) VariablesList final{
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


            /**
             * @brief 构造函数
             */
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


            /**
             * @brief 增加变量到列表中
             * @param ThisVariable 要增加的变量
             */
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


            /**
             * @brief 添加自由度变量及其反力变量到列表中
             * @param pThisDofVariable 要增加的自由度变量
             */
            int AddDof(const VariableData* pThisDofVariable){
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


            /**
             * @brief 添加自由度变量及其反力变量到列表中
             * @param pThisDofVariable 要增加的自由度变量
             * @param pThisDofReaction 要增加的反力变量
             */
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


            /**
             * @brief 获取自由度变量
             * @param DofIndex 自由度变量在 mDofVariables 中的索引
             */
            const VariableData& GetDofVariable(int DofIndex) const{
                return *mDofVariables[DofIndex];
            }


            /**
             * @brief 获取自由度反力变量
             * @param DofIndex 自由度变量在 mDofReactions 中的索引
             */
            const VariableData* pGetDofReaction(int DofIndex) const{
                return mDofReactions[DofIndex];
            }


            /**
             * @brief 设置自由度反力变量
             * @param pThisDofReaction 要设置的自由度反力变量
             * @param DofIndex 源自由度反力变量在 mDofReactions 中的索引
             */
            void SetDofReaction(const VariableData* pThisDofReaction, int DofIndex){
                QUEST_DEBUG_ERROR_IF(static_cast<std::size_t>(DofIndex) >= mDofReactions.size()) << "The given dof with index = " << DofIndex  << " is not stored in this variables list" << std::endl;
                mDofReactions[DofIndex] = pThisDofReaction;
            }


            /**
             * @brief 获取变量在 VariablesListDataValueContainer 中的位置
             * @param VariableKey 变量的键值
             */
            IndexType Index(IndexType VariableKey) const{
                return GetPosition(VariableKey);
            }


            /**
             * @brief 获取变量在 VariablesListDataValueContainer 中的位置
             * @param ThisVariable 要获取的变量
             */
            template<typename TDataType>
            IndexType Index(const Variable<TDataType>& ThisVariable) const{
                return GetPosition(ThisVariable.SourceKey());
            }


            /**
             * @brief 获取变量在 VariablesListDataValueContainer 中的位置
             * @param pThisVariable 要获取的变量的指针
             */
            IndexType Index(const VariableData* pThisVariable) const{
                return GetPosition(pThisVariable->SourceKey());
            }


            /**
             * @brief 获取变量数量
             */
            SizeType DataSize() const{
                return mDataSize;
            }


            /**
             * @brief 获取存储全部变量的容器
             * @details VariablesContainerType 为 std::vector<const VariableData*>
             */
            const VariablesContainerType& Variables(){
                return mVariables;
            }


            /**
             * @brief 判断变量是否存在
             * @param rThisVariable 要判断的变量
             */
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


            /**
             * @brief 判断是否有变量
             */
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


            /**
             * @brief 设置变量在 mKeys 和 mPositions 中的位置
             * @param Key 变量的键值
             * @param ThePosition 变量在 mPositions 中的位置
             */
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


            /**
             * @brief 获取变量在 mKeys 和 mPositions 中的位置
             * @param Key 变量的键值
             * @param TableSize mKeys 和 mPositions 的大小
             * @param HashFunctionIndex 哈希函数的索引
             */
            SizeType GetHashIndex(std::size_t Key, std::size_t TableSize, std::size_t HashFunctionIndex) const{
                return (Key >> HashFunctionIndex) & (TableSize - 1);
            }


            /**
             * @brief 获取变量在 VariablesListDataValueContainer 中的位置
             * @param Key 变量的键值
             */
            SizeType GetPosition(IndexType Key) const{
                SizeType index = GetHashIndex(Key,mPositions.size(),mHashFunctionIndex);
                return mPositions[index];
            }


            /**
             * @brief 调整 mKeys 和 mPositions 的大小
             * @details 当哈希表的大小不够，或者哈希函数导致冲突时，通过调整哈希函数或增加容量来重新组织数据，确保哈希表性能和数据一致性
             */
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
            /**
             * @brief 变量数量
             */
            SizeType mDataSize = 0;

            /**
             * @brief 哈希函数的索引
             */
            SizeType mHashFunctionIndex = 0;

            /**
             * @brief 存储变量键值的Vector
             * @details std::vector<std::size_t> mKeys;
             * 用于判断变量是否存在，mKeys[i] 与 mVariables[i] 对应
             */
            KeysContainerType mKeys = {static_cast<IndexType>(-1)};

            /**
             * @brief 存储变量在 VariablesListDataValueContainer 中的位置的Vector
             * @details std::vector<std::size_t> mPositions;
             * mKeys[i] 与 mPositions[i] 对应
             */
            PositionsContanerType mPositions = {static_cast<IndexType>(-1)};

            /**
             * @brief 存储变量的Vector
             * @details std::vector<const VariableData*> mVariables;
             */
            VariablesContainerType mVariables;

            /**
             * @brief 存储自由度变量的Vector
             * @details std::vector<const VariableData*> mDofVariables;
             */
            VariablesContainerType mDofVariables;

            /**
             * @brief 存储自由度反力变量的Vector
             * @details std::vector<const VariableData*> mDofReactions;
             * 例如自由度为节点x方向位移，自由度反力变量即为对应的约束反力Fx
             */
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