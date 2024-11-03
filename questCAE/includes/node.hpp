/*----------------------------------------
节点类，将节点的位置和状态信息集成在一起
----------------------------------------*/

#ifndef QUEST_NODE_HPP
#define QUEST_NODE_HPP

// 系统头文件
#include <string>
#include <iostream>
#include <sstream>
#include <cstddef>
#include <atomic>

// 项目头文件
#include "includes/define.hpp"
#include "includes/lock_object.hpp"
#include "geometries/point.hpp"
#include "includes/dof.hpp"
#include "container/pointer_vector_set.hpp"
#include "container/variables_list_data_value_container.hpp"
#include "container/flags.hpp"
#include "intrusive_ptr/intrusive_ptr.hpp"
#include "container/global_pointers_vector.hpp"
#include "container/data_value_container.hpp"
#include "container/nodal_data.hpp"
#include "includes/quest_flags.hpp"

namespace Quest{

    class Node: public Point,public Flags{
        public:
            QUEST_CLASS_INTRUSIVE_POINTER_DEFINITION(Node);

            using NodeType = Node;
            using BaseType = Point;
            using PointType = Point;
            using DofType = Dof<double>;
            using IndexType = std::size_t;
            using SizeType = std::size_t;
            using DofsContainerType = std::vector<std::unique_ptr<Dof<double>>>;
            using SolutionStepsNodalDataContainerType = VariablesListDataValueContainer;
            using BlockType = VariablesListDataValueContainer::BlockType;

        public:
            Node():
                BaseType(),
                Flags(),
                mNodalData(0),
                mDofs(),
                mData(),
                mInitialPosition(),
                mNodeLock()
            {
                CreateSolutionStepData();
            }


            explicit Node(IndexType NewId):
                BaseType(),
                Flags(),
                mNodalData(NewId),
                mDofs(),
                mData(),
                mInitialPosition(),
                mNodeLock()
            {
                QUEST_ERROR << "Calling the default constructor for the node ... illegal operation!" << std::endl;
                CreateSolutionStepData();
            }


            Node(IndexType NewId, const double& NewX):
                BaseType(NewX),
                Flags(),
                mNodalData(NewId),
                mDofs(),
                mData(),
                mInitialPosition(NewX),
                mNodeLock()
            {
                CreateSolutionStepData();
            }


            Node(IndexType NewId, const double& NewX, const double& NewY):
                BaseType(NewX, NewY),
                Flags(),
                mNodalData(NewId),
                mDofs(),
                mData(),
                mInitialPosition(NewX, NewY),
                mNodeLock()
            {
                CreateSolutionStepData();
            }


            Node(IndexType NewId, const double& NewX, const double& NewY, const double& NewZ):
                BaseType(NewX, NewY, NewZ),
                Flags(),
                mNodalData(NewId),
                mDofs(),
                mData(),
                mInitialPosition(NewX, NewY, NewZ),
                mNodeLock()
            {
                CreateSolutionStepData();
            }


            Node(IndexType NewId, const PointType& rThisPoint):
                BaseType(rThisPoint),
                Flags(),
                mNodalData(NewId),
                mDofs(),
                mData(),
                mInitialPosition(rThisPoint),
                mNodeLock()
            {
                CreateSolutionStepData();
            }


            Node(const Node& rOtherNode) = delete;


            template<typename TVectorType>
            Node(IndexType NewId, const vector_expression<TVectorType>& rOtherCoordinates):
                BaseType(rOtherCoordinates),
                Flags(),
                mNodalData(NewId),
                mDofs(),
                mData(),
                mInitialPosition(rOtherCoordinates),
                mNodeLock()
            {
                CreateSolutionStepData();
            }


            Node(IndexType NewId, const std::vector<double>& rOtherCoordinates):
                BaseType(rOtherCoordinates),
                Flags(),
                mNodalData(NewId),
                mDofs(),
                mData(),
                mInitialPosition(),
                mNodeLock()
            {
                CreateSolutionStepData();
            }


            Node(IndexType NewId, const double& NewX, const double& NewY, const double& NewZ, VariablesList::Pointer pVariablesList, const BlockType* ThisData, SizeType NewQueueSize = 1):
                BaseType(NewX, NewY, NewZ),
                Flags(),
                mNodalData(NewId, pVariablesList, ThisData, NewQueueSize),  
                mDofs(),
                mData(),
                mInitialPosition(NewX, NewY, NewZ),
                mNodeLock()
            {}


            typename Node::Pointer Clone(){
                Node::Pointer p_new_node = Quest::make_intrusive<Node>(this->Id(),(*this)[0],(*this)[1],(*this)[2]);
                p_new_node->mNodalData = this->mNodalData;

                Node::DofsContainerType& my_dofs = (this)->GetDofs();
                for(typename DofsContainerType::const_iterator it_dof = my_dofs.begin(); it_dof!= my_dofs.end(); ++it_dof){
                    p_new_node->pAddDof(**it_dof);
                }

                p_new_node->mData = this->mData;
                p_new_node->mInitialPosition = this->mInitialPosition;

                p_new_node->Set(Flags(*this));

                return p_new_node;
            }


            ~Node() override{
                ClearSolutionStepData();
            }


            unsigned int use_count() const noexcept{
                return mReferenceCounter;
            }


            IndexType Id() const{
                return mNodalData.Id();
            }


            IndexType GetId() const{
                return mNodalData.Id();
            }


            void SetId(IndexType NewId){
                mNodalData.SetId(NewId);
            }


            LockObject& GetLock(){
                return mNodeLock;
            }


            inline void SetLock(){
                mNodeLock.lock();
            }


            inline void UnSetLock(){
                mNodeLock.unlock();
            }


            Node& operator = (const Node& rOther){
                BaseType::operator=(rOther);
                Flags::operator=(rOther);

                mNodalData = rOther.mNodalData;

                for(typename DofsContainerType::const_iterator it_dof = rOther.mDofs.begin(); it_dof!= rOther.mDofs.end(); ++it_dof){
                    pAddDof(**it_dof);
                }

                mData = rOther.mData;
                mInitialPosition = rOther.mInitialPosition;

                return *this;
            }


            bool operator == (const Node& rOther){
                return PointType::operator == (rOther);
            }


            template<typename TVariableType>
            typename TVariableType::Type& operator()(const TVariableType& rThisVariable, IndexType SolutionStepIndex){
                return GetSolutionStepValue(rThisVariable, SolutionStepIndex);
            }


            template<typename TVariableType>
            typename TVariableType::Type& operator()(const TVariableType& rThisVariable){
                return GetSolutionStepValue(rThisVariable);
            }


            template<typename TDataType>
            TDataType& operator[](const Variable<TDataType>& rThisVariable){
                return GetValues(rThisVariable);
            }


            template<typename TDataType>
            const TDataType& operator[](const Variable<TDataType>& rThisVariable) const{
                return GetValues(rThisVariable);
            }


            double& operator[](IndexType ThisIndex){
                return BaseType::operator[](ThisIndex);
            }


            double operator[](IndexType ThisIndex) const{
                return BaseType::operator[](ThisIndex);
            }


            void CreateSolutionStepData(){
                SolutionStepData().PushFront();
            }


            void CloneSolutionStepData(){
                SolutionStepData().CloneFront();
            }


            void OverwriteSolutionStepData(IndexType SourceSolutionStepIndex, IndexType DestinationSourceSolutionStepIndex){
                SolutionStepData().AssignData(SolutionStepData().Data(SourceSolutionStepIndex), DestinationSourceSolutionStepIndex);
            }


            void ClearSolutionStepData(){
                SolutionStepData().Clear();
            }


            void SetSolutionStepVariablesList(VariablesList::Pointer pVariablesList){
                SolutionStepData().SetVariablesList(pVariablesList);
            }


            VariablesListDataValueContainer& SolutionStepData(){
                return mNodalData.GetSolutionStepData();
            }


            const VariablesListDataValueContainer& SolutionStepData() const{
                return mNodalData.GetSolutionStepData();
            }


            DataValueContainer& GetData(){
                return mData;
            }


            const DataValueContainer& GetData() const{
                return mData;
            }


            template<typename TVariableType>
            typename TVariableType::Type& GetSolutionStepValue(const TVariableType& rThisVariable){
                return SolutionStepData().GetValue(rThisVariable);
            }


            template<typename TVariableType>
            const typename TVariableType::Type& GetSolutionStepValue(const TVariableType& rThisVariable) const{
                return SolutionStepData().GetValue(rThisVariable);
            }


            template<typename TVariableType>
            typename TVariableType::Type& GetSolutionStepValue(const TVariableType& rThisVariable, IndexType SolutionStepIndex){
                return SolutionStepData().GetValue(rThisVariable, SolutionStepIndex);
            }


            template<typename TVariableType>
            const typename TVariableType::Type& GetSolutionStepValue(const TVariableType& rThisVariable, IndexType SolutionStepIndex) const{
                return SolutionStepData().GetValue(rThisVariable, SolutionStepIndex);
            }


            bool SolutionStepsDataHas(const VariableData& rThisVariable) const{
                return SolutionStepData().Has(rThisVariable);
            }


            template<typename TVariableType>
            typename TVariableType::Type& FastGetSolutionStepValue(const TVariableType& rThisVariable){
                return SolutionStepData().FastGetValue(rThisVariable);
            }


            template<typename TVariableType>
            const typename TVariableType::Type& FastGetSolutionStepValue(const TVariableType& rThisVariable) const{
                return SolutionStepData().FastGetValue(rThisVariable);
            }   


            template<typename TVariableType>
            typename TVariableType::Type& FastGetSolutionStepValue(const TVariableType& rThisVariable, IndexType SolutionStepIndex){
                return SolutionStepData().FastGetValue(rThisVariable, SolutionStepIndex);
            }


            template<typename TVariableType>
            const typename TVariableType::Type& FastGetSolutionStepValue(const TVariableType& rThisVariable, IndexType SolutionStepIndex) const{
                return SolutionStepData().FastGetValue(rThisVariable, SolutionStepIndex);
            }


            template<typename TVariableType>
            typename TVariableType::Type& FastGetSolutionStepValue(const TVariableType& rThisVariable, IndexType SolutionStepIndex, IndexType ThisPosition){
                return SolutionStepData().FastGetValue(rThisVariable, SolutionStepIndex, ThisPosition);
            } 


            template<typename TVariableType>
            typename TVariableType::Type& FastGetSolutionStepValue(const TVariableType& rThisVariable, IndexType ThisPosition){
                return SolutionStepData().FastGetCurrentValue(rThisVariable, ThisPosition);
            }


            template<typename TVariableType>
            typename TVariableType::Type& GetValue(const TVariableType& rThisVariable){
                return GetValue(rThisVariable);
            }


            template<typename TVariableType>    
            const typename TVariableType::Type& GetValue(const TVariableType& rThisVariable) const{
                return GetValues(rThisVariable);
            }


            template<typename TVariableType>
            typename TVariableType::Type& GetValues(const TVariableType& rThisVariable, IndexType SolutionStepIndex){
                if(!mData.Has(rThisVariable)){
                    return SolutionStepData().GetValue(rThisVariable, SolutionStepIndex);
                }

                return mData.GetValue(rThisVariable);
            }


            template<typename TVariableType>
            const typename TVariableType::Type& GetValues(const TVariableType& rThisVariable, IndexType SolutionStepIndex) const{
                if(!mData.Has(rThisVariable)){
                    return SolutionStepData().GetValue(rThisVariable, SolutionStepIndex);
                }

                return mData.GetValue(rThisVariable);
            }


            template<typename TVariableType>
            void SetValue(const TVariableType& rThisVariable, const typename TVariableType::Type& rValue){
                mData.SetValue(rThisVariable, rValue);
            }


            template<typename TDataType>
            bool Has(const Variable<TDataType>& rThisVariable) const{
                return mData.Has(rThisVariable);
            }


            template<typename TVariableType>
            inline void Fix(const TVariableType& rDofVariable){
                for(auto it_dof = mDofs.begin(); it_dof != mDofs.end(); ++it_dof){
                    if((*it_dof)->GetVariable() == rDofVariable){
                        (*it_dof)->FixDof();
                        return;
                    }
                }

                #ifdef QUEST_DEBUG
                    if(OpenMPUtils::IsInParallel() != 0){
                        QUEST_ERROR << "Attempting to Fix the variable: " << rDofVariable << " within a parallel region. This is not permitted. Create the Dof first by pAddDof" << std::endl;
                    }
                #endif

                pAddDof(rDofVariable)->FixDof();
            }


            template<typename TVariableType>
            inline void Free(const TVariableType& rDofVariable){
                for(auto it_dof = mDofs.begin(); it_dof != mDofs.end(); ++it_dof){
                    if((*it_dof)->GetVariable() == rDofVariable){
                        (*it_dof)->FreeDof();
                        return;
                    }
                }

                #ifdef QUEST_DEBUG
                    if(OpenMPUtils::IsInParallel() != 0){
                        QUEST_ERROR << "Attempting to Free the variable: " << rDofVariable << " within a parallel region. This is not permitted. Create the Dof first by pAddDof" << std::endl;
                    }
                #endif

                pAddDof(rDofVariable)->FreeDof();
            }


            IndexType GetBufferSize() const{
                return SolutionStepData().QueueSize();
            }


            void SetBufferSize(IndexType NeBufferSize){
                SolutionStepData().Resize(NeBufferSize);
            }


            const PointType& GetInitialPosition() const{
                return mInitialPosition;
            }


            PointType& GetInitialPosition(){
                return mInitialPosition;
            }


            double& X0(){
                return mInitialPosition.X();
            }


            double& Y0(){
                return mInitialPosition.Y();
            }


            double& Z0(){
                return mInitialPosition.Z();
            }


            double X0() const{
                return mInitialPosition.X();
            }


            double Y0() const{
                return mInitialPosition.Y();
            }


            double Z0() const{
                return mInitialPosition.Z();
            }


            void SetInitialPosition(const PointType& NewwInitialPosition){
                mInitialPosition.X() = NewwInitialPosition.X();
                mInitialPosition.Y() = NewwInitialPosition.Y();
                mInitialPosition.Z() = NewwInitialPosition.Z();
            }


            void SetInitialPosition(double NewX, double NewY, double NewZ){
                mInitialPosition.X() = NewX;
                mInitialPosition.Y() = NewY;
                mInitialPosition.Z() = NewZ;
            }


            VariablesList::Pointer pGetVariablesList(){
                return SolutionStepData().pGetVariablesList();
            }


            const VariablesList::Pointer pGetVariablesList() const{
                return SolutionStepData().pGetVariablesList();
            }


            template<typename TVariableType>
            inline unsigned int GetDofPosition(const TVariableType& rDofVariable) const{
                typename DofsContainerType::const_iterator it_dof = mDofs.end();
                for(it_dof = mDofs.begin(); it_dof != mDofs.end(); ++it_dof){
                    if((*it_dof)->GetVariable() == rDofVariable){
                        break;
                    }
                }

                return it_dof - mDofs.begin();
            }


            template<typename TVariableType>
            inline const DofType& GetDof(const TVariableType& rDofVariable, int pos) const{
                typename DofsContainerType::const_iterator it_begin = mDofs.begin();
                typename DofsContainerType::const_iterator it_end = mDofs.end();
                typename DofsContainerType::const_iterator it;
                if(pos < it_end - it_begin){
                    it = it_begin + pos;
                    if((*it)->GetVariable() == rDofVariable){
                        return **it;
                    }
                }

                for(auto it_dof = mDofs.begin(); it_dof != mDofs.end(); ++it_dof){
                    if((*it_dof)->GetVariable() == rDofVariable){
                        return **it_dof;
                    }
                }

                QUEST_ERROR << "Non-existent DOF in node #" << Id() << " for variable: " << rDofVariable.Name() << std::endl;
            }


            template<typename TVariableType>
            inline const DofType& GetDof(const TVariableType& rDofVariable) const{
                for(auto it_dof = mDofs.begin(); it_dof != mDofs.end(); ++it_dof){
                    if((*it_dof)->GetVariable() == rDofVariable){
                        return **it_dof;
                    }
                }

                QUEST_ERROR << "Non-existent DOF in node #" << Id() << " for variable: " << rDofVariable.Name() << std::endl;
            }


            DofsContainerType& GetDofs(){
                return mDofs;
            }


            const DofsContainerType& GetDofs() const{
                return mDofs;
            }


            template<typename TVariableType>
            inline typename DofType::Pointer pGetDof(const TVariableType& rDofVariable) const{
                for(auto it_dof = mDofs.begin(); it_dof != mDofs.end(); ++it_dof){
                    if((*it_dof)->GetVariable() == rDofVariable){
                        return (*it_dof).get();
                    }
                }

                QUEST_ERROR << "Non-existent DOF in node #" << Id() << " for variable: " << rDofVariable.Name() << std::endl;
            }


            template<typename TVariableType>
            inline typename DofType::Pointer pGetDof(const TVariableType& rDofVariable, int pos) const{
                const auto it_begin = mDofs.begin();
                const auto it_end = mDofs.end();

                if(pos < it_end - it_begin){
                    auto it = it_begin + pos;
                    if((*it)->GetVariable() == rDofVariable){
                        return (*it).get();
                    }
                }

                for(auto it_dof = it_begin; it_dof != it_end; ++it_dof){
                    if((*it_dof)->GetVariable() == rDofVariable){
                        return (*it_dof).get();
                    }
                }

                QUEST_ERROR << "Non-existent DOF in node #" << Id() << " for variable: " << rDofVariable.Name() << std::endl;
            }


            template<typename TVariableType>
            inline typename DofType::Pointer pAddDof(const TVariableType& rDofVariable){
                QUEST_TRY
                
                for(auto it_dof = mDofs.begin(); it_dof != mDofs.end(); ++it_dof){
                    if((*it_dof)->GetVariable() == rDofVariable){
                        return (*it_dof).get();
                    }
                }

                mDofs.push_back(Quest::make_unique<DofType>(&mNodalData, rDofVariable));

                DofType* p_new_dof = mDofs.back().get();

                SortDofs();

                return p_new_dof;

                QUEST_CATCH(*this);
            }


            inline typename DofType::Pointer pAddDof(const DofType& SourceDof){
                QUEST_TRY

                for(auto it_dof = mDofs.begin(); it_dof != mDofs.end(); ++it_dof){
                    if((*it_dof)->GetVariable() == SourceDof.GetVariable()){
                        if((*it_dof)->GetReaction() != SourceDof.GetReaction()){
                            **it_dof = SourceDof;
                            (*it_dof)->SetNodalData(&mNodalData);
                        }
                        return (*it_dof).get();
                    }
                }

                mDofs.push_back(Quest::make_unique<DofType>(SourceDof));
                mDofs.back()->SetNodalData(&mNodalData);

                DofType* p_new_dof = mDofs.back().get();
                SortDofs();

                return p_new_dof;

                QUEST_CATCH(*this);
            }


            template<typename TVariableType, typename TReactionType>
            inline typename DofType::Pointer pAddDof(const TVariableType& rDofVariable, const TReactionType& rDofReaction){
                QUEST_TRY

                for(auto it_dof = mDofs.begin(); it_dof != mDofs.end(); ++it_dof){
                    if((*it_dof)->GetVariable() == rDofVariable){
                        (*it_dof)->SetReaction(rDofReaction);
                        return (*it_dof).get();
                    }
                }

                mDofs.push_back(Quest::make_unique<DofType>(&mNodalData, rDofVariable, rDofReaction));

                DofType* p_new_dof = mDofs.back().get();

                SortDofs();

                return p_new_dof;

                QUEST_CATCH(*this);
            }


            template<typename TVariableType>
            inline DofType& AddDof(const TVariableType& rDofVariable){
                QUEST_TRY

                for(auto it_dof = mDofs.begin(); it_dof != mDofs.end(); ++it_dof){
                    if((*it_dof)->GetVariable() == rDofVariable){
                        return **it_dof;
                    }
                }

                mDofs.push_back(Quest::make_unique<DofType>(&mNodalData, rDofVariable));

                DofType* p_new_dof = mDofs.back().get();

                SortDofs();

                return *p_new_dof;

                QUEST_CATCH(*this);
            }


            template<typename TVariableType, typename TReactionType>
            inline DofType& AddDof(const TVariableType& rDofVariable, const TReactionType& rDofReaction){
                QUEST_TRY

                for(auto it_dof = mDofs.begin(); it_dof != mDofs.end(); ++it_dof){
                    if((*it_dof)->GetVariable() == rDofVariable){
                        (*it_dof)->SetReaction(rDofReaction);
                        return **it_dof;
                    }
                }

                mDofs.push_back(Quest::make_unique<DofType>(&mNodalData, rDofVariable, rDofReaction));

                DofType* p_new_dof = mDofs.back().get();

                SortDofs(); 

                return *p_new_dof;  

                QUEST_CATCH(*this);
            }


            inline bool HasDofFor(const VariableData& rDofVariable) const{
                for(auto it_dof = mDofs.begin(); it_dof != mDofs.end(); ++it_dof){
                    if((*it_dof)->GetVariable() == rDofVariable){
                        return true;
                    }
                }

                return false;
            }


            inline bool IsFixed(const VariableData& rDofVariable) const{
                for(auto it_dof = mDofs.begin(); it_dof != mDofs.end(); ++it_dof){
                    if((*it_dof)->GetVariable() == rDofVariable){
                        return (*it_dof)->IsFixed();
                    }
                }

                return false;
            }


            inline bool IsActive() const{
                return IsDefined(ACTIVE) ? Is(ACTIVE) : true;
            }


            std::string Info() const override{
                std::stringstream buffer;
                buffer << "Node #" << Id();
                return buffer.str();
            }


            void PrintInfo(std::ostream& rOstream) const override{
                rOstream << Info();
            }


            void PrintData(std::ostream& rOstream) const override{
                BaseType::PrintData(rOstream);
                if(!mDofs.empty()){
                    rOstream << std::endl << "    Dofs :" << std::endl;
                }
                for(typename DofsContainerType::const_iterator i = mDofs.begin(); i != mDofs.end(); ++i){
                    rOstream << "        " << (*i)->Info() << std::endl;
                }
            }


        protected:

        private:
            friend void intrusive_ptr_add_ref(const NodeType* x){
                x->mReferenceCounter.fetch_add(1, std::memory_order_relaxed);
            }

            friend void intrusive_ptr_release(const NodeType* x){
                if(x->mReferenceCounter.fetch_sub(1, std::memory_order_release) == 1){
                    std::atomic_thread_fence(std::memory_order_acquire);
                    delete x;
                }
            }

            void SortDofs(){
                std::sort(mDofs.begin(), mDofs.end(), [](const Quest::unique_ptr<DofType>& First, const Quest::unique_ptr<DofType>& Second)->bool{
                    return First->GetVariable().Key() < Second->GetVariable().Key();
                });
            }


            friend class Serializer;

            void save(Serializer& rSerializer) const override{
                QUEST_SERIALIZE_SAVE_BASE_CLASS(rSerializer, Point);
                QUEST_SERIALIZE_SAVE_BASE_CLASS(rSerializer, Flags);
                rSerializer.save("NodelData", &mNodalData);
                rSerializer.save("Data", mData);
                rSerializer.save("Initial Position", mInitialPosition);
                rSerializer.save("Data", mDofs);
            }


            void load(Serializer& rSerializer) override{
                QUEST_SERIALIZE_LOAD_BASE_CLASS(rSerializer, Point);
                QUEST_SERIALIZE_LOAD_BASE_CLASS(rSerializer, Flags);
                NodalData* p_nodal_data = &mNodalData;
                rSerializer.load("NodelData", p_nodal_data);
                rSerializer.load("Data", mData);
                rSerializer.load("Initial Position", mInitialPosition);
                rSerializer.load("Data", mDofs);
            }

        private:
            NodalData mNodalData;
            DofsContainerType mDofs;
            DataValueContainer mData;
            PointType mInitialPosition;
            LockObject mNodeLock;
            mutable std::atomic<int> mReferenceCounter{0};

    };

    inline std::istream& operator >> (std::istream& rIstream, Node& rThis);

    inline std::ostream& operator << (std::ostream& rOstream, const Node& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }

} // namespace Quest

#endif //QUEST_NODE_HPP