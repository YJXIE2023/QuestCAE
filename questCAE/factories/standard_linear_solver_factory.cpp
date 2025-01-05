#include "includes/define.hpp"
#include "factories/standard_linear_solver_factory.hpp"
#include "linear_solvers/linear_solver.hpp"
#include "linear_solvers/cg_solver.hpp"
#include "linear_solvers/scaling_solver.hpp"
#include "linear_solvers/skyline_lu_custom_scalar_solver.hpp"
#include "space/ublas_space.hpp"


namespace Quest{

    void RegisterLinearSolvers(){
        using SpaceType = TUblasSparseSpace<double>;
        using LocalSpaceType = TUblasDenseSpace<double>;
        using ComplexSpaceType = TUblasDenseSpace<std::complex<double>>;
        using ComplexLocalSpaceType = TUblasDenseSpace<std::complex<double>>;

        using CGSolverType = CGSolver<SpaceType, LocalSpaceType>;
        using SkyLineLUComplexSolverType = SkylineLUCustomScalarSolver<ComplexSpaceType, ComplexLocalSpaceType>;
        using ScalingSolverType = ScalingSolver<SpaceType, LocalSpaceType>;

        static auto CGSolverFactory = StandardLinearSolverFactory<SpaceType, LocalSpaceType, CGSolverType>();
        static auto ScalingSolverFactory = StandardLinearSolverFactory<SpaceType, LocalSpaceType, ScalingSolverType>();
        static auto SkyLineLUComplexSolverFactory = StandardLinearSolverFactory<ComplexSpaceType, ComplexLocalSpaceType, SkyLineLUComplexSolverType>();

        QUEST_REGISTER_LINEAR_SOLVER("cg", CGSolverFactory);
        QUEST_REGISTER_LINEAR_SOLVER("scaling", ScalingSolverFactory);
        // QUEST_REGISTER_COMPLEX_LINEAR_SOLVER("skyline_lu_complex", SkyLineLUComplexSolverFactory);
    }

}