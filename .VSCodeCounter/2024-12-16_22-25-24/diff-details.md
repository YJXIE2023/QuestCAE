# Diff Details

Date : 2024-12-16 22:25:24

Directory /home/xie/code/QuestCAE/questCAE

Total : 83 files,  17293 codes, 6251 comments, 6662 blanks, all 30206 lines

[Summary](results.md) / [Details](details.md) / [Diff Summary](diff.md) / Diff Details

## Files
| filename | language | code | comment | blank | total |
| :--- | :--- | ---: | ---: | ---: | ---: |
| [questCAE/container/array_1d.hpp](/questCAE/container/array_1d.hpp) | C++ | 0 | 95 | 12 | 107 |
| [questCAE/container/csr_matrix.hpp](/questCAE/container/csr_matrix.hpp) | C++ | 579 | 6 | 175 | 760 |
| [questCAE/container/data_value_container.hpp](/questCAE/container/data_value_container.hpp) | C++ | 0 | 89 | 3 | 92 |
| [questCAE/container/flags.hpp](/questCAE/container/flags.hpp) | C++ | 1 | 138 | 34 | 173 |
| [questCAE/container/geometry_container.hpp](/questCAE/container/geometry_container.hpp) | C++ | 163 | 5 | 86 | 254 |
| [questCAE/container/global_pointers_vector.hpp](/questCAE/container/global_pointers_vector.hpp) | C++ | 224 | 147 | 68 | 439 |
| [questCAE/container/model.cpp](/questCAE/container/model.cpp) | C++ | 194 | 5 | 63 | 262 |
| [questCAE/container/model.hpp](/questCAE/container/model.hpp) | C++ | 50 | 6 | 56 | 112 |
| [questCAE/container/nodal_data.hpp](/questCAE/container/nodal_data.hpp) | C++ | 0 | 51 | 11 | 62 |
| [questCAE/container/periodic_variable_container.hpp](/questCAE/container/periodic_variable_container.hpp) | C++ | 87 | 7 | 31 | 125 |
| [questCAE/container/pointer_hash_map_set.hpp](/questCAE/container/pointer_hash_map_set.hpp) | C++ | 246 | 5 | 119 | 370 |
| [questCAE/container/pointer_vector.hpp](/questCAE/container/pointer_vector.hpp) | C++ | 227 | 170 | 79 | 476 |
| [questCAE/container/pointer_vector_map.hpp](/questCAE/container/pointer_vector_map.hpp) | C++ | 398 | 6 | 123 | 527 |
| [questCAE/container/pointer_vector_map_iterator.hpp](/questCAE/container/pointer_vector_map_iterator.hpp) | C++ | 33 | 6 | 19 | 58 |
| [questCAE/container/pointer_vector_set.hpp](/questCAE/container/pointer_vector_set.hpp) | C++ | 439 | 211 | 128 | 778 |
| [questCAE/container/set_identity_function.hpp](/questCAE/container/set_identity_function.hpp) | C++ | 18 | 7 | 10 | 35 |
| [questCAE/container/sparse_contiguous_row_graph.hpp](/questCAE/container/sparse_contiguous_row_graph.hpp) | C++ | 299 | 7 | 101 | 407 |
| [questCAE/container/sparse_graph.hpp](/questCAE/container/sparse_graph.hpp) | C++ | 269 | 7 | 102 | 378 |
| [questCAE/container/system_vector.hpp](/questCAE/container/system_vector.hpp) | C++ | 179 | 5 | 80 | 264 |
| [questCAE/container/variable.hpp](/questCAE/container/variable.hpp) | C++ | 2 | 106 | 25 | 133 |
| [questCAE/container/variable_data.hpp](/questCAE/container/variable_data.hpp) | C++ | 0 | 95 | 29 | 124 |
| [questCAE/container/variables_list.hpp](/questCAE/container/variables_list.hpp) | C++ | 0 | 109 | 12 | 121 |
| [questCAE/container/variables_list_data_value_container.hpp](/questCAE/container/variables_list_data_value_container.hpp) | C++ | 0 | 264 | 64 | 328 |
| [questCAE/geometries/geometry.hpp](/questCAE/geometries/geometry.hpp) | C++ | 1,553 | 809 | 431 | 2,793 |
| [questCAE/geometries/geometry_data.hpp](/questCAE/geometries/geometry_data.hpp) | C++ | 220 | 89 | 56 | 365 |
| [questCAE/geometries/geometry_dimension.hpp](/questCAE/geometries/geometry_dimension.hpp) | C++ | 65 | 22 | 30 | 117 |
| [questCAE/geometries/geometry_shape_function_container.hpp](/questCAE/geometries/geometry_shape_function_container.hpp) | C++ | 218 | 151 | 71 | 440 |
| [questCAE/geometries/point.hpp](/questCAE/geometries/point.hpp) | C++ | 113 | 69 | 59 | 241 |
| [questCAE/includes/accessor.hpp](/questCAE/includes/accessor.hpp) | C++ | 110 | 89 | 37 | 236 |
| [questCAE/includes/communicator.hpp](/questCAE/includes/communicator.hpp) | C++ | 172 | 5 | 241 | 418 |
| [questCAE/includes/condition.hpp](/questCAE/includes/condition.hpp) | C++ | 510 | 354 | 109 | 973 |
| [questCAE/includes/constitutive_law.hpp](/questCAE/includes/constitutive_law.hpp) | C++ | 582 | 4 | 305 | 891 |
| [questCAE/includes/define.hpp](/questCAE/includes/define.hpp) | C++ | 391 | 7 | 64 | 462 |
| [questCAE/includes/dof.hpp](/questCAE/includes/dof.hpp) | C++ | 13 | 178 | 45 | 236 |
| [questCAE/includes/element.hpp](/questCAE/includes/element.hpp) | C++ | 526 | 5 | 201 | 732 |
| [questCAE/includes/geometrical_object.hpp](/questCAE/includes/geometrical_object.hpp) | C++ | 153 | 95 | 59 | 307 |
| [questCAE/includes/global_pointer.hpp](/questCAE/includes/global_pointer.hpp) | C++ | 192 | 76 | 38 | 306 |
| [questCAE/includes/indexed_object.hpp](/questCAE/includes/indexed_object.hpp) | C++ | 61 | 25 | 27 | 113 |
| [questCAE/includes/initial_state.hpp](/questCAE/includes/initial_state.hpp) | C++ | 82 | 66 | 45 | 193 |
| [questCAE/includes/key_hash.hpp](/questCAE/includes/key_hash.hpp) | C++ | 0 | 55 | 1 | 56 |
| [questCAE/includes/lock_object.hpp](/questCAE/includes/lock_object.hpp) | C++ | 0 | 27 | 6 | 33 |
| [questCAE/includes/master_slave_constraint.hpp](/questCAE/includes/master_slave_constraint.hpp) | C++ | 216 | 5 | 106 | 327 |
| [questCAE/includes/mesh.hpp](/questCAE/includes/mesh.hpp) | C++ | 510 | 6 | 229 | 745 |
| [questCAE/includes/model_part.hpp](/questCAE/includes/model_part.hpp) | C++ | 901 | 5 | 598 | 1,504 |
| [questCAE/includes/node.hpp](/questCAE/includes/node.hpp) | C++ | 613 | 342 | 173 | 1,128 |
| [questCAE/includes/process_info.hpp](/questCAE/includes/process_info.hpp) | C++ | 0 | 137 | -31 | 106 |
| [questCAE/includes/properties.hpp](/questCAE/includes/properties.hpp) | C++ | 369 | 190 | 86 | 645 |
| [questCAE/includes/quest_components.hpp](/questCAE/includes/quest_components.hpp) | C++ | 0 | 30 | 7 | 37 |
| [questCAE/includes/quest_export_api.hpp](/questCAE/includes/quest_export_api.hpp) | C++ | 5 | 0 | 3 | 8 |
| [questCAE/includes/quest_filesystem.hpp](/questCAE/includes/quest_filesystem.hpp) | C++ | 0 | 13 | 3 | 16 |
| [questCAE/includes/quest_flags.hpp](/questCAE/includes/quest_flags.hpp) | C++ | 41 | 4 | 7 | 52 |
| [questCAE/includes/quest_parameters.hpp](/questCAE/includes/quest_parameters.hpp) | C++ | 1 | 330 | 0 | 331 |
| [questCAE/includes/registry.hpp](/questCAE/includes/registry.hpp) | C++ | 0 | 58 | 15 | 73 |
| [questCAE/includes/registry_item.hpp](/questCAE/includes/registry_item.hpp) | C++ | 0 | 117 | 33 | 150 |
| [questCAE/includes/serializer.hpp](/questCAE/includes/serializer.hpp) | C++ | 5 | 0 | 1 | 6 |
| [questCAE/includes/table.hpp](/questCAE/includes/table.hpp) | C++ | 408 | 82 | 172 | 662 |
| [questCAE/includes/table_stream.hpp](/questCAE/includes/table_stream.hpp) | C++ | 188 | 5 | 50 | 243 |
| [questCAE/includes/variables.hpp](/questCAE/includes/variables.hpp) | C++ | 12 | 337 | 96 | 445 |
| [questCAE/integration/integration_info.cpp](/questCAE/integration/integration_info.cpp) | C++ | 94 | 4 | 21 | 119 |
| [questCAE/integration/integration_info.hpp](/questCAE/integration/integration_info.hpp) | C++ | 113 | 85 | 42 | 240 |
| [questCAE/integration/integration_point.hpp](/questCAE/integration/integration_point.hpp) | C++ | 113 | 112 | 70 | 295 |
| [questCAE/integration/quadrature.hpp](/questCAE/integration/quadrature.hpp) | C++ | 96 | 46 | 47 | 189 |
| [questCAE/integration/quadrilateral_gauss_legendre_integration_points.hpp](/questCAE/integration/quadrilateral_gauss_legendre_integration_points.hpp) | C++ | 142 | 21 | 55 | 218 |
| [questCAE/linear_solvers/direct_solver.hpp](/questCAE/linear_solvers/direct_solver.hpp) | C++ | 41 | 24 | 24 | 89 |
| [questCAE/linear_solvers/iterative_solver.hpp](/questCAE/linear_solvers/iterative_solver.hpp) | C++ | 8 | 2 | 4 | 14 |
| [questCAE/linear_solvers/linear_solver.hpp](/questCAE/linear_solvers/linear_solver.hpp) | C++ | 128 | 117 | 71 | 316 |
| [questCAE/linear_solvers/preconditioner/preconditioner.hpp](/questCAE/linear_solvers/preconditioner/preconditioner.hpp) | C++ | 61 | 53 | 41 | 155 |
| [questCAE/linear_solvers/reorderer/reorderer.hpp](/questCAE/linear_solvers/reorderer/reorderer.hpp) | C++ | 62 | 40 | 41 | 143 |
| [questCAE/sources/accessor.cpp](/questCAE/sources/accessor.cpp) | C++ | 106 | 4 | 23 | 133 |
| [questCAE/sources/communicator.cpp](/questCAE/sources/communicator.cpp) | C++ | 402 | 4 | 232 | 638 |
| [questCAE/sources/constitutive_law.cpp](/questCAE/sources/constitutive_law.cpp) | C++ | 844 | 4 | 240 | 1,088 |
| [questCAE/sources/initial_state.cpp](/questCAE/sources/initial_state.cpp) | C++ | 102 | 4 | 38 | 144 |
| [questCAE/sources/master_slave_constraint.cpp](/questCAE/sources/master_slave_constraint.cpp) | C++ | 7 | 4 | 4 | 15 |
| [questCAE/sources/model_part.cpp](/questCAE/sources/model_part.cpp) | C++ | 1,652 | 5 | 474 | 2,131 |
| [questCAE/sources/process_info.cpp](/questCAE/sources/process_info.cpp) | C++ | 152 | 4 | 36 | 192 |
| [questCAE/utilities/adjoint_extensions.hpp](/questCAE/utilities/adjoint_extensions.hpp) | C++ | 30 | 7 | 23 | 60 |
| [questCAE/utilities/atomic_utilities.hpp](/questCAE/utilities/atomic_utilities.hpp) | C++ | 149 | 5 | 49 | 203 |
| [questCAE/utilities/indirect_scalar_fwd.hpp](/questCAE/utilities/indirect_scalar_fwd.hpp) | C++ | 8 | 5 | 7 | 20 |
| [questCAE/utilities/math_utils.cpp](/questCAE/utilities/math_utils.cpp) | C++ | 18 | 0 | 5 | 23 |
| [questCAE/utilities/math_utils.hpp](/questCAE/utilities/math_utils.hpp) | C++ | 1,038 | 357 | 205 | 1,600 |
| [questCAE/utilities/parallel_utilities.hpp](/questCAE/utilities/parallel_utilities.hpp) | C++ | 24 | 0 | 0 | 24 |
| [questCAE/utilities/reduction_utilities.hpp](/questCAE/utilities/reduction_utilities.hpp) | C++ | 214 | 5 | 83 | 302 |
| [questCAE/utilities/table_stream_utility.hpp](/questCAE/utilities/table_stream_utility.hpp) | C++ | 51 | 5 | 24 | 80 |

[Summary](results.md) / [Details](details.md) / [Diff Summary](diff.md) / Diff Details