/*----------------------------------
管理与MPI并行环境的类
删除CreateFillCommunicator函数
-----------------------------------*/

#ifndef PARALLEL_ENVIROMENT_HPP
#define PARALLEL_ENVIROMENT_HPP

// 系统头文件
#include <string>
#include <iostream>
#include <unordered_map>

// 项目头文件
#include "includes/define.hpp"
#include "includes/data_communicator.hpp"
#include "includes/fill_communicator.hpp"

namespace Quest{

    // 抽象基类，为其他管理类提供接口
    class QUEST_API EnvironmentManager{
        public:
            typedef std::unique_ptr<EnvironmentManager> Pointer;

            EnvironmentManager(EnvironmentManager& rOther) = delete;

            virtual ~EnvironmentManager() = default;

            virtual bool IsInitialized() const = 0;

            virtual bool IsFinalized() const = 0;

        protected:

        private:
            EnvironmentManager() = default;

    };

    class QUEST_API ParallelEnvironment{
        public:
            constexpr static bool MakeDefault = true;
            constexpr static bool DoNotMakeDefault = false;

            static DataCommunicator& GetDataCommunicator(const std::string& rName);

            static DataCommunicator& GetDefaultDataCommunicator();

            static void SetDefaultDataCommunicator(const std::string& rName);

            static int GetDefaultRank();

            static int GetDefaultSize();

            static std::string RetrieveResigteredName(const DataCommunicator& rComm);

            static void SetUpMPIEnvironment(EnvironmentManager::Pointer pEnviromentManager);

            template<typename TDataCommunicatorInputType>
            static void RegisterFillCommunicatorFactory(std::function<FillCommunicator::Pointer(ModelPart&,TDataCommunicatorInputType&)> FillCommunicatorFactory);

            template<typename TDataCommunicatorInputType>
            static void RegisterCommunicatorFactory(std::function<Communicator::UniquePointer(ModelPart&,TDataCommunicatorInputType&)> CommunicatorFactory);

            static FillCommunicator::Pointer CreateFillCommunicatorFromGlobalParallelism(ModelPart& rModelPart, const std::string& rDataCommunicatorName);

            static FillCommunicator::Pointer CreateFillCommunicatorFromGlobalParallelism(ModelPart& rModelPart, const DataCommunicator& rDataCommunicator);

            static Communicator::UniquePointer CreateCommunicatorFromGlobalParallelism(ModelPart& rModelPart, const std::string& rDataCommunicatorName);

            static Communicator::UniquePointer CreateCommunicatorFromGlobalParallelism(ModelPart& rModelPart, const DataCommunicator& rDataCommunicator);

            static void RegisterDataCommunicator(const std::string& rName, DataCommunicator::UniquePointer pPrototype, const bool Default = DoNotMakeDefault);

            static void UnregisterDataCommunicator(const std::string& rName);

            static bool HadDataCommunicator(const std::string& rName);

            static std::string GetDefaultDataCommunicatorName();

            static bool MPIIsInitialized();

            static bool MPIIsFinalized();

            static std::string Info();

            static void PrintInfo(std::ostream& rOstream);

            static void PrintData(std::ostream& rOstream);

        protected:

        private:
            ParallelEnvironment();
            ~ParallelEnvironment();

            static void Create();

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

            std::unordered_map<std::string, DataCommunicator::UniquePointer> mDataCommunicators;

            std::unordered_map<std::string, DataCommunicator::UniquePointer>::iterator mDefaultCommunicator;

            std::function<FillCommunicator::Pointer(ModelPart&,const std::string&)> mFillCommunicatorStringFactory;

            std::function<FillCommunicator::Pointer(ModelPart&,const DataCommunicator&)> mFillCommunicatorReferenceFactory;

            std::function<Communicator::UniquePointer(ModelPart&,const std::string&)> mCommunicatorStringFactory;

            std::function<Communicator::UniquePointer(ModelPart&,const DataCommunicator&)> mCommunicatorReferenceFactory;

            ParallelEnvironment(const ParallelEnvironment& rOther) = delete;

            ParallelEnvironment& operator=(const ParallelEnvironment& rOther) = delete;

        private:
            int mDefaultRank;
            int mDefaultSize;

            EnvironmentManager::Pointer mpEnvironmentManager;

            static ParallelEnvironment* mpInstance;
            static bool mDestoryed;

    };

} // namespace Quest


#endif // PARALLEL_ENVIROMENT_HPP