/*-------------------------------------------------------
保存日志消息及其属性，包括消息的来源、严重性、级别和类别
-------------------------------------------------------*/

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

            explicit LoggerMessage(const std::string& TheLabel):
                mLabel(TheLabel),mLevel(1),mSeverity(Severity::INFO),mCategory(Category::STATUS),mMessageSource(),mDistributedFilter(DistributedFilter::FromRoot()){}

            LoggerMessage(const LoggerMessage& Other): mLabel(Other.mLabel),mMessage(Other.mMessage),mLevel(Other.mLevel),mLocation(Other.mLocation),
                mSeverity(Other.mSeverity),mCategory(Other.mCategory),mMessageSource(Other.mMessageSource),mDistributedFilter(Other.mDistributedFilter){}

            virtual ~LoggerMessage(){}

            LoggerMessage& operator=(const LoggerMessage& Other){
                mLabel = Other.mLabel;
                mMessage = Other.mMessage;
                mLevel = Other.mLevel;
                mSeverity = Other.mSeverity;
                mCategory = Other.mCategory;
                mMessageSource = Other.mMessageSource;

                return *this;
            }

            void SetLabel(const std::string& TheLabel){
                mLabel = TheLabel;
            }

            const std::string& GetLabel()const {
                return mLabel;
            }

            void SetMessage(const std::string& TheMessage){
                mMessage = TheMessage;
            }

            const std::string& GetMessage()const {
                return mMessage;
            }

            void SetLevel(std::size_t TheLevel){
                mLevel = TheLevel;
            }

            std::size_t GetLevel()const {
                return mLevel;
            }

            void SetLocation(const CodeLocation& TheLocation){
                mLocation = TheLocation;
            }

            CodeLocation GetLocation()const {
                return mLocation;
            }

            void SetSerity(const Severity& TheSeverity){
                mSeverity = TheSeverity;
            }

            Severity GetSeverity()const {
                return mSeverity;
            }

            void SetCategory(const Category& TheCategory){
                mCategory = TheCategory;
            }

            Category GetCategory()const {
                return mCategory;
            }

            bool IsDistributed()const {
                return mDistributedFilter.IsDistributed();
            }

            bool WriteInThisRank()const {
                return mDistributedFilter.WriteFromRank(mMessageSource.GetRank());
            }

            int GetSourceRank()const {
                return mMessageSource.GetRank();
            }

            void SetTime(){
                mTime = std::chrono::system_clock::now();
            }

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
            std::string mLabel;
            std::string mMessage;
            std::size_t mLevel;
            CodeLocation mLocation;
            Severity mSeverity;
            Category mCategory;
            MessageSource mMessageSource;
            DistributedFilter mDistributedFilter;
            TimePointType mTime;

    };

    std::ostream& operator<<(std::ostream& rOstream, const LoggerMessage& rThis);


}// namespace Quest

#endif // LOGGER_MESSAGE_HPP