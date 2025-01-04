#include "includes/define.hpp"
#include "factories/standard_preconditioner_factory.hpp"
#include "space/ublas_space.hpp"

#include "linear_solvers/preconditioner/preconditioner.hpp"
#include "linear_solvers/preconditioner/diagonal_preconditioner.hpp"


namespace Quest{

    void RegisterPreconditioners(){
        using SpaceType = TUblasSparseSpace<double>;
        using LocalSpaceType = TUblasDenseSpace<double>;

        using PreconditionerType = Preconditioner<SpaceType, LocalSpaceType>;
        using DiagonalPreconditionerType = DiagonalPreconditioner<SpaceType, LocalSpaceType>;

        static auto PreconditionerFactory = StandardPreconditionerFactory<SpaceType, LocalSpaceType, PreconditionerType>();
        static auto DiagonalPreconditionerFactory = StandardPreconditionerFactory<SpaceType, LocalSpaceType, DiagonalPreconditionerType>();

        QUEST_REGISTER_PRECONDITIONER("none", PreconditionerFactory);
        QUEST_REGISTER_PRECONDITIONER("diagonal", DiagonalPreconditionerFactory);
    };

}