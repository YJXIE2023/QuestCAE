#ifndef QUEST_TIMER_HPP
#define QUEST_TIMER_HPP

// 系统头文件
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <unordered_map>
#include <chrono>

// 项目头文件
#include "includes/define.hpp"


namespace Quest{

    /**
     * @brief 计时器类
     */
    class QUEST_API(QUEST_CORE) Timer{
        class TimerData;

        public:
            QUEST_CLASS_POINTER_DEFINITION(Timer);

            using TimeType = double;
            using ContainerType = std::map<std::string, TimerData>;
            using InternalNameDatabaseType = std::unordered_map<std::string, std::string>;
            
            static constexpr std::size_t NumberOfZeros = 3;

        public:
            Timer();

            virtual ~Timer();

            /**
             * @brief 开始计时
             */
            static void Start(std::string const& rIntervalName);

            /**
             * @brief 停止计时
             */
            static void Stop(std::string const& rIntervalName);

            /**
             * @brief 返回计算时间
             */
            static inline double GetTime()
            {
                const auto current_time = std::chrono::steady_clock::now();
                return std::chrono::duration_cast<std::chrono::duration<double>>(current_time.time_since_epoch()).count();
            }


            /**
             * @brief 返回从给定的 StartTime 到当前时间的经过秒数
             */
            static inline double ElapsedSeconds(const std::chrono::steady_clock::time_point StartTime)
            {
                const auto current_time = std::chrono::steady_clock::now();
                return std::chrono::duration_cast<std::chrono::duration<double>>(current_time - StartTime).count();
            }


            /**
             * @brief 设置一个输出文件 *.time，用于存储时间数据
             */
            static int SetOuputFile(std::string const& rOutputFileName);


            /**
             * @brief 关闭输出文件
             */
            static int CloseOuputFile();


            /**
             * @brief 获取一个变量，用于指示信息是否在屏幕上打印
             */
            static bool GetPrintOnScreen();


            /**
             * @brief 设置一个变量，用于指示信息是否在屏幕上打印
             */
            static void SetPrintOnScreen(bool const PrintOnScreen);


            /**
             * @brief 获取一个变量，用于指示信息是否在每个时间间隔上打印
             */
            static bool GetPrintIntervalInformation();


            /**
             * @brief 设置一个变量，用于指示信息是否在每个时间间隔上打印
             */
            static void SetPrintIntervalInformation(bool const PrintIntervalInformation);


            /**
             * @brief 将内部信息打印到给定的流中
             */
            static void PrintIntervalInformation(
                std::ostream& rOStream,
                std::string const& rIntervalName,
                const double StartTime,
                const double StopTime
            );


            /**
             * @brief 打印内部信息
             */
            static void PrintIntervalInformation(
                std::string const& rIntervalName,
                const double StartTime,
                const double StopTime
            );


            /**
             * @brief 打印所有计时器信息
             */
            static void PrintTimingInformation();


            /**
             * @brief 打印所有计时器信息到给定的流中
             */
            static void PrintTimingInformation(std::ostream& rOStream);


            virtual std::string Info() const
            {
                return "Timer";
            }


            virtual void PrintInfo(std::ostream& rOStream) const{}


            virtual void PrintData(std::ostream& rOStream) const
            {
                PrintTimingInformation(rOStream);
            }


        protected:

        private:
            /**
             * @brief 用于管理时间的内部类
             */
            class TimerData{
                public:
                    TimerData(): mRepeatNumber(0), mStartTime(0), mTotalElapsedTime(0), mMaximumTime(0), mMinimumTime(0){}

                    double GetStartTime(){
                        return mStartTime;
                    }

                    void SetStartTime(double startTime){
                        mStartTime = startTime;
                    }

                    double GetTotalElapsedTime(){
                        return mTotalElapsedTime;
                    }

                    void SetTotalElapsedTime(double totalElapsedTime){
                        mTotalElapsedTime = totalElapsedTime;
                    }

                    void Update(double StopTime){
                        double elapsed = StopTime - mStartTime;
                        if(mRepeatNumber == 0)
                            mMinimumTime = elapsed;
                        mTotalElapsedTime += elapsed;
                        if(mMaximumTime < elapsed)
                            mMaximumTime = elapsed;

                        if((mMinimumTime > elapsed))
                            mMinimumTime = elapsed;

                        mRepeatNumber++;
                    }

                    void PrintData(std::ostream& rOstream, double GlobalElapsedTime = -1.00) const;

                private:
                    int mRepeatNumber;
                    double mStartTime;
                    double mTotalElapsedTime;
                    double mMaximumTime;
                    double mMinimumTime;
            };


            /**
             * @brief 返回表中的内部名称
             */
            static std::string GetInternalName(const std::string& rName);

            /**
             * @brief 创建标签栈实例
             */
            static std::vector<std::string>& GetLabelsStackInstance()
            {
            static std::vector<std::string> instance;
            return instance;
            }

            /**
             * @brief 将其中的所有标签连接成一个完整的标签字符串
             */
            static std::string CreateFullLabel(){
                const auto& r_labels_stack = GetLabelsStackInstance();
                std::string result;
                for(const auto& r_label : r_labels_stack){
                result += "/" + r_label;
                }
                return result;
            }


            Timer& operator=(const Timer&){return *this;}

        private:
            /**
             * @brief 时间表名称
             */
            static InternalNameDatabaseType msInternalNameDatabase;    

            /**
             * @brief 时间表
             */
            static ContainerType msTimeTable;                             

            /**
             * @brief 输出文件流
             */
            static std::ofstream msOutputFile;                          

            /**
             * @brief 计时器计数器
             */
            static std::size_t msCounter;                                

            /**
             * @brief 指示信息是否在屏幕上打印
             */
            static bool msPrintOnScreen;                               

            /**
             * @brief 是否打印时间间隔信息
             */
            static bool msPrintIntervalInformation;                       

            /**
             * @brief 计时器开始时间
             */
            static const std::chrono::steady_clock::time_point mStartTime; /// The starting time

    };


    inline std::ostream& operator<<(std::ostream& rOstream, const Timer& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }

}


#endif //QUEST_TIMER_HPP