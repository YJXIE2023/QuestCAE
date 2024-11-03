/*---------------------------------
实现系统中不同主从约束的接口
控制从属自由度与主自由度之间的关系
---------------------------------*/

#ifndef QUEST_MASTER_SLAVE_CONSTRAINT_HPP   
#define QUEST_MASTER_SLAVE_CONSTRAINT_HPP  

// 项目头文件
#include "includes/define.hpp"
#include "includes/node.hpp"
#include "container/flags.hpp"
#include "container/variable.hpp"
#include "includes/process_info.hpp"
#include "includes/indexed_object.hpp"

namespace Quest{

    class QUEST_API(QUEST_CORE) MasterSlaveConstraint: public IndexedObject, public Flags{
        public:
            QUEST_CLASS_POINTER_DEFINITION(MasterSlaveConstraint);

            using BaseType = IndexedObject;
            using IndexType = std::size_t;
            using DofType = Dof<double>;
            using DofPointerVectorType = std::vector<DofType::Pointer>;
            using NodeType = Node;
            using EquationIdVectorType = std::vector<std::size_t>;
            using MatrixType = Matrix;
            using VectorType = Vector;
            using VariableType = Quest::Variable<double>;


        public:
            explicit MasterSlaveConstraint(IndexType Id = 0):IndexedObject(Id),Flags(){}


            virtual ~MasterSlaveConstraint() override{}


            MasterSlaveConstraint(const MasterSlaveConstraint& rOther):
                BaseType(rOther),
                mData(rOther.mData)
            {}


            MasterSlaveConstraint& operator = (const MasterSlaveConstraint& rOther){
                BaseType::operator=(rOther);
                mData = rOther.mData;
                return *this;
            }


            virtual MasterSlaveConstraint::Pointer Create(
                IndexType Id,
                DofPointerVectorType& rMasterDofsVector,
                DofPointerVectorType& rSlaveDofsVector,
                const MatrixType& rRelationMatrix,
                const VectorType& rConstantVector
            ) const {
                QUEST_TRY

                QUEST_ERROR << "Create not implemented in MasterSlaveConstraintBaseClass" << std::endl;

                QUEST_CATCH("")
            }


            virtual MasterSlaveConstraint::Pointer Create(
                IndexType Id,
                NodeType& rMasterNode,
                const VariableType& rMasterVariable,
                NodeType& rSlaveNode,
                const VariableType& rSlaveVariable,
                const double Weight,
                const double Constant
            ) const {
                QUEST_TRY

                QUEST_ERROR << "Create not implemented in MasterSlaveConstraintBaseClass" << std::endl;

                QUEST_CATCH("")
            }


            virtual Pointer Clone(IndexType NewId) const{
                QUEST_TRY

                QUEST_WARNING("MasterSlaveConstraint") << " Call base class constraint Clone " << std::endl;
                MasterSlaveConstraint::Pointer p_new_const = Quest::make_shared<MasterSlaveConstraint>(*this);
                p_new_const->SetId(NewId);
                p_new_const->SetData(this->GetData());
                p_new_const->Set(Flags(*this));
                return p_new_const;

                QUEST_CATCH("")
            }


            virtual void Clear(){}


            virtual void Initailize(const ProcessInfo& rCurrentProcessInfo){}


            virtual void Finalize(const ProcessInfo& rCurrentProcessInfo){
                this->Clear();
            }


            virtual void InitializeSolutionStep(const ProcessInfo& rCurrentProcessInfo){}


            virtual void InitializeNonLinearIteration(const ProcessInfo& rCurrentProcessInfo){}


            virtual void FinalizeNonLinearIteration(const ProcessInfo& rCurrentProcessInfo){}


            virtual void FinalizeSolutionStep(const ProcessInfo& rCurrentProcessInfo){}


            virtual void GetDofList(
                DofPointerVectorType& rSlaveDofsVector,
                DofPointerVectorType& rMasterDofsVector,
                const ProcessInfo& rCurrentProcessInfo
            ) const {
                QUEST_ERROR << "GetDofList not implemented in MasterSlaveConstraintBaseClass" << std::endl;
            }


            virtual void SetDofList(
                const DofPointerVectorType& rSlaveDofsVector,
                const DofPointerVectorType& rMasterDofsVector,
                const ProcessInfo& rCurrentProcessInfo
            ){
                QUEST_ERROR << "SetDofList not implemented in MasterSlaveConstraintBaseClass" << std::endl;
            }


            virtual void EquationIdVector(
                EquationIdVectorType& rSlaveEquationIds,
                EquationIdVectorType& rMasterEquationIds,
                const ProcessInfo& rCurrentProcessInfo
            ) const {
                if (rSlaveEquationIds.size() != 0){
                    rSlaveEquationIds.resize(0);
                }

                if (rMasterEquationIds.size() != 0){
                    rMasterEquationIds.resize(0);
                }
            }


            virtual const DofPointerVectorType& GetSlaveDofsVector() const{
                QUEST_ERROR << "GetSlaveDofsVector not implemented in MasterSlaveConstraintBaseClass" << std::endl;
            }


            virtual void SetSlaveDofsVector(const DofPointerVectorType& rSlaveDofsVector){
                QUEST_ERROR << "SetSlaveDofsVector not implemented in MasterSlaveConstraintBaseClass" << std::endl;
            }


            virtual const DofPointerVectorType& GetMasterDofsVector() const{
                QUEST_ERROR << "GetMasterDofsVector not implemented in MasterSlaveConstraintBaseClass" << std::endl;
            }


            virtual void SetMasterDofsVector(const DofPointerVectorType& rMasterDofsVector){    
                QUEST_ERROR << "SetMasterDofsVector not implemented in MasterSlaveConstraintBaseClass" << std::endl;
            }


            virtual void ResetSlaveDofs(const ProcessInfo& rCurrentProcessInfo){
                QUEST_ERROR << "ResetSlaveDofs not implemented in MasterSlaveConstraintBaseClass" << std::endl;
            }


            virtual void Apply(const ProcessInfo& rCurrentProcessInfo){
                QUEST_ERROR << "Apply not implemented in MasterSlaveConstraintBaseClass" << std::endl;
            }


            virtual void SetLocalSystem(
                const MatrixType& rRelationMatrix,
                const VectorType& rConstantVector,
                const ProcessInfo& rCurrentProcessInfo
            ){
                QUEST_TRY

                QUEST_ERROR << "SetLocalSystem not implemented in MasterSlaveConstraintBaseClass" << std::endl;

                QUEST_CATCH("")
            }


            virtual void GetLocalSystem(
                MatrixType& rRelationMatrix,
                VectorType& rConstantVector,
                const ProcessInfo& rCurrentProcessInfo
            ) const {
                QUEST_TRY

                this->CalculateLocalSystem(rRelationMatrix, rConstantVector, rCurrentProcessInfo);

                QUEST_CATCH("")
            }


            virtual void CalculateLocalSystem(
                MatrixType& rRelationMatrix,
                VectorType& rConstantVector,
                const ProcessInfo& rCurrentProcessInfo
            ) const {
                if(rRelationMatrix.size1() != 0){
                    rRelationMatrix.resize(0,0,false);
                }

                if(rConstantVector.size() != 0){
                    rConstantVector.resize(0,false);
                }
            }


            virtual int Check(const ProcessInfo& rCurrentProcessInfo) const{
                QUEST_TRY

                QUEST_ERROR_IF(this->Id() < 1) << "MasterSlaveConstraint found with Id " << this->Id() << std::endl;

                return 0;

                QUEST_CATCH("")
            }


            virtual std::string GetInfo() const{
                return " Constraint base class !";
            }


            virtual void PrintInfo(std::ostream& rOstream) const override{
                rOstream << " MasterSlaveConstraint Id : " << this->Id() << std::endl;
            }


            DataValueContainer& Data(){
                return mData;
            }


            const DataValueContainer& GetData() const{
                return mData;
            }


            void SetData(const DataValueContainer& rThisData){
                mData = rThisData;
            }


            template<typename TDataType>
            bool Has(const Variable<TDataType>& rThisVariable) const{
                return mData.Has(rThisVariable);
            }


            template<typename TVariableType>
            void SetValue(
                const TVariableType& rThisVariable,
                const typename TVariableType::Type& rValue
            ){
                mData.SetValue(rThisVariable, rValue);
            }


            template<typename TVariableType>
            typename TVariableType::Type& GetValue(const TVariableType& rThisVariable){
                return mData.GetValue(rThisVariable);
            }


            template<typename TVariableType>
            typename TVariableType::Type& GetValue(const TVariableType& rThisVariable) const{
                return mData.GetValue(rThisVariable);
            }


            bool IsActive() const;

        protected:

        private:
            friend class Serializer;

            virtual void save(Serializer& rSerializer) const override{
                QUEST_SERIALIZE_SAVE_BASE_CLASS(rSerializer, IndexedObject);
                QUEST_SERIALIZE_SAVE_BASE_CLASS(rSerializer, Flags);
                rSerializer.save("Data", mData);
            }

            virtual void load(Serializer& rSerializer) override{
                QUEST_SERIALIZE_LOAD_BASE_CLASS(rSerializer, IndexedObject);
                QUEST_SERIALIZE_LOAD_BASE_CLASS(rSerializer, Flags);
                rSerializer.load("Data", mData);
            }

        private:
            DataValueContainer mData;

    };

    QUEST_API_EXTERN template class QUEST_API(QUEST_CORE) QuestComponents<MasterSlaveConstraint>;

    inline std::istream& operator >> (std::istream& rIstream, MasterSlaveConstraint& rThis);

    inline std::ostream& operator << (std::ostream& rOstream, const MasterSlaveConstraint& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;

        return rOstream;
    }

} // namespace Quest

#endif  //QUEST_MASTER_SLAVE_CONSTRAINT_HPP