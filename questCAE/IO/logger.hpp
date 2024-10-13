/*--------------------------------------------
logger类负责将消息写入输出流中
单例模式输出，输出列表在软件中时唯一的
---------------------------------------------*/

#ifndef QUEST_LOGGER_HPP
#define QUEST_LOGGER_HPP

// 系统头文件
#include <string>
#include <iostream>

// 项目头文件
#include "IO/logger_message.hpp"
#include "IO/logger_output.hpp"
#include "includes/exceptions.hpp"

namespace Quest{
    
    class QUEST_API(QUEST_CORE) Logger{
        public:
            using LoggerOutputContainerType = std::vector<LoggerOutput::Pointer>;
            using Severity = LoggerMessage::Severity;
            using Category = LoggerMessage::Category;
            using DistributedFilter = LoggerMessage::DistributedFilter;

            explicit Logger(const std::string& TheLabel);

            Logger();

            Logger(const Logger& rOther) = delete;
            
            virtual ~Logger();

            Logger& operator=(const Logger& rOther) = delete;

            static LoggerOutputContainerType& GetOutputsInstance(){
                static LoggerOutputContainerType instance;
                return instance;
            }

            static LoggerOutput& GetDefaultOutputInstance(){
                static LoggerOutput defaultOutputInstance(std::cout);
                return defaultOutputInstance;
            }

            static void AddOutput(LoggerOutput::Pointer pTheOutput);

            static void RemoveOutput(LoggerOutput::Pointer pTheOutput);

            static void Flush();

            const std::string& GetCurrentMessage(){
                return mCurrentMessage.GetMessage();
            }

            virtual std::string Info() const;

            virtual void PrintInfo(std::ostream& rOstream) const;

            virtual void PrintData(std::ostream& rOstream) const;

            template<typename StreamValueType>
            Logger& operator << (const StreamValueType& rValue){
                mCurrentMessage << rValue;
                return *this;
            }

            Logger& operator << (std::ostream& (*pf)(std::ostream&));

            Logger& operator << (const char* rString);

            Logger& operator << (const CodeLocation& TheLocation);

            Logger& operator << (const Severity& TheSeverity);

            Logger& operator << (const Category& TheCategory);

        protected:

        private:
            LoggerMessage mCurrentMessage;
    };

    inline std::istream& operator >> (std::istream& rIstreamm, Logger& rThis);

    inline std::ostream& operator << (std::ostream& rOstream, const Logger& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }

    #define QUEST_LOG_OCCURRENCES_LINE(line) quest_log_loop_counter##line
    #define QUEST_LOG_OCCURRENCES QUEST_LOG_OCCURRENCES_LINE(__LINE__)

    #define QUEST_INFO(label) Quest::Logger(label) << QUEST_CODE_LOCATION << Quest::Logger::Severity::INFO
    #define QUEST_INFO_IF(label, conditional) if (conditional) Quest::Logger(label) << QUEST_CODE_LOCATION << Quest::Logger::Severity::INFO
    #ifdef QUEST_DEBUG
        #define QUEST_INFO_ONCE(label) static int QUEST_LOG_OCCURRENCES = -1; if(++QUEST_LOG_OCCURRENCES == 0) Quest::Logger(label) << QUEST_CODE_LOCATION << Quest::Logger::Secerity::INFO
        #define QUEST_INFO_FIRST_N(label, logger_count) static int QUEST_LOG_OCCURRENCES = -1; if(++QUEST_LOG_OCCURRENCES < logger_count) Quest::Logger(label) << QUEST_CODE_LOCATION << Quest::Logger::Secerity::INFO
    #else
        #define QUEST_INFO_ONCE(label) if(false) QUEST_INFO(label)
        #define QUEST_INFO_FIRST_N(label, logger_count) if(false) QUEST_INFO(label)
    #endif

    #define QUEST_INFO_ALL_RANKS(label) QUEST_INFO(label) << Quest::Logger::DistributedFilter::FromALlRanks()
    #define QUEST_INDO_IF_ALL_RANKS(label, conditional) QUEST_INFO_IF(label, conditional) << Quest::Logger::DistributedFilter::FromALlRanks()
    #define QUEST_INFO_ONCE_ALL_RANKS(label) QUEST_INFO_ONCE(label) << Quest::Logger::DistributedFilter::FromALlRanks()
    #define QUEST_INFO_FIRST_N_ALL_RANKS(label, logger_count) QUEST_INFO_FIRST_N(label, logger_count) << Quest::Logger::DistributedFilter::FromALlRanks()

    #define QUEST_WARNING(label) Quest::Logger(label) << QUEST_CODE_LOCATION << Quest::Logger::Severity::WARNING
    #define QUEST_WARNING_IF(label, conditional) if (conditional) Quest::Logger(label) << QUEST_CODE_LOCATION << Quest::Logger::Severity::WARNING
    #ifdef QUEST_DEBUG
        #define QUEST_WARNING_ONCE(label) static int QUEST_LOG_OCCURRENCES = -1; if(++QUEST_LOG_OCCURRENCES == 0) Quest::Logger(label) << QUEST_CODE_LOCATION << Quest::Logger::Secerity::WARNING
        #define QUEST_WARNING_FIRST_N(label, logger_count) static int QUEST_LOG_OCCURRENCES = -1; if(++QUEST_LOG_OCCURRENCES < logger_count) Quest::Logger(label) << QUEST_CODE_LOCATION << Quest::Logger::Secerity::WARNING
    #else
        #define QUEST_WARNING_ONCE(label) if(false) QUEST_WARNING(label)
        #define QUEST_WARNING_FIRST_N(label, logger_count) if(false) QUEST_WARNING(label)
    #endif

    #define QUEST_WARNING_ALL_RANKS(label) QUEST_WARNING(label) << Quest::Logger::DistributedFilter::FromALlRanks()
    #define QUEST_WARNING_IF_ALL_RANKS(label, conditional) QUEST_WARNING_IF(label, conditional) << Quest::Logger::DistributedFilter::FromALlRanks()
    #define QUEST_WARNING_ONCE_ALL_RANKS(label) QUEST_WARNING_ONCE(label) << Quest::Logger::DistributedFilter::FromALlRanks()
    #define QUEST_WARNING_FIRST_N_ALL_RANKS(label, logger_count) QUEST_WARNING_FIRST_N(label, logger_count) << Quest::Logger::DistributedFilter::FromALlRanks()

    #define QUEST_DETAIL(label) Quest::Logger(label) << QUEST_CODE_LOCATION << Quest::Logger::Severity::DETAIL
    #define QUEST_DETAIL_IF(label, conditional) if (conditional) Quest::Logger(label) << QUEST_CODE_LOCATION << Quest::Logger::Severity::DETAIL
    #ifdef QUEST_DEBUG
        #define QUEST_DETAIL_ONCE(label) static int QUEST_LOG_OCCURRENCES = -1; if(++QUEST_LOG_OCCURRENCES == 0) Quest::Logger(label) << QUEST_CODE_LOCATION << Quest::Logger::Secerity::DETAIL
        #define QUEST_DETAIL_FIRST_N(label, logger_count) static int QUEST_LOG_OCCURRENCES = -1; if(++QUEST_LOG_OCCURRENCES < logger_count) Quest::Logger(label) << QUEST_CODE_LOCATION << Quest::Logger::Secerity::DETAIL
    #else
        #define QUEST_DETAIL_ONCE(label) if(false) QUEST_DETAIL(label)
        #define QUEST_DETAIL_FIRST_N(label, logger_count) if(false) QUEST_DETAIL(label)
    #endif

    #define QUEST_DETAIL_ALL_RANKS(label) QUEST_DETAIL(label) << Quest::Logger::DistributedFilter::FromALlRanks()
    #define QUEST_DETAIL_IF_ALL_RANKS(label, conditional) QUEST_DETAIL_IF(label, conditional) << Quest::Logger::DistributedFilter::FromALlRanks()
    #define QUEST_DETAIL_ONCE_ALL_RANKS(label) QUEST_DETAIL_ONCE(label) << Quest::Logger::DistributedFilter::FromALlRanks()
    #define QUEST_DETAIL_FIRST_N_ALL_RANKS(label, logger_count) QUEST_DETAIL_FIRST_N(label, logger_count) << Quest::Logger::DistributedFilter::FromALlRanks()

    #ifdef QUEST_DEBUG
        #define QUEST_TRACE(label) Quest::Logger(label) << QUEST_CODE_LOCATION << Quest::Logger::Severity::TRACE
        #define QUEST_TRACE_IF(label, conditional) if (conditional) Quest::Logger(label) << QUEST_CODE_LOCATION << Quest::Logger::Severity::TRACE
        #define QUEST_TRACE_ONCE(label) static int QUEST_LOG_OCCURRENCES = -1; if(++QUEST_LOG_OCCURRENCES == 0) Quest::Logger(label) << QUEST_CODE_LOCATION << Quest::Logger::Secerity::TRACE
        #define QUEST_TRACE_FIRST_N(label, logger_count) static int QUEST_LOG_OCCURRENCES = -1; if(++QUEST_LOG_OCCURRENCES < logger_count) Quest::Logger(label) << QUEST_CODE_LOCATION << Quest::Logger::Secerity::TRACE
    #else
        #define QUEST_TRACE(label) if(false) QUEST_WARNING(label)
        #define QUEST_TRACE_IF(label, conditional) if(false) QUEST_WARNING(label)
        #define QUEST_TRACE_ONCE(label) if(false) QUEST_WARNING(label)
        #define QUEST_TRACE_FIRST_N(label, logger_count) if(false) QUEST_WARNING(label)
    #endif
    
    #define QUEST_TRACE_ALL_RANKS(label) QUEST_TRACE(label) << Quest::Logger::DistributedFilter::FromALlRanks()
    #define QUEST_TRACE_IF_ALL_RANKS(label, conditional) QUEST_TRACE_IF(label, conditional) << Quest::Logger::DistributedFilter::FromALlRanks()
    #define QUEST_TRACE_ONCE_ALL_RANKS(label) QUEST_TRACE_ONCE(label) << Quest::Logger::DistributedFilter::FromALlRanks()
    #define QUEST_TRACE_FIRST_N_ALL_RANKS(label, logger_count) QUEST_TRACE_FIRST_N(label, logger_count) << Quest::Logger::DistributedFilter::FromALlRanks()

    #ifdef QUEST_ENABLE_CHECK_POINT
        #define QUEST_CHECK_POINT(label) Quest::Logger(label) << Quest::Logger::Category::CHECKING
    #else
        #define QUEST_CHECK_POINT(label) \
        if(false)\
            Quest::Logger(label) << Quest::Logger::Category::CHECKING
    #endif


} // namespace Quest

#endif //QUEST_LOGGER_HPP