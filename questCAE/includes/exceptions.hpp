/*--------------------------------------------
基于标准库中exception类定义处理异常的类
where方法用于获取错误的位置
可通过QUEST_ERROR宏来使用该类
----------------------------------------------*/

#ifndef QUEST_EXCEPTION_HPP
#define QUEST_EXCEPTION_HPP

// System includes
#include <stdexcept>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

// Project includes
#include "includes/quest_export_api.hpp"
#include "includes/code_location.hpp"
#include "utilities/stl_vector_io.hpp"

namespace Quest{
    
    class QUEST_API Exception:public std::exception
    {
    public:
        
        Exception();

        explicit Exception(const std::string& rWhat);

        Exception(const std::string& rWhat, const CodeLocation& rLocation);

        Exception(const Exception& rOther);

        ~Exception() noexcept override;
        
        Exception& operator=(const Exception& rOther) = delete;

        Exception& operator<<(const CodeLocation& rLocation);

        template<typename StreamValueType>
        Exception& operator<<(const StreamValueType& rValue)
        {
            std::stringstream buffer;
            buffer << rValue;

            appendMessage(buffer,str());

            return *this;
        }

        Exception& operator<<(std::ostream& (*pf)(std::ostream&));

        Exception& operator<<(const char* pString);

        void appendMessage(const std::string& rMessage);

        void addToCallStack(const CodeLocation& rLocation);

        const char* what() const noexcept override;

        const std::string& message() const;

        const CodeLocation where() const;

        virtual std::string info() const;

        virtual void printInfo(std::ostream& rOstream) const;

        virtual void printData(std::ostream& rOstream) const;

    private:

        std::string mMessage;
        std::string mWhat;
        std::vector<CodeLocation> mCallStack;

        void updateWhat();

    };//class Exception

    #define QUEST_ERROR throw Quest::Exception("Error: ",QUEST_CODE_LOCATION)
    #define QUEST_ERROR_IF(condition) if(condition) QUEST_ERROR
    #define QUEST_ERROR_IF_NOT(condition) if(!(condition)) QUEST_ERROR

    #ifdef QUEST_DEBUG
        #define QUEST_DEBUG_ERROR QUEST_ERROR
        #define QUEST_DEBUG_ERROR_IF(condition) if(condition) QUEST_ERROR_IF(condition)
        #define QUEST_DEBUG_ERROR_IF_NOT(condition) if(!(condition)) QUEST_ERROR_IF_NOT(condition)
    #else
        #define QUEST_DEBUG_ERROR if(false) QUEST_ERROR
        #define QUEST_DEBUG_ERROR_IF(condition) if(false) QUEST_ERROR_IF(condition)
        #define QUEST_DEBUG_ERROR_IF_NOT(condition) if(false) QUEST_ERROR_IF_NOT(condition)
    #endif

    std::istream& operator>>(std::istream& rIstream, Exception& rException);

    QUEST_API std::ostream& operator<<(std::ostream& rOstream, const Exception& rException);
}



#endif // QUEST_EXCEPTION_HPP