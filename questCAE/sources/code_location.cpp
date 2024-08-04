/*-------------------------------------------------------
code_location.hpp类实现代码
---------------------------------------------------------*/

#define LIBRARY_EXPORTS

//包含项目头文件
#include "includes/code_location.hpp"
#include "includes/check.hpp"

namespace Quest{

    CodeLocation::CodeLocation():mFileName("Unknown"),mFunctionName("Unknown"),mLineNumber(0){}

    CodeLocation::CodeLocation(const std::string& FileName,const std::string& FunctionName,std::size_t LineNumber):
        mFileName(FileName),mFunctionName(FunctionName),mLineNumber(LineNumber){}

    const std::string& CodeLocation::GetFileName() const {return mFileName;}

    const std::string& CodeLocation::GetFunctionName() const {return mFunctionName;}

    std::size_t CodeLocation::GetLineNumber() const {return mLineNumber;}

    std::string CodeLocation::CleanFileName() const{
        std::string clean_file_name(mFileName);
        ReplaceAll(clean_file_name,"\\","/");

        std::size_t quest_root_position = clean_file_name.find("/applications/");
        if(quest_root_position != std::string::npos){
            clean_file_name.erase(0,quest_root_position+1);
            return clean_file_name;
        }

        if (quest_root_position == std::string::npos)
            quest_root_position = clean_file_name.rfind("/quest/");
        if (quest_root_position != std::string::npos)
            clean_file_name.erase(0,quest_root_position+1);
        return clean_file_name;
    }

    std::string CodeLocation::CleanFunctionName() const{
        std::string clean_function_name(mFunctionName);
        RemoveNamespace(clean_function_name,"QUest");
        RemoveNamespace(clean_function_name,"std");
        ReduceTemplateArgumentsToFirstN(clean_function_name, "ublas::vector", 1);
        ReduceTemplateArgumentsToFirstN(clean_function_name, "ublas::matrix", 1);
        ReduceTemplateArgumentsToFirstN(clean_function_name, "iterators::indirect_iterator", 1);
        ReduceTemplateArgumentsToFirstN(clean_function_name, "PointerVectorSet", 1);
        ReduceTemplateArgumentsToFirstN(clean_function_name, "basic_string", 1);
        ReplaceAll(clean_function_name, "__int64", "int");
        ReplaceAll(clean_function_name, "basic_string<char,...>", "string");
        ReduceTemplateArgumentsToFirstN(clean_function_name, "compressed_matrix", 0);
        ReplaceAll(clean_function_name, "ublas::vector<double,...>", "Vector");
        ReplaceAll(clean_function_name, "ublas::DenseMatrix<double,...>", "Matrix");
        ReduceTemplateArgumentsToFirstN(clean_function_name, "ResidualBasedBlockBuilderAndSolver", 1);
        ReduceTemplateArgumentsToFirstN(clean_function_name, "ResidualBasedLinearStrategy", 1);
        ReplaceAll(clean_function_name, "Dof<double>", "Dof");
        ReplaceAll(clean_function_name, "Node", "Node");

        return clean_function_name;
    }

    void CodeLocation::RemoveNamespace(std::string& FunctionName, const std::string& Namespace){
        std::string namespace_string = Namespace+"::";
        ReplaceAll(FunctionName,namespace_string,"");
    }

    void CodeLocation::ReduceTemplateArgumentsToFirstN(std::string& FunctionName, const std::string& TemplateName,std::size_t NumberOfArgumentsToKeep){
        std::size_t start_position = 0;
        while((start_position = FunctionName.find(TemplateName,start_position))!=std::string::npos){
            start_position += TemplateName.size();
            std::size_t template_position = GetNextPositionSkippingWhiteSpaces(FunctionName,start_position);
            std::string::iterator arguments_iterator = FunctionName.begin() + template_position;
            std::size_t number_of_open_parenthesis = 0;
            std::size_t number_of_open_templates = 1;
            arguments_iterator++;
            std::size_t number_of_arguments = 0;
            if(*arguments_iterator != '>')
                number_of_arguments = 1;
            std::size_t replace_position = std::string::npos;
            if(number_of_arguments > NumberOfArgumentsToKeep)
                replace_position = arguments_iterator-FunctionName.begin();
            while(arguments_iterator!=FunctionName.end()&&number_of_open_templates>0){
                if(*arguments_iterator == '<')
                    number_of_open_templates++;
                else if(*arguments_iterator == '>')
                    number_of_open_templates--;
                else if(*arguments_iterator == '(')
                    number_of_open_parenthesis++;
                else if(*arguments_iterator == ')')
                    number_of_open_parenthesis--;
                else if(*arguments_iterator == ','){
                    if(number_of_open_parenthesis==0 && number_of_open_templates==1)
                        number_of_arguments++;
                    if((number_of_arguments>NumberOfArgumentsToKeep) && (replace_position==std::string::npos))
                        replace_position = arguments_iterator-FunctionName.begin()+1;
                }
                arguments_iterator++;
            }
            if(replace_position!=std::string::npos){
                std::size_t replace_size = arguments_iterator-FunctionName.begin()-1-replace_position;
                FunctionName.replace(replace_position,replace_size,"...");
            }
        }
    }

    void CodeLocation::ReplaceAll(std::string& ThisString, const std::string& FromString,const std::string& ToString){
        std::size_t start_position = 0;
        while((start_position = ThisString.find(FromString,start_position))!= std::string::npos){
            ThisString.replace(start_position,FromString.length(),ToString);
            start_position += ToString.length();
        }
    }

    std::size_t CodeLocation::GetNextPositionSkippingWhiteSpaces(const std::string& ThisString,std::size_t Position){
        char c = ThisString[Position];
        while(std::isspace(c))
            c = ThisString[++Position];
        return Position;
    }

    std::ostream& operator<<(std::ostream& rOstream,const CodeLocation& Location){
        rOstream<<Location.CleanFileName()<<":"<<Location.GetLineNumber()<<":"<<Location.CleanFunctionName();
        return rOstream;
    }
}