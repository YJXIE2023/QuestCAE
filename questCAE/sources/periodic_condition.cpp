#include "includes/periodic_condition.hpp"

namespace Quest{

    PeriodicCondition::PeriodicCondition(IndexType NewId):
        Condition(NewId){}
    

    PeriodicCondition::PeriodicCondition(IndexType NewId, const NodesArrayType& ThisNodes):
        Condition(NewId,ThisNodes){}
    

    PeriodicCondition::PeriodicCondition(IndexType NewId, GeometryType::Pointer pGeometry):
        Condition(NewId,pGeometry){}


    PeriodicCondition::PeriodicCondition(IndexType NewId, GeometryType::Pointer pGeometry, PropertiesType::Pointer pProperties):
        Condition(NewId,pGeometry,pProperties){}


    PeriodicCondition::PeriodicCondition(PeriodicCondition const& rOther):
        Condition(rOther){}


    PeriodicCondition::~PeriodicCondition(){}


    PeriodicCondition& PeriodicCondition::operator =(PeriodicCondition const& rOther)
    {
        Condition::operator =(rOther);

        return *this;
    }


    Condition::Pointer PeriodicCondition::Create(IndexType NewId, NodesArrayType const& ThisNodes, PropertiesType::Pointer pProperties) const
    {
        return Quest::make_intrusive< PeriodicCondition >(NewId, GetGeometry().Create(ThisNodes), pProperties);
    }


    int PeriodicCondition::Check(const ProcessInfo& rCurrentProcessInfo) const
    {
        QUEST_TRY;

        return Condition::Check(rCurrentProcessInfo);

        QUEST_CATCH("");
    }


    void PeriodicCondition::CalculateLocalSystem(MatrixType& rLeftHandSideMatrix, VectorType& rRightHandSideVector, const ProcessInfo& rCurrentProcessInfo)
    {
        rLeftHandSideMatrix.resize(0,0,false);
        rRightHandSideVector.resize(0,false);
    }


    void PeriodicCondition::CalculateLeftHandSide(MatrixType& rLeftHandSideMatrix, const ProcessInfo& rCurrentProcessInfo)
    {
        rLeftHandSideMatrix.resize(0,0,false);
    }


    void PeriodicCondition::CalculateRightHandSide(VectorType& rRightHandSideVector, const ProcessInfo& rCurrentProcessInfo)
    {
        MatrixType LHS;
        CalculateLocalSystem(LHS,rRightHandSideVector,rCurrentProcessInfo);
    }


    void PeriodicCondition::EquationIdVector(EquationIdVectorType& rResult, const ProcessInfo& rCurrentProcessInfo) const
    {
        rResult.resize(0);
    }


    void PeriodicCondition::GetDofList(DofsVectorType& ElementalDofList, const ProcessInfo& rCurrentProcessInfo) const
    {
        const GeometryType& rGeom = this->GetGeometry();
        const PeriodicVariablesContainer& rPeriodicVariables = this->GetProperties().GetValue(PERIODIC_VARIABLES);
        const unsigned int BlockSize = rPeriodicVariables.Size();
        const unsigned int NumNodes = rGeom.PointsNumber();
        const unsigned int LocalSize = NumNodes * BlockSize;

        if (ElementalDofList.size() != LocalSize)
            ElementalDofList.resize(LocalSize);

        unsigned int LocalIndex = 0;

        for ( unsigned int n = 0; n < NumNodes; n++)
        {
            for(PeriodicVariablesContainer::DoubleVariablesConstIterator itDVar = rPeriodicVariables.DoubleVariablesBegin(); itDVar != rPeriodicVariables.DoubleVariablesEnd(); ++itDVar)
                ElementalDofList[LocalIndex++] = rGeom[n].pGetDof(*itDVar);
        }
    }


    void PeriodicCondition::GetValuesVector(Vector& Values, int Step) const{}


    void PeriodicCondition::save(Serializer& rSerializer) const
    {
        QUEST_SERIALIZE_SAVE_BASE_CLASS(rSerializer, Condition );
    }


    void PeriodicCondition::load(Serializer& rSerializer)
    {
        QUEST_SERIALIZE_LOAD_BASE_CLASS(rSerializer, Condition );
    }

}