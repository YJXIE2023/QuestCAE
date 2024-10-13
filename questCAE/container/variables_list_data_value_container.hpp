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

            ~VariablesListDataValueContainer(){
                Clear();
            }

            template<typename TDataType>
            TDataType& operator()(const Variable<TDataType>& rThisVariable){
                return GetValue(rThisVariable);
            }

            template<typename TDataType>
            TDataType& operator()(const Variable<TDataType>& rThisVariable, SizeType QueueIndex){
                return GetValue(rThisVariable, QueueIndex);
            }

            template<typename TDataType>
            const TDataType& operator()(const Variable<TDataType>& rThisVariable) const{
                return GetValue(rThisVariable);
            }

            template<typename TDataType>
            const TDataType& operator()(const Variable<TDataType>& rThisVariable, SizeType QueueIndex) const{
                return GetValue(rThisVariable, QueueIndex);
            }

            template<typename TDataType>
            TDataType& operator[](const Variable<TDataType>& rThisVariable){
                return GetValue(rThisVariable);
            }

            template<typename TDataType>
            const TDataType& operator[](const Variable<TDataType>& rThisVariable) const{
                return GetValue(rThisVariable);
            }

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

            template<typename TDataType>
            TDataType& GetValue(const Variable<TDataType>& rThisVariable){
                QUEST_DEBUG_ERROR_IF(!mpVariablesList) << "This container don't have a variables list assigned." << std::endl;
                QUEST_DEBUG_ERROR_IF_NOT(mpVariablesList->Has(rThisVariable)) << "This container don't have this variable: " << rThisVariable << std::endl;
                return *(reinterpret_cast<TDataType*>(Position(rThisVariable))+rThisVariable.GetComponentIndex());
            }

            template<typename TDataType>
            TDataType& GetValue(const Variable<TDataType>& rThisVariable, SizeType QueueIndex){
                QUEST_DEBUG_ERROR_IF(!mpVariablesList) << "This container don't have a variables list assigned." << std::endl;
                QUEST_DEBUG_ERROR_IF_NOT(mpVariablesList->Has(rThisVariable)) << "This container don't have this variable: " << rThisVariable << std::endl;
                return *(reinterpret_cast<TDataType*>(Position(rThisVariable, QueueIndex))+rThisVariable.GetComponentIndex());
            }

            template<typename TDataType>
            const TDataType& GetValue(const Variable<TDataType>& rThisVariable) const{
                QUEST_DEBUG_ERROR_IF(!mpVariablesList) << "This container don't have a variables list assigned." << std::endl;
                QUEST_DEBUG_ERROR_IF_NOT(mpVariablesList->Has(rThisVariable)) << "This container don't have this variable: " << rThisVariable << std::endl;
                return *(reinterpret_cast<const TDataType*>(Position(rThisVariable))+rThisVariable.GetComponentIndex());
            }

            template<typename TDataType>
            const TDataType& GetValue(const Variable<TDataType>& rThisVariable, SizeType QueueIndex) const{
                QUEST_DEBUG_ERROR_IF(!mpVariablesList) << "This container don't have a variables list assigned." << std::endl;
                QUEST_DEBUG_ERROR_IF_NOT(mpVariablesList->Has(rThisVariable)) << "This container don't have this variable: " << rThisVariable << std::endl;
                return *(reinterpret_cast<const TDataType*>(Position(rThisVariable, QueueIndex))+rThisVariable.GetComponentIndex());
            }

            template<typename TDataType>
            TDataType& FastGetValue(const Variable<TDataType>& rThisVariable){
                return *(reinterpret_cast<TDataType*>(Position(rThisVariable))+rThisVariable.GetComponentIndex());
            }

            template<typename TDataType>
            TDataType* pFastGetValue(const Variable<TDataType>& rThisVariable){
                return reinterpret_cast<TdataType*>(Position(rThisVariable))+rThisVariable.GetComponentIndex();
            }

            template<typename TDataType>
            TDataType& FastGetValue(const Variable<TDataType>& rThisVariable, SizeType QueueIndex){
                return *(reinterpret_cast<TDataType*>(Position(rThisVariable, QueueIndex))+rThisVariable.GetComponentIndex());
            }

            template<typename TDataType>
            TDataType& FastGetValue(const Variable<TDataType>& rThisVariable, SizeType QueueIndex, SizeType ThisPosition){
                QUEST_DEBUG_ERROR_IF(!mpVariablesList) << "This container don't have a variables list assigned." << std::endl;
                QUEST_DEBUG_ERROR_IF_NOT(mpVariablesList->Has(rThisVariable)) << "This container don't have this variable: " << rThisVariable << std::endl;
                QUEST_DEBUG_ERROR_IF((ThisPosition+1)>mQueueSize) << "Trying to access data from step " << ThisPosition << " but only " << mQueueSize << " steps are stored." << std::endl;
                return *(reinterpret_cast<TDataType*>(Position(rThisVariable, QueueIndex))+rThisVariable.GetComponentIndex());
            }

            template<typename TDataType>
            TDataType& FastGetCurrentValue(const Variable<TDataType>& rThisVariable, SizeType ThisPosition){
                QUEST_DEBUG_ERROR_IF(!mpVariablesList) << "This container don't have a variables list assigned." << std::endl;
                QUEST_DEBUG_ERROR_IF_NOT(mpVariablesList->Has(rThisVariable)) << "This container don't have this variable: " << rThisVariable << std::endl;
                return *(reinterpret_cast<TDataType*>(mpCurrentPosition+ThisPosition)+rThisVariable.GetComponentIndex());
            }

            template<typename TDataType>
            const TDataType& FastGetValue(const Variable<TDataType>& rThisVariable) const{
                return *(reinterpret_cast<const TDataType*>(Position(rThisVariable))+rThisVariable.GetComponentIndex());
            }

            template<typename TDataType>
            const TDataType* pFastGetValue(const Variable<TDataType>& rThisVariable) const{
                return reinterpret_cast<const TdataType*>(Position(rThisVariable))+rThisVariable.GetComponentIndex();
            }

            template<typename TDataType>
            const TDataType& FastGetValue(const Variable<TDataType>& rThisVariable, SizeType QueueIndex) const{
                return *(reinterpret_cast<const TDataType*>(Position(rThisVariable, QueueIndex))+rThisVariable.GetComponentIndex());
            }

            template<typename TDataType>
            const TDataType& FastGetValue(const Variable<TDataType>& rThisVariable, SizeType QueueIndex, SizeType ThisPosition) const{
                return *(reinterpret_cast<const TDataType*>(Position(QueueIndex)+ThisPosition)+rThisVariable.GetComponentIndex());
            }

            template<typename TDataType>
            const TDataType& FastGetCurrentValue(const Variable<TDataType>& rThisVariable, SizeType ThisPosition) const{
                return *(reinterpret_cast<const TDataType*>(mpCurrentPosition+ThisPosition)+rThisVariable.GetComponentIndex());
            }

            SizeType Size() const{
                if(!mpVariablesList){
                    return 0;
                }

                return mpVariablesList->DataSize();
            }

            SizeType QueueSize() const{
                return mQueueSize;
            }

            SizeType TotalSize() const{
                if(!mpVariablesList){
                    return 0;
                }

                return mpVariablesList->DataSize() * mQueueSize;
            }

            template<typename TDataType>
            void SetValue(const Variable<TDataType>& rThisVariable, const TDataType& rValue){
                GetValue(rThisVariable) = rValue;
            }

            template<typename TDataType>
            void SetValue(const Variable<TDataType>& rThisVariable, const TDataType& rValue, SizeType QueueIndex){
                GetValue(rThisVariable, QueueIndex) = rValue;
            }

            void Clear(){
                DestructAllElements();
                if(mpData){
                    free(mpData);
                }

                mpData = 0;
            }

            VariablesList::Pointer pGetVariablesList(){
                return mpVariablesList;
            }

            const VariablesList::Pointer pGetVariablesList() const{
                return mpVariablesList;
            }

            VariablesList& GetVariablesList(){
                return *mpVariablesList;
            }

            const VariablesList& GetVariablesList() const{
                return *mpVariablesList;
            }

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

            BlockType* Data(){
                return mpData;
            }

            const BlockType* Data() const{
                return mpData;
            }

            BlockType* Data(SizeType QueueIndex){
                return Position(QueueIndex);
            }

            BlockType* Data(const VariableData& rThisVariable){
                QUEST_DEBUG_ERROR_IF(!mpVariablesList->Has(rThisVariable)) << "Variable " << rThisVariable.Name() << " is not in the variables list." << std::endl;
                return Position(rThisVariable);
            }

            SizeType DataSize(){
                if(!mpVariablesList){
                    return 0;
                }

                return mpVariablesList->DataSize()*sizeof(BlockType);
            }

            SizeType TotalDataSize(){
                if(!mpVariablesList){
                    return 0;
                }

                return mpVariablesList->DataSize()*sizeof(BlockType)*mQueueSize;
            }

            void AssignData(BlockType* Source, SizeType QueueIndex){
                AssignData(Source, Position(QueueIndex));
            }

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

            void AssignZero(){
                QUEST_DEBUG_ERROR_IF(!mpVariablesList) << "This container don't have a variables list assigned." << std::endl;
                for(VariablesList::const_iterator iVariables = mpVariablesList->begin(); iVariables != mpVariablesList->end(); ++iVariables){
                    iVariables->AssignZero(mpCurrentPosition+LocalOffset(*iVariables));
                }
            }

            void AssignZero(const SizeType QueueIndex){
                QUEST_DEBUG_ERROR_IF(!mpVariablesList) << "This container don't have a variables list assigned." << std::endl;
                BlockType* position = Position(QueueIndex);
                for(VariablesList::const_iterator iVariables = mpVariablesList->begin(); iVariables != mpVariablesList->end(); ++iVariables){
                    iVariables->AssignZero(position+LocalOffset(*iVariables));
                }
            }

            bool Has(const VariableData& rThisVariable) const{
                if(!mpVariablesList){
                    return false;
                }

                return mpVariablesList->Has(rThisVariable);
            }

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
            inline void Allocate(){
                QUEST_DEBUG_ERROR_IF(!mpVariablesList) << "This container don't have a variables list assigned." << std::endl;
                mpData = (BlockType*) malloc(mpVariablesList->DataSize() * sizeof(BlockType) * mQueueSize);
            }

            inline void Reallocate(){
                QUEST_DEBUG_ERROR_IF(!mpVariablesList) << "This container don't have a variables list assigned." << std::endl;
                mpData = (BlockType*) realloc(mpData, mpVariablesList->DataSize() * sizeof(BlockType) * mQueueSize);
            }

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

            void AssignData(BlockType* Source, BlockType* Destination){
                QUEST_DEBUG_ERROR_IF(!mpVariablesList) << "This container don't have a variables list assigned." << std::endl;
                for(VariablesList::const_iterator iVariables = mpVariablesList->begin(); iVariables != mpVariablesList->end(); ++iVariables){
                    const SizeType offset = LocalOffset(*iVariables);
                    iVariables->Assign(Source+offset, Destination+offset);
                }
            }

            inline SizeType LocalOffset(const VariableData& rThisVariable) const{
                QUEST_DEBUG_ERROR_IF(!mpVariablesList) << "This container don't have a variables list assigned." << std::endl;
                return mpVariablesList->Index(rThisVariable.SourceKey());
            }

            inline BlockType* Position(const VariableData& rThisVariable) const{
                QUEST_DEBUG_ERROR_IF(!mpVariablesList) << "This container don't have a variables list assigned." << std::endl;
                QUEST_DEBUG_ERROR_IF_NOT(mpVariablesList->Has(rThisVariable)) << "This container don't have this variable: " << rThisVariable << std::endl;
                return mpCurrentPosition + mpVariablesList->Index(rThisVariable.SourceKey());
            }

            inline BlockType* Position(const VariableData& rThisVariable, SizeType ThisIndex) const{
                QUEST_DEBUG_ERROR_IF(!mpVariablesList) << "This container don't have a variables list assigned." << std::endl;
                QUEST_DEBUG_ERROR_IF_NOT(mpVariablesList->Has(rThisVariable)) << "This container don't have this variable: " << rThisVariable << std::endl;
                QUEST_DEBUG_ERROR_IF((ThisIndex+1)>mQueueSize) << "Trying to access data from step " << ThisIndex << " but only " << mQueueSize << " steps are stored." << std::endl;
                return Position(ThisIndex)+mpVariablesList->Index(rThisVariable.SourceKey());
            }

            inline BlockType* Position() const{
                return mpCurrentPosition;
            }

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
            SizeType mQueueSize;

            BlockType* mpCurrentPosition;

            ContainerType mpData;

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