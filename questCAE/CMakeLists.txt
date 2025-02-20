set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${CMAKE_SOURCE_DIR}/exLibraries)


# 将当前目录下所有文件夹名称加入到变量中
SET(ListFolders container factories geometries includes integration linear_solvers molder processes solving_strategies sources space utilities)


# Quest核心中排除的源代码
SET(LIST_OF_SOURCE_EXCEPTIONS ${CMAKE_CURRENT_SOURCE_DIR}/sources/quest_version.cpp)


# 变量所有文件夹，处理源代码文件（去除需排除的源代码文件）
FOREACH(subFolder ${ListFolders})
    # 遍历所有源代码文件
    FILE(GLOB_RECURSE ${subFolder}Sources "${subFolder}/*.cpp" "${subFolder}/*.c")

    # 辅助文件
    SET(AUXILIAR_CORE_SOURCES "")
    FOREACH(file ${${subFolder}Sources})
        SET(EXCEPTION_CEHCK 0)
        FOREACH(exception ${LIST_OF_SOURCE_EXCEPTIONS})
            IF(${file} STREQUAL ${exception})
                SET(EXCEPTION_CEHCK 1)
                BREAK()
            ENDIF(${file} STREQUAL ${exception})
        ENDFOREACH(exception ${LIST_OF_SOURCE_EXCEPTIONS})
        IF(${EXCEPTION_CEHCK} EQUAL 0)
            LIST(APPEND AUXILIAR_CORE_SOURCES ${file})
        ENDIF(${EXCEPTION_CEHCK} EQUAL 0)
    ENDFOREACH(file ${${subFolder}Sources})

    # 将辅助文件中的源代码文件加入到最终文件列表中
    LIST(APPEND QUEST_CORE_SOURCES ${AUXILIAR_CORE_SOURCES})
ENDFOREACH(subFolder ${ListFolders})


# 将python接口文件放入同一文件中
FILE(GLOB_RECURSE QUEST_PYTHON_SOURCES "python/*.cpp")


# 版本源文件
SET(QUEST_VERSION_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/sources/quest_version.cpp)


# 将所有IO源代码放入同一文件夹中
FILE(GLOB_RECURSE QUEST_CORE_INPUT_OUTPUT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/IO/*.cpp)


# 将版本源文件编译为库文件
add_library(QuestVersion OBJECT ${QUEST_VERSION_SOURCES})
# 编译过程中创建QUEST_VERSION宏
set_target_properties(QuestVersion PROPERTIES COMPILE_DEFINITIONS "QUEST_VERSION=IMPORT,API")
# 向QuestVersion库中添加宏定义
target_compile_definitions(QuestVersion PRIVATE
    QUEST_MAJOR_VERSION=${QuestMultiphysics_MAJOR_VERSION}
    QUEST_MINOR_VERSION=${QuestMultiphysics_MINOR_VERSION}
    QUEST_PATCH_VERSION="${QuestMultiphysics_PATCH_VERSION}"
    QUEST_SHA1_NUMBER="${QuestMultiphysics_SHA1_NUMBER}"
    QUEST_BRANCH_NAME="${QuestMultiphysics_BRANCH_NAME}"
    QUEST_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
)


# 将除VERSION以外的源代码编译为QuestCore库文件，将 KratosVersion 目标中的对象文件添加到 KratosCore 目标
add_library(QuestCore SHARED ${QUEST_CORE_SOURCES} ${QUEST_CORE_INPUT_OUTPUT_SOURCES} $<TARGET_OBJECTS:QuestVersion>)


# 针对旧版的GCC编译器，显示链接std::filesystem库
if(${CMAKE_COMPILER_IS_GNUCXX})
    if(${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 9.1)
        set(QUEST_EXTERNAL_LIBRARIES ${QUEST_EXTERNAL_LIBRARIES} -lstdc++fs)
    endif()
endif()


# 链接额外的动态库文件
target_link_libraries(QuestCore PUBLIC ${QUEST_EXTERNAL_LIBRARIES})
set_target_properties(QuestCore PROPERTIES COMPILE_DEFINITIONS "QUEST_CORE=IMPORT,API")


# 处理统一构建（通过将多个源文件合并为一个单一的翻译单元来减少头文件的重复包含，从而提高编译速度）
if(CMAKE_UNITY_BUILD MATCHES ON)
    set_target_properties(QuestCore PROPERTIES UNITY_BUILD_BATCH_SIZE ${QUEST_UNITY_BUILD_BATCH_SIZE})
endif(CMAKE_UNITY_BUILD MATCHES ON)


# 向QuestCore库中添加宏定义
target_compile_definitions(QuestCore PRIVATE
    QUEST_MAJOR_VERSION=${QuestMultiphysics_MAJOR_VERSION}
    QUEST_MINOR_VERSION=${QuestMultiphysics_MINOR_VERSION}
)


# 创建一个Python模块，并编译其中的 C++ 代码，启用薄 LTO 优化
pybind11_add_module(Quest MODULE THIN_LTO ${QUEST_PYTHON_SOURCES})
target_link_libraries(Quest PRIVATE QuestCore)


# 处理启用预编译头文件（PCH）的情况
# $<$<COMPILE_LANGUAGE:CXX>确保只有在C++袁文文我那件中编译时才会使用预编译头文件
# ${CMAKE_CURRENT_SOURCE_DIR}/includes/model_part.hpp是预编译头文件的文件名
if(QUEST_USE_PCH MATCHES ON)
    target_precompile_headers(QuestCore PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/includes/model_part.hpp>")
endif(QUEST_USE_PCH MATCHES ON)


# 定义Python版本宏
target_compile_definitions(Quest PRIVATE
    PYTHON_VERSION_MAJOR=${PYTHON_VERSION_MAJOR}
    PYTHON_VERSION_MINOR=${PYTHON_VERSION_MINOR}
    PYTHON_VERSION_PATCH=${PYTHON_VERSION_PATCH}
)


# 将特定的源文件排除在统一构建之外（源文件的编译过程与统一构建不兼容或消耗过多内存）
if(CMAKE_UNITY_BUILD MATCHES ON)
    ## 消耗过多内存
    set_source_files_properties (${CMAKE_CURRENT_SOURCE_DIR}/sources/kratos_parameters.cpp PROPERTIES SKIP_UNITY_BUILD_INCLUSION TRUE)

    ## 由于QuestComponents导致冲突
    file(GLOB_RECURSE KRATOS_STRATEGIES_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/solving_strategies/*.cpp)
    FOREACH(exception ${KRATOS_STRATEGIES_SOURCES})
        set_source_files_properties (${exception} PROPERTIES SKIP_UNITY_BUILD_INCLUSION TRUE)
    ENDFOREACH(exception ${KRATOS_STRATEGIES_SOURCES})

    ## 源文件本身占用的内存过多
    set_source_files_properties (${CMAKE_CURRENT_SOURCE_DIR}/python/add_strategies_to_python.cpp PROPERTIES SKIP_UNITY_BUILD_INCLUSION TRUE)
endif(CMAKE_UNITY_BUILD MATCHES ON)


# 将当前源代码目录的 名称（即最后一级文件夹名）提取出来，并存储在 CURRENT_DIR_NAME 变量中
get_filename_component(CURRENT_DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
# 处理 Python 脚本文件的安装过程,
# ${INSTALL_PYTHON_USING_LINKS}控制是否将 Python 脚本文件作为 符号链接安装
# ${CMAKE_CURRENT_SOURCE_DIR}/python_scripts：这是 Python 脚本所在的源代码路径
# QuestMultiphysics/ 表示 Python 脚本在目标目录中的位置
quest_python_install_directory(${INSTALL_PYTHON_USING_LINKS} ${CMAKE_CURRENT_SOURCE_DIR}/python_scripts QuestMultiphysics/) 



# 设置Quest和QuestCore的安装路径
install(TARGET Quest DESTINATION libs)
install(TARGET QuestCore DESTINATION libs)


# 设置MPI相关依赖
if(${USE_MPI} MATCHES ON)
    add_subdirectory(mpi)
endif(${USE_MPI} MATCHES ON)


# 将Quest和QuestCore更新至QUEST_KERNEL和QUEST_PYTHON_INTERFACE变量中
set(QUEST_KERNEL "${QUEST_KERNEL};QuestCore" PARENT_SCOPE)
set(QUEST_PYTHON_INTERFACE "${QUEST_PYTHON_INTERFACE};Quest" PARENT_SCOPE)