#include "includes/define_python.hpp"
#include "python/add_linear_solvers_to_python.hpp"
#include "linear_solvers/cg_solver.hpp"
#include "includes/dof.hpp"
#include "space/ublas_space.hpp"
#include "includes/ublas_complex_interface.hpp"

#include "linear_solvers/reorderer/reorderer.hpp"
#include "linear_solvers/direct_solver.hpp"
#include "linear_solvers/skyline_lu_custom_scalar_solver.hpp"

#include "linear_solvers/preconditioner/preconditioner.hpp"
#include "linear_solvers/preconditioner/diagonal_preconditioner.hpp"

namespace Quest::Python{

    template<typename TDataType>
    using TSpaceType = UblasSpace<TDataType, boost::numeric::ublas::compressed_matrix<TDataType>, boost::numeric::ublas::vector<TDataType>>;

    template<typename TDataType>
    using TLocalSpaceType = UblasSpace<TDataType, DenseMatrix<TDataType>, DenseVector<TDataType>>;

    template<typename TDataType, typename TOtherDataType>
    using TLinearSolverType = LinearSolver<TSpaceType<TDataType>, TLocalSpaceType<TOtherDataType>>;

    template<typename TDataType>
    using TDirectSolverType = DirectSolver<TUblasSparseSpace<TDataType>, TUblasDenseSpace<TDataType>>;



    void AddLinearSolversToPython(pybind11::module& m){
        namespace py = pybind11;   

        using SpaceType = UblasSpace<double, CompressedMatrix, boost::numeric::ublas::vector<double>>;
        using LocalSpaceType = UblasSpace<double, Matrix, Vector>;
        using ComplexSpaceType = TUblasSparseSpace<std::complex<double>>;
        using ComplexLocalSpaceType = TUblasDenseSpace<std::complex<double>>;

        using LinearSolverType = LinearSolver<SpaceType,  LocalSpaceType>;
        using IterativeSolverType = IterativeSolver<SpaceType,  LocalSpaceType>;
        using CGSolverType = CGSolver<SpaceType,  LocalSpaceType>;
        
        using ComplexLinearSolverType = TLinearSolverType<std::complex<double>, std::complex<double>>;
        using MixedLinearSolverType = TLinearSolverType<double, std::complex<double>>;
        using ComplexDirectSolverType = TDirectSolverType<std::complex<double>>;
        using ComplexSkylineLUSolverType = SkylineLUCustomScalarSolver<ComplexSpaceType, ComplexLocalSpaceType>;

        bool (LinearSolverType::*pointer_to_solve)(LinearSolverType::SparseMatrixType& rA, LinearSolverType::VectorType& rX, LinearSolverType::VectorType& rB) = &LinearSolverType::Solve;
        void (LinearSolverType::*pointer_to_solve_eigen)(LinearSolverType::SparseMatrixType& rK, LinearSolverType::SparseMatrixType& rM,LinearSolverType::DenseVectorType& Eigenvalues, LinearSolverType::DenseMatrixType& Eigenvectors) = &LinearSolverType::Solve;
        bool (ComplexLinearSolverType::*pointer_to_complex_solve)(ComplexLinearSolverType::SparseMatrixType& rA, ComplexLinearSolverType::VectorType& rX, ComplexLinearSolverType::VectorType& rB) = &ComplexLinearSolverType::Solve;
        void (ComplexLinearSolverType::*pointer_to_complex_solve_eigen)(ComplexLinearSolverType::SparseMatrixType& rK, ComplexLinearSolverType::SparseMatrixType& rM, ComplexLinearSolverType::DenseVectorType& Eigenvalues, ComplexLinearSolverType::DenseMatrixType& Eigenvectors) = &ComplexLinearSolverType::Solve;
        void (MixedLinearSolverType::*pointer_to_mixed_solve_eigen)(MixedLinearSolverType::SparseMatrixType& rK, MixedLinearSolverType::SparseMatrixType& rM,MixedLinearSolverType::DenseVectorType& Eigenvalues, MixedLinearSolverType::DenseMatrixType& Eigenvectors) = &MixedLinearSolverType::Solve;

        // 预处理器
        using PreconditionerType = Preconditioner<SpaceType,  LocalSpaceType>;
        py::class_<PreconditionerType, PreconditionerType::Pointer>(m,"Preconditioner")
            .def(py::init<>())
            .def("__str__", PrintObject<PreconditionerType>);


        using DiagonalPreconditionerType = DiagonalPreconditioner<SpaceType,  LocalSpaceType>;
        py::class_<DiagonalPreconditionerType, DiagonalPreconditionerType::Pointer, PreconditionerType>(m,"DiagonalPreconditioner")
            .def(py::init<>())
            .def("__str__", PrintObject<DiagonalPreconditionerType>);

        // 线性求解器
        py::class_<LinearSolverType, LinearSolverType::Pointer>(m,"LinearSolver")
            .def(py::init<>())
            .def("Initialize",&LinearSolverType::Initialize)
            .def("Solve",pointer_to_solve)
            .def("Solve",pointer_to_solve_eigen)
            .def("Clear",&LinearSolverType::Clear)
            .def("__str__", PrintObject<LinearSolverType>)
            .def( "GetIterationsNumber",&LinearSolverType::GetIterationsNumber);

        py::class_<ComplexLinearSolverType, ComplexLinearSolverType::Pointer>(m,"ComplexLinearSolver")
            .def(py::init< >() )
            .def("Initialize",&ComplexLinearSolverType::Initialize)
            .def("Solve",pointer_to_complex_solve)
            .def("Solve",pointer_to_complex_solve_eigen)
            .def("Clear",&ComplexLinearSolverType::Clear)
            .def("__str__", PrintObject<ComplexLinearSolverType>);

        py::class_<MixedLinearSolverType, MixedLinearSolverType::Pointer>(m,"MixedLinearSolver")
            .def(py::init< >() )
            .def("Initialize",&MixedLinearSolverType::Initialize)
            .def("Solve",pointer_to_mixed_solve_eigen)
            .def("Clear",&MixedLinearSolverType::Clear)
            .def("__str__", PrintObject<MixedLinearSolverType>);

        py::class_<IterativeSolverType, IterativeSolverType::Pointer, LinearSolverType>(m,"IterativeSolver")
            .def(py::init< >() )
            .def("__str__", PrintObject<IterativeSolverType>);

        py::class_<CGSolverType, CGSolverType::Pointer,IterativeSolverType>(m,"CGSolver")
            .def(py::init<double>())
            .def(py::init<double, unsigned int>())
            .def(py::init<double, unsigned int,  PreconditionerType::Pointer>())
            .def(py::init<Parameters,  PreconditionerType::Pointer>())
            .def("__str__", PrintObject<CGSolverType>);

        using ReordererType = Reorderer<SpaceType,  LocalSpaceType >;
        using DirectSolverType = DirectSolver<SpaceType,  LocalSpaceType, ReordererType >;
        
        
        py::class_<ReordererType, ReordererType::Pointer>(m,"Reorderer")
            .def(py::init< >() )
            .def("__str__", PrintObject<ReordererType>)
            .def( "Initialize",&ReordererType::Initialize)
            .def( "Reorder",&ReordererType::Reorder)
            .def( "InverseReorder",&ReordererType::InverseReorder);
        
        py::class_<DirectSolverType, DirectSolverType::Pointer, LinearSolverType>(m,"DirectSolver")
            .def(py::init< >() )
            .def(py::init<Parameters>())
            .def("__str__", PrintObject<DirectSolverType>);

        py::class<ComplexDirectSolverType, ComplexDirectSolverType::Pointer, ComplexLinearSolverType>(m,"ComplexDirectSolver")
            .def(py::init< >() )
            .def(py::init<Parameters>())
            .def("__str__", PrintObject<ComplexDirectSolverType>);

        py::class_<ComplexSkylineLUSolverType, typename ComplexSkylineLUSolverType::Pointer, ComplexDirectSolverType>(m,"ComplexSkylineLUSolver")
            .def(py::init< >())
            .def(py::init<Parameters&>())
            .def("__str__", PrintObject<ComplexSkylineLUSolverType>);
    }

}