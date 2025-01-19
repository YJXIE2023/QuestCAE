// 项目头文件
#include "includes/define.hpp"
#include "includes/variables.hpp"
#include "includes/quest_flags.hpp"
#include "includes/kernel.hpp"
#include "includes/node.hpp"
#include "includes/element.hpp"
#include "includes/condition.hpp"
#include "includes/constitutive_law.hpp"
#include "includes/geometrical_object.hpp"
#include "includes/master_slave_constraint.hpp"

#include "factories/standard_linear_solver_factory.hpp"
#include "factories/standard_preconditioner_factory.hpp"

namespace Quest{

    QuestApplication::QuestApplication(const std::string& ApplicationName):
        mApplicationName(ApplicationName),
        mMasterSlaveConstraint(),

        mpVariableData(QuestComponents<VariableData>::pGetComponents()),
        mpIntVariables(QuestComponents<Variable<int> >::pGetComponents()),
        mpUnsignedIntVariables(QuestComponents<Variable<unsigned int> >::pGetComponents()),
        mpDoubleVariables(QuestComponents<Variable<double> >::pGetComponents()),
        mpArray1DVariables(QuestComponents<Variable<Array1d<double, 3> > >::pGetComponents()),
        mpArray1D4Variables(QuestComponents<Variable<Array1d<double, 4> > >::pGetComponents()),
        mpArray1D6Variables(QuestComponents<Variable<Array1d<double, 6> > >::pGetComponents()),
        mpArray1D9Variables(QuestComponents<Variable<Array1d<double, 9> > >::pGetComponents()),
        mpQuaternionVariables(QuestComponents<Variable<Quaternion<double> > >::pGetComponents()),
        mpVectorVariables(QuestComponents<Variable<Vector> >::pGetComponents()),
        mpMatrixVariables(QuestComponents<Variable<Matrix> >::pGetComponents()),
        mpGeometries(QuestComponents<GeometryType>::pGetComponents()),
        mpElements(QuestComponents<Element>::pGetComponents()),
        mpConditions(QuestComponents<Condition>::pGetComponents()),
        mpModelers(QuestComponents<Modeler>::pGetComponents()),
        mpRegisteredObjects(&(Serializer::GetRegisteredObjects())),
        mpRegisteredObjectsName(&(Serializer::GetRegisteredObjectsName())) {}


    void QuestApplication::RegisterQuestCore() {

        QuestApplication::RegisterVariables();

        RegisterLinearSolvers();
        RegisterPreconditioners();

        Serializer::Register("Node", NodeType());
        Serializer::Register("Dof", Dof<double>());
        Serializer::Register("Geometry", GeometryType());
        Serializer::Register("Element", Element());
        Serializer::Register("Condition", Condition());
        Serializer::Register("Modeler", Modeler());
        Serializer::Register("Properties", Properties());
        Serializer::Register("GeometricalObject", GeometricalObject());

        Serializer::Register("Node3D", NodeType());
        Serializer::Register("DofDouble", Dof<double>());

        Serializer::Register("MasterSlaveConstraint", MasterSlaveConstraint());

        QUEST_REGISTER_CONSTRAINT("MasterSlaveConstraint",mMasterSlaveConstraint);
        QUEST_REGISTER_MODELER("Modeler", mModeler);

        Serializer::Register("Point", mPointPrototype);

        QUEST_REGISTER_FLAG(STRUCTURE);
        QUEST_REGISTER_FLAG(FLUID);
        QUEST_REGISTER_FLAG(THERMAL);
        QUEST_REGISTER_FLAG(VISITED);
        QUEST_REGISTER_FLAG(SELECTED);
        QUEST_REGISTER_FLAG(BOUNDARY);
        QUEST_REGISTER_FLAG(INLET);
        QUEST_REGISTER_FLAG(OUTLET);
        QUEST_REGISTER_FLAG(SLIP);
        QUEST_REGISTER_FLAG(INTERFACE);
        QUEST_REGISTER_FLAG(CONTACT);
        QUEST_REGISTER_FLAG(TO_SPLIT);
        QUEST_REGISTER_FLAG(TO_ERASE);
        QUEST_REGISTER_FLAG(TO_REFINE);
        QUEST_REGISTER_FLAG(NEW_ENTITY);
        QUEST_REGISTER_FLAG(OLD_ENTITY);
        QUEST_REGISTER_FLAG(ACTIVE);
        QUEST_REGISTER_FLAG(MODIFIED);
        QUEST_REGISTER_FLAG(RIGID);
        QUEST_REGISTER_FLAG(SOLID);
        QUEST_REGISTER_FLAG(MPI_BOUNDARY);
        QUEST_REGISTER_FLAG(INTERACTION);
        QUEST_REGISTER_FLAG(ISOLATED);
        QUEST_REGISTER_FLAG(MASTER);
        QUEST_REGISTER_FLAG(SLAVE);
        QUEST_REGISTER_FLAG(INSIDE);
        QUEST_REGISTER_FLAG(FREE_SURFACE);
        QUEST_REGISTER_FLAG(BLOCKED);
        QUEST_REGISTER_FLAG(MARKER);
        QUEST_REGISTER_FLAG(PERIODIC);
        QUEST_REGISTER_FLAG(WALL);

        QUEST_ADD_FLAG_TO_QUEST_COMPONENTS(ALL_DEFINED);
        QUEST_ADD_FLAG_TO_QUEST_COMPONENTS(ALL_TRUE);

        QUEST_REGISTER_CONSTITUTIVE_LAW("ConstitutiveLaw", mConstitutiveLaw);
    }

}