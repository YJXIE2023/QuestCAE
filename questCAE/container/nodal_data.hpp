/*----------------------------------
管理与节点相关的各种信息
----------------------------------*/

#ifndef QUEST_NODAL_DATA_HPP
#define QUEST_NODAL_DATA_HPP

// 系统头文件
#include <iostream>

// 项目头文件
#include "container/variables_list_data_value_container.hpp"


namespace Quest{

    class QUEST_API(QUEST_CORE) NodalData{
        public:
            QUEST_CLASS_POINTER_DEFINITION(NodalData);

            using IndexType = std::size_t;
            using SizeType = std::size_t;
            using SolutionStepsNodalDataContainerType = VariablesListDataValueContainer;
            using BlockType = VariablesListDataValueContainer::BlockType;

            NodalData(IndexType TheId);

            NodalData(IndexType TheId, VariablesList::Pointer pVariablesList, SizeType NewQueueSize = 1);

            NodalData(IndexType TheId, VariablesList::Pointer pVariablesList, const BlockType* ThisData, SizeType NeQueueSize = 1);

            ~NodalData(){}

            NodalData& operator = (const NodalData& rOther);

            IndexType Id() const{
                return mId;
            }

            IndexType GetId() const{
                return mId;
            }

            void SetId(IndexType NewId){
                mId = NewId;
            }

            void SerSolutionStepData(const SolutionStepsNodalDataContainerType& TheData){
                mSolutionStepsNodalData = TheData;
            }

            VariablesListDataValueContainer& GetSolutionStepData(){
                return mSolutionStepsNodalData;
            }

            const VariablesListDataValueContainer& GetSolutionStepData() const{
                return mSolutionStepsNodalData;
            }

            std::string Info() const;

            void PrintInfo(std::ostream& rOstream) const;

            void PrintData(std::ostream& rOstream) const;

        protected:

        private:
            friend class Serializer;

            void save(Serializer& rSerializer) const;
            void load(Serializer& rSerializer);

            NodalData(): mId(0), mSolutionStepsNodalData(){}

            NodalData(const NodalData& rOther);

        private:
            IndexType mId;
            SolutionStepsNodalDataContainerType mSolutionStepsNodalData;

    };

    inline std::istream& operator >> (std::istream& rIstream, NodalData& rThis){}

    inline std::ostream& operator << (std::ostream& rOstream, const NodalData& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }

} // namespace Quest

#endif //QUEST_NODAL_DATA_HPP