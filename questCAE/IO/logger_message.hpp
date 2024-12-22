#ifndef LOGGER_MESSAGE_HPP
#define LOGGER_MESSAGE_HPP

// 系统头文件
#include<string>
#include<sstream>
#include<iostream>
#include<map>
#include<chrono>

// 项目头文件
#include "includes/quest_export_api.hpp"
#include "includes/code_location.hpp"
#include "utilities/stl_vector_io.hpp"

namespace Quest{

    class DataCommunicator;

    /**
     * @brief 日志消息类
     * @details 包含消息的来源、严重性、级别和类别
     */
    class QUEST_API(QUEST_CORE) LoggerMessage{
        public:
            using TimePointType = std::chrono::system_clock::time_point;

            enum class Severity{
                WARNING,
                INFO,
                DETAIL,
                DEBUG,
                TRACE
            };

            enum class Category{
                STATUS,
                CRITICAL,
                STATISTICS,
                PROFILING,
                CHECKING
            };

            /**
             * @brief 日志消息来源过滤器
             * @details 通过设置是否在所有节点上打印日志、是否在特定节点上打印日志，以及是否涉及分布式环境，来细粒度地控制日志消息的传播
             */
            class DistributedFilter{
                public:
                    DistributedFilter(const DistributedFilter& rOther):
                        mIsDistributed(rOther.mIsDistributed), mPrintFromAllRanks(rOther.mPrintFromAllRanks), mSourceRank(rOther.mSourceRank){}

                    static DistributedFilter FromRoot(){
                        return DistributedFilter(false, false, 0);
                    }

                    static DistributedFilter FromRank(int TheRank){
                        return DistributedFilter(true, false, TheRank);
                    }

                    static DistributedFilter FromALlRanks(){
                        return DistributedFilter(true, true, 0);
                    }

                    bool WriteFromRank(int Rank)const {
                        return mPrintFromAllRanks || Rank == mSourceRank;
                    }

                    bool IsDistributed()const {
                        return mIsDistributed;
                    }

                    int GetRank()const {
                        return mSourceRank;
                    }

                protected:

                private:
                    DistributedFilter():mPrintFromAllRanks(false),mSourceRank(0){}
                    DistributedFilter(bool IsDistributed, bool PrintFromAllRanks, int TheRank):
                        mIsDistributed(IsDistributed), mPrintFromAllRanks(PrintFromAllRanks), mSourceRank(TheRank){}

                    bool mIsDistributed;
                    bool mPrintFromAllRanks;
                    int mSourceRank;
            };

            class MessageSource{
                public:
                    MessageSource();

                    MessageSource(int TheRank):mRank(TheRank){}

                    int GetRank()const {
                        return mRank;
                    }
                
                protected:

                private:
                    int mRank;
            };

            /**
             * @brief 构造函数
             */
            explicit LoggerMessage(const std::string& TheLabel):
                mLabel(TheLabel),mLevel(1),mSeverity(Severity::INFO),mCategory(Category::STATUS),mMessageSource(),mDistributedFilter(DistributedFilter::FromRoot()){}

            /**
             * @brief 拷贝构造函数
             */
            LoggerMessage(const LoggerMessage& Other): mLabel(Other.mLabel),mMessage(Other.mMessage),mLevel(Other.mLevel),mLocation(Other.mLocation),
                mSeverity(Other.mSeverity),mCategory(Other.mCategory),mMessageSource(Other.mMessageSource),mDistributedFilter(Other.mDistributedFilter){}

            /**
             * @brief 析构函数
             */
            virtual ~LoggerMessage(){}

            /**
             * @brief 赋值运算符
             */
            LoggerMessage& operator=(const LoggerMessage& Other){
                mLabel = Other.mLabel;
                mMessage = Other.mMessage;
                mLevel = Other.mLevel;
                mSeverity = Other.mSeverity;
                mCategory = Other.mCategory;
                mMessageSource = Other.mMessageSource;

                return *this;
            }

            /**
             * @brief 设置日志消息标签
             */
            void SetLabel(const std::string& TheLabel){
                mLabel = TheLabel;
            }

            /**
             * @brief 获取日志消息标签
             */
            const std::string& GetLabel()const {
                return mLabel;
            }

            /**
             * @brief 设置日志消息内容
             */
            void SetMessage(const std::string& TheMessage){
                mMessage = TheMessage;
            }

            /**
             * @brief 获取日志消息内容
             */
            const std::string& GetMessage()const {
                return mMessage;
            }

            /**
             * @brief 设置日志消息级别
             */
            void SetLevel(std::size_t TheLevel){
                mLevel = TheLevel;
            }

            /**
             * @brief 获取日志消息级别
             */
            std::size_t GetLevel()const {
                return mLevel;
            }

            /**
             * @brief 设置错误代码未知
             */
            void SetLocation(const CodeLocation& TheLocation){
                mLocation = TheLocation;
            }

            /**
             * @brief 获取错误代码位置
             */
            CodeLocation GetLocation()const {
                return mLocation;
            }

            /**
             * @brief 设置日志消息严重性
             */
            void SetSerity(const Severity& TheSeverity){
                mSeverity = TheSeverity;
            }

            /**
             * @brief 获取日志消息严重性
             */
            Severity GetSeverity()const {
                return mSeverity;
            }

            /**
             * @brief 设置日志消息类别
             */
            void SetCategory(const Category& TheCategory){
                mCategory = TheCategory;
            }

            /**
             * @brief 获取日志消息类别
             */
            Category GetCategory()const {
                return mCategory;
            }

            /**
             * @brief 判断是否为分布式
             */
            bool IsDistributed()const {
                return mDistributedFilter.IsDistributed();
            }

            /**
             * @brief 判断是否在当前进程上打印日志
             */
            bool WriteInThisRank()const {
                return mDistributedFilter.WriteFromRank(mMessageSource.GetRank());
            }

            /**
             * @brief 设置日志消息来源
             */
            int GetSourceRank()const {
                return mMessageSource.GetRank();
            }

            /**
             * @brief 设置时间戳
             */
            void SetTime(){
                mTime = std::chrono::system_clock::now();
            }

            /**
             * @brief 获取时间戳
             */
            const TimePointType& GetTime()const {
                return mTime;
            }

            virtual std::string Info()const;

            virtual void PrintInfo(std::ostream& rOstream)const;

            virtual void PrintData(std::ostream& rOstream)const;

            template<typename StreamValueType>
            LoggerMessage& operator<<(const StreamValueType& rValue){
                std::stringstream buffer;
                buffer << rValue;

                mMessage.append(buffer.str());
                return *this;
            }

            LoggerMessage& operator<<(std::ostream& (*pf)(std::ostream&));

            LoggerMessage& operator<<(const char* rString);

            LoggerMessage& operator<<(const CodeLocation& TheLocation);

            LoggerMessage& operator<<(const Severity& TheSeverity);

            LoggerMessage& operator<<(const Category& TheCategory);

            LoggerMessage& operator<<(const DistributedFilter& TheMessageSource);

            LoggerMessage& operator<<(const DataCommunicator& TheDataCommunicator);

        protected:

        private:
            /**
             * @brief 日志消息标签
             */
            std::string mLabel;

            /**
             * @brief 日志消息内容
             */
            std::string mMessage;

            /**
             * @brief 日志消息级别
             */
            std::size_t mLevel;

            /**
             * @brief 错误代码位置
             */
            CodeLocation mLocation;

            /**
             * @brief 日志消息严重性
             */
            Severity mSeverity;

            /**
             * @brief 日志消息类别
             */
            Category mCategory;

            /**
             * @brief 日志消息来源(进程号)
             */
            MessageSource mMessageSource;

            /**
             * @brief 日志消息分布式过滤器
             */
            DistributedFilter mDistributedFilter;

            /**
             * @brief 日志消息时间戳
             */
            TimePointType mTime;

    };

    std::ostream& operator<<(std::ostream& rOstream, const LoggerMessage& rThis);


}// namespace Quest

#endif // LOGGER_MESSAGE_HPP