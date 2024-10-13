/*--------------------------------------------
LoggerOutput为日志消息输出类的基类
--------------------------------------------*/

#ifndef LOGGER_OUTPUT_HPP
#define LOGGER_OUTPUT_HPP

// 系统头文件
#include <string>
#include <iostream>
#include <map>

// 项目头文件
#include "includes/define.hpp"
#include "IO/logger_message.hpp"
#include "container/flags.hpp"

namespace Quest{

    class QUEST_API(QUEST_CORE) LoggerOutput{
        public:
            QUEST_DEFINE_LOCAL_FLAG(WARNING_PREFIX);
            QUEST_DEFINE_LOCAL_FLAG(INFO_PREFIX);
            QUEST_DEFINE_LOCAL_FLAG(DETAIL_PREFIX);
            QUEST_DEFINE_LOCAL_FLAG(DEBUG_PREFIX);
            QUEST_DEFINE_LOCAL_FLAG(TRACE_PREFIX);

            QUEST_CLASS_POINTER_DEFINITION(LoggerOutput);

            explicit LoggerOutput(std::ostream& rOutputSream):mpStream(&rOutputSream),mMaxLevel(1),
                mSeverity(LoggerMessage::Severity::INFO),mCategory(LoggerMessage::Category::STATUS){
                mOptions.Set(WARNING_PREFIX, true);
                mOptions.Set(INFO_PREFIX, false);
                mOptions.Set(DETAIL_PREFIX, false);
                mOptions.Set(DEBUG_PREFIX, false);
                mOptions.Set(TRACE_PREFIX, false);
            }

            LoggerOutput(const LoggerOutput& other):mpStream(other.mpStream),mMaxLevel(other.mMaxLevel),mSeverity(other.mSeverity),
                mCategory(other.mCategory){}

            virtual ~LoggerOutput(){}

            LoggerOutput& operator=(const LoggerOutput& Other) = delete;

            virtual void WriteHeader();

            virtual void WriteMessage(const LoggerMessage& TheMessage);

            virtual void Flush();

            void SetMaxLevel(std::size_t TheLevel){
                mMaxLevel = TheLevel;
            }

            std::size_t GetMaxLevel() const {
                return mMaxLevel;
            }

            void SetSeverity(const LoggerMessage::Severity& TheSeverity){
                mSeverity = TheSeverity;
            }

            LoggerMessage::Severity GetSeverity() const {
                return mSeverity;
            }

            void SetCategory(const LoggerMessage::Category& TheCategory){
                mCategory = TheCategory;
            }

            LoggerMessage::Category GetCategory() const {
                return mCategory;
            }

            void SetOption(const Quest::Flags ThisFlag, bool Value){
                mOptions.Set(ThisFlag, Value);
            }

            bool GetOption(const Quest::Flags ThisFlag){
                return mOptions.Is(ThisFlag);
            }

            virtual std::string Info() const;

            virtual void PrintInfo(std::ostream& rOstream) const;

            virtual void PrintData(std::ostream& rOstream) const;

            template<typename StreamValueType>
            LoggerOutput& operator<<(const StreamValueType& rVlaue){
                std::stringstream buffer;
                buffer << rVlaue;
                WriteMessage(buffer.str());

                GetStream() << buffer.str();

                return *this;
            }

            LoggerOutput& operator<<(std::ostream& (*pf)(std::ostream&));

            LoggerOutput& operator<<(const char* rString);

        protected:
            LoggerOutput():mpStream(nullptr),mMaxLevel(1),mSeverity(LoggerMessage::Severity::INFO),
                mCategory(LoggerMessage::Category::STATUS){
                    mOptions.Set(WARNING_PREFIX, true);
                    mOptions.Set(INFO_PREFIX, false);
                    mOptions.Set(DETAIL_PREFIX, false);
                    mOptions.Set(DEBUG_PREFIX, false);
                    mOptions.Set(TRACE_PREFIX, false);
                }

            std::ostream& GetStream() {return *mpStream;}
            std::ostream* pGetStream() {return mpStream;}
            void SetStream(std::ostream* pStream) {mpStream = pStream;}

            virtual void SetMessageColor(LoggerMessage::Severity MessageSeverity);
            virtual void ResetMessageColor(LoggerMessage::Severity MessageSeverity);

        private:
            std::ostream* mpStream;
            std::size_t mMaxLevel;
            LoggerMessage::Severity mSeverity;
            LoggerMessage::Category mCategory;
            Quest::Flags mOptions;

    };

    std::ostream& operator<<(std::ostream& rOstream, const LoggerOutput& rThis);

} // namespace Quest

#endif // LOGGER_OUTPUT_HPP