#include "includes/define_python.hpp"
#include "includes/model.hpp"
#include "python/add_model_to_python.hpp"

namespace Quest{

    ModelPart& Model_GetModelPart(Model& rModel, const std::string& rFullModelPartName){
        return rModel.GetModelPart(rFullModelPartName);
    }

    void AddModelToPython(pybind11::module& m){
        namespace py = pybind11;

        py::class_<Model>(m, "Model")
            .def(py::init<>())
            .def("Reset", &Model::Reset)
            .def("CreateModelPart", [&](Model &self, const std::string &Name) { return &self.CreateModelPart(Name); }, py::return_value_policy::reference_internal)
            .def("CreateModelPart", [&](Model &self, const std::string &Name, unsigned int BufferSize) { return &self.CreateModelPart(Name, BufferSize); }, py::return_value_policy::reference_internal)
            .def("DeleteModelPart", &Model::DeleteModelPart)
            .def("GetModelPart", &Model_GetModelPart, py::return_value_policy::reference_internal)
            .def("HasModelPart", &Model::HasModelPart)
            .def("GetModelPartNames", &Model::GetModelPartNames)
            .def("__getitem__", &Model_GetModelPart, py::return_value_policy::reference_internal)
            .def("__str__", PrintObject<Model>);
    }

}