/*------------------------------
string_utilities.hpp类的实现代码
------------------------------*/

// 系统头文件
#include <algorithm>
#include <iostream>
#include <cctype>
#include <sstream>

// 项目头文件
#include "utilities/string_utilities.hpp"

namespace Quest::StringUtilities{

    std::string ConvertCamelCaseToSnakeCase(const std::string& rString){
        std::string str(1, tolower(rString[0]));

        for(auto it = rString.begin() + 1; it!= rString.end(); ++it){
            if(std::isupper(*it) && *(it - 1)!= '_' && std::islower(*(it - 1))){
                str += '_';
            }
            str += *it;
        }

        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        return str;
    }


    std::string ConvertSnakeCaseToCamelCase(const std::string& rString){
        std::string output;

        if(!rString.empty()){
            output.reserve(rString.size());
            bool upper_switch = rString[0] == '_' ? false : true;

            for(auto it : rString){
                QUEST_ERROR_IF(!(std::isalnum(it) || it == '_') || std::isupper(it))
                    << "Invalid character '" << it << "' in snake case string '" << rString << "\'";

                if(it == '_'){
                    QUEST_ERROR_IF(upper_switch) << "Repeated underscores in snake case string '" << rString << "\'";
                    upper_switch = true;
                } else {
                    if(upper_switch){
                        output.push_back(std::toupper(it));
                    } else {
                        output.push_back(it);
                    }
                }
            }
        }
        return output;
    }


    std::string EraseParitialString(const std::string& rMainString, const std::string& rToErase){
        std::string ouput = rMainString;

        std::size_t pos = ouput.find(rToErase);
        if(pos!= std::string::npos){
            ouput.erase(pos, rToErase.size());
        }

        return ouput;
    }


    bool ContainsPartialString(const std::string& rMainString, const std::string& rToCheck){
        const std::string& output = rMainString;

        std::size_t pos = output.find(rToCheck);
        if(pos != std::string::npos){
            return true;
        }

        return false;
    }


    std::string RemoveWhitespace(const std::string& rString){
        std::string output;

        for(auto it : rString){
            if(!std::isspace(it)){
                output += it;
            }
        }
    }


    std::vector<std::string> SplitStringByDelimiter(const std::string& rString, const char Delimiter){
        std::istringstream iss(rString);
        std::string token;

        std::vector<std::string> output;
        while(std::getline(iss, token, Delimiter)){
            output.push_back(token);
        }

        return output;
    }


    std::string ReplaceAllSubstrings(const std::string& rInputString, const std::string& rStringToBeReplaced, const std::string& rStringToReplace){
        std::string output(rInputString);

        std::size_t pos = 0;
        while((pos = output.find(rStringToBeReplaced, pos))!= std::string::npos){
            output.replace(pos, rStringToBeReplaced.size(), rStringToReplace);
            pos += rStringToReplace.length();
        }

        return output;
    }


    std::string Trim(const std::string& rInputString, const bool RemoveNullchar){
        return TrimLeft(TrimRight(rInputString, RemoveNullchar), RemoveNullchar);
    }

    std::function<bool(std::string::value_type)> TrimChar(const bool RemoveNullChar){
        if (RemoveNullChar){
            return [] (auto character){
                return std::isspace(character) || character == '\0';
            };
        }

        return [] (auto character){
            return std::isspace(character);
        };
    }


    std::string TrimLeft(const std::string& rInputString, const bool RemoveNullchar){
        std::string output(rInputString);
        const auto trim_char = TrimChar(RemoveNullchar);

        output.erase(output.begin(), std::find_if(output.begin(),output.end(),[trim_char](std::string::value_type ch){return !trim_char(ch);}));
        return output;
    }


    std::string TrimRight(const std::string& rInputString, const bool RemoveNullchar){
        std::string output(rInputString);
        const auto trim_char = TrimChar(RemoveNullchar);

        output.erase(std::find_if(output.rbegin(),output.rend(),[trim_char](std::string::value_type ch){return !trim_char(ch);}).base(), output.end());
        return output;
    }

} // namespace Quest::StringUtilities