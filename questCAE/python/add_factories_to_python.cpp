#include "includes/define_python.hpp"
#include "python/add_factories_to_python.hpp"
#include "factories/linear_factory.hpp"
#include "factories/preconditioner_factory.hpp"
#include "factories/register_factories.hpp"


namespace Quest{

    using SpaceType = UblasSpace<double, CompressedMatrix, boost::numeric::ublas::vector<double>>;
    using LocalSpaceType = UblasSpace<double, Matrix, Vector>;
    using LinearSolverType = LinearSolver<SpaceType, LocalSpaceType>;
    using ComplexSpaceType = TUblasSparseSpace<std::complex<double>>;
    using ComplexLocalSpaceType = TUblasDenseSpace<std::complex<double>>;

    using LinearSolverFactoryType = LinearSolverFactory<SpaceType, LocalSpaceType>;
    using ComplexLinearSolverFactoryType = LinearSolverFactory<ComplexSpaceType, ComplexLocalSpaceType>;
    using PreconditionerFactoryType = PreconditionerFactory<SpaceType, LocalSpaceType>;
    using ExplicitBuilderType = ExplicitBuilder<SpaceType, LocalSpaceType>;
    using ExplicitBuilderFactoryType = Factory<ExplicitBuilderType>;

    void AddFactoriesToPython(pybind11::module& m){

        namespace py = pybind11;

        py::class_<LinearSolverFactoryType, LinearSolverFactoryType::Pointer>(m, "LinearSolverFactory")
            .def(py::init<>())
            .def("Create", &LinearSolverFactoryType::Create)
            .def("Has", &LinearSolverFactoryType::Has);

        py::class_<ComplexLinearSolverFactoryType, ComplexLinearSolverFactoryType::Pointer>(m, "ComplexLinearSolverFactory")
            .def(py::init<>())
            .def("Create", &ComplexLinearSolverFactoryType::Create)
            .def("Has", &ComplexLinearSolverFactoryType::Has);

        py::class_<PreconditionerFactoryType, PreconditionerFactoryType::Pointer>(m, "PreconditionerFactory")
            .def(py::init<>())
            .def("Create", &PreconditionerFactoryType::Create)
            .def("Has", &PreconditionerFactoryType::Has);

        py::class_<FactoryBase, FactoryBase::Pointer >(m, "FactoryBase")
            .def("Has", &FactoryBase::Has);

        py::class_<ExplicitBuilderFactoryType, ExplicitBuilderFactoryType::Pointer, FactoryBase>(m, "ExplicitBuilderFactory")
            .def(py::init<>())
            .def("Create", [](ExplicitBuilderFactoryType& rExplicitBuilderFactory, Kratos::Parameters Settings) {return rExplicitBuilderFactory.Create(Settings);})
            .def("__str__", PrintObject<ExplicitBuilderFactoryType>);

    }

}