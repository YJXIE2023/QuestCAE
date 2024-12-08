/*------------------------------------------------
用于在多个容器中高效管理和访问共享变量的类
------------------------------------------------*/

#ifndef QUEST_VARIABLES_LIST_DATA_VALUE_CONTAINER_HPP
#define QUEST_VARIABLES_LIST_DATA_VALUE_CONTAINER_HPP

// 系统头文件
#include <string>
#include <iostream>
#include <cstddef>
#include <cstring>

// 项目头文件
#include "includes/define.hpp"
#include "container/variable.hpp"
#include "container/variables_list.hpp"

namespace Quest{

    class QUEST_API(QUEST_CORE) VariablesListDataValueContainer{
        public:
            QUEST_CLASS_POINTER_DEFINITION(VariablesListDataValueContainer);

            using BlockType = VariablesList::BlockType;
            using ContainerType = BlockType*;
            using IndexType = std::size_t;
            using SizeType = std::size_t;


            /**
             * @brief 构造函数
             * @param NewQueueSize 分析步的数量
             */
            explicit VariablesListDataValueContainer(SizeType NewQueueSize = 1):
                mQueueSize(NewQueueSize),
                mpCurrentPosition(0),
                mpData(0),
                mpVariablesList(nullptr)
            {
                if(!mpVariablesList){
                    return;
                }

                Allocate();

                mpCurrentPosition = mpData;

                const SizeType size = mpVariablesList->DataSize();
                for(VariablesList::const_iterator iVariables = mpVariablesList->begin(); iVariables != mpVariablesList->end(); ++iVariables){
                    BlockType* position = Position(*iVariables);
                    for(SizeType i = 0; i < mQueueSize; ++i){
                        iVariables->AssignZero(position+i*size);
                    }
                }
            }


            /**
             * @brief 复制构造函数
             * @param rOther 被复制的对象
             */
            VariablesListDataValueContainer(const VariablesListDataValueContainer& rOther):
                mQueueSize(rOther.mQueueSize),
                mpCurrentPosition(0),
                mpData(0),
                mpVariablesList(rOther.mpVariablesList)
            {
                if(!mpVariablesList){
                    return;
                }

                Allocate();

                mpCurrentPosition = mpData+(rOther.mpCurrentPosition-rOther.mpData);

                const SizeType size = mpVariablesList->DataSize();
                for(VariablesList::const_iterator iVariables = mpVariablesList->begin(); iVariables != mpVariablesList->end(); ++iVariables){
                    const SizeType offset = LocalOffset(*iVariables);
                    for(SizeType i = 0; i < mQueueSize; ++i){
                        const SizeType total_offset = i*size+offset;
                        iVariables->Copy(rOther.mpData+total_offset, mpData+total_offset);
                    }
                }
            }


            /**
             * @brief 构造函数
             * @param pVariablesList 变量列表对象
             * @param NewQueueSize 分析步的数量
             */
            VariablesListDataValueContainer(VariablesList::Pointer pVariablesList, SizeType NewQueueSize = 1):
                mQueueSize(NewQueueSize),
                mpCurrentPosition(0),
                mpData(0),
                mpVariablesList(pVariablesList)
            {
                if(!mpVariablesList){
                    return;
                }

                Allocate();

                mpCurrentPosition = mpData;

                const SizeType size = mpVariablesList->DataSize();
                for(VariablesList::const_iterator iVariables = mpVariablesList->begin(); iVariables != mpVariablesList->end(); ++iVariables){
                    BlockType* position = Position(*iVariables);
                    for(SizeType i = 0; i < mQueueSize; ++i){
                        iVariables->AssignZero(position+i*size);
                    }
                }
            }


            /**
             * @brief 构造函数
             * @param pVariablesList 变量列表对象的指针
             * @param ThsData 存储数据的块的头指针
             * @param NewQueueSize 分析步的数量
             */
            VariablesListDataValueContainer(VariablesList::Pointer pVariablesList, const BlockType* ThisData, SizeType NewQueueSize = 1):
                mQueueSize(NewQueueSize),
                mpCurrentPosition(0),
                mpData(0),
                mpVariablesList(pVariablesList)
            {
                if(!mpVariablesList){
                    return;
                }

                Allocate();

                mpCurrentPosition = mpData;

                const SizeType size = mpVariablesList->DataSize();
                for(VariablesList::const_iterator iVariables = mpVariablesList->begin(); iVariables != mpVariablesList->end(); ++iVariables){
                    const SizeType offset = LocalOffset(*iVariables);
                    for(SizeType i = 0; i < mQueueSize; ++i){
                        const SizeType total_offset = i*size+offset;
                        iVariables->Copy(ThisData+total_offset, mpData+total_offset);
                    }
                }
            }


            /**
             * @brief 析构函数
             */
            ~VariablesListDataValueContainer(){
                Clear();
            }


            /**
             * @brief 获取变量的值
             * @param rThisVariable 变量对象
             */
            template<typename TDataType>
            TDataType& operator()(const Variable<TDataType>& rThisVariable){
                return GetValue(rThisVariable);
            }


            /**
             * @brief 获取变量的值
             * @param rThisVariable 变量对象
             * @param QueueIndex 分析步的索引
             */
            template<typename TDataType>
            TDataType& operator()(const Variable<TDataType>& rThisVariable, SizeType QueueIndex){
                return GetValue(rThisVariable, QueueIndex);
            }


            /**
             * @brief 获取变量的值
             * @param rThisVariable 变量对象
             */
            template<typename TDataType>
            const TDataType& operator()(const Variable<TDataType>& rThisVariable) const{
                return GetValue(rThisVariable);
            }


            /**
             * @brief 获取变量的值
             * @param rThisVariable 变量对象
             * @param QueueIndex 分析步的索引
             */
            template<typename TDataType>
            const TDataType& operator()(const Variable<TDataType>& rThisVariable, SizeType QueueIndex) const{
                return GetValue(rThisVariable, QueueIndex);
            }


            /**
             * @brief 下标访问
             * @param rThisVariable 变量对象
             */
            template<typename TDataType>
            TDataType& operator[](const Variable<TDataType>& rThisVariable){
                return GetValue(rThisVariable);
            }


            /**
             * @brief 下标常量访问
             * @param rThisVariable 变量对象
             */
            template<typename TDataType>
            const TDataType& operator[](const Variable<TDataType>& rThisVariable) const{
                return GetValue(rThisVariable);
            }


            /**
             * @brief 赋值运算符重载
             */
            VariablesListDataValueContainer& operator=(const VariablesListDataValueContainer& rOther){
                if(rOther.mpVariablesList == 0){
                    Clear();
                } else if((mpVariablesList == rOther.mpVariablesList) && (mQueueSize == rOther.mQueueSize)){
                    mpCurrentPosition = mpData+(rOther.mpCurrentPosition-rOther.mpData);

                    const SizeType size = mpVariablesList->DataSize();
                    for(VariablesList::const_iterator iVariables = mpVariablesList->begin(); iVariables != mpVariablesList->end(); ++iVariables){
                        const SizeType offset = LocalOffset(*iVariables);
                        for(SizeType i = 0; i < mQueueSize; ++i){
                            const SizeType total_offset = i*size+offset;
                            iVariables->Assign(rOther.mpData+total_offset, mpData+total_offset);
                        }
                    }
                } else {
                    DestructAllElements();

                    mQueueSize = rOther.mQueueSize;
                    mpVariablesList = rOther.mpVariablesList;
                    Allocate();

                    mpCurrentPosition = mpData+(rOther.mpCurrentPosition-rOther.mpData);

                    const SizeType size = mpVariablesList->DataSize();
                    for(VariablesList::const_iterator iVariables = mpVariablesList->begin(); iVariables != mpVariablesList->end(); ++iVariables){
                        const SizeType offset = LocalOffset(*iVariables);
                        for(SizeType i = 0; i < mQueueSize; ++i){
                            const SizeType total_offset = i*size+offset;
                            iVariables->Copy(rOther.mpData+total_offset, mpData+total_offset);
                        }
                    }
                }

                return *this;
            }


            /**
             * @brief 获取变量的值
             * @param rThisVariable 变量对象
             */
            template<typename TDataType>
            TDataType& GetValue(const Variable<TDataType>& rThisVariable){
                QUEST_DEBUG_ERROR_IF(!mpVariablesList) << "This container don't have a variables list assigned." << std::endl;
                QUEST_DEBUG_ERROR_IF_NOT(mpVariablesList->Has(rThisVariable)) << "This container don't have this variable: " << rThisVariable << std::endl;
                return *(reinterpret_cast<TDataType*>(Position(rThisVariable))+rThisVariable.GetComponentIndex());
            }


            /**
             * @brief 获取变量的值
             * @param rThisVariable 变量对象
             * @param QueueIndex 分析步的索引
             */
            template<typename TDataType>
            TDataType& GetValue(const Variable<TDataType>& rThisVariable, SizeType QueueIndex){
                QUEST_DEBUG_ERROR_IF(!mpVariablesList) << "This container don't have a variables list assigned." << std::endl;
                QUEST_DEBUG_ERROR_IF_NOT(mpVariablesList->Has(rThisVariable)) << "This container don't have this variable: " << rThisVariable << std::endl;
                return *(reinterpret_cast<TDataType*>(Position(rThisVariable, QueueIndex))+rThisVariable.GetComponentIndex());
            }


            /**
             * @brief 获取变量的常量引用
             * @param rThisVariable 变量对象
             */
            template<typename TDataType>
            const TDataType& GetValue(const Variable<TDataType>& rThisVariable) const{
                QUEST_DEBUG_ERROR_IF(!mpVariablesList) << "This container don't have a variables list assigned." << std::endl;
                QUEST_DEBUG_ERROR_IF_NOT(mpVariablesList->Has(rThisVariable)) << "This container don't have this variable: " << rThisVariable << std::endl;
                return *(reinterpret_cast<const TDataType*>(Position(rThisVariable))+rThisVariable.GetComponentIndex());
            }


            /**
             * @brief 获取变量的常量引用
             * @param rThisVariable 变量对象
             * @param QueueIndex 分析步的索引
             */
            template<typename TDataType>
            const TDataType& GetValue(const Variable<TDataType>& rThisVariable, SizeType QueueIndex) const{
                QUEST_DEBUG_ERROR_IF(!mpVariablesList) << "This container don't have a variables list assigned." << std::endl;
                QUEST_DEBUG_ERROR_IF_NOT(mpVariablesList->Has(rThisVariable)) << "This container don't have this variable: " << rThisVariable << std::endl;
                return *(reinterpret_cast<const TDataType*>(Position(rThisVariable, QueueIndex))+rThisVariable.GetComponentIndex());
            }


            /**
             * @brief 快速获取变量的值
             * @details 该函数不进行检查，直接返回变量的值的引用，因此效率较高，但不安全，仅在确保变量的有效性时使用
             * @param rThisVariable 变量对象
             */
            template<typename TDataType>
            TDataType& FastGetValue(const Variable<TDataType>& rThisVariable){
                return *(reinterpret_cast<TDataType*>(Position(rThisVariable))+rThisVariable.GetComponentIndex());
            }


            /**
             * @brief 快速获取变量的值
             * @details 该函数不进行检查，直接返回变量的值的指针，因此效率较高，但不安全，仅在确保变量的有效性时使用
             * @param rThisVariable 变量对象
             */
            template<typename TDataType>
            TDataType* pFastGetValue(const Variable<TDataType>& rThisVariable){
                return reinterpret_cast<TdataType*>(Position(rThisVariable))+rThisVariable.GetComponentIndex();
            }


            /**
             * @brief 快速获取变量的常量引用
             * @details 该函数不进行检查，直接返回变量的值的引用，因此效率较高，但不安全，仅在确保变量的有效性时使用
             * @param rThisVariable 变量对象
             * @param QueueIndex 分析步的索引
             */
            template<typename TDataType>
            TDataType& FastGetValue(const Variable<TDataType>& rThisVariable, SizeType QueueIndex){
                return *(reinterpret_cast<TDataType*>(Position(rThisVariable, QueueIndex))+rThisVariable.GetComponentIndex());
            }


            /**
             * @brief 快速获取变量的常量引用
             * @details 基于变量的位置的指针，快速获取变量的值
             * @param rThisVariable 变量对象
             * @param QueueIndex 分析步的索引
             * @param ThisPosition 指向变量数据的指针
             */
            template<typename TDataType>
            TDataType& FastGetValue(const Variable<TDataType>& rThisVariable, SizeType QueueIndex, SizeType ThisPosition){
                QUEST_DEBUG_ERROR_IF(!mpVariablesList) << "This container don't have a variables list assigned." << std::endl;
                QUEST_DEBUG_ERROR_IF_NOT(mpVariablesList->Has(rThisVariable)) << "This container don't have this variable: " << rThisVariable << std::endl;
                QUEST_DEBUG_ERROR_IF((ThisPosition+1)>mQueueSize) << "Trying to access data from step " << ThisPosition << " but only " << mQueueSize << " steps are stored." << std::endl;
                return *(reinterpret_cast<TDataType*>(Position(rThisVariable, QueueIndex))+rThisVariable.GetComponentIndex());
            }


            /**
             * @brief 快速获取当前位置的变量的值
             * @details 基于变量的位置的指针，快速获取变量的值
             * @param rThisVariable 变量对象
             * @param ThisPosition 指向变量数据的指针
             */
            template<typename TDataType>
            TDataType& FastGetCurrentValue(const Variable<TDataType>& rThisVariable, SizeType ThisPosition){
                QUEST_DEBUG_ERROR_IF(!mpVariablesList) << "This container don't have a variables list assigned." << std::endl;
                QUEST_DEBUG_ERROR_IF_NOT(mpVariablesList->Has(rThisVariable)) << "This container don't have this variable: " << rThisVariable << std::endl;
                return *(reinterpret_cast<TDataType*>(mpCurrentPosition+ThisPosition)+rThisVariable.GetComponentIndex());
            }


            /**
             * @brief 快速获取变量的常量引用
             * @details 该函数不进行检查，直接返回变量的值的指针，因此效率较高，但不安全，仅在确保变量的有效性时使用
             * @param rThisVariable 变量对象
             */
            template<typename TDataType>
            const TDataType& FastGetValue(const Variable<TDataType>& rThisVariable) const{
                return *(reinterpret_cast<const TDataType*>(Position(rThisVariable))+rThisVariable.GetComponentIndex());
            }


            /**
             * @brief 快速获取变量的常量指针引用
             * @details 该函数不进行检查，直接返回变量的值的指针，因此效率较高，但不安全，仅在确保变量的有效性时使用
             * @param rThisVariable 变量对象
             */
            template<typename TDataType>
            const TDataType* pFastGetValue(const Variable<TDataType>& rThisVariable) const{
                return reinterpret_cast<const TdataType*>(Position(rThisVariable))+rThisVariable.GetComponentIndex();
            }


            /**
             * @brief 快速获取变量的常量引用
             * @details 该函数不进行检查，直接返回变量的值的指针，因此效率较高，但不安全，仅在确保变量的有效性时使用
             * @param rThisVariable 变量对象
             * @param QueueIndex 分析步的索引
             */
            template<typename TDataType>
            const TDataType& FastGetValue(const Variable<TDataType>& rThisVariable, SizeType QueueIndex) const{
                return *(reinterpret_cast<const TDataType*>(Position(rThisVariable, QueueIndex))+rThisVariable.GetComponentIndex());
            }


            /**
             * @brief 快速获取变量的常量引用
             * @details 基于变量的位置的指针，快速获取变量的值
             * @param rThisVariable 变量对象
             * @param QueueIndex 分析步的索引
             * @param ThisPosition 变量的位置
             */
            template<typename TDataType>
            const TDataType& FastGetValue(const Variable<TDataType>& rThisVariable, SizeType QueueIndex, SizeType ThisPosition) const{
                return *(reinterpret_cast<const TDataType*>(Position(QueueIndex)+ThisPosition)+rThisVariable.GetComponentIndex());
            }


            /**
             * @brief 快速获取变量的常量引用
             * @details 基于变量的位置的指针，快速获取变量的值
             * @param rThisVariable 变量对象
             * @param ThisPosition 变量值的位置与当前位置的偏移量
             */
            template<typename TDataType>
            const TDataType& FastGetCurrentValue(const Variable<TDataType>& rThisVariable, SizeType ThisPosition) const{
                return *(reinterpret_cast<const TDataType*>(mpCurrentPosition+ThisPosition)+rThisVariable.GetComponentIndex());
            }


            /**
             * @brief 变量的数量
             */
            SizeType Size() const{
                if(!mpVariablesList){
                    return 0;
                }

                return mpVariablesList->DataSize();
            }


            /**
             * @brief 获取分析步的数量
             */
            SizeType QueueSize() const{
                return mQueueSize;
            }


            /**
             * @brief 变量数量*分析步的数量
             */
            SizeType TotalSize() const{
                if(!mpVariablesList){
                    return 0;
                }

                return mpVariablesList->DataSize() * mQueueSize;
            }


            /**
             * @brief 设置变量的值
             * @param rThisVariable 变量对象
             * @param rValue 变量的值
             */
            template<typename TDataType>
            void SetValue(const Variable<TDataType>& rThisVariable, const TDataType& rValue){
                GetValue(rThisVariable) = rValue;
            }


            /**
             * @brief 设置变量的值
             * @param rThisVariable 变量对象
             * @param rValue 变量的值
             * @param QueueIndex 分析步的索引
             */
            template<typename TDataType>
            void SetValue(const Variable<TDataType>& rThisVariable, const TDataType& rValue, SizeType QueueIndex){
                GetValue(rThisVariable, QueueIndex) = rValue;
            }


            /**
             * @brief 销毁所有元素并释放空间
             */
            void Clear(){
                DestructAllElements();
                if(mpData){
                    free(mpData);
                }

                mpData = 0;
            }


            /**
             * @brief 获取变量列表对象的指针
             */
            VariablesList::Pointer pGetVariablesList(){
                return mpVariablesList;
            }


            /**
             * @brief 获取变量列表对象的常量指针
             */
            const VariablesList::Pointer pGetVariablesList() const{
                return mpVariablesList;
            }


            /**
             * @brief 获取变量列表对象的引用
             */
            VariablesList& GetVariablesList(){
                return *mpVariablesList;
            }


            /**
             * @brief 获取变量列表对象的常量引用
             */
            const VariablesList& GetVariablesList() const{
                return *mpVariablesList;
            }


            /**
             * @brief 设置变量列表对象
             */
            void SetVariablesList(VariablesList::Pointer pVariablesList){
                DestructAllElements();

                mpVariablesList = pVariablesList;

                if(!mpVariablesList){
                    return;
                }

                Reallocate();
                mpCurrentPosition = mpData;

                const SizeType size = mpVariablesList->DataSize();
                for(VariablesList::const_iterator iVariables = mpVariablesList->begin(); iVariables != mpVariablesList->end(); ++iVariables){
                    BlockType* position = Position(*iVariables);
                    for(SizeType i = 0; i < mQueueSize; ++i){
                        iVariables->AssignZero(position+i*size);
                    }
                }
            }


            /**
             * @brief 设置变量列表对象
             * @param ThisQueueSize 分析步的数量
             */
            void SetVariablesList(VariablesList::Pointer pVariablesList, SizeType ThisQueueSize){
                DestructAllElements();

                mpVariablesList = pVariablesList;
                mQueueSize = ThisQueueSize;

                if(!mpVariablesList){
                    return;
                }

                Reallocate();
                mpCurrentPosition = mpData;

                const SizeType size = mpVariablesList->DataSize();
                for(VariablesList::const_iterator iVariables = mpVariablesList->begin(); iVariables != mpVariablesList->end(); ++iVariables){
                    BlockType* position = Position(*iVariables);
                    for(SizeType i = 0; i < mQueueSize; ++i){
                        iVariables->AssignZero(position+i*size);
                    }
                }
            }


            /**
             * @brief 重新设置分析步的数量
             */
            void Resize(SizeType NewSize){
                if(NewSize == mQueueSize){
                    return;
                }

                if(!mpVariablesList){
                    return;
                }

                if(NewSize < mQueueSize){
                    for(SizeType i = NewSize; i < mQueueSize; ++i){
                        DestructElements(i);
                    }
                    
                    const SizeType size = mpVariablesList->DataSize();
                    BlockType* temp = (BlockType*)malloc(size*sizeof(BlockType)*NewSize);
                    for(SizeType i = 0; i < NewSize; ++i){
                        memcpy(temp+i*size,Position(i),size*sizeof(BlockType));
                    }

                    mQueueSize = NewSize;
                    free(mpData);
                    mpData = temp;
                    mpCurrentPosition = mpData;
                } else {
                    const SizeType difference = NewSize - mQueueSize;
                    const SizeType old_size = mQueueSize;
                    const SizeType current_offset = mpCurrentPosition - mpData;

                    mQueueSize = NewSize;
                    Reallocate();
                    
                    SizeType size = mpVariablesList->DataSize();
                    mpCurrentPosition = mpData+current_offset;

                    const SizeType region_size = old_size*size-current_offset;
                    memmove(mpCurrentPosition+difference*size, mpCurrentPosition, region_size*sizeof(BlockType));

                    for(VariablesList::const_iterator iVariables = mpVariablesList->begin(); iVariables != mpVariablesList->end(); ++iVariables){
                        BlockType* position = mpCurrentPosition + LocalOffset(*iVariables);
                        for(SizeType i = 0; i < difference; ++i){
                            iVariables->AssignZero(position+i*size);
                        }
                    }

                    mpCurrentPosition += difference*size;
                }
            }


            /**
             * @brief 获取指向数据内存块的头指针
             */
            BlockType* Data(){
                return mpData;
            }


            /**
             * @brief 获取指向数据内存块的头指针
             */
            const BlockType* Data() const{
                return mpData;
            }


            /**
             * @brief 获取指向指定分析步的数据内存块的头指针
             */
            BlockType* Data(SizeType QueueIndex){
                return Position(QueueIndex);
            }


            /**
             * @brief 获取指向特定变量的数据内存的指针
             */
            BlockType* Data(const VariableData& rThisVariable){
                QUEST_DEBUG_ERROR_IF(!mpVariablesList->Has(rThisVariable)) << "Variable " << rThisVariable.Name() << " is not in the variables list." << std::endl;
                return Position(rThisVariable);
            }


            /**
             * @brief 每个分析步所有数据所需的内存大小
             */
            SizeType DataSize(){
                if(!mpVariablesList){
                    return 0;
                }

                return mpVariablesList->DataSize()*sizeof(BlockType);
            }


            /**
             * @brief 所有分析步所有数据所需的内存大小
             */
            SizeType TotalDataSize(){
                if(!mpVariablesList){
                    return 0;
                }

                return mpVariablesList->DataSize()*sizeof(BlockType)*mQueueSize;
            }


            /**
             * @brief 将数据分配到某分析步的位置
             * @param Source 数据源
             * @param QueueIndex 目标分析步的索引
             */
            void AssignData(BlockType* Source, SizeType QueueIndex){
                AssignData(Source, Position(QueueIndex));
            }


            /**
             * @brief 复制数据
             */
            void CloneFront(){
                if(mQueueSize == 0){
                    Resize(1);
                    return;
                }

                if(mQueueSize == 1){
                    return;
                }

                QUEST_DEBUG_ERROR_IF(!mpVariablesList) << "This container don't have a variables list assigned." << std::endl;
                const SizeType size = mpVariablesList->DataSize();
                BlockType* position = (mpCurrentPosition == mpData) ? mpData+TotalSize()-size : mpCurrentPosition-size;
                AssignData(mpCurrentPosition, position);
                mpCurrentPosition = position;
            }


            /**
             * @brief 将数据插入到头部
             */
            void PushFront(){
                if(mQueueSize == 0){
                    Resize(1);
                    return;
                }

                if(mQueueSize == 1){
                    return;
                }

                QUEST_DEBUG_ERROR_IF(!mpVariablesList) << "This container don't have a variables list assigned." << std::endl;
                const SizeType size = mpVariablesList->DataSize();
                mpCurrentPosition = (mpCurrentPosition == mpData) ? mpData+TotalSize()-size : mpCurrentPosition-size;
                AssignZero();
            }


            /**
             * @brief 将数据分配为零值
             */
            void AssignZero(){
                QUEST_DEBUG_ERROR_IF(!mpVariablesList) << "This container don't have a variables list assigned." << std::endl;
                for(VariablesList::const_iterator iVariables = mpVariablesList->begin(); iVariables != mpVariablesList->end(); ++iVariables){
                    iVariables->AssignZero(mpCurrentPosition+LocalOffset(*iVariables));
                }
            }


            /**
             * @brief 将数据分配为零值
             */
            void AssignZero(const SizeType QueueIndex){
                QUEST_DEBUG_ERROR_IF(!mpVariablesList) << "This container don't have a variables list assigned." << std::endl;
                BlockType* position = Position(QueueIndex);
                for(VariablesList::const_iterator iVariables = mpVariablesList->begin(); iVariables != mpVariablesList->end(); ++iVariables){
                    iVariables->AssignZero(position+LocalOffset(*iVariables));
                }
            }


            /**
             * @brief 变量是否存在
             */
            bool Has(const VariableData& rThisVariable) const{
                if(!mpVariablesList){
                    return false;
                }

                return mpVariablesList->Has(rThisVariable);
            }


            /**
             * @brief 是否存在变量
             */
            bool IsEmpty() const{
                if(!mpVariablesList){
                    return true;
                }

                return mpVariablesList->IsEmpty();
            }

            std::string Info() const{
                std::stringstream buffer;
                buffer << "variables list data value container";

                return buffer.str();
            }

            void PrintInfo(std::ostream& rOstream) const{
                rOstream << Info();
            }

            void PrintData(std::ostream& rOstream) const{
                if(!mpVariablesList){
                    rOstream << "No variables list assigned." << std::endl;
                }

                for(VariablesList::const_iterator iVariables = mpVariablesList->begin(); iVariables != mpVariablesList->end(); ++iVariables){
                    rOstream << "   ";
                    for(SizeType i = 0; i < mQueueSize; ++i){
                        rOstream << i << ": ";
                        iVariables->Print(Position(*iVariables, i), rOstream);
                        rOstream << " ";
                    }
                    rOstream << std::endl;
                }
            }

        protected:

        private:
            /**
             * @brief 分配内存
             */
            inline void Allocate(){
                QUEST_DEBUG_ERROR_IF(!mpVariablesList) << "This container don't have a variables list assigned." << std::endl;
                mpData = (BlockType*) malloc(mpVariablesList->DataSize() * sizeof(BlockType) * mQueueSize);
            }


            /**
             * @brief 重新分配内存
             */
            inline void Reallocate(){
                QUEST_DEBUG_ERROR_IF(!mpVariablesList) << "This container don't have a variables list assigned." << std::endl;
                mpData = (BlockType*) realloc(mpData, mpVariablesList->DataSize() * sizeof(BlockType) * mQueueSize);
            }


            /**
             * @brief 销毁所有某个元素
             */
            void DestructElements(SizeType ThisIndex){
                if(!mpVariablesList){
                    return;
                }

                if(mpData == 0){
                    return;
                }
                BlockType* position = Position(ThisIndex);
                for(VariablesList::const_iterator iVariables = mpVariablesList->begin(); iVariables!= mpVariablesList->end(); ++iVariables){
                    iVariables->Destruct(position+LocalOffset(*iVariables));
                }
            }


            /**
             * @brief 销毁所有元素
             */
            void DestructAllElements(){
                if(!mpVariablesList){
                    return;
                }

                if(mpData == 0){
                    return;
                }

                const SizeType size = mpVariablesList->DataSize();
                for(VariablesList::const_iterator iVariables = mpVariablesList->begin(); iVariables!= mpVariablesList->end(); ++iVariables){
                    BlockType* position = mpData + LocalOffset(*iVariables);
                    for(SizeType i = 0; i < mQueueSize; ++i){
                        iVariables->Destruct(position+i*size);
                    }
                }
            }


            /**
             * @brief 将数据从源位置分配到目标位置
             * @param Source 源位置
             * @param Destination 目标位置
             */
            void AssignData(BlockType* Source, BlockType* Destination){
                QUEST_DEBUG_ERROR_IF(!mpVariablesList) << "This container don't have a variables list assigned." << std::endl;
                for(VariablesList::const_iterator iVariables = mpVariablesList->begin(); iVariables != mpVariablesList->end(); ++iVariables){
                    const SizeType offset = LocalOffset(*iVariables);
                    iVariables->Assign(Source+offset, Destination+offset);
                }
            }


            /**
             * @brief 获取变量在内存块中的偏移量
             * @param rThisVariable 变量对象
             */
            inline SizeType LocalOffset(const VariableData& rThisVariable) const{
                QUEST_DEBUG_ERROR_IF(!mpVariablesList) << "This container don't have a variables list assigned." << std::endl;
                return mpVariablesList->Index(rThisVariable.SourceKey());
            }


            /**
             * @brief 获取变量在内存中的位置
             * @param rThisVariable 变量对象
             * @param QueueIndex 分析步的索引
             */
            inline BlockType* Position(const VariableData& rThisVariable) const{
                QUEST_DEBUG_ERROR_IF(!mpVariablesList) << "This container don't have a variables list assigned." << std::endl;
                QUEST_DEBUG_ERROR_IF_NOT(mpVariablesList->Has(rThisVariable)) << "This container don't have this variable: " << rThisVariable << std::endl;
                return mpCurrentPosition + mpVariablesList->Index(rThisVariable.SourceKey());
            }


            /**
             * @brief 获取某分析步中变量在内存中的位置
             * @param rThisVariable 变量对象
             * @param ThisIndex 分析步的索引
             */
            inline BlockType* Position(const VariableData& rThisVariable, SizeType ThisIndex) const{
                QUEST_DEBUG_ERROR_IF(!mpVariablesList) << "This container don't have a variables list assigned." << std::endl;
                QUEST_DEBUG_ERROR_IF_NOT(mpVariablesList->Has(rThisVariable)) << "This container don't have this variable: " << rThisVariable << std::endl;
                QUEST_DEBUG_ERROR_IF((ThisIndex+1)>mQueueSize) << "Trying to access data from step " << ThisIndex << " but only " << mQueueSize << " steps are stored." << std::endl;
                return Position(ThisIndex)+mpVariablesList->Index(rThisVariable.SourceKey());
            }


            /**
             * @brief 返回指针当前位置
             */
            inline BlockType* Position() const{
                return mpCurrentPosition;
            }


            /**
             * @brief 基于分析步索引获取变量在内存中的位置
             * @param ThisIndex 变量的索引
             */
            inline BlockType* Position(SizeType ThisIndex) const{
                QUEST_DEBUG_ERROR_IF(!mpVariablesList) << "This container don't have a variables list assigned." << std::endl;
                const SizeType total_size = TotalSize();
                BlockType* position = mpCurrentPosition + ThisIndex*mpVariablesList->DataSize();
                return (position<(mpData+total_size)) ? position : position-total_size;
            }


            friend class Serializer;

            void save(Serializer& rSerializer) const{
                QUEST_ERROR_IF(!mpVariablesList) << "Cannot save a container without a variables list" << std::endl;
                QUEST_ERROR_IF(mpData == 0) << "Cannot sava an empty variables list container" << std::endl;

                rSerializer.save("Variables List", mpVariablesList);
                rSerializer.save("QueueSize", mQueueSize);
                if(mpVariablesList->DataSize() != 0){
                    rSerializer.save("QueueIndex", SizeType(mpCurrentPosition-mpData)/mpVariablesList->DataSize());
                } else {
                    rSerializer.save("QueueIndex", SizeType(0));
                }

                const SizeType size = mpVariablesList->DataSize();
                for(VariablesList::const_iterator iVariables = mpVariablesList->begin(); iVariables != mpVariablesList->end(); ++iVariables){
                    BlockType* position = mpData + LocalOffset(*iVariables);
                    for(SizeType i = 0; i < mQueueSize; ++i){
                        iVariables->Save(rSerializer, position+i*size);
                    }
                }
            }

            void load(Serializer& rSerializer){
                rSerializer.load("Variables List", mpVariablesList);
                rSerializer.load("QueueSize", mQueueSize);
                SizeType queue_index;
                rSerializer.load("QueueIndex", queue_index);
                Allocate();

                if(queue_index > mQueueSize){
                    QUEST_THROW_ERROR(std::invalid_argument, "Invalid Queue index loaded: ", queue_index)
                    mpCurrentPosition = mpData + queue_index*mpVariablesList->DataSize();
                }

                std::string name;
                for(SizeType i = 0; i < mQueueSize; ++i){
                    AssignZero(i);
                }

                const SizeType size = mpVariablesList->DataSize();
                for(VariablesList::const_iterator iVariables = mpVariablesList->begin(); iVariables != mpVariablesList->end(); ++iVariables){
                    BlockType* position = mpData + LocalOffset(*iVariables);
                    for(SizeType i = 0; i < mQueueSize; ++i){
                        iVariables->Load(rSerializer, position+i*size);
                    }
                }
            }

        private:
            /**
             * @brief 分析步的数量
             */
            SizeType mQueueSize;


            /**
             * @brief 当前指针指向的位置
             */
            BlockType* mpCurrentPosition;


            /**
             * @brief 存储所有变量的数据的内存块的头指针
             */
            ContainerType mpData;


            /**
             * @brief 变量列表
             * @details 变量列表对象负责管理变量数量及变量在内存块中的偏移量
             */
            VariablesList::Pointer mpVariablesList;

    };

    inline std::istream& operator >> (std::istream& rIstream, VariablesListDataValueContainer& rThis){}

    inline std::ostream& operator << (std::ostream& rOstream, const VariablesListDataValueContainer& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }


} // namespace Quest

#endif //QUEST_VARIABLES_LIST_DATA_VALUE_CONTAINER_HPP