/*----------------------------------------
logger.h头文件实现代码
-----------------------------------------*/

// 系统头文件
#include <algorithm>

// 项目头文件
#include "includes/define.hpp"
#include "IO/logger.hpp"

namespace Quest{
    
    Logger::Logger(const std::string& TheLabel): mCurrentMessage(TheLabel){}

    Logger::~Logger(){
        auto outputs = GetOutputsInstance();
        #pragma omp critical
        {
            GetDefaultOutputInstance().WriteMessage(mCurrentMessage);
            for(auto iOutput = outputs.begin(); iOutput!= outputs.end(); ++iOutput)
                (*iOutput)->WriteMessage(mCurrentMessage);
        }
    }

    void Logger::AddOutput(LoggerOutput::Pointer pTheOutput){
        #pragma omp critical
        {
            GetOutputsInstance().push_back(pTheOutput);
        }
    }

    void Logger::RemoveOutput(LoggerOutput::Pointer pTheOutput){
        QUEST_TRY

        #pragma omp critical
        {
            auto i = std::find(GetOutputsInstance().begin(), GetOutputsInstance().end(), pTheOutput);
            if (i != GetOutputsInstance().end()){
                GetOutputsInstance().erase(i);
            }
        }

        QUEST_CATCH("");
    }

    void Logger::Flush(){
        auto outputs = GetOutputsInstance();
        GetDefaultOutputInstance().Flush();
        for(auto iOutput = outputs.begin(); iOutput!= outputs.end(); ++iOutput){
            (*iOutput)->Flush();
        }
    }

    std::string Logger::Info() const{
        return "Logger";
    }

    void Logger::PrintInfo(std::ostream& rOstream) const {}

    void Logger::PrintData(std::ostream& rOstream) const {}

    Logger& Logger::operator << (std::ostream& (*pf)(std::ostream&)){
        mCurrentMessage << pf;

        return *this;
    }

    Logger& Logger::operator << (const char* rString){
        mCurrentMessage << rString;

        return *this;
    }

    Logger& Logger::operator << (const CodeLocation& TheLocation){
        mCurrentMessage << TheLocation;

        return *this;
    }

    Logger& Logger::operator << (const Severity& TheSeverity){
        mCurrentMessage << TheSeverity;

        return *this;
    }

    Logger& Logger::operator << (const Category& TheCategory){
        mCurrentMessage << TheCategory;

        return *this;
    }

} // namespace Quest