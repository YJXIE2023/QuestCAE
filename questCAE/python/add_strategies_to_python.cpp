#include "includes/define_python.hpp"
#include "python/add_strategies_to_python.hpp"
#include "includes/model_part.hpp"
#include "spaces/ublas_space.hpp"
#include "includes/ublas_complex_interface.hpp"

#include "solving_strats/strategies/solving_strategy.hpp"
#include "solving_strats/strategies/implicit_solving_strategy.hpp"
#include "solving_strats/strategies/explicit_solving_strategy.hpp"

#include "solving_strategies/schemes/scheme.hpp"

#include "solving_strategies/builder_and_solvers/builder_and_solver.hpp"
#include "solving_strategies/builder_and_solvers/explicit_builder.hpp"

#include "linear_solvers/linear_solver.hpp"


namespace Quest::Python{
    namespace py = pybind11;

    using SparseSpaceType = UblasSpace<double, CompressedMatrix, boost::numeric::ublas::vector<double>>;
    using LocalSpaceType = UblasSpace<double, Matrix, Vector>;
    using ComplexSparseSpaceType = UblasSpace<std::complex<double>, ComplexCompressedMatrix, boost::numeric::ublas::vector<std::complex<double>>>;
    using ComplexLocalSpaceType = UblasSpace<std::complex<double>, ComplexMatrix, ComplexVector>;


    double Dot(SparseSpaceType& dummy, SparseSpaceType::VectorType& rX, SparseSpaceType::VectorType& rY){
        return dummy.Dot(rX, rY);
    }

    template< typename TSpaceType >
    void ScaleAndAdd(TSpaceType& dummy, const double A, const typename TSpaceType::VectorType& rX, const double B, typename TSpaceType::VectorType& rY){
        dummy.ScaleAndAdd(A, rX, B, rY);
    }

    template< typename TSpaceType >
    void Mult(TSpaceType& dummy, typename TSpaceType::MatrixType& rA, typename TSpaceType::VectorType& rX, typename TSpaceType::VectorType& rY){
        dummy.Mult(rA, rX, rY);
    }

    void TransposeMult(SparseSpaceType& dummy, SparseSpaceType::MatrixType& rA, SparseSpaceType::VectorType& rX, SparseSpaceType::VectorType& rY){
        dummy.TransposeMult(rA, rX, rY);
    }

    template< typename TSpaceType >
    typename TSpaceType::IndexType Size(TSpaceType& dummy, typename TSpaceType::VectorType const& rV){
        return rV.size();
    }

    template< typename TSpaceType >
    typename TSpaceType::IndexType Size1(TSpaceType& dummy, typename TSpaceType::MatrixType const& rM){
        return rM.size1();
    }

    template< typename TSpaceType >
    typename TSpaceType::IndexType Size2(TSpaceType& dummy, typename TSpaceType::MatrixType const& rM){
        return rM.size2();
    }

    template< typename TSpaceType >
    void ResizeMatrix(TSpaceType& dummy, typename TSpaceType::MatrixType& A, unsigned int i1, unsigned int i2){
        dummy.Resize(A, i1, i2);
    }

    template< typename TSpaceType >
    void ResizeVector(TSpaceType& dummy, typename TSpaceType::VectorType& x, unsigned int i1){
        dummy.Resize(x, i1);
    }

    template< typename TSpaceType >
    void SetToZeroMatrix(TSpaceType& dummy, typename TSpaceType::MatrixType& A){
        dummy.SetToZero(A);
    }

    template< typename TSpaceType >
    void SetToZeroVector(TSpaceType& dummy, typename TSpaceType::VectorType& x){
        dummy.SetToZero(x);
    }

    template< typename TSpaceType >
    void ClearMatrix(TSpaceType& dummy, typename TSpaceType::MatrixPointerType& A){
        dummy.Clear(A);
    }

    template< typename TSpaceType >
    void ClearVector(TSpaceType& dummy, typename TSpaceType::VectorPointerType& x){
        dummy.Clear(x);
    }

    double TwoNorm(SparseSpaceType& dummy, SparseSpaceType::VectorType& x){
        return dummy.TwoNorm(x);
    }

    template< typename TSpaceType >
    void UnaliasedAdd(TSpaceType& dummy, typename TSpaceType::VectorType& x, const double A, const typename TSpaceType::VectorType& rY){ // x+= a*Y
        dummy.UnaliasedAdd(x, A, rY);
    }

    void MoveMesh(Scheme< SparseSpaceType, LocalSpaceType >& dummy, ModelPart::NodesContainerType& rNodes){
        VariableUtils().UpdateCurrentPosition(rNodes, DISPLACEMENT);
    }

    template< typename TSpaceType >
    typename TSpaceType::MatrixPointerType CreateEmptyMatrixPointer(TSpaceType& dummy){
        return dummy.CreateEmptyMatrixPointer();
    }

    template< typename TSpaceType >
    typename TSpaceType::VectorPointerType CreateEmptyVectorPointer(TSpaceType& dummy){
        return dummy.CreateEmptyVectorPointer();
    }

    CompressedMatrix& GetMatRef(Kratos::shared_ptr<CompressedMatrix>& dummy){
        return *dummy;
    }

    Vector& GetVecRef(Kratos::shared_ptr<Vector>& dummy){
        return *dummy;
    }


    template<typename TSpaceType >
    py::class_< TSpaceType > CreateSpaceInterface(pybind11::module& m, std::string Name){
        py::class_< TSpaceType > binder(m,Name.c_str());
        binder.def(py::init<>());

        binder.def("ClearMatrix", ClearMatrix<TSpaceType>);
        binder.def("ClearVector", ClearVector<TSpaceType>);
        binder.def("ResizeMatrix", ResizeMatrix<TSpaceType>);
        binder.def("ResizeVector", ResizeVector<TSpaceType>);
        binder.def("SetToZeroMatrix", SetToZeroMatrix<TSpaceType>);
        binder.def("SetToZeroVector", SetToZeroVector<TSpaceType>);
        binder.def("ScaleAndAdd", ScaleAndAdd<TSpaceType>);
        binder.def("Mult", Mult<TSpaceType>);
        binder.def("Size", Size<TSpaceType>);
        binder.def("Size1", Size1<TSpaceType>);
        binder.def("Size2", Size2<TSpaceType>);
        binder.def("UnaliasedAdd", UnaliasedAdd<TSpaceType>);
        binder.def("CreateEmptyMatrixPointer", CreateEmptyMatrixPointer<TSpaceType>);
        binder.def("CreateEmptyVectorPointer", CreateEmptyVectorPointer<TSpaceType>);

        return binder;
    }



    void AddStrategiesToPython(pybind11::module& m){
        using LinearSolverType = LinearSolver<SparseSpaceType, LocalSpaceType >;
        using BaseSchemeType = Scheme< SparseSpaceType, LocalSpaceType >;

        py::class_<BaseSchemeType, typename BaseSchemeType::Pointer >(m,"Scheme")
            .def(py::init< >())
            .def("Create", &BaseSchemeType::Create)
            .def("Initialize", &BaseSchemeType::Initialize)
            .def("SchemeIsInitialized", &BaseSchemeType::SchemeIsInitialized)
            .def("ElementsAreInitialized", &BaseSchemeType::ElementsAreInitialized)
            .def("ConditionsAreInitialized", &BaseSchemeType::ConditionsAreInitialized)
            .def("InitializeElements", &BaseSchemeType::InitializeElements)
            .def("InitializeConditions", &BaseSchemeType::InitializeConditions)
            .def("InitializeSolutionStep", &BaseSchemeType::InitializeSolutionStep)
            .def("FinalizeSolutionStep", &BaseSchemeType::FinalizeSolutionStep)
            .def("InitializeNonLinIteration", &BaseSchemeType::InitializeNonLinIteration)
            .def("FinalizeNonLinIteration", &BaseSchemeType::FinalizeNonLinIteration)
            .def("Predict", &BaseSchemeType::Predict)
            .def("Update", &BaseSchemeType::Update)
            .def("CalculateOutputData", &BaseSchemeType::CalculateOutputData)
            .def("Clean", &BaseSchemeType::Clean)
            .def("Clear",&BaseSchemeType::Clear)
            .def("MoveMesh", MoveMesh)
            .def("Check", [](const BaseSchemeType& self, const ModelPart& rModelPart){ return self.Check(rModelPart); })
            .def("GetDefaultParameters",&BaseSchemeType::GetDefaultParameters)
            .def("Info", &BaseSchemeType::Info);


        using BuilderAndSolverType = BuilderAndSolver< SparseSpaceType, LocalSpaceType, LinearSolverType >;
        using DofsArrayType = typename ModelPart::DofsArrayType

        py::class_< BuilderAndSolverType, typename BuilderAndSolverType::Pointer>(m,"BuilderAndSolver")
            .def(py::init<LinearSolverType::Pointer > ())
            .def(py::init<LinearSolverType::Pointer, Parameters >() )
            .def(py::init<LinearSolverType::Pointer > ())
            .def("Create", &BuilderAndSolverType::Create)
            .def("SetCalculateReactionsFlag", &BuilderAndSolverType::SetCalculateReactionsFlag)
            .def("GetCalculateReactionsFlag", &BuilderAndSolverType::GetCalculateReactionsFlag)
            .def("SetDofSetIsInitializedFlag", &BuilderAndSolverType::SetDofSetIsInitializedFlag)
            .def("GetDofSetIsInitializedFlag", &BuilderAndSolverType::GetDofSetIsInitializedFlag)
            .def("SetReshapeMatrixFlag", &BuilderAndSolverType::SetReshapeMatrixFlag)
            .def("GetReshapeMatrixFlag", &BuilderAndSolverType::GetReshapeMatrixFlag)
            .def("GetEquationSystemSize", &BuilderAndSolverType::GetEquationSystemSize)
            .def("BuildLHS", &BuilderAndSolverType::BuildLHS)
            .def("BuildRHS", &BuilderAndSolverType::BuildRHS)
            .def("Build", &BuilderAndSolverType::Build)
            .def("SystemSolve", &BuilderAndSolverType::SystemSolve)
            .def("BuildAndSolve", &BuilderAndSolverType::BuildAndSolve)
            .def("BuildAndSolveLinearizedOnPreviousIteration", &BuilderAndSolverType::BuildAndSolveLinearizedOnPreviousIteration)
            .def("BuildRHSAndSolve", &BuilderAndSolverType::BuildRHSAndSolve)
            .def("ApplyDirichletConditions", &BuilderAndSolverType::ApplyDirichletConditions)
            .def("ApplyConstraints", &BuilderAndSolverType::ApplyConstraints)
            .def("SetUpDofSet", &BuilderAndSolverType::SetUpDofSet)
            .def("GetDofSet",  [](BuilderAndSolverType& self) -> DofsArrayType& {return self.GetDofSet();}, py::return_value_policy::reference_internal)
            .def("SetUpSystem", &BuilderAndSolverType::SetUpSystem)
            .def("ResizeAndInitializeVectors", &BuilderAndSolverType::ResizeAndInitializeVectors)
            .def("InitializeSolutionStep", &BuilderAndSolverType::InitializeSolutionStep)
            .def("FinalizeSolutionStep", &BuilderAndSolverType::FinalizeSolutionStep)
            .def("CalculateReactions", &BuilderAndSolverType::CalculateReactions)
            .def("Clear", &BuilderAndSolverType::Clear)
            .def("Check", &BuilderAndSolverType::Check)
            .def("GetDefaultParameters",&BuilderAndSolverType::GetDefaultParameters)
            .def("SetEchoLevel", &BuilderAndSolverType::SetEchoLevel)
            .def("GetEchoLevel", &BuilderAndSolverType::GetEchoLevel)
            .def("Info", &BuilderAndSolverType::Info);


        using ExplicitBuilderType = ExplicitBuilder< SparseSpaceType, LocalSpaceType >;

        py::class_<ExplicitBuilderType, typename ExplicitBuilderType::Pointer>(m, "ExplicitBuilder")
            .def(py::init<>())
            .def("SetCalculateReactionsFlag", &ExplicitBuilderType::SetCalculateReactionsFlag)
            .def("GetCalculateReactionsFlag", &ExplicitBuilderType::GetCalculateReactionsFlag)
            .def("SetDofSetIsInitializedFlag", &ExplicitBuilderType::SetDofSetIsInitializedFlag)
            .def("GetDofSetIsInitializedFlag", &ExplicitBuilderType::GetDofSetIsInitializedFlag)
            .def("SetResetDofSetFlag", &ExplicitBuilderType::SetResetDofSetFlag)
            .def("GetResetDofSetFlag", &ExplicitBuilderType::GetResetDofSetFlag)
            .def("SetResetLumpedMassVectorFlag", &ExplicitBuilderType::SetResetLumpedMassVectorFlag)
            .def("GetResetLumpedMassVectorFlag", &ExplicitBuilderType::GetResetLumpedMassVectorFlag)
            .def("GetEquationSystemSize", &ExplicitBuilderType::GetEquationSystemSize)
            .def("BuildRHS", &ExplicitBuilderType::BuildRHS)
            .def("ApplyConstraints", &ExplicitBuilderType::ApplyConstraints)
            .def("GetDofSet", [](ExplicitBuilderType& self) -> DofsArrayType& {return self.GetDofSet();}, py::return_value_policy::reference_internal)
            .def("GetLumpedMassMatrixVector", &ExplicitBuilderType::GetLumpedMassMatrixVector, py::return_value_policy::reference_internal)
            .def("Initialize", &ExplicitBuilderType::Initialize)
            .def("InitializeSolutionStep", &ExplicitBuilderType::InitializeSolutionStep)
            .def("FinalizeSolutionStep", &ExplicitBuilderType::FinalizeSolutionStep)
            .def("Clear", &ExplicitBuilderType::Clear)
            .def("Check", &ExplicitBuilderType::Check)
            .def("GetDefaultParameters",&ExplicitBuilderType::GetDefaultParameters)
            .def("SetEchoLevel", &ExplicitBuilderType::SetEchoLevel)
            .def("GetEchoLevel", &ExplicitBuilderType::GetEchoLevel)
            .def("Info", &ExplicitBuilderType::Info);


        auto sparse_space_binder = CreateSpaceInterface< SparseSpaceType >(m,"UblasSparseSpace");
        sparse_space_binder.def("TwoNorm", TwoNorm);
        sparse_space_binder.def("Dot", Dot);
        sparse_space_binder.def("TransposeMult", TransposeMult);

        auto cplx_sparse_space_binder = CreateSpaceInterface< ComplexSparseSpaceType >(m,"UblasComplexSparseSpace");


        using BaseSolvingStrategyType = SolvingStrategy< SparseSpaceType, LocalSpaceType >
        py::class_< BaseSolvingStrategyType, typename BaseSolvingStrategyType::Pointer >(m,"BaseSolvingStrategy")
            .def(py::init<ModelPart&, Parameters >() )
            .def(py::init < ModelPart&, bool >())
            .def("Create", &BaseSolvingStrategyType::Create)
            .def("Predict", &BaseSolvingStrategyType::Predict)
            .def("Initialize", &BaseSolvingStrategyType::Initialize)
            .def("Solve", &BaseSolvingStrategyType::Solve)
            .def("IsConverged", &BaseSolvingStrategyType::IsConverged)
            .def("CalculateOutputData", &BaseSolvingStrategyType::CalculateOutputData)
            .def("SetEchoLevel", &BaseSolvingStrategyType::SetEchoLevel)
            .def("GetEchoLevel", &BaseSolvingStrategyType::GetEchoLevel)
            .def("SetRebuildLevel", &BaseSolvingStrategyType::SetRebuildLevel)
            .def("GetRebuildLevel", &BaseSolvingStrategyType::GetRebuildLevel)
            .def("SetMoveMeshFlag", &BaseSolvingStrategyType::SetMoveMeshFlag)
            .def("MoveMeshFlag", &BaseSolvingStrategyType::GetMoveMeshFlag)
            .def("GetMoveMeshFlag", &BaseSolvingStrategyType::GetMoveMeshFlag)
            .def("MoveMesh", &BaseSolvingStrategyType::MoveMesh)
            .def("Clear", &BaseSolvingStrategyType::Clear)
            .def("Check", &BaseSolvingStrategyType::Check)
            .def("GetDefaultParameters",&BaseSolvingStrategyType::GetDefaultParameters)
            .def("GetSystemMatrix", &BaseSolvingStrategyType::GetSystemMatrix, py::return_value_policy::reference_internal)
            .def("GetSystemVector", &BaseSolvingStrategyType::GetSystemVector, py::return_value_policy::reference_internal)
            .def("GetSolutionVector", &BaseSolvingStrategyType::GetSolutionVector, py::return_value_policy::reference_internal)
            .def("InitializeSolutionStep", &BaseSolvingStrategyType::InitializeSolutionStep)
            .def("FinalizeSolutionStep", &BaseSolvingStrategyType::FinalizeSolutionStep)
            .def("SolveSolutionStep", &BaseSolvingStrategyType::SolveSolutionStep)
            .def("GetModelPart", [](BaseSolvingStrategyType& self) -> ModelPart& { return self.GetModelPart(); })
            .def("Info", &BaseSolvingStrategyType::Info);


        using ImplicitSolvingStrategyType = ImplicitSolvingStrategy< SparseSpaceType, LocalSpaceType, LinearSolverType >
        py::class_<ImplicitSolvingStrategyType, typename ImplicitSolvingStrategyType::Pointer, BaseSolvingStrategyType>(m,"ImplicitSolvingStrategy")
            .def(py::init<ModelPart&, Parameters >() )
            .def(py::init < ModelPart&, bool >())
            .def("SetStiffnessMatrixIsBuilt", &ImplicitSolvingStrategyType::SetStiffnessMatrixIsBuilt)
            .def("GetStiffnessMatrixIsBuilt", &ImplicitSolvingStrategyType::GetStiffnessMatrixIsBuilt);


        using BaseExplicitSolvingStrategyType = ExplicitSolvingStrategy< SparseSpaceType, LocalSpaceType >
        py::class_< BaseExplicitSolvingStrategyType, typename BaseExplicitSolvingStrategyType::Pointer, BaseSolvingStrategyType >(m,"BaseExplicitSolvingStrategy")
            .def(py::init<ModelPart &, bool, int>())
            .def(py::init<ModelPart&, typename ExplicitBuilderType::Pointer, bool, int>());
    }
}