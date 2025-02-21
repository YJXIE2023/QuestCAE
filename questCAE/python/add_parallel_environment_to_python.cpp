#include "python/add_parallel_environment_to_python.hpp"
#include "include/parallel_environment.hpp"

namespace Quest::Python{
    
    void AddParallelEnvironmentToPython(pybind11::module &m){
        namespace py = pybind11;

        py::class_<ParallelEnvironment, std::unique_ptr<ParallelEnvironment, py::nodelete>>(m,"ParallelEnvironment")
            .def_property_readonly("MakeDefault", [](const ParallelEnvironment& self) { return ParallelEnvironment::MakeDefault; })
            .def_property_readonly("DoNotMakeDefault", [](const ParallelEnvironment& self) { return ParallelEnvironment::DoNotMakeDefault; })
            .def_static("UnregisterDataCommunicator", &ParallelEnvironment::UnregisterDataCommunicator)
            .def_static("GetDataCommunicator",&ParallelEnvironment::GetDataCommunicator, py::return_value_policy::reference)
            .def_static("GetDefaultDataCommunicator",&ParallelEnvironment::GetDefaultDataCommunicator, py::return_value_policy::reference)
            .def_static("SetDefaultDataCommunicator",&ParallelEnvironment::SetDefaultDataCommunicator)
            .def_static("GetDefaultRank",&ParallelEnvironment::GetDefaultRank)
            .def_static("GetDefaultSize",&ParallelEnvironment::GetDefaultSize)
            .def_static("HasDataCommunicator",&ParallelEnvironment::HasDataCommunicator)
            .def_static("GetDefaultDataCommunicatorName",&ParallelEnvironment::GetDefaultDataCommunicatorName)
            .def_static("CreateFillCommunicator", [](ModelPart& rModelPart)->FillCommunicator::Pointer{
                QUEST_WARNING("CreateFillCommunicator") << "This function is deprecated, use \"CreateFillCommunicatorFromGlobalParallelism\" instead" << std::endl;
                return ParallelEnvironment::CreateFillCommunicatorFromGlobalParallelism(rModelPart, ParallelEnvironment::GetDefaultDataCommunicator());
            })
            .def_static("CreateFillCommunicatorFromGlobalParallelism", [](ModelPart& rModelPart, const std::string& rDataCommunicatorName)->FillCommunicator::Pointer{return ParallelEnvironment::CreateFillCommunicatorFromGlobalParallelism(rModelPart, rDataCommunicatorName);})
            .def_static("CreateFillCommunicatorFromGlobalParallelism", [](ModelPart& rModelPart, const DataCommunicator& rDataCommunicator)->FillCommunicator::Pointer{return ParallelEnvironment::CreateFillCommunicatorFromGlobalParallelism(rModelPart, rDataCommunicator);})
            .def_static("CreateCommunicatorFromGlobalParallelism", [](ModelPart& rModelPart, const std::string& rDataCommunicatorName)->Communicator::UniquePointer{return ParallelEnvironment::CreateCommunicatorFromGlobalParallelism(rModelPart, rDataCommunicatorName);})
            .def_static("CreateCommunicatorFromGlobalParallelism", [](ModelPart& rModelPart, DataCommunicator& rDataCommunicator)->Communicator::UniquePointer{return ParallelEnvironment::CreateCommunicatorFromGlobalParallelism(rModelPart, rDataCommunicator);})
            .def_static("Info", []() {
                std::stringstream ss;
                ParallelEnvironment::PrintInfo(ss);
                ss << std::endl;
                ParallelEnvironment::PrintData(ss);
                return ss.str();
            });
    }
    
}