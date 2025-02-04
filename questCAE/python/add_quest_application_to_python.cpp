#include "includes/define_python.hpp"
#include "includes/quest_application.hpp"
#include "python/add_quest_application_to_python.hpp"

namespace Quest{

    namespace py = pybind11;

    void RegisterToPythonApplicationVariables(std::string ApplicationName){
        auto comp = QuestComponents<VariableData>::GetComponents();
        auto m = pybind11::module::import(ApplicationName.c_str()); 

        for(auto item = comp.begin(); item!=comp.end(); item++)
        {
            auto& var = (item->second);
            std::string name = item->first;

            m.attr(name.c_str()) = var;
        }
    }


    void AddQuestApplicationToPython(pybind11::module& m){
        py::class_<QuestApplication, QuestApplication::Pointer>(m,"QuestApplication")
            .def(py::init<std::string>())
            .def("Register", [](QuestApplication& self){
                std::cout << "*************************************" << std::endl;
                std::cout << "application name = " << self.Name() << std::endl;
                self.Register();
                RegisterToPythonApplicationVariables(self.Name());
            })
            .def("__str__", PrintObject<QuestApplication>);
    }

}