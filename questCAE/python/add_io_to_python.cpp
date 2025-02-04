#include "includes/define_python.hpp"
#include "includes/io.hpp"
#include "includes/model_part_io.hpp"
#include "python/add_io_to_python.hpp"
#include "containers/flags.hpp"
#include "IO/unv_output.hpp"


namespace Quest::Python{

    void ReadInitialValues1(IO& IO, IO::NodesContainerType& rThisNodes, IO::ElementsContainerType& rThisElements, IO::ConditionsContainerType& rThisConditions){ IO.ReadInitialValues(rThisNodes, rThisElements, rThisConditions);}
    void ReadInitialValues2(IO& IO, ModelPart& rThisModelPart){ IO.ReadInitialValues(rThisModelPart);}

    void AddIOToPython(pybind11::module& m){
        namespace py = pybind11;

        py::class_<IO, IO::Pointer> io_python_interface = py::class_<IO, IO::Pointer>(m,"IO")
            .def("ReadNode",&IO::ReadNode)
            .def("ReadNodes",&IO::ReadNodes)
            .def("WriteNodes",&IO::WriteNodes)
            .def("ReadProperties",[](IO& self, Properties& rThisProperties){self.ReadProperties(rThisProperties);})
            .def("ReadProperties",[](IO& self,IO::PropertiesContainerType& rThisProperties){self.ReadProperties(rThisProperties);})
            .def("ReadGeometries",&IO::ReadGeometries)
            .def("WriteGeometries",&IO::WriteGeometries)
            .def("ReadElements",&IO::ReadElements)
            .def("WriteElements",&IO::WriteElements)
            .def("ReadConditions",&IO::ReadConditions)
            .def("WriteConditions",&IO::WriteConditions)
            .def("ReadInitialValues",&ReadInitialValues1)
            .def("ReadInitialValues",&ReadInitialValues2)
            .def("ReadMesh",&IO::ReadMesh)
            .def("ReadModelPart",&IO::ReadModelPart)
            .def("WriteModelPart", py::overload_cast<const ModelPart&>(&IO::WriteModelPart));

        io_python_interface.attr("READ") = IO::READ;
        io_python_interface.attr("WRITE") =IO::WRITE;
        io_python_interface.attr("APPEND") = IO::APPEND;
        io_python_interface.attr("IGNORE_VARIABLES_ERROR" ) = IO::IGNORE_VARIABLES_ERROR;
        io_python_interface.attr("SKIP_TIMER" ) = IO::SKIP_TIMER;
        io_python_interface.attr("MESH_ONLY" ) = IO::MESH_ONLY;
        io_python_interface.attr("SCIENTIFIC_PRECISION" ) = IO::SCIENTIFIC_PRECISION;

        py::class_<ModelPartIO, ModelPartIO::Pointer, IO>(m, "ModelPartIO")
            .def(py::init<const std::filesystem::path&>())
            .def(py::init<const std::filesystem::path&, const Flags>());

        py::class_<UnvOutput, UnvOutput::Pointer>(m, "UnvOutput")
            .def(py::init<ModelPart&, const std::string &>())
            .def("InitializeMesh", &UnvOutput::InitializeOutputFile)
            .def("WriteMesh", &UnvOutput::WriteMesh)
            .def("PrintOutput", (void (UnvOutput::*)(const Variable<bool>&, const double)) &UnvOutput::WriteNodalResults)
            .def("PrintOutput", (void (UnvOutput::*)(const Variable<int>&, const double)) &UnvOutput::WriteNodalResults)
            .def("PrintOutput", (void (UnvOutput::*)(const Variable<double>&, const double)) &UnvOutput::WriteNodalResults)
            .def("PrintOutput", (void (UnvOutput::*)(const Variable<Array1d<double,3>>&, const double)) &UnvOutput::WriteNodalResults);
    }

}