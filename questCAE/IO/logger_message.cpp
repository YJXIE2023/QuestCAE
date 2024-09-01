/*-----------------------------------------
logger_message头文件中的实现
------------------------------------------*/


#include "IO/logger_message.hpp"
#include "includes/parallel_environment.hpp"

namespace Quest{

    LoggerMessage::MessageSource::MessageSource(){
        mRank = ParallelEnveiroment::GetDefaultRank();
    }

    std::string LoggerMessage::Info()const{
        return "LoggerMessage";
    }

    void LoggerMessage::PrintInfo(std::ostream& ostream) const {
        ostream << Info();
    }

    void LoggerMessage::PrintData(std::ostream& ostream) const {
        ostream << mMessage;
    }

    LoggerMessage& LoggerMessage::operator<<(const char* rString){
        mMessage.append(rString);
        return 
        *this;
    }

    LoggerMessage& LoggerMessage::operator<<(std::ostream& (*pf)(std::ostream&)){
        std::stringstream buffer;
        pf(buffer);
        mMessage.append(buffer.str());
        return *this;
    }

    LoggerMessage& LoggerMessage::operator<<(const CodeLocation& TheLocation){
        mLocation = TheLocation;
        return *this;
    }

    LoggerMessage& LoggerMessage::operator<<(const Severity& TheSeverity){
        mSeverity = TheSeverity;
        return *this;
    }

    LoggerMessage& LoggerMessage::operator<<(const Category& TheCategory){
        mCategory = TheCategory;
        return *this;
    }

    LoggerMessage& LoggerMessage::operator<<(const DistributedFilter& TheFilter){
        mDistributedFilter = TheFilter;
        return *this;
    }

    LoggerMessage& LoggerMessage::operator<<(const DataCommunicator& TheCommunicator){
        mMessageSource = MessageSource(TheCommunicator.Rank());
        return *this;
    }

    std::ostream& operator<<(std::ostream& ostream, const LoggerMessage& rThis){
        rThis.PrintInfo(ostream);
        ostream<<std::endl;
        rThis.PrintData(ostream);
        return ostream;
    }


} // namespace Quest