# Details

Date : 2024-10-09 22:08:59

Directory /home/xie/code/QuestCAE/questCAE

Total : 71 files,  8743 codes, 498 comments, 2999 blanks, all 12240 lines

[Summary](results.md) / Details / [Diff Summary](diff.md) / [Diff Details](diff-details.md)

## Files
| filename | language | code | comment | blank | total |
| :--- | :--- | ---: | ---: | ---: | ---: |
| [questCAE/CMakeLists.txt](/questCAE/CMakeLists.txt) | CMake | 3 | 0 | 1 | 4 |
| [questCAE/IO/logger.cpp](/questCAE/IO/logger.cpp) | C++ | 64 | 5 | 24 | 93 |
| [questCAE/IO/logger.hpp](/questCAE/IO/logger.hpp) | C++ | 122 | 6 | 43 | 171 |
| [questCAE/IO/logger_message.cpp](/questCAE/IO/logger_message.cpp) | C++ | 53 | 3 | 17 | 73 |
| [questCAE/IO/logger_message.hpp](/questCAE/IO/logger_message.hpp) | C++ | 168 | 5 | 64 | 237 |
| [questCAE/IO/logger_output.cpp](/questCAE/IO/logger_output.cpp) | C++ | 99 | 5 | 27 | 131 |
| [questCAE/IO/logger_output.hpp](/questCAE/IO/logger_output.hpp) | C++ | 93 | 5 | 37 | 135 |
| [questCAE/container/array_1d.hpp](/questCAE/container/array_1d.hpp) | C++ | 411 | 12 | 84 | 507 |
| [questCAE/container/data_value_container.cpp](/questCAE/container/data_value_container.cpp) | C++ | 58 | 4 | 14 | 76 |
| [questCAE/container/data_value_container.hpp](/questCAE/container/data_value_container.hpp) | C++ | 172 | 5 | 82 | 259 |
| [questCAE/container/flags.cpp](/questCAE/container/flags.cpp) | C++ | 59 | 3 | 18 | 80 |
| [questCAE/container/flags.hpp](/questCAE/container/flags.hpp) | C++ | 175 | 6 | 62 | 243 |
| [questCAE/container/global_pointers_vector.hpp](/questCAE/container/global_pointers_vector.hpp) | C++ | 7 | 5 | 5 | 17 |
| [questCAE/container/nodal_data.cpp](/questCAE/container/nodal_data.cpp) | C++ | 35 | 4 | 14 | 53 |
| [questCAE/container/nodal_data.hpp](/questCAE/container/nodal_data.hpp) | C++ | 58 | 5 | 33 | 96 |
| [questCAE/container/variable.hpp](/questCAE/container/variable.hpp) | C++ | 167 | 5 | 50 | 222 |
| [questCAE/container/variable_data.cpp](/questCAE/container/variable_data.cpp) | C++ | 67 | 4 | 26 | 97 |
| [questCAE/container/variable_data.hpp](/questCAE/container/variable_data.hpp) | C++ | 97 | 21 | 53 | 171 |
| [questCAE/container/variables_list.cpp](/questCAE/container/variables_list.cpp) | C++ | 44 | 4 | 6 | 54 |
| [questCAE/container/variables_list.hpp](/questCAE/container/variables_list.hpp) | C++ | 326 | 6 | 104 | 436 |
| [questCAE/container/variables_list_data_value_container.hpp](/questCAE/container/variables_list_data_value_container.hpp) | C++ | 561 | 5 | 142 | 708 |
| [questCAE/container/weak_pointer_vector.hpp](/questCAE/container/weak_pointer_vector.hpp) | C++ | 202 | 6 | 67 | 275 |
| [questCAE/container/weak_pointer_vector_iterator.hpp](/questCAE/container/weak_pointer_vector_iterator.hpp) | C++ | 24 | 6 | 15 | 45 |
| [questCAE/includes/check.hpp](/questCAE/includes/check.hpp) | C++ | 185 | 6 | 47 | 238 |
| [questCAE/includes/code_location.hpp](/questCAE/includes/code_location.hpp) | C++ | 54 | 6 | 31 | 91 |
| [questCAE/includes/data_communicator.hpp](/questCAE/includes/data_communicator.hpp) | C++ | 496 | 53 | 117 | 666 |
| [questCAE/includes/define.hpp](/questCAE/includes/define.hpp) | C++ | 69 | 14 | 23 | 106 |
| [questCAE/includes/dof.hpp](/questCAE/includes/dof.hpp) | C++ | 286 | 6 | 76 | 368 |
| [questCAE/includes/exceptions.hpp](/questCAE/includes/exceptions.hpp) | C++ | 61 | 7 | 36 | 104 |
| [questCAE/includes/fill_communicator.hpp](/questCAE/includes/fill_communicator.hpp) | C++ | 49 | 4 | 28 | 81 |
| [questCAE/includes/fnv_1a_hash.hpp](/questCAE/includes/fnv_1a_hash.hpp) | C++ | 44 | 5 | 24 | 73 |
| [questCAE/includes/global_pointer.hpp](/questCAE/includes/global_pointer.hpp) | C++ | 9 | 5 | 7 | 21 |
| [questCAE/includes/global_variables.hpp](/questCAE/includes/global_variables.hpp) | C++ | 19 | 3 | 6 | 28 |
| [questCAE/includes/key_hash.hpp](/questCAE/includes/key_hash.hpp) | C++ | 158 | 33 | 37 | 228 |
| [questCAE/includes/lock_object.hpp](/questCAE/includes/lock_object.hpp) | C++ | 54 | 6 | 15 | 75 |
| [questCAE/includes/model_part.hpp](/questCAE/includes/model_part.hpp) | C++ | 0 | 0 | 1 | 1 |
| [questCAE/includes/mpi_serializer.hpp](/questCAE/includes/mpi_serializer.hpp) | C++ | 27 | 5 | 12 | 44 |
| [questCAE/includes/parallel_enviroment.hpp](/questCAE/includes/parallel_enviroment.hpp) | C++ | 88 | 7 | 69 | 164 |
| [questCAE/includes/process_info.hpp](/questCAE/includes/process_info.hpp) | C++ | 135 | 5 | 90 | 230 |
| [questCAE/includes/quest_components.hpp](/questCAE/includes/quest_components.hpp) | C++ | 168 | 5 | 53 | 226 |
| [questCAE/includes/quest_export_api.hpp](/questCAE/includes/quest_export_api.hpp) | C++ | 32 | 3 | 8 | 43 |
| [questCAE/includes/quest_filesystem.hpp](/questCAE/includes/quest_filesystem.hpp) | C++ | 20 | 9 | 17 | 46 |
| [questCAE/includes/quest_parameters.hpp](/questCAE/includes/quest_parameters.hpp) | C++ | 197 | 18 | 154 | 369 |
| [questCAE/includes/quset.hpp](/questCAE/includes/quset.hpp) | C++ | 6 | 6 | 10 | 22 |
| [questCAE/includes/registry.hpp](/questCAE/includes/registry.hpp) | C++ | 91 | 5 | 42 | 138 |
| [questCAE/includes/registry_item.hpp](/questCAE/includes/registry_item.hpp) | C++ | 157 | 6 | 73 | 236 |
| [questCAE/includes/serializer.hpp](/questCAE/includes/serializer.hpp) | C++ | 767 | 6 | 216 | 989 |
| [questCAE/includes/smart_pointers.hpp](/questCAE/includes/smart_pointers.hpp) | C++ | 54 | 5 | 19 | 78 |
| [questCAE/includes/stream_serializer.hpp](/questCAE/includes/stream_serializer.hpp) | C++ | 29 | 5 | 18 | 52 |
| [questCAE/includes/ublas_complex_interface.hpp](/questCAE/includes/ublas_complex_interface.hpp) | C++ | 51 | 6 | 12 | 69 |
| [questCAE/includes/ublas_interface.hpp](/questCAE/includes/ublas_interface.hpp) | C++ | 54 | 6 | 16 | 76 |
| [questCAE/includes/variables.hpp](/questCAE/includes/variables.hpp) | C++ | 10 | 5 | 6 | 21 |
| [questCAE/sources/code_location.cpp](/questCAE/sources/code_location.cpp) | C++ | 103 | 4 | 18 | 125 |
| [questCAE/sources/exceptions.cpp](/questCAE/sources/exceptions.cpp) | C++ | 70 | 5 | 24 | 99 |
| [questCAE/sources/fill_communicator.cpp](/questCAE/sources/fill_communicator.cpp) | C++ | 39 | 4 | 19 | 62 |
| [questCAE/sources/key_hash.cpp](/questCAE/sources/key_hash.cpp) | C++ | 16 | 4 | 9 | 29 |
| [questCAE/sources/process_info.cpp](/questCAE/sources/process_info.cpp) | C++ | 0 | 0 | 1 | 1 |
| [questCAE/sources/queat_parameters.cpp](/questCAE/sources/queat_parameters.cpp) | C++ | 833 | 6 | 301 | 1,140 |
| [questCAE/sources/quest_components.cpp](/questCAE/sources/quest_components.cpp) | C++ | 0 | 4 | 2 | 6 |
| [questCAE/sources/quest_filesystem.cpp](/questCAE/sources/quest_filesystem.cpp) | C++ | 35 | 5 | 11 | 51 |
| [questCAE/sources/registry.cpp](/questCAE/sources/registry.cpp) | C++ | 86 | 5 | 33 | 124 |
| [questCAE/sources/registry_item.cpp](/questCAE/sources/registry_item.cpp) | C++ | 111 | 4 | 30 | 145 |
| [questCAE/utilities/color_utilities.hpp](/questCAE/utilities/color_utilities.hpp) | C++ | 48 | 3 | 7 | 58 |
| [questCAE/utilities/counter.hpp](/questCAE/utilities/counter.hpp) | C++ | 47 | 6 | 22 | 75 |
| [questCAE/utilities/openmp_utils.hpp](/questCAE/utilities/openmp_utils.hpp) | C++ | 75 | 6 | 18 | 99 |
| [questCAE/utilities/parallel_utilities.cpp](/questCAE/utilities/parallel_utilities.cpp) | C++ | 89 | 5 | 28 | 122 |
| [questCAE/utilities/parallel_utilities.hpp](/questCAE/utilities/parallel_utilities.hpp) | C++ | 308 | 15 | 80 | 403 |
| [questCAE/utilities/quaternion.hpp](/questCAE/utilities/quaternion.hpp) | C++ | 293 | 16 | 74 | 383 |
| [questCAE/utilities/stl_vector_io.hpp](/questCAE/utilities/stl_vector_io.hpp) | C++ | 17 | 5 | 10 | 32 |
| [questCAE/utilities/string_utilities.cpp](/questCAE/utilities/string_utilities.cpp) | C++ | 107 | 5 | 41 | 153 |
| [questCAE/utilities/string_utilities.hpp](/questCAE/utilities/string_utilities.hpp) | C++ | 31 | 16 | 20 | 67 |

[Summary](results.md) / Details / [Diff Summary](diff.md) / [Diff Details](diff-details.md)