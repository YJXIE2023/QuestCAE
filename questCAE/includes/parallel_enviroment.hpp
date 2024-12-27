/*----------------------------------
管理与MPI并行环境的类
删除CreateFillCommunicator函数
-----------------------------------*/

#ifndef QUEST_PARALLEL_ENVIROMENT_HPP
#define QUEST_PARALLEL_ENVIROMENT_HPP

// 系统头文件
#include <string>
#include <iostream>
#include <unordered_map>

// 项目头文件
#include "includes/define.hpp"
#include "includes/data_communicator.hpp"
#include "includes/fill_communicator.hpp"

namespace Quest{

    /**
     * @class ParallelEnvironment
     * @brief 管理MPI并行环境的类
     */
    class QUEST_API(QUEST_CORE) EnvironmentManager{
        public:
            typedef std::unique_ptr<EnvironmentManager> Pointer;

            EnvironmentManager(EnvironmentManager& rOther) = delete;

            virtual ~EnvironmentManager() = default;

            /**
             * @brief 检查MPI环境是否已经初始化
             */
            virtual bool IsInitialized() const = 0;

            /**
             * @brief 检查MPI环境是否已经终止
             */
            virtual bool IsFinalized() const = 0;

        protected:

        private:
            EnvironmentManager() = default;

    };

    /**
     * @class ParallelEnvironment
     * @brief 用于保存与 MPI 相关的一般数据（或在非 MPI 运行中适用的序列化等价物）的容器类
     * @details 此类管理一组 DataCommunicator 的注册表，这些对象可用于执行 MPI 通信操作
     */
    class QUEST_API(QUEST_CORE) ParallelEnvironment{
        public:
            constexpr static bool MakeDefault = true;
            constexpr static bool DoNotMakeDefault = false;

            /**
             * @brief 获取指定名称的 DataCommunicator 对象
             */
            static DataCommunicator& GetDataCommunicator(const std::string& rName);

            /**
             * @brief 获取默认的 DataCommunicator 对象
             */
            static DataCommunicator& GetDefaultDataCommunicator();

            /**
             * @brief 设置默认的 DataCommunicator 对象
             */
            static void SetDefaultDataCommunicator(const std::string& rName);

            /**
             * @brief 获取默认线程号
             */
            static int GetDefaultRank();

            /**
             * @brief 获取默认线程数
             */
            static int GetDefaultSize();

            /**
             * @brief 获取默认 DataCommunicator 提供的 MPI 通信器大小
             */
            static std::string RetrieveResigteredName(const DataCommunicator& rComm);

            /**
             * @brief 建立 MPI 环境
             */
            static void SetUpMPIEnvironment(EnvironmentManager::Pointer pEnviromentManager);

            /**
             * @brief 注册填充通信器工厂
             * 此方法接收提供的填充通信器指针工厂并将其保存，以便稍后使用
             * @param CommunicatorFactory 工厂函数，返回一个指向（并行或串行）填充通信器的指针
             */
            template<typename TDataCommunicatorInputType>
            static void RegisterFillCommunicatorFactory(std::function<FillCommunicator::Pointer(ModelPart&,TDataCommunicatorInputType&)> FillCommunicatorFactory);

            /**
             * @brief 注册通信器工厂
             * 此方法接收提供的通信器指针工厂并将其保存，以便稍后使用
             * @param CommunicatorFactory 工厂函数，返回一个指向（并行或串行）通信器的指针
             */
            template<typename TDataCommunicatorInputType>
            static void RegisterCommunicatorFactory(std::function<Communicator::UniquePointer(ModelPart&,TDataCommunicatorInputType&)> CommunicatorFactory);

            /**
             * @brief 创建一个填充通信器对象
             * 此方法使用先前注册的填充通信器工厂来创建一个新的填充通信器指针
             * @param rModelPart 将应用填充通信器的模型部分
             * @param rDataCommunicatorName 用于填充通信器构造的数据通信器名称
             * @return FillCommunicator::Pointer 指向新填充通信器实例的指针
             */
            static FillCommunicator::Pointer CreateFillCommunicatorFromGlobalParallelism(ModelPart& rModelPart, const std::string& rDataCommunicatorName);

            /**
             * @brief 创建一个填充通信器对象
             * 此方法使用先前注册的填充通信器工厂来创建一个新的填充通信器指针
             * @param rModelPart 将应用填充通信器的模型部分
             * @param rDataCommunicator 构造填充通信器时使用的数据通信器的引用
             * @return FillCommunicator::Pointer 指向新填充通信器实例的指针
             */
            static FillCommunicator::Pointer CreateFillCommunicatorFromGlobalParallelism(ModelPart& rModelPart, const DataCommunicator& rDataCommunicator);

            /**
             * @brief 创建一个通信器对象
             * 此方法使用先前注册的通信器工厂来创建一个新的通信器指针
             * @param rModelPart 用于从中检索变量列表的模型部分
             * @param rDataCommunicatorName 构造通信器时要检索的数据通信器的名称
             * @return Communicator::UniquePointer 指向新通信器的唯一指针
             */
            static Communicator::UniquePointer CreateCommunicatorFromGlobalParallelism(ModelPart& rModelPart, const std::string& rDataCommunicatorName);

            /**
             * @brief 创建一个通信器对象
             * 此方法使用先前注册的通信器工厂来创建一个新的通信器指针
             * @param rModelPart 用于从中检索变量列表的模型部分
             * @param rDataCommunicator 用于构造通信器的数据通信器的引用
             * @return Communicator::UniquePointer 指向新通信器的唯一指针
             */
            static Communicator::UniquePointer CreateCommunicatorFromGlobalParallelism(ModelPart& rModelPart, const DataCommunicator& rDataCommunicator);

            /** 
             * @brief 添加一个 DataCommunicator 实例到 ParallelEnvironment 中
             * @param rName 用于在 ParallelEnvironment 中标识 DataCommunicator 的名称。
             * @param rPrototype DataCommunicator 实例。
             * @param Default 如果设置为 ParallelEnvironment::MakeDefault（即 true），则提供的 DataCommunicator 也将被设置为默认值。
             */
            static void RegisterDataCommunicator(const std::string& rName, DataCommunicator::UniquePointer pPrototype, const bool Default = DoNotMakeDefault);

            /**
             * @brief 注销一个 DataCommunicator 实例
             * @param rName 用于在 ParallelEnvironment 中标识 DataCommunicator 的名称。
             */
            static void UnregisterDataCommunicator(const std::string& rName);

            /**
             * @brief 检查是否存在指定名称的 DataCommunicator 对象
             */
            static bool HadDataCommunicator(const std::string& rName);

            /**
             * @brief 获取默认数据通信器名称
             */
            static std::string GetDefaultDataCommunicatorName();

            /**
             * @brief 检查 MPI 环境是否已经初始化
             */
            static bool MPIIsInitialized();

            /**
             * @brief 检查 MPI 环境是否已经终止
             */
            static bool MPIIsFinalized();

            static std::string Info();

            static void PrintInfo(std::ostream& rOstream);

            static void PrintData(std::ostream& rOstream);

        protected:

        private:
            ParallelEnvironment();
            ~ParallelEnvironment();

            /**
             * @brief 创建 ParallelEnvironment 单例
             */
            static void Create();

            /**
             * @brief 创建MPI环境的细节
             */
            void SetupMPIEnvironmentDetail(EnvironmentManager::Pointer pEnviromentManager);

            template<typename TDataCommunicatorInputType>
            void RegisterFillCommunicatorFactoryDetail(std::function<FillCommunicator::Pointer(ModelPart&,TDataCommunicatorInputType&)> FillCommunicatorFactory);

            template<typename TDataCommunicatorInputType>
            void ResigterDataCommunicatorFactoryDetail(std::function<Communicator::UniquePointer(ModelPart&,TDataCommunicatorInputType&)> CommunicatorFactory);

            void RegisterDataCommunicatorFactoryDetail(const std::string& Name, DataCommunicator::UniquePointer pPrototype, const bool Default = DoNotMakeDefault);

            void UnregisterDataCommunicatorDetail(const std::string& Name);

            DataCommunicator& GetDataCommunicatorDetail(const std::string& rName) const;

            DataCommunicator& GetDefaultDataCommunicatorDetail() const;

            void SetDefaultDataCommunicatorDetail(const std::string& rName);

            bool HasDataCommunicatorDetail(const std::string& rName) const;

            bool MPIIsInitializedDetail() const;

            bool MPIIsFinalizedDetail() const;

            static ParallelEnvironment& GetInstance();

            void SetAsDefault(std::unordered_map<std::string, DataCommunicator::UniquePointer>::iterator& rThisCommunicator);

            std::string InfoDetail() const;

            void PrintInfoDetail(std::ostream& rOstream) const;

            void PrintDataDetail(std::ostream& rOstream) const;

            ParallelEnvironment(const ParallelEnvironment& rOther) = delete;

            ParallelEnvironment& operator=(const ParallelEnvironment& rOther) = delete;

        private:
            /**
             * @brief 默认线程号
             */
            int mDefaultRank;

            /**
             * @brief 默认线程数
             */
            int mDefaultSize;

            /**
             * @brief MPI环境管理器
             */
            EnvironmentManager::Pointer mpEnvironmentManager;

            /**
             * @brief 单例模式
             */
            static ParallelEnvironment* mpInstance;

            /**
             * @brief 是否已经销毁
             */
            static bool mDestoryed;

            /**
             * @brief 数据通信器注册表
             */
            std::unordered_map<std::string, DataCommunicator::UniquePointer> mDataCommunicators;

            /**
             * @brief 默认数据通信器
             */
            std::unordered_map<std::string, DataCommunicator::UniquePointer>::iterator mDefaultCommunicator;

            /**
             * @brief 填充通信器字符串工厂
             */
            std::function<FillCommunicator::Pointer(ModelPart&,const std::string&)> mFillCommunicatorStringFactory;

            /**
             * @brief 填充通讯器引用工厂
             */
            std::function<FillCommunicator::Pointer(ModelPart&,const DataCommunicator&)> mFillCommunicatorReferenceFactory;

            /**
             * @brief 通讯器字符串工厂
             */
            std::function<Communicator::UniquePointer(ModelPart&,const std::string&)> mCommunicatorStringFactory;

            /**
             * @brief 通讯器引用工厂
             */
            std::function<Communicator::UniquePointer(ModelPart&,const DataCommunicator&)> mCommunicatorReferenceFactory;

    };

} // namespace Quest


#endif // PARALLEL_ENVIROMENT_HPP