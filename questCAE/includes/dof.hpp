/*-------------------------------------------------------------
节点自由度的轻量级的表示
存储自由度的物理量、状态（自由或受约束）以及该自由度在数据结构中的实际值
-------------------------------------------------------------*/

#ifndef QUEST_DOF_HPP
#define QUEST_DOF_HPP

// 系统头文件
#include <string>
#include <iostream>
#include <sstream>
#include <cstddef>

// 项目头文件
#include "includes/define.hpp"
#include "container/data_value_container.hpp"
#include "container/nodal_data.hpp"

namespace Quest{
    
    template<typename TDataType,typename TVariableType = Variable<TDataType>>
    struct DofTrait{
        static const int Id;
    };

    template<typename TDataType> 
    struct DofTrait<TDataType>{  
        static const int Id = 0;  
    };

    template<typename TDataType>
    class Dof{
        public:
            using Pointer = Dof*;
            using IndexType = std::size_t;
            using EquationIdType = std::size_t;
            using SolutionStepsDataContainerType = VariablesListDataValueContainer;

            template<typename TVariableType>
            Dof(NodalData* pThisNodalData, const TVariableType& rThisVariable):
                mIsFixed(false),
                mVariableType(DofTrait<TDataType, TVariableType>::Id),
                mReactionType(DofTrait<TDataType, Variable<TDataType>>::Id),
                mEquationId(IndexType()),
                mpNodalData(pThisNodalData)
            {
                QUEST_DEBUG_ERROR_IF_NOT(pThisNodalData->GetSolutionStepData().Has(rThisVariable))
                    << "The Dof-Variable " << rThisVariable.Name() << " is not "
                    << "in the list of variables" << std::endl;

                mIndex = mpNodalData->GetSolutionStepData().pGetVariablesList()->AddDof(&rThisVariable);
            }

            template<typename TVariableType, typename TReactionType>
            Dof(NodalData* pThisNodalData, const TVariableType& rThisVariable, const TReactionType& rThisReaction):
                mIsFixed(false),
                mVariableType(DofTrait<TDataType, TVariableType>::Id),
                mReactionType(DofTrait<TDataType, TReactionType>::Id),
                mEquationId(IndexType()),
                mpNodalData(pThisNodalData)
            {
                QUEST_DEBUG_ERROR_IF_NOT(pThisNodalData->GetSolutionStepData().Has(rThisVariable))
                    << "The Dof-Variable " << rThisVariable.Name() << " is not "
                    << "in the list of variables" << std::endl;

                QUEST_DEBUG_ERROR_IF_NOT(pThisNodalData->GetSolutionStepData().Has(rThisReaction))
                    << "The Dof-Reaction " << rThisReaction.Name() << " is not "
                    << "in the list of variables" << std::endl;
                
                return mpNodalData->GetSolutionStepData().pGetVariablesList()->AddDof(&rThisVariable, &rThisReaction);
            }

            Dof():
                mIsFixed(false),
                mVariableType(DofTrait<TDataType, Variable<TDataType>>::Id),
                mReactionType(DofTrait<TDataType, Variable<TDataType>>::Id),
                mEquationId(IndexType()),
                mIndex(),
                mpNodalData(){}

            Dof(const Dof& rOther):
                mIsFixed(rOther.mIsFixed),
                mVariableType(rOther.mVariableType),
                mReactionType(rOther.mReactionType),
                mEquationId(rOther.mEquationId),
                mIndex(rOther.mIndex),
                mpNodalData(rOther.mpNodalData){}

            ~Dof(){}

            Dof& operator=(const Dof& rOther){
                mIsFixed = rOther.mIsFixed;
                mVariableType = rOther.mVariableType;
                mReactionType = rOther.mReactionType;
                mEquationId = rOther.mEquationId;
                mIndex = rOther.mIndex;
                mpNodalData = rOther.mpNodalData;

                return *this;
            }

            template<typename TVariableType>
            typename TVariableType::Type& operator()(const TVariableType& rThisVariable,IndexType SolutionStepIndex = 0){
                return GetSolutionStepValue(rThisVariable,SolutionStepIndex);
            }

            template<typename TVariableType>
            const typename TVariableType::Type& operator()(const TVariableType& rThisVariable,IndexType SolutionStepIndex = 0) const{
                return GetSolutionStepValue(rThisVariable,SolutionStepIndex);
            }

            TDataType& operator()(IndexType SolutionStepIndex = 0){
                return GetSolutionStepValue(SolutionStepIndex);
            }

            const TDataType& operator()(IndexType SolutionStepIndex = 0) const{
                return GetSolutionStepValue(SolutionStepIndex);
            }

            TDataType& operator[](IndexType SolutionStepIndex){
                return GetSolutionStepValue(SolutionStepIndex);
            }

            const TDataType& operator[](IndexType SolutionStepIndex) const{
                return GetSolutionStepValue(SolutionStepIndex);
            }

            TDataType& GetSolutionStepValue(IndexType SolutionStepIndex = 0){
                return GetReference(GetVariable(),mpNodalData->GetSolutionStepData(),SolutionStepIndex,mVariableType);
            }

            const TDataType& GetSolutionStepValue(IndexType SolutionStepIndex = 0) const{
                return GetReference(GetVariable(),mpNodalData->GetSolutionStepData(),SolutionStepIndex,mVariableType);
            }

            template<typename TVariableType>
            typename TVariableType::Type& GetSolutionStepValue(const TVariableType& rThisVariable,IndexType SolutionStepIndex = 0){
                return mpNodalData->GetSolutionStepData().GetValue(rThisVariable,SolutionStepIndex);
            }

            template<typename TVariableType>
            const typename TVariableType::Type& GetSolutionStepValue(const TVariableType& rThisVariable,IndexType SolutionStepIndex = 0) const{
                return mpNodalData->GetSolutionStepData().GetValue(rThisVariable,SolutionStepIndex);
            }

            TDataType& GetSolutionStepReactionValue(IndexType SolutionStepIndex = 0){
                return GetReference(GetReaction(),mpNodalData->GetSolutionStepData(),SolutionStepIndex,mReactionType);
            }

            const TDataType& GetSolutionStepReactionValue(IndexType SolutionStepIndex = 0) const{
                return GetReference(GetReaction(),mpNodalData->GetSolutionStepData(),SolutionStepIndex,mReactionType);
            }

            IndexType Id() const{
                return mpNodalData->GetId();
            }

            IndexType GetId() const{
                return mpNodalData->GetId();
            }

            const VariableData& GetVariable() const{
                return mpNodalData->GetSolutionStepData().GetVariablesList().GetDofVariable(mIndex);
            }

            const VariableData& GetReaction() const{
                auto p_reaction = mpNodalData->GetSolutionStepData().GetVariablesList().pGetDofReaction(mIndex);
                return (p_reaction == nullptr)? msNone : *p_reaction;
            }

            template<typename TReactionType>
            void SetReaction(const TReactionType& rReaction){
                mReactionType = DofTrait<TDataType, TReactionType>::Id;
                mpNodalData->GetSolutionStepData().pGetVariablesList()->SetDofReaction(&rReaction, mIndex);
            }

            EquationIdType GetEquationId() const{
                return mEquationId;
            }

            void SetEquationId(EquationIdType EquationId){
                mEquationId = EquationId;
            }

            void FixDof(){
                mIsFixed = false;
            }

            SolutionStepsDataContainerType* GetSolutionStepsData(){
                return &(mpNodalData->GetSolutionStepData());
            }

            void SetNodalData(NodalData* pNewNodalData){
                auto p_variable = &GetVariable();
                auto p_reaction = mpNodalData->GetSolutionStepData().pGetVariablesList()->pGetDofReaction(mIndex);
                mpNodalData = pNewNodalData;
                if(p_reaction != nullptr){
                    mIndex = mpNodalData->GetSolutionStepData().pGetVariablesList()->AddDof(p_variable,p_reaction);
                } else {
                    mIndex = mpNodalData->GetSolutionStepData().pGetVariablesList()->AddDof(p_variable);
                }
            }

            bool HasReaction() const{
                return (mpNodalData->GetSolutionStepData().pGetVariablesList()->pGetDofReaction(mIndex) != nullptr);
            }

            bool IsFixed() const{
                return mIsFixed;
            }

            bool IsFree() const{
                return !IsFixed();
            }

            std::string Info() const{
                std::stringstream buffer;

                if(IsFixed()){
                    buffer << "Fix " << GetVariable().Name() << " degree of freedom";
                } else {
                    buffer << "Free " << GetVariable().Name() << " degree of freedom";
                }

                return buffer.str();
            }

            void PrintInfo(std::ostream& rOstream) const{
                rOstream << Info();
            }

            void PrintData(std::ostream& rOstream) const{
                rOstream << "    Variables                 : " << GetVariable().Name() << std::endl;
                rOstream << "    Reaction                  : " << GetReaction().Name() << std::endl;
                if(IsFixed()){
                    rOstream << "    IsFixed                     : True" << std::endl;
                } else {
                    rOstream << "    IsFixed                     : False" << std::endl;
                }
                rOstream << "    Equation Id                 : " << mEquationId << std::endl;
            }

        protected:

        private:
            TDataType& GetReference(const VariableData& ThisVariable,VariablesListDataValueContainer& rData,IndexType SolutionStepIndex,int ThisId){
                switch(ThisId){
                    case 0:
                        return rData.GetValue(static_cast<const Variable<TDataType>& >(ThisVariable),SolutionStepIndex);
                }
                QUEST_ERROR << "Not supported type for Dof" << std::endl;
            }


            const TDataType& GetReference(const VariableData& ThisVariable,const VariablesListDataValueContainer& rData,IndexType SolutionStepIndex,int ThisId) const{
                switch(ThisId){
                    case 0:
                        return rData.GetValue(static_cast<const Variable<TDataType>& >(ThisVariable),SolutionStepIndex);
                }
                QUEST_ERROR << "Not supported type for Dof" << std::endl;
            }

            
            friend class Serializer;

            void save(Serializer& rSerializer) const{
                rSerializer.save("IsFixed",static_cast<bool>(mIsFixed));
                rSerializer.save("EquationId",static_cast<EquationIdType>(mEquationId));
                rSerializer.save("NodalData",mpNodalData);
                rSerializer.save("VariableType",static_cast<int>(mVariableType));
                rSerializer.save("ReactionType",static_cast<int>(mReactionType));
                rSerializer.save("Index",static_cast<int>(mIndex));
            }

            void load(Serializer& rSerializer){
                std::string name;
                bool is_fixed;
                rSerializer.load("IsFixed",is_fixed);
                mIsFixed = is_fixed;
                EquationIdType equation_id; 
                rSerializer.load("EquationId",equation_id);
                mEquationId = equation_id;
                rSerializer.load("NodalData",mpNodalData);

                int variable_type;
                int reaction_type;
                rSerializer.load("VariableType",variable_type);
                rSerializer.load("ReactionType",reaction_type);
                mVariableType = variable_type;
                mReactionType = reaction_type;

                int index;
                rSerializer.load("Index",index);
                mIndex = index;
            }

        private:
            static const Variable<TDataType> msNone;
            static constexpr int msIsFixedPosition = 63;

            int mIsFixed : 1;
            int mVariableType : 4;
            int mReactionType : 4;
            int mIndex : 6;
            EquationIdType mEquationId : 48;

            NodalData* mpNodalData;

    };

    template<typename TDataType>
    inline std::istream& operator>>(std::istream& rIStream,Dof<TDataType>& rThis){}

    template<typename TDataType>
    inline std::ostream& operator<<(std::ostream& rOStream,const Dof<TDataType>& rThis){
        rThis.PrintInfo(rOStream);
        rOStream << std::endl;
        rThis.PrintData(rOStream);

        return rOStream;
    }

    template<typename TDataType>

    inline bool operator > (const Dof<TDataType>& First,const Dof<TDataType>& Second){
        if(First.Id() == Second.Id()){
            return (First.GetVariable().Key() > Second.GetVariable().Key());
        }

        return (First.Id() > Second.Id());
    }

    template<typename TDataType>
    inline bool operator < (const Dof<TDataType>& First,const Dof<TDataType>& Second){
        if(First.Id() == Second.Id()){
            return (First.GetVariable().Key() < Second.GetVariable().Key());
        }

        return (First.Id() < Second.Id());
    }

    template<typename TDataType>
    inline bool operator >= (const Dof<TDataType>& First,const Dof<TDataType>& Second){ 
        if(First.Id() == Second.Id()){  
            return (First.GetVariable().Key() >= Second.GetVariable().Key());  
        }  

        return (First.Id() > Second.Id());  
    }

    template<typename TDataType>
    inline bool operator <= (const Dof<TDataType>& First,const Dof<TDataType>& Second){  
        if(First.Id() == Second.Id()){  
            return (First.GetVariable().Key() <= Second.GetVariable().Key());  
        }  

        return (First.Id() < Second.Id());  
    }

    template<typename TDataType>
    inline bool operator == (const Dof<TDataType>& First,const Dof<TDataType>& Second){
        return (First.Id() == Second.Id() && First.GetVariable().Key() == Second.GetVariable().Key());
    }

} // namespace Quest

#endif //QUEST_DOF_HPP