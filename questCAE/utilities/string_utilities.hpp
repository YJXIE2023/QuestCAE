/*--------------------------------
字符串操作相关的工具函数
--------------------------------*/

#ifndef QUEST_STRING_UTILITIES_HPP
#define QUEST_STRING_UTILITIES_HPP

// 系统头文件
#include <string>
#include <vector>

// 项目头文件
#include "includes/define.hpp"

namespace Quest{

    namespace StringUtilities{

        // 将给定的字符串 rString 从 CamelCase 格式转换为 snake_case 格式，并返回转换后的字符串
        [[nodiscard]] std::string QUEST_API(QUEST_CORE) ConvertCamelCaseToSnakeCase(const std::string& rString);

        // 将给定的字符串 rString 从 snake_case 格式转换为 CamelCase 格式，并返回转换后的字符串
        [[nodiscard]] std::string QUEST_API(QUEST_CORE) ConvertSnakeCaseToCamelCase(const std::string& rString);

        // 删除给定字符串中的第一个出现的指定子字符串，并返回删除后的结果
        [[nodiscard]] std::string QUEST_API(QUEST_CORE) EraseParitialString(const std::string& rMainString, const std::string& rToErase);

        // 检查主字符串中是否包含指定的子字符串，并返回一个布尔值来表示结果
        [[nodiscard]] bool QUEST_API(QUEST_CORE) ContainsPartialString(const std::string& rMainString, const std::string& rToCheck);

        // 移除字符串中的所有空白字符，并返回处理后的字符串
        [[nodiscard]] std::string QUEST_API(QUEST_CORE) RemoveWhitespace(const std::string& rString);

        // 根据指定的分隔符将字符串拆分为多个子字符串，并返回一个包含这些子字符串的向量
        [[nodiscard]] std::vector<std::string> QUEST_API(QUEST_CORE) SplitStringByDelimiter(const std::string& rString, const char Delimiter);

        // 在输入字符串中替换所有出现的子字符串，并返回更新后的字符串
        [[nodiscard]] std::string QUEST_API(QUEST_CORE) ReplaceAllSubstrings(const std::string& rInputString, const std::string& rStringToBeReplaced, const std::string& rStringToReplace);

        // 去除字符串两端的空白字符（如空格、制表符等）
        [[nodiscard]] std::string QUEST_API(QUEST_CORE) Trim(const std::string& rInputString,const bool RemoveNullChar = false);

        // 去除字符串左端的空白字符（如空格、制表符等）
        [[nodiscard]] std::string QUEST_API(QUEST_CORE) TrimLeft(const std::string& rInputString,const bool RemoveNullChar = false);

        // 去除字符串右端的空白字符（如空格、制表符等）
        [[nodiscard]] std::string QUEST_API(QUEST_CORE) TrimRight(const std::string& rInputString,const bool RemoveNullChar = false);

        // 将对象 rThisClass 的数据以格式化的方式输出到给定的输出流 rOStream 中，带有指定的缩进
        template<typename TClass>
        static void PrintDataWithIndentation(std::ostream& rOstream, const TClass& rThisClass, const std::string Identation = "\t"){
            std::stringstream buffer;
            std::string line;
            rThisClass.PrintData(buffer);

            const std::string& r_output = buffer.str();

            std::istringstream iss(r_output);
            while (std::getline(iss, line)) {
                rOstream << Identation << line << "\n";
            }
        }
    }

} // namespace Quest

#endif // STRING_UTILITIES_HPP