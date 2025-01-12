#ifndef QUEST_PROCESS_HPP
#define QUEST_PROCESS_HPP

// 项目头文件
#include "includes/define.hpp"
#include "includes/quest_flags.hpp"
#include "includes/quest_parameters.hpp"
#include "includes/define_registry.hpp"

namespace Quest{

    /**
     * @brief 前向声明
     */
    class Model;


    /**
     * @brief 所有过程的基类
     * @details 所有过程的参数必须在构造时传递
     */
    class Process : public Flags{
        public:
            QUEST_CLASS_POINTER_DEFINITION(Process);

        public:
            /**
             * @brief 构造函数
             */
            Process() : Flags() {}

            /**
             * @brief 复制构造函数
             */
            explicit Process(const Flags& options) : Flags(options) {}

            /**
             * @brief 析构函数
             */
            ~Process() override {}

            /**
             * @brief 函数调用运算符重载，调用Execute函数
             */
            void operator()()
            {
                Execute();
            }

            /**
             * @brief 创建一个指向过程的指针
             */
            virtual Process::Pointer Create(
                Model& rModel,
                Parameters ThisParameters
            ){
                QUEST_ERROR << "Calling base class create. Please override this method in the corresonding Process" << std::endl;
                return nullptr;
            }

            /**
             * @brief 执行过程的算法
             */
            virtual void Execute() {}

            /**
             * @brief 计算之前的初始化函数
             */
            virtual void ExecuteInitialize(){}

            /**
             * @brief 此函数在求解循环之前执行一次，且在所有求解器构建完成之后执行
             */
            virtual void ExecuteBeforeSolutionLoop(){}

            /**
             * @brief 在每个时间步中求解阶段之前执行
             */
            virtual void ExecuteInitializeSolutionStep(){}

            /**
             * @brief 在每个时间步中求解阶段之后执行
             */
            virtual void ExecuteFinalizeSolutionStep(){}

            /**
             * @brief 在每个时间步中输出结果之前执行
             */
            virtual void ExecuteBeforeOutputStep(){}

            /**
             * @brief 在每个时间步中输出结果之后执行
             */
            virtual void ExecuteAfterOutputStep(){}

            /**
             * @brief 此函数在计算结束时调用
             */
            virtual void ExecuteFinalize(){}

            /**
             * @brief 在ExecuteFinalize之后调用一次，用于验证输入是否正确
             */
            virtual int Check()
            {
                return 0;
            }

            /**
             * @brief 此方法用于清除指派的条件
             */
            virtual void Clear(){}

            /**
             * @brief 提供默认参数，以避免不同构造函数之间的冲突
             */
            virtual const Parameters GetDefaultParameters() const
            {
                QUEST_ERROR << "Calling the base Process class GetDefaultParameters. Please implement the GetDefaultParameters in your derived process class." << std::endl;
                const Parameters default_parameters = Parameters(R"({})" );

                return default_parameters;
            }


            std::string Info() const override
            {
                return "Process";
            }


            void PrintInfo(std::ostream& rOStream) const override
            {
                rOStream << Info();
            }


            void PrintData(std::ostream& rOStream) const override{}

        protected:

        private:
            Process& operator=(const Process& rOther);

        private:
            QUEST_REGISTRY_ADD_PROTOTYPE("Processes.QuestMultiphysics", Process, Process)
            QUEST_REGISTRY_ADD_PROTOTYPE("Processes.All", Process, Process)

    };


    inline std::istream& operator >> (std::istream& rIstream, Process& rThis);

    inline std::ostream& operator << (std::ostream& rOstream, const Process& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }

}

#endif //QUEST_PROCESS_HPP