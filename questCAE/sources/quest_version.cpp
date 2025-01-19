// 系统头文件
#include <string>

// 项目头文件
#include "includes/quest_version.hpp"

namespace Quest{

    // 补丁版本号
    #ifndef QUEST_PATCH_VERSION
    #define QUEST_PATCH_VERSION "0"
    #endif

    // git 版本号
    #ifndef QUEST_SHA1_NUMBER
    #define QUEST_SHA1_NUMBER "0"
    #endif

    // git 分支名
    #ifndef QUEST_BRANCH_NAME
    #define QUEST_BRANCH_NAME ""
    #endif

    // 构建方式
    #ifndef QUEST_BUILD_TYPE
    #define QUEST_BUILD_TYPE "Release"
    #endif

    // 架构类型
    #if defined(__x86_64__) || defined(_M_X64) || defined(__amd64)
    #define QUEST_ARCH_TYPE "x86_64"
    #elif defined(__i386__) || defined(_M_IX86) || defined(_X86_) || defined(__i386)
    #define QUEST_ARCH_TYPE "x86"
    #elif defined(__arm__)
    #define QUEST_ARCH_TYPE "ARM32"
    #elif defined(__aarch64__)
    #define QUEST_ARCH_TYPE "ARM64"
    #else
    #define QUEST_ARCH_TYPE "Unknown architecture"
    #endif

    // 完整版本号
    #ifndef QUEST_TO_STRING_
        #define QUEST_TO_STRING_(X) #X
    #endif
    #ifndef QUEST_TO_STRING
        #define QUEST_TO_STRING(X) QUEST_TO_STRING_(X)
    #endif
    #define QUEST_VERSION_STRING \
    QUEST_TO_STRING(QUEST_MAJOR_VERSION) "." \
    QUEST_TO_STRING(QUEST_MINOR_VERSION) "." \
    QUEST_TO_STRING(QUEST_PATCH_VERSION) "-" \
    QUEST_BRANCH_NAME "-" \
    QUEST_SHA1_NUMBER "-" \
    QUEST_BUILD_TYPE  "-" \
    QUEST_ARCH_TYPE

    // 操作系统类型
    #if defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
        #define QUEST_OS_NAME "GNU/Linux"
    #elif defined(__APPLE__) && defined(__MACH__)
        #define QUEST_OS_NAME "Mac OS"
    #elif defined(_WIN32) || defined(_WIN64)
        #define QUEST_OS_NAME "Windows"
    #else
        #define QUEST_OS_NAME "Unknown OS"
    #endif

    // 编译器类型
    #if defined(__INTEL_COMPILER) || defined(__INTEL_LLVM_COMPILER)
        #define QUEST_COMPILER_LABEL "Intel-" \
        QUEST_TO_STRING(__VERSION__)
    #elif defined(__clang__)
        #define QUEST_COMPILER_LABEL "Clang-" \
        QUEST_TO_STRING(__clang_major__) \
        "." \
        QUEST_TO_STRING(__clang_minor__)
    #elif defined(__GNUC__) || defined(__GNUG__)
        #define QUEST_COMPILER_LABEL "GCC-" \
        QUEST_TO_STRING(__GNUC__) \
        "." \
        QUEST_TO_STRING(__GNUC_MINOR__)
    #elif defined(_MSC_VER)
        #define QUEST_COMPILER_LABEL "MSVC-" \
        QUEST_TO_STRING(_MSC_VER)
    #else
        #define QUEST_COMPILER_LABEL "Unknown compiler"
    #endif


    std::string GetPatchVersion() {
        return QUEST_PATCH_VERSION;
    }

    std::string GetCommit() {
        return QUEST_SHA1_NUMBER;
    }

    std::string GetBranchName() {
        return QUEST_BRANCH_NAME;
    }

    std::string GetBuildType() {
        return QUEST_BUILD_TYPE;
    }

    std::string GetVersionString() {
        return QUEST_VERSION_STRING;
    }

    std::string GetOSName() {
        return QUEST_OS_NAME;
    }

    std::string GetCompiler() {
        return QUEST_COMPILER_LABEL;
    }

}