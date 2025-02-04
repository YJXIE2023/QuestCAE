#include "includes/define_python.hpp"
#include "includes/quest_parameters.hpp"
#include "python/add_quest_parameters_to_python.hpp"

namespace Quest{

    pybind11::list items(const Parameters& self){
        pybind11::list t;
        for(Parameters::const_iterator it=self.begin(); it!=self.end(); ++it)
            t.append( std::make_tuple(it.name(), *it) );
        return t;
    }


    pybind11::list keys(const Parameters& self){
        pybind11::list t;
        for(Parameters::const_iterator it=self.begin(); it!=self.end(); ++it)
            t.append(it.name());
        return t;
    }


    pybind11::list values(const Parameters& self){
        pybind11::list t;
        for(Parameters::const_iterator it=self.begin(); it!=self.end(); ++it)
            t.append(*it);
        return t;
    }


    template<typename T>
    void Append(Parameters &rParameters, const T& obj) {
        rParameters.Append(obj);
    }


    Parameters GetValue(Parameters &rParameters, const std::string& rEntry) {
        return rParameters.GetValue(rEntry);
    }


    Parameters GetArrayItem(Parameters &rParameters, const std::size_t Index) {
        return rParameters.GetArrayItem(Index);
    }



    void AddQuestParametersToPython(pybind11::module& m){
        namespace py = pybind11;

        py::class_<Parameters, Parameters::Pointer >(m,"Parameters")
            .def(py::init<>())
            .def(py::init<const std::string&>())
            .def(py::init<std::ifstream&>())
            .def(py::init<const Parameters&>())
            .def("__copy__", [](const Parameters& self) {auto a = Quest::make_shared<Parameters>(self); return a;})
            .def("__copy__", [](const Parameters& self, py::object memo) {auto a = Quest::make_shared<Parameters>(self); return a;})
            .def("__deepcopy__", [](const Parameters& self) {auto a = Quest::make_shared<Parameters>(); Parameters& r_a = *a;r_a = self; return a;})
            .def("__deepcopy__", [](const Parameters& self, py::object memo) {auto a = Quest::make_shared<Parameters>(); Parameters& r_a = *a;r_a = self; return a;})
            .def("WriteJsonString", &Parameters::WriteJsonString)
            .def("PrettyPrintJsonString", &Parameters::PrettyPrintJsonString)
            .def("Has", &Parameters::Has)
            .def("Clone", &Parameters::Clone)
            .def("AddValue", &Parameters::AddValue)
            .def("AddEmptyValue", &Parameters::AddEmptyValue)
            .def("AddEmptyArray", &Parameters::AddEmptyArray)
            .def("RemoveValue", &Parameters::RemoveValue)
            .def("RemoveValues", &Parameters::RemoveValues)
            .def("ValidateAndAssignDefaults", &Parameters::ValidateAndAssignDefaults)
            .def("RecursivelyValidateAndAssignDefaults", &Parameters::RecursivelyValidateAndAssignDefaults)
            .def("AddMissingParameters", &Parameters::AddMissingParameters)
            .def("RecursivelyAddMissingParameters", &Parameters::RecursivelyAddMissingParameters)
            .def("ValidateDefaults", &Parameters::ValidateDefaults)
            .def("RecursivelyValidateDefaults", &Parameters::RecursivelyValidateDefaults)
            .def("IsEquivalentTo",&Parameters::IsEquivalentTo)
            .def("HasSameKeysAndTypeOfValuesAs",&Parameters::HasSameKeysAndTypeOfValuesAs)
            .def("IsNull", &Parameters::IsNull)
            .def("IsNumber", &Parameters::IsNumber)
            .def("IsDouble", &Parameters::IsDouble)
            .def("IsInt", &Parameters::IsInt)
            .def("IsBool", &Parameters::IsBool)
            .def("IsString", &Parameters::IsString)
            .def("IsStringArray", &Parameters::IsStringArray)
            .def("IsArray", &Parameters::IsArray)
            .def("IsVector", &Parameters::IsVector)
            .def("IsMatrix", &Parameters::IsMatrix)
            .def("IsSubParameter", &Parameters::IsSubParameter)
            .def("GetDouble", &Parameters::GetDouble)
            .def("GetInt", &Parameters::GetInt)
            .def("GetBool", &Parameters::GetBool)
            .def("GetString", &Parameters::GetString)
            .def("GetStringArray", &Parameters::GetStringArray)
            .def("GetVector", &Parameters::GetVector)
            .def("GetMatrix", &Parameters::GetMatrix)
            .def("SetDouble", &Parameters::SetDouble)
            .def("SetInt", &Parameters::SetInt)
            .def("SetBool", &Parameters::SetBool)
            .def("SetString", &Parameters::SetString)
            .def("SetStringArray", &Parameters::SetStringArray)
            .def("SetVector", &Parameters::SetVector)
            .def("SetMatrix", &Parameters::SetMatrix)
            .def("AddDouble", &Parameters::AddDouble)
            .def("AddInt", &Parameters::AddInt)
            .def("AddBool", &Parameters::AddBool)
            .def("AddString", &Parameters::AddString)
            .def("AddStringArray", &Parameters::AddStringArray)
            .def("AddVector", &Parameters::AddVector)
            .def("AddMatrix", &Parameters::AddMatrix)
            .def("size", &Parameters::size)
            .def("__setitem__", &Parameters::SetValue)
            .def("__getitem__", GetValue)
            .def("__setitem__", &Parameters::SetArrayItem)
            .def("__getitem__" , GetArrayItem)
            .def("__iter__", [](Parameters& self){ return py::make_iterator(self.begin(), self.end()); } , py::keep_alive<0,1>())
            .def("items", &items )
            .def("keys", &keys )
            .def("values", &values )
            .def("__str__", PrintObject<Parameters>)
            .def("AddEmptyList", &Parameters::AddEmptyArray)
            .def("Append", Append<int>) 
            .def("Append", Append<bool>)
            .def("Append", Append<double>) 
            .def("Append", Append<Vector>) 
            .def("Append", Append<Matrix>) 
            .def("Append", Append<std::string>)
            .def("Append", Append<Parameters>)
            .def("CopyValuesFromExistingParameters", &Parameters::CopyValuesFromExistingParameters);
    }

}