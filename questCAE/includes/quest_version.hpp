// 系统头文件
#include <string>

// 项目头文件
#include "includes/quest_export_api.hpp"

namespace Quest{

    // 主版本号宏
    #ifndef QUEST_MAJOR_VERSION
    #define QUEST_MAJOR_VERSION 9
    #endif

    // 次版本号宏
    #ifndef QUEST_MINOR_VERSION
    #define QUEST_MINOR_VERSION 0
    #endif

    // 判断版本号是否一致的底层宏
    #define QUEST_VERSION_EQ(MAJOR,MINOR) \
    ((QUEST_MAJOR_VERSION == (MAJOR)) && (QUEST_MINOR_VERSION == (MINOR)))

    #define QUEST_VERSION_ QUEST_VERSION_EQ

    // 判断版本号是否小于某个版本的底层宏
    #define QUEST_VERSION_LT(MAJOR,MINOR)                                  \
    (QUEST_MAJOR_VERSION < (MAJOR) || (QUEST_MAJOR_VERSION == (MAJOR) && (QUEST_MINOR_VERSION < (MINOR) )))

    // 判断版本号是否大于某个版本的底层宏
    #define QUEST_VERSION_LE(MAJOR,MINOR) \
        (QUEST_VERSION_LT(MAJOR,MINOR) || QUEST_VERSION_EQ(MAJOR,MINOR))

    // 判断版本是否大于某个版本的宏
    #define QUEST_VERSION_GT(MAJOR,MINOR) (0 == QUEST_VERSION_LE(MAJOR,MINOR))

    // 判断版本是否小于某个版本的宏
    #define QUEST_VERSION_GE(MAJOR,MINOR) (0 == QUEST_VERSION_LT(MAJOR,MINOR))

    // 获取主版本号
    constexpr int GetMajorVersion() {
        return QUEST_MAJOR_VERSION;
    }

    // 获取次版本号
    constexpr int GetMinorVersion() {
        return QUEST_MINOR_VERSION;
    }


    QUEST_API_EXPORT std::string GetPatchVersion();  // 获取补丁版本号
    QUEST_API_EXPORT std::string GetCommit();        // 获取提交版本号
    QUEST_API_EXPORT std::string GetBranchName();    // 获取分支名称
    QUEST_API_EXPORT std::string GetBuildType();
    QUEST_API_EXPORT std::string GetVersionString();
    QUEST_API_EXPORT std::string GetOSName();
    QUEST_API_EXPORT std::string GetCompiler();

}