cmake_minimum_required(VERSION 3.20)
project(QuestCAE)
set(CMAKE_CXX_STANDARD 17)  
set(CMAKE_CXX_STANDARD_REQUIRED ON)  
set(CMAKE_CXX_EXTENSIONS OFF) 


#定义用户自定义编译选项
SET( CMAKE_CONFIGURATION_TYPES Release RelWithDebInfo Debug FullDebug Custom)
SET( BASIC_DEBUG_FLAGS "${CMAKE_CXX_FLAGS_DEBUG}")
SET( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")  
SET( CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")  
SET( CMAKE_CXX_FLAGS_DEBUG "${BASIC_DEBUG_FLAGS} -DNDEBUG -DQUEST_DEBUG")
SET( CMAKE_CXX_FLAGS_FULLDEBUG "${BASIC_DEBUG_FLAGS} -D_DEBUG -DQUEST_DEBUG")
SET( CMAKE_CXX_FLAGS_CUSTOM "${CMAKE_CXX_FLAGS_CUSTOM}")

#定义必须的内部CMake标志


#-----------------------------------------
# 选择共享内存并行化选项
#-----------------------------------------


#-----------------------------------------
# openMP默认选项
#-----------------------------------------
if(NOT DEFINED QUEST_SHARED_MEMORY_PARALLELIZATION)
    message(STATUS "\"QUEST_SHARED_MEMORY_PARALLELIZATION\" not defined, defaulting to \"OpenMP\"") 
    SET(QUEST_SHARED_MEMORY_PARALLELIZATION "OpenMP")
endif(NOT DEFINED QUEST_SHARED_MEMORY_PARALLELIZATION)

if (QUEST_SHARED_MEMORY_PARALLELIZATION STREQUAL "OpenMP")
    find_package(OpenMP)
    if(OPENMP_FOUND)
        message(STATUS "Using OpenMP for shared memory parallelization")
        add_compile_definitions(QUEST_SMP_OPENMP)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
    else(OPENMP_FOUND)
        message(FATAL_ERROR "OpenMP could not be found!")
        add_compile_definitions(QUEST_SMP_NONE)
    endif (OPENMP_FOUND)

elseif (QUEST_SHARED_MEMORY_PARALLELIZARION STREQUAL "C++11")
    message(STATUS "Using C++11 for shared memory parallelization")
    add_compile_definitions(QUEST_SMP_CXX11)
    message(WARNING "Using C++11 for shared memory parallelization is highly experimental and not fully supported!")

elseif (QUEST_SHARED_MEMORY_PARALLELIZARION STREQUAL "None")
    add_compile_definitions(QUEST_SMP_NONE)
    message(STATUS "Shared memory parallelization is disabled")

else()
    message(FATAL_ERROR "Invalid value for \"QUEST_SHARED_MEMORY_PARALLELIZATION\"! Avaiable options are \"OpenMP\", \"C++11\", or \"None\"")
endif()

OPTION(QUEST_NO_TRY_CATCH "Disabled the use of try-catch through the code" OFF)
if(QUEST_NO_TRY_CATCH MATCHES ON)
    add_compile_definitions(QUEST_NO_TRY_CATCH)
endif(QUEST_NO_TRY_CATCH MATCHES ON)