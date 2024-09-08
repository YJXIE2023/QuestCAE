/*-----------------------------------------------
CodeLocation类保存了代码的位置，包括文件名、函数名和行号
--------------------------------------------------*/

#ifndef QUEST_CODE_LOCATION_HPP
#define QUEST_CODE_LOCATION_HPP

//系统头文件
#include <string>
#include <iostream>

//自定义头文件
#include "includes/quest_export_api.hpp"


namespace Quest{

    class QUEST_API CodeLocation
    {
    public:

        CodeLocation();

        CodeLocation(CodeLocation&& rOther) noexcept = default;

        CodeLocation(const CodeLocation& rOther) noexcept = default;

        CodeLocation(const std::string& rFileName, const std::string& rFunctionName, std::size_t lineNumber);

        CodeLocation& operator=(const CodeLocation& rOther) {
            mFileName = rOther.mFileName;
            mFunctionName = rOther.mFunctionName;
            mLineNumber = rOther.mLineNumber;
            
            return *this;
        }

        std::string CleanFileName() const;

        std::string CleanFunctionName() const;

        const std::string& GetFileName() const;

        const std::string& GetFunctionName() const;

        std::size_t GetLineNumber() const;

    private:
        std::string mFileName;
        std::string mFunctionName;
        std::size_t mLineNumber;

        static void RemoveNamespace(std::string& rFunctionName, const std::string& rNamesapce);

        static void ReduceTemplateArgumentsToFirstN(std::string& rFunctionName, const std::string& rTemplateName, std::size_t UnmberOfArgumentsToKeep);

        static std::size_t GetNextPositionSkippingWhiteSpaces(const std::string& rThisString, std::size_t Position);

        static void ReplaceAll(std::string& ThisString, const std::string& rFromeSstring, const std::string& rToString);
    };

    std::ostream& operator<<(std::ostream& rOstream, const CodeLocation& rCodeLocation);

// 宏定义
#if defined(QUEST_CODE_LOCATION)
#undef QUEST_CODE_LOCATION
#endif

#if defined(QUEST_CURRENT_FUNCTION)
#undef QUEST_CURRENT_FUNCTION
#endif

#if defined(__PRETTY_FUNCTION__)
#define QUEST_CURRENT_FUNCTION __PRETTY_FUNCTION__
#elif defined(__GNUC__) 
#define QUEST_CURRENT_FUNCTION __PRETTY_FUNCTION__
#elif defined(__FUNCTION__)
#define QUEST_CURRENT_FUNCTION __FUNCTION__
#elif defiend(__func__) 
#define QUEST_CURRENT_FUNCTION __func__
#else
#define QUEST_CURRENT_FUNCTION "unknown function"
#endif

#define QUEST_CODE_LOCATION Quest::CodeLocation(__FILE__, QUEST_CURRENT_FUNCTION, __LINE__)


};


#endif //QUEST_CODE_LOCATION_HPP