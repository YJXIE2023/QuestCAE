/*--------------------------------------------
quest_components.hpp类的实现代码
---------------------------------------------*/

// 项目头文件
#include "includes/quest_components.hpp"
#include "geometries/register_quest_components_for_geometry.hpp"
#include "includes/element.hpp"
#include "includes/condition.hpp"
#include "includes/constitutive_law.hpp"
#include "includes/master_slave_constraint.hpp"
#include "modeler/modeler.hpp"
#include "utilities/quaternion.hpp"
#include "factories/register_factories.hpp"
#include "factories/linear_solver_factory.hpp"
#include "factories/preconditioner_factory.hpp"

namespace Quest{

    void AddQuestComponent(const std::string& rName, const Variable<bool>& rComponent)
    {
        QuestComponents<Variable<bool>>::Add(rName, rComponent);
    }

    void AddQuestComponent(const std::string& rName, const Variable<int>& rComponent)
    {
        QuestComponents<Variable<int>>::Add(rName, rComponent);
    }

    void AddQuestComponent(const std::string& rName, const Variable<unsigned int>& rComponent)
    {
        QuestComponents<Variable<unsigned int>>::Add(rName, rComponent);
    }

    void AddQuestComponent(const std::string& rName, const Variable<double>& rComponent)
    {
        QuestComponents<Variable<double>>::Add(rName, rComponent);
    }

    void AddQuestComponent(const std::string& rName, const Variable<Array1d<double, 3>>& rComponent)
    {
        QuestComponents<Variable<Array1d<double, 3>>>::Add(rName, rComponent);
    }

    void AddQuestComponent(const std::string& rName, const Variable<Array1d<double, 4>>& rComponent)
    {
        QuestComponents<Variable<Array1d<double, 4>>>::Add(rName, rComponent);
    }

    void AddQuestComponent(const std::string& rName, const Variable<Array1d<double, 6>>& rComponent)
    {
        QuestComponents<Variable<Array1d<double, 6>>>::Add(rName, rComponent);
    }

    void AddQuestComponent(const std::string& rName, const Variable<Array1d<double, 9>>& rComponent)
    {
        QuestComponents<Variable<Array1d<double, 9>>>::Add(rName, rComponent);
    }

    void AddQuestComponent(const std::string& rName, const Variable<Quaternion<double>>& rComponent)
    {
        QuestComponents<Variable<Quaternion<double>>>::Add(rName, rComponent);
    }

    void AddQuestComponent(const std::string& rName, const Variable<Vector>& rComponent)
    {
        QuestComponents<Variable<Vector>>::Add(rName, rComponent);
    }

    void AddQuestComponent(const std::string& rName, const Variable<Matrix>& rComponent)
    {
        QuestComponents<Variable<Matrix>>::Add(rName, rComponent);
    }

    void AddQuestComponent(const std::string& rName, const Variable<std::string>& rComponent)
    {
        QuestComponents<Variable<std::string>>::Add(rName, rComponent);
    }

    void AddQuestComponent(const std::string& rName, const Variable<Flags>& rComponent)
    {
        QuestComponents<Variable<Flags>>::Add(rName, rComponent);
    }

    void AddQuestComponent(const std::string& rName, const Geometry<Node>& rComponent)
    {
        QuestComponents<Geometry<Node>>::Add(rName, rComponent);
    }

    void AddQuestComponent(const std::string& rName, const Element& rComponent)
    {
        QuestComponents<Element>::Add(rName, rComponent);
    }

    void AddQuestComponent(const std::string& rName, const Condition& rComponent)
    {
        QuestComponents<Condition>::Add(rName, rComponent);
    }

    void AddQuestComponent(const std::string& rName, const Modeler& rComponent)
    {
        QuestComponents<Modeler>::Add(rName, rComponent);
    }

    void AddQuestComponent(const std::string& rName, const ConstitutiveLaw& rComponent)
    {
        QuestComponents<ConstitutiveLaw>::Add(rName, rComponent);
    }

    void AddQuestComponent(const std::string& rName, const Variable<ConstitutiveLaw::Pointer>& rComponent)
    {
        QuestComponents<Variable<ConstitutiveLaw::Pointer>>::Add(rName, rComponent);
    }

    template class QuestComponents<Variable<bool>>;
    template class QuestComponents<Variable<int>>;
    template class QuestComponents<Variable<unsigned int>>;
    template class QuestComponents<Variable<double>>;
    template class QuestComponents<Variable<Array1d<double, 3>>>;
    template class QuestComponents<Variable<Array1d<double, 4>>>;
    template class QuestComponents<Variable<Array1d<double, 6>>>;
    template class QuestComponents<Variable<Array1d<double, 9>>>;
    template class QuestComponents<Variable<Quaternion<double>>>;
    template class QuestComponents<Variable<Vector>>;
    template class QuestComponents<Variable<Matrix>>;
    template class QuestComponents<Variable<std::string>>;
    template class QuestComponents<Variable<Flags>>;
    template class QuestComponents<Flags>;
    template class QuestComponents<DataCommunicator>;

    template class QuestComponents<Geometry<Node>>;
    template class QuestComponents<Element>;
    template class QuestComponents<Condition>;
    template class QuestComponents<ConstitutiveLaw>;
    template class QuestComponents<Variable<ConstitutiveLaw::Pointer>>;
    template class QuestComponents<MasterSlaveConstraint>;
    template class QuestComponents<Modeler>;

    using RealSparseSpace = UblasSpace<double, boost::numeric::ublas::compressed_matrix<double>, boost::numeric::ublas::vector<double>>;
    using RealDenseSpace = UblasSpace<double, DenseMatrix<double>, DenseVector<double>>;
    using ComplexSparseSpace = UblasSpace<std::complex<double>, boost::numeric::ublas::compressed_matrix<std::complex<double>>, boost::numeric::ublas::vector<std::complex<double>>>;
    using ComplexDenseSpace = UblasSpace<std::complex<double>, DenseMatrix<std::complex<double>>, DenseVector<std::complex<double>>>;

    template class QuestComponents<LinearSolverFactory<RealSparseSpace, RealDenseSpace>>;
    template class QuestComponents<LinearSolverFactory<ComplexSparseSpace, ComplexDenseSpace>>;
    template class QuestComponents<PreconditionerFactory<RealSparseSpace, RealDenseSpace>>;
    template class QuestComponents<ExplicitBuilder<RealSparseSpace, RealDenseSpace>>;

    QuestComponents<VariableData>::ComponentsContainerType QuestComponents<VariableData>::msComponents;

}