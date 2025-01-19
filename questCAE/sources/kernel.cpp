// 系统头文件
#include <iostream>

// 项目头文件
#include "includes/kernel.hpp"
#include "includes/quest_version.hpp"
#include "includes/data_communicator.hpp"
#include "includes/parallel_enviroment.hpp"
#include "IO/logger.hpp"
#include "utilities/parallel_utilities.hpp"

namespace Quest{

    Kernel::Kernel() : mpQuestCoreApplication(Quest::make_shared<QuestApplication>(std::string("QuestMultiphysics"))) {
        Initialize();
    }


    Kernel::Kernel(bool IsDistributedRun) : mpQuestCoreApplication(Quest::make_shared<QuestApplication>(std::string("QuestMultiphysics"))) {
        mIsDistributedRun = IsDistributedRun;
        Initialize();
    }


    void Kernel::Initialize() {
        QUEST_INFO("") << " |  /           |                  \n"
                        << " ' /   __| _` | __|  _ \\   __|    \n"
                        << " . \\  |   (   | |   (   |\\__ \\  \n"
                        << "_|\\_\\_|  \\__,_|\\__|\\___/ ____/\n"
                        << "           Multi-Physics " << Kernel::Version() << "\n"
                        << "           Compiled for "  << Kernel::OSName()  << " and " << Kernel::PythonVersion() << " with " << Kernel::Compiler() << std::endl;

        PrintParallelismSupportInfo();

        if (!IsImported("QuestMultiphysics")) {
            this->ImportApplication(mpQuestCoreApplication);
        }
    }


    std::unordered_set<std::string> &Kernel::GetApplicationsList() {
        static std::unordered_set<std::string> application_list;
        return application_list;
    }


    bool Kernel::IsImported(const std::string& ApplicationName) const {
        if (GetApplicationsList().find(ApplicationName) !=
            GetApplicationsList().end())
            return true;
        else
            return false;
    }


    bool Kernel::IsDistributedRun() {
        return mIsDistributedRun;
    }


    void Kernel::ImportApplication(QuestApplication::Pointer pNewApplication) {
        if (IsImported(pNewApplication->Name()))
            QUEST_ERROR << "importing more than once the application : "
                        << pNewApplication->Name() << std::endl;

        pNewApplication->Register();
        Kernel::GetApplicationsList().insert(pNewApplication->Name());
    }


    std::string Kernel::Info() const { return "kernel"; }


    void Kernel::PrintInfo(std::ostream& rOStream) const { rOStream << "kernel"; }


    void Kernel::PrintData(std::ostream& rOStream) const {
        rOStream << "Variables:" << std::endl;
        QuestComponents<VariableData>().PrintData(rOStream);
        rOStream << std::endl;
        rOStream << "Geometries:" << std::endl;
        QuestComponents<Geometry<Node>>().PrintData(rOStream);
        rOStream << std::endl;
        rOStream << "Elements:" << std::endl;
        QuestComponents<Element>().PrintData(rOStream);
        rOStream << std::endl;
        rOStream << "Conditions:" << std::endl;
        QuestComponents<Condition>().PrintData(rOStream);
        rOStream << std::endl;
        rOStream << "Modelers:" << std::endl;
        QuestComponents<Modeler>().PrintData(rOStream);
        rOStream << std::endl;
        rOStream << "Loaded applications:" << std::endl;
        auto& application_list = Kernel::GetApplicationsList();
        rOStream << "    Number of loaded applications = " << application_list.size() << std::endl;
        for (auto it = application_list.begin(); it != application_list.end(); ++it)
            rOStream << "    " << *it << std::endl;
    }


    std::string Kernel::BuildType() {
        return GetBuildType();
    }


    std::string Kernel::Version() {
        return GetVersionString();
    }


    std::string Kernel::OSName() {
        return GetOSName();
    }


    std::string Kernel::PythonVersion() {
        return mPyVersion;
    }


    std::string Kernel::Compiler() {
        return GetCompiler();
    }


    void Kernel::SetPythonVersion(std::string pyVersion) {
        mPyVersion = pyVersion;
    }


    void Kernel::PrintParallelismSupportInfo() const
    {
        #ifdef QUEST_SMP_NONE
        constexpr bool threading_support = false;
        #else
        constexpr bool threading_support = true;
        #endif

        #ifdef QUEST_USING_MPI
        constexpr bool mpi_support = true;
        #else
        constexpr bool mpi_support = false;
        #endif

        Logger logger("");
        logger << LoggerMessage::Severity::INFO;

        if (threading_support) {
            if (mpi_support) {
                logger << "Compiled with threading and MPI support." << std::endl;
            }
            else {
                logger << "Compiled with threading support." << std::endl;
            }
        }
        else if (mpi_support) {
            logger << "Compiled with MPI support." << std::endl;
        }
        else {
            logger << "Serial compilation." << std::endl;
        }

        if (threading_support) {
            logger << "Maximum number of threads: " << ParallelUtilities::GetNumThreads() << "." << std::endl;
        }

        if (mpi_support) {
            if (mIsDistributedRun) {
                const DataCommunicator& r_world = ParallelEnvironment::GetDataCommunicator("World");
                logger << "MPI world size:         " << r_world.Size() << "." << std::endl;
            }
            else {
                logger << "Running without MPI." << std::endl;
            }
        }
    }

    bool Kernel::mIsDistributedRun = false;
    std::string Kernel::mPyVersion = std::string("Undefined");

}