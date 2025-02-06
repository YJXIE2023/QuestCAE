#include <pybind11/pybind11.h>

#include "includes/define_python.hpp"
#include "includes/quest_version.hpp"
#include "includes/add_accessors_to_python.hpp"
#include "includes/add_communicator_to_python.hpp"
#include "includes/add_constitutive_law_to_python.hpp"
#include "includes/add_constraint_to_python.hpp"
#include "includes/add_containers_to_python.hpp"
#include "includes/add_data_communicator_to_python.hpp"
#include "includes/add_dofs_to_python.hpp"
#include "includes/add_factories_to_python.hpp"
#include "includes/add_geometries_to_python.hpp"
#include "includes/add_geometry_data_to_python.hpp"
#include "includes/add_global_pointers_to_python.hpp"
#include "includes/add_globals_to_python.hpp"
#include "includes/add_io_to_python.hpp"
#include "includes/add_kernel_to_python.hpp"
#include "includes/add_linear_solvers_to_python.hpp"
#include "includes/add_logger_to_python.hpp"
#include "includes/add_matrix_market_interface_to_python.hpp"
#include "includes/add_matrix_to_python.hpp"
#include "includes/add_mesh_to_python.hpp"
#include "includes/add_model_part_to_python.hpp"
#include "includes/add_model_to_python.hpp"
#include "includes/add_node_to_python.hpp"
#include "includes/add_parallel_environment_to_python.hpp"
#include "includes/add_points_to_python.hpp"
#include "includes/add_process_info_to_python.hpp"
#include "includes/add_properties_to_python.hpp"
#include "includes/add_quaternion_to_python.hpp"
#include "includes/add_quest_application_to_python.hpp"
#include "includes/add_quest_parameters_to_python.hpp"
#include "includes/add_registry_to_python.hpp"
#include "includes/add_serializer_to_python.hpp"
#include "includes/add_sparse_matrices_to_python.hpp"
#include "includes/add_strategies_to_python.hpp"
#include "includes/add_table_to_python.hpp"


namespace Quest::Python{

    PYBIND11_MODULE(quest, m){
        namespace py = pybind11;

        AddAccessorsToPython(m);
        AddCommunicatorToPython(m);
        AddConstitutiveLawToPython(m);
        AddConstraintToPython(m);
        AddContainersToPython(m);
        AddDataCommunicatorToPython(m);
        AddDofsToPython(m);
        AddFactoriesToPython(m);
        AddGeometriesToPython(m);
        AddGeometryDataToPython(m);
        AddGlobalPointersToPython(m);
        AddGlobalsToPython(m);
        AddIOToPython(m);
        AddKernelToPython(m);        
        AddLinearSolversToPython(m);
        AddLoggerToPython(m);
        AddMatrixMarketInterfaceToPython(m);
        AddMatrixToPython(m);
        AddMeshToPython(m);
        AddModelPartToPython(m);
        AddModelToPython(m);
        AddNodeToPython(m);
        AddParallelEnvironmentToPython(m);
        AddPointsToPython(m);
        AddProcessInfoToPython(m);
        AddPropertiesToPython(m);
        AddQuaternionToPython(m);
        AddQuestApplicationToPython(m);
        AddQuestParametersToPython(m);
        AddRegistryToPython(m);
        AddSerializerToPython(m);
        AddSparseMatricesToPython(m);
        AddStrategiesToPython(m);
        AddTableToPython(m);
    }

}