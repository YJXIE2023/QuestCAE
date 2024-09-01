/*---------------------------------------
LoggerOutput类的实现代码
---------------------------------------*/

//系统头文件
#include <sstream>

//项目头文件
#include "IO/logger_output.hpp"
#include "includes/kernal.hpp"

#ifdef QUEST_COLORED_OUTPUT
    #include "utilities/color_utilities.hpp"
#endif

namespace Quest{
    
    QUEST_CREATE_LOCAL_FLAG(LoggerOutput, WARNING_PREFIX, 0);
    QUEST_CREATE_LOCAL_FLAG(LoggerOutput, INFO_PREFIX, 1);
    QUEST_CREATE_LOCAL_FLAG(LoggerOutput, DEBUG_PREFIX, 2);
    QUEST_CREATE_LOCAL_FLAG(LoggerOutput, DEBUG_PREFIX, 3);
    QUEST_CREATE_LOCAL_FLAG(LoggerOutput, TRACE_PREFIX, 4);

    std::string LoggerOutput::Info() const{
        return "LoggerOutput";
    }

    void LoggerOutput::WriteHeader(){
        auto& rOstream = GetStream();
        rOstream<< "---------------------------------------\n"
                << "-----------Q U E S T  C A E -----------\n"
                << "---------------------------------------\n"
                << "           Multi_Physics "<< Quest::Version() << "\n"
                << "           Compiled for "<<Quest::OSName()<<" and "<<Quest::PythonVersion()<<" with "<<Quest::Compiler()<<std::endl;
    }

    void LoggerOutput::WriteMessage(const LoggerMessage& TheMessage){
        auto& rOstream = GetStream();
        auto message_severity = TheMessage.GetSeverity();
        if (TheMessage.WriteInThisRank() && message_severity <= mSeverity){
            SetMessageColor(message_severity);

            switch(message_severity){
                case LoggerMessage::Severity::WARNING:
                    if (mOptions.Is(WARNING_PREFIX))
                        rOstream << "[WARNING] ";
                    break;
                case LoggerMessage::Severity::INFO:
                    if (mOptions.Is(INFO_PREFIX))
                        rOstream << "[INFO] ";
                    break;
                case LoggerMessage::Severity::DETAIL:
                    if (mOptions.Is(DEBUG_PREFIX))
                        rOstream << "[DETAIL] ";
                    break;
                case LoggerMessage::Severity::DEBUG:
                    if (mOptions.Is(DEBUG_PREFIX))
                        rOstream << "[DEBUG] ";
                    break;
                case LoggerMessage::Severity::TRACE:
                    if (mOptions.Is(TRACE_PREFIX))
                        rOstream << "[TRACE] ";
                    break;
                default:
                    break;
            }

            if(TheMessage.IsDistribute())
                rOstream<< "Rank "<<TheMessage.GetSourceRank()<<": ";
            
            if (TheMessage.GetLabel().size())
                rOstream << TheMessage.GetLabel() << ": "<<TheMessage.GetMessage();
            else
                rOstream << TheMessage.GetMessage();

            ResetMessageColor(massage_severity);
        }
    }

    void LoggerOutput::Flush(){
        GetStream() << std::flush;
    }

    void LoggerOutput::PrintInfo(std::ostream& rOstream) const{
        rOstream<< Info();
    }

    void LoggerOutput::PrintData(std::ostream& rOstream) const{
        rOstream<< "Max Level: "<< mMaxLevel;
    }

    LoggerOutput& LoggerOutput::operator << (const char* rString){
        GetStream() << rString;
        return *this;
    }

    LoggerOutput& LoggerOutput::operator << (std::ostream& (*rFunc)(std::ostream&)){
        std::stringstream buffer;
        rFunc(buffer);

        GetStream() << buffer.str();

        return *this;
    }

    void LoggerOutput::SetMessageColor(LoggerMessage::Severity MessageSeverity){
        #ifdef QQUEST_COLORED_OUTPUT
            if (MessageSeverity == LoggerMessage::Severity::WARNING)
                GetStream() << KYEL;
        #endif
    }

    void LoggerOutput::ResetMessageColor(LoggerMessage::Severity MessageSeverity){
        #ifdef QUEST_COLORED_OUTPUT
            if (MessageSeverity == LoggerMessage::Severity::WARNING)
                GetStream() << RST;
        #endif
    }

    std::ostream& operator << (std::ostream& rOstream, const LoggerOutput& rThis){
        rThis.PrintInfo(rOstream);
        rOstream<<std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }




}// namespace Quest