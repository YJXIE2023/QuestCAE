// 系统头文件
#include <sstream>

// 项目头文件
#include "includes/kernel.hpp"
#include "includes/define_python.hpp"
#include "python/add_kernel_to_python.hpp"

// 宏定义
#ifndef QUEST_TO_STRING_
    #define QUEST_TO_STRING_(X) #X
#endif
#ifndef QUEST_TO_STRING
    #define QUEST_TO_STRING(X) QUEST_TO_STRING_(X)
#endif

// python版本宏
#if defined(PYTHON_VERSION_MAJOR) && defined(PYTHON_VERSION_MINOR)
    #define QUEST_PYTHON_VERSION "Python" \
    QUEST_TO_STRING(PYTHON_VERSION_MAJOR) \
    "." \
    QUEST_TO_STRING(PYTHON_VERSION_MINOR)
#else
    #define QUEST_PYTHON_VERSION "Unknown Python Version"
#endif


namespace Quest{

    bool HasFlag(Kernel& rKernel, const std::string& flag_name){
        return QuestComponents<Flags>::Has(flag_name);
    }


    Flags GetFlag(Kernel& rKernel, const std::string& flag_name){
        return QuestComponents<Flags>::Get(flag_name);
    }


    template <class TVariableType>
    bool HasVariable(Kernel& rKernel, const std::string& variable_name){
        return QuestComponents<TVariableType>::Has(variable_name);
    }


    template <class TVariableType>
    const TVariableType& GetVariable(Kernel& rKernel, const std::string& variable_name){
        if (QuestComponents<TVariableType>::Has(variable_name)) {
            return QuestComponents<TVariableType>::Get(variable_name);
        }

        return TVariableType::StaticObject();
    }


    bool HasConstitutiveLaw(Kernel& rKernel, const std::string& constitutive_law_name)
    {
        return QuestComponents<ConstitutiveLaw>::Has(constitutive_law_name);
    }


    const ConstitutiveLaw& GetConstitutiveLaw(Kernel& rKernel, const std::string& constitutive_law_name)
    {
        return QuestComponents<ConstitutiveLaw>::Get(constitutive_law_name);
    }


    template <class TVariableType>
    void PrintVariablesName(Kernel& rKernel)
    {
        QuestComponents<TVariableType> kratos_components;
        kratos_components.PrintData(std::cout);
    }


    template <class TVariableType>
    std::string GetVariableNames(Kernel& rKernel)
    {
        QuestComponents<TVariableType> kratos_components;
        std::stringstream buffer;
        kratos_components.PrintData(buffer);
        return buffer.str();
    }


    void AddKernelToPython(pybind11::module& m){
        namespace py = pybind11;

        py::class_<Kernel, Kernel::Pointer>(m,"Kernel")
            .def(py::init<>())
            .def(py::init<bool>())
            .def("Initialize", [](Kernel& self){ self.Initialize(); })
            .def("ImportApplication", &Kernel::ImportApplication)
            .def("InitializeApplication",  [](Kernel& self, QuestApplication& App){ self.Initialize(); })
            .def("IsImported", &Kernel::IsImported)
            .def_static("IsDistributedRun", &Kernel::IsDistributedRun)
            .def("HasFlag", HasFlag)
            .def("GetFlag", GetFlag)
            .def("HasBoolVariable", HasVariable<Variable<bool> >)
            .def("GetBoolVariable", GetVariable<Variable<bool> >, py::return_value_policy::reference_internal)
            .def("HasIntVariable", HasVariable<Variable<int> >)
            .def("GetIntVariable", GetVariable<Variable<int> >, py::return_value_policy::reference_internal)
            .def("HasUnsignedIntVariable", HasVariable<Variable<unsigned int> >)
            .def("GetUnsignedIntVariable", GetVariable<Variable<unsigned int> >, py::return_value_policy::reference_internal)
            .def("HasDoubleVariable", HasVariable<Variable<double> >)
            .def("GetDoubleVariable", GetVariable<Variable<double> >, py::return_value_policy::reference_internal)
            .def("HasArrayVariable", HasVariable<Variable<Array1d<double, 3> > >)
            .def("HasArray4Variable", HasVariable<Variable<Array1d<double, 4> > >)
            .def("HasArray6Variable", HasVariable<Variable<Array1d<double, 6> > >)
            .def("HasArray9Variable", HasVariable<Variable<Array1d<double, 9> > >)
            .def("GetArrayVariable", GetVariable<Variable<Array1d<double, 3> > >, py::return_value_policy::reference_internal)
            .def("GetArray4Variable", GetVariable<Variable<Array1d<double, 4> > >, py::return_value_policy::reference_internal)
            .def("GetArray6Variable", GetVariable<Variable<Array1d<double, 6> > >, py::return_value_policy::reference_internal)
            .def("GetArray9Variable", GetVariable<Variable<Array1d<double, 9> > >, py::return_value_policy::reference_internal)
            .def("HasVectorVariable", HasVariable<Variable<Vector> >)
            .def("GetVectorVariable", GetVariable<Variable<Vector> >, py::return_value_policy::reference_internal)
            .def("HasMatrixVariable", HasVariable<Variable<Matrix> >)
            .def("GetMatrixVariable", GetVariable<Variable<Matrix> >, py::return_value_policy::reference_internal)
            .def("HasStringVariable", HasVariable<Variable<std::string> >)
            .def("GetStringVariable", GetVariable<Variable<std::string> >, py::return_value_policy::reference_internal)
            .def("HasFlagsVariable", HasVariable<Variable<Flags> >)
            .def("GetFlagsVariable", GetVariable<Variable<Flags> >, py::return_value_policy::reference_internal)
            .def("HasVariableData", HasVariable<VariableData>)
            .def("PrintAllVariables", PrintVariablesName<VariableData>)
            .def("PrintBoolVariables", PrintVariablesName<Variable<bool> >)
            .def("PrintIntVariables", PrintVariablesName<Variable<int> >)
            .def("PrintUnsignedIntVariables", PrintVariablesName<Variable<int> >)
            .def("PrintDoubleVariables", PrintVariablesName<Variable<double> >)
            .def("PrintArrayVariables",PrintVariablesName<Variable<Array1d<double, 3> > >)
            .def("PrintArray4Variables",PrintVariablesName<Variable<Array1d<double, 4> > >)
            .def("PrintArray6Variables",PrintVariablesName<Variable<Array1d<double, 6> > >)
            .def("PrintArray9Variables",PrintVariablesName<Variable<Array1d<double, 9> > >)
            .def("PrintVectorVariables", PrintVariablesName<Variable<Vector> >)
            .def("PrintMatrixVariables", PrintVariablesName<Variable<Matrix> >)
            .def("PrintStringVariables", PrintVariablesName<Variable<std::string> >)
            .def("PrintFlagsVariables", PrintVariablesName<Variable<Flags> >)
            .def("GetAllVariableNames", GetVariableNames<VariableData>)
            .def("GetBoolVariableNames", GetVariableNames<Variable<bool> >)
            .def("GetIntVariableNames", GetVariableNames<Variable<int> >)
            .def("GetUnsignedIntVariableNames", GetVariableNames<Variable<int> >)
            .def("GetDoubleVariableNames", GetVariableNames<Variable<double> >)
            .def("GetArrayVariableNames",GetVariableNames<Variable<Array1d<double, 3> > >)
            .def("GetArrayVariableNames",GetVariableNames<Variable<Array1d<double, 4> > >)
            .def("GetArrayVariableNames",GetVariableNames<Variable<Array1d<double, 6> > >)
            .def("GetArrayVariableNames",GetVariableNames<Variable<Array1d<double, 9> > >)
            .def("GetVectorVariableNames", GetVariableNames<Variable<Vector> >)
            .def("GetMatrixVariableNames", GetVariableNames<Variable<Matrix> >)
            .def("GetStringVariableNames", GetVariableNames<Variable<std::string> >)
            .def("GetFlagsVariableNames", GetVariableNames<Variable<Flags> >)
            .def("__str__", PrintObject<Kernel>)
            .def("HasConstitutiveLaw", HasConstitutiveLaw)
            .def("GetConstitutiveLaw", GetConstitutiveLaw, py::return_value_policy::reference_internal)
            .def_static("Version", &Kernel::Version)
            .def_static("BuildType", &Kernel::BuildType)
            .def_static("RegisterPythonVersion", [](){Kernel::SetPythonVersion(QUEST_PYTHON_VERSION);})
            .def_static("OSName", &Kernel::OSName)
            .def_static("PythonVersion", &Kernel::PythonVersion)
            .def_static("Compiler", &Kernel::Compiler);
    }

}