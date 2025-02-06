#include "includes/define_python.hpp"
#include "includes/parallel_environment.hpp"
#include "IO/logger.hpp"


namespace Quest::Python{
    namespace py = pybind11;

    const DataCommunicator& getDataCommunicator(pybind11::kwargs kwargs) {
        if (kwargs.contains("data_communicator")) {
            const DataCommunicator& r_data_communicator = py::cast<DataCommunicator&>(kwargs["data_communicator"]);
            return r_data_communicator;
        }
        else {
            return ParallelEnvironment::GetDefaultDataCommunicator();
        }
    }

    /** 
     * @brief 使用Logger类打印来自Python脚本的参数
     */
    void printImpl(pybind11::args args, pybind11::kwargs kwargs, Logger::Severity severity, bool useKwargLabel, LoggerMessage::DistributedFilter filterOption) {
        if(len(args) == 0)
            std::cout << "ERROR" << std::endl;

        std::stringstream buffer;
        Logger::Severity severityOption = severity;
        Logger::Category categoryOption = Logger::Category::STATUS;

        std::string label;

        // Get the label
        unsigned int to_skip = 0; //if the kwargs label is false, consider the first entry of the args as the label
        if(useKwargLabel) {
            if(kwargs.contains("label")) {
                label = py::str(kwargs["label"]);
            } else {
                label = "";
            }
        } else {
            label = py::str(args[0]); //if the kwargs label is false, consider the first entry of the args as the label
            to_skip = 1;
        }

        unsigned int counter = 0;
        for(auto item : args)
        {
            if(counter >= to_skip)
            {
                buffer << item;
                if(counter < len(args))
                    buffer << " ";
            }
            counter++;
        }

        // Extract the options
        if(kwargs.contains("severity")) {
            severityOption = py::cast<Logger::Severity>(kwargs["severity"]);
        }

        if(kwargs.contains("category")) {
            categoryOption = py::cast<Logger::Category>(kwargs["category"]);
        }

        // Send the message and options to the logger
        Logger logger(label);
        logger << buffer.str() << severityOption << categoryOption << std::endl;
        logger << filterOption << getDataCommunicator(kwargs);
    }


    bool isPrintingRank(pybind11::kwargs kwargs) {
        const DataCommunicator& r_data_communicator = getDataCommunicator(kwargs);
        return r_data_communicator.Rank() == 0;
    }

    void printDefault(pybind11::args args, pybind11::kwargs kwargs) {
        if (isPrintingRank(kwargs)) {
            printImpl(args, kwargs, Logger::Severity::INFO, true, LoggerMessage::DistributedFilter::FromRoot());
        }
    }

    void printInfo(pybind11::args args, pybind11::kwargs kwargs) {
        if (isPrintingRank(kwargs)) {
            printImpl(args, kwargs, Logger::Severity::INFO, false, LoggerMessage::DistributedFilter::FromRoot());
        }
    }

    void printWarning(pybind11::args args, pybind11::kwargs kwargs) {
        if (isPrintingRank(kwargs)) {
            printImpl(args, kwargs, Logger::Severity::WARNING, false, LoggerMessage::DistributedFilter::FromRoot());
        }
    }

    void printDefaultOnAllRanks(pybind11::args args, pybind11::kwargs kwargs) {
        printImpl(args, kwargs, Logger::Severity::INFO, true, LoggerMessage::DistributedFilter::FromAllRanks());
    }

    void printInfoOnAllRanks(pybind11::args args, pybind11::kwargs kwargs) {
        printImpl(args, kwargs, Logger::Severity::INFO, false, LoggerMessage::DistributedFilter::FromAllRanks());
    }

    void printWarningOnAllRanks(pybind11::args args, pybind11::kwargs kwargs) {
        printImpl(args, kwargs, Logger::Severity::WARNING, false, LoggerMessage::DistributedFilter::FromAllRanks());
    }


    void AddLoggerToPython(pybind11::module& m){
        auto logger_output = py::class_<LoggerOutput, Kratos::shared_ptr<LoggerOutput>>(m,"LoggerOutput")
            .def("SetMaxLevel", &LoggerOutput::SetMaxLevel)
            .def("GetMaxLevel", &LoggerOutput::GetMaxLevel)
            .def("SetSeverity", &LoggerOutput::SetSeverity)
            .def("GetSeverity", &LoggerOutput::GetSeverity)
            .def("SetCategory", &LoggerOutput::SetCategory)
            .def("GetCategory", &LoggerOutput::GetCategory)
            .def("SetOption", &LoggerOutput::SetOption)
            .def("GetOption", &LoggerOutput::GetOption)
            ;

        logger_output.attr("WARNING_PREFIX") = LoggerOutput::WARNING_PREFIX;
        logger_output.attr("INFO_PREFIX") = LoggerOutput::INFO_PREFIX;
        logger_output.attr("DETAIL_PREFIX") = LoggerOutput::DETAIL_PREFIX;
        logger_output.attr("DEBUG_PREFIX") = LoggerOutput::DEBUG_PREFIX;
        logger_output.attr("TRACE_PREFIX") = LoggerOutput::TRACE_PREFIX;


        py::class_<Logger, Kratos::shared_ptr<Logger>> logger_scope(m,"Logger");
        logger_scope.def(py::init<std::string const &>());
        logger_scope.def_static("Print", printDefault); 
        logger_scope.def_static("PrintInfo",printInfo); 
        logger_scope.def_static("PrintWarning", printWarning); 
        logger_scope.def_static("PrintOnAllRanks", printDefaultOnAllRanks);
        logger_scope.def_static("PrintInfoOnAllRanks",printInfoOnAllRanks);
        logger_scope.def_static("PrintWarningOnAllRanks", printWarningOnAllRanks);
        logger_scope.def_static("Flush", Logger::Flush);
        logger_scope.def_static("GetDefaultOutput", &Logger::GetDefaultOutputInstance, py::return_value_policy::reference); //_internal )
        logger_scope.def_static("AddOutput", &Logger::AddOutput);
        logger_scope.def_static("RemoveOutput", &Logger::RemoveOutput);


        py::enum_<Logger::Severity>(logger_scope,"Severity")
            .value("WARNING", Logger::Severity::WARNING)
            .value("INFO", Logger::Severity::INFO)
            .value("DETAIL", Logger::Severity::DETAIL)
            .value("DEBUG", Logger::Severity::DEBUG)
            .value("TRACE", Logger::Severity::TRACE);

        py::enum_<Logger::Category>(logger_scope,"Category")
            .value("STATUS", Logger::Category::STATUS)
            .value("CRITICAL", Logger::Category::CRITICAL)
            .value("STATISTICS", Logger::Category::STATISTICS)
            .value("PROFILING", Logger::Category::PROFILING)
            .value("CHECKING", Logger::Category::CHECKING);
    }
}