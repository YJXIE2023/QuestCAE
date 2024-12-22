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

    /**
     * @brief 日志输出基类
     * @details 定义了日志输出的接口，并提供了基础的功能
     */
    class QUEST_API(QUEST_CORE) LoggerOutput{
        public:
            QUEST_DEFINE_LOCAL_FLAG(WARNING_PREFIX);
            QUEST_DEFINE_LOCAL_FLAG(INFO_PREFIX);
            QUEST_DEFINE_LOCAL_FLAG(DETAIL_PREFIX);
            QUEST_DEFINE_LOCAL_FLAG(DEBUG_PREFIX);
            QUEST_DEFINE_LOCAL_FLAG(TRACE_PREFIX);

            QUEST_CLASS_POINTER_DEFINITION(LoggerOutput);

            /**
             * @brief 构造函数
             * @param rOutputSream 输出流
             */
            explicit LoggerOutput(std::ostream& rOutputSream):mpStream(&rOutputSream),mMaxLevel(1),
                mSeverity(LoggerMessage::Severity::INFO),mCategory(LoggerMessage::Category::STATUS){
                mOptions.Set(WARNING_PREFIX, true);
                mOptions.Set(INFO_PREFIX, false);
                mOptions.Set(DETAIL_PREFIX, false);
                mOptions.Set(DEBUG_PREFIX, false);
                mOptions.Set(TRACE_PREFIX, false);
            }

            /**
             * @brief 拷贝构造函数
             */
            LoggerOutput(const LoggerOutput& other):mpStream(other.mpStream),mMaxLevel(other.mMaxLevel),mSeverity(other.mSeverity),
                mCategory(other.mCategory){}

            /**
             * @brief 析构函数
             */
            virtual ~LoggerOutput(){}

            /**
             * @brief 赋值运算符
             */
            LoggerOutput& operator=(const LoggerOutput& Other) = delete;

            /**
             * @brief 写入日志头部信息
             */
            virtual void WriteHeader();

            /**
             * @brief 写入日志消息
             */
            virtual void WriteMessage(const LoggerMessage& TheMessage);

            /**
             * @brief 将缓冲区的数据强制刷新到日志输出目标中
             */
            virtual void Flush();

            /**
             * @brief 设置日志最大输出等级
             */
            void SetMaxLevel(std::size_t TheLevel){
                mMaxLevel = TheLevel;
            }

            /**
             * @brief 获取日志最大输出等级
             */
            std::size_t GetMaxLevel() const {
                return mMaxLevel;
            }

            /**
             * @brief 设置日志的严重性级别
             */
            void SetSeverity(const LoggerMessage::Severity& TheSeverity){
                mSeverity = TheSeverity;
            }

            /**
             * @brief 获取日志的严重性级别
             */
            LoggerMessage::Severity GetSeverity() const {
                return mSeverity;
            }

            /**
             * @brief 设置日志的类别
             */
            void SetCategory(const LoggerMessage::Category& TheCategory){
                mCategory = TheCategory;
            }

            /**
             * @brief 获取日志的类别
             */
            LoggerMessage::Category GetCategory() const {
                return mCategory;
            }

            /**
             * @brief 设置日志输出标志
             */
            void SetOption(const Quest::Flags ThisFlag, bool Value){
                mOptions.Set(ThisFlag, Value);
            }

            /**
             * @brief 获取日志输出标志
             */
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
            /**
             * @brief 输出流指针
             */
            std::ostream* mpStream;

            /**
             * @brief 日志最大输出等级
             */
            std::size_t mMaxLevel;

            /**
             * @brief 日志的严重性级别
             */
            LoggerMessage::Severity mSeverity;

            /**
             * @brief 日志的类别
             */
            LoggerMessage::Category mCategory;

            /**
             * @brief 日志输出选项
             */
            Quest::Flags mOptions;

    };

    std::ostream& operator<<(std::ostream& rOstream, const LoggerOutput& rThis);

} // namespace Quest

#endif // LOGGER_OUTPUT_HPP