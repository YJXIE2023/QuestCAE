#include "includes/define_python.hpp"
#include "includes?process_info.hpp"
#include "python/add_process_info_to_python.hpp"

namespace Quest::Python{

    ProcessInfo::Pointer ProcessInfoGetPreviousSolutionStepInfo(ProcessInfo& rProcessInfo){
        return rProcessInfo.pGetPreviousSolutionStepInfo();
    }

    ProcessInfo::Pointer ProcessInfoGetPreviousTimeStepInfo(ProcessInfo& rProcessInfo){
        return rProcessInfo.pGetPreviousTimeStepInfo();
    }



    void AddProcessInfoToPython(pybind11::module& m){
        namespace py = pybind11;

        py::class_<ProcessInfo, ProcessInfo::Pointer, DataValueContainer, Flags >(m,"ProcessInfo")
            .def(py::init<>())
            .def("CreateSolutionStepInfo", &ProcessInfo::CreateSolutionStepInfo)
            .def("GetPreviousSolutionStepInfo", ProcessInfoGetPreviousSolutionStepInfo)
            .def("GetPreviousTimeStepInfo", ProcessInfoGetPreviousTimeStepInfo)
            .def("__str__", PrintObject<ProcessInfo>);
    }

}