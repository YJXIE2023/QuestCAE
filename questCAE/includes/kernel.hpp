#ifndef QUEST_KERNEL_HPP
#define QUEST_KERNEL_HPP

// 系统头文件
#include <string>
#include <iostream>
#include <unordered_map>

// 项目头文件
#include "includes/define.hpp"
#include "includes/quest_application.hpp"

namespace Quest{

    /**
     * @brief  Quest 内核类，负责同步整个 Quest 系统及其应用程序的各个部分
     * @details Kernel 时系统中第一个被创建的组件，之后将用来将应用程序继承到 Quest 系统中。
     * Kernel 会读取 Quest 中定义的变量列表，并通过每个应用程序添加到 Quest 中，
     * 同步该应用程序和变量与 Quest 的变量，并将新的变量添加到 Quest 中
     * 在添加完所有应用程序后，Kernel 会初始化 Quest，将变量键分配给所有在 Quest 和已添加应用程序的变量
     * 最后，通过调用每个应用程序的 InitializeApplication 方法，在每个应用程序中同步初始化的变量键
     * 
     * 使用 Kernel 的顺序如下：
     * 1、使用默认构造函数创建 Kernel
     * 2、使用 ImportApplication 方法将应用程序添加到 Quest 中
     * 3、使用 Initialize 方法初始化 Quest
     * 4、使用 InitializeApplication 方法初始化每个应用程序
     */
    class QUEST_API(QUEST_CORE) Kernel{
        public:
            QUEST_CLASS_POINTER_DEFINITION(Kernel);

        public:
            /**
             * @brief 默认构造函数
             */
            Kernel();


            /**
             * @brief 构造函数
             */
            Kernel(bool IsDistributedRun);


            /**
             * @brief 复制构造函数
             */
            Kernel(const Kernel& rOther) {}


            /**
             * @brief 析构函数
             */
            virtual ~Kernel() {}


            /**
             * @brief 将应用程序添加到 Quest 中
             * 首先调用新应用程序的祖册方法，以便创建该应用程序的组件列表，然后将其组件列表与 Quest 系统的组件列表同步
             * 同步的组件列表包括变量、单元和边界条件
             */
            void ImportApplication(QuestApplication::Pointer pNewApplication);


            /**
             * @brief 初始化 Quest 变量
             */
            void Initialize();


            /**
             * @brief 初始化并同步每个应用程序中的变量、单元和条件列表
             */
            void InitializeApplication(QuestApplication& NewApplication) {}


            /**
             * @brief 判断某个应用程序是否已经被添加到 Quest 中
             */
            bool IsImported(const std::string& ApplicationName) const;


            /**
             * @brief 判断是否为分布式运行
             */
            static bool IsDistributedRun();


            /**
             * @brief 获取应用程序列表
             */
            static std::unordered_set<std::string>& GetApplicationsList();


            /**
             * @brief 返回版本号
             */
            static std::string Version();


            /**
             * @brief 返回构建类型
             */
            static std::string BuildType();


            /**
             * @brief 返回操作系统的名称
             */
            static std::string OSName();


            /**
             * @brief 返回python版本号
             */
            static std::string PythonVersion();


            /**
             * @brief 返回编译器名称
             */
            static std::string Compiler();


            /**
             * @brief 设置python版本
             */
            static void SetPythonVersion(std::string);


            /**
             * @brief 打印并输出有关并行支持的信息
             */
            void PrintParallelismSupportInfo() const;


            virtual std::string Info() const;

            virtual void PrintInfo(std::ostream& rOStream) const;

            virtual void PrintData(std::ostream& rOStream) const;

        protected:

        private:
            /**
             * @brief 赋值运算符重载
             */
            Kernel& operator = (const Kernel& rOther);

            /**
             * @brief 注册核心变量
             */
            void RegisterQuestCore();

        private:
            /**
             * @brief 应用程序对象指针
             */
            QuestApplication::Pointer mpQuestCoreApplication;


            /**
             * @brief 用于指示当前的计算是否在分布式环境中进行的
             */
            static bool mIsDistributedRun;


            /**
             * @brief 当前使用的 Python 版本号
             */
            static std::string mPyVersion;

    };

    inline std::istream& operator >> (std::istream& rIstream, Kernel& rThis);

    inline std::ostream& operator << (std::ostream& rOstream, const Kernel& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }

}

#endif //QUEST_KERNEL_HPP