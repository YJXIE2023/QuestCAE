#include "includes/define_python.hpp"
#include "includes/serializer.hpp"
#include "includes/stream_serializer.hpp"
#include "python/add_serializer_to_python.hpp"
#include "includes/model_part.hpp"
#include "includes/quest_parameters.hpp"
#include "containers/model.hpp"

namespace Quest::Python{
    namespace py = pybind11;

    template< class TObjectType >
    void SerializerSave(Serializer& rSerializer, std::string const & rName, TObjectType& rObject){
        return rSerializer.save(rName, rObject);
    }

    template< class TObjectType >
    void SerializerLoad(Serializer& rSerializer, std::string const & rName, TObjectType& rObject){
        return rSerializer.load(rName, rObject);
    }

    template< class TObjectType >
    void SerializerLoadFromBeginning(Serializer& rSerializer, std::string const & rName, TObjectType& rObject){
        rSerializer.SetLoadState();
        return rSerializer.load(rName, rObject);
    }

    void SerializerPrint(Serializer& rSerializer){
        std::cout << "Serializer buffer:";
        std::cout << ((std::stringstream*)(rSerializer.pGetBuffer()))->str();
    }



    void AddSerializerToPython(pybind11::module& m){
        auto serializer_py_interface = py::class_<Serializer, Serializer::Pointer >(m,"Serializer")
            .def(py::init([](const std::string& FileName) {
                            QUEST_WARNING("DEPRECATION") << "Please use FileSerializer(FileName) instead of Serializer(FileName)" << std::endl;
                            return std::make_shared<FileSerializer>(FileName);
                        }
                    )
                )
            .def(py::init([](const std::string& FileName, Serializer::TraceType& rTraceType) {
                            QUEST_WARNING("DEPRECATION") << "Please use FileSerializer(FileName,TraceType) instead of Serializer(FileName,TraceType)" << std::endl;
                            return std::make_shared<FileSerializer>(FileName,rTraceType);
                        }
                    )
                )
            .def("Load",SerializerLoad<ModelPart>)
            .def("LoadFromBeginning",SerializerLoadFromBeginning<ModelPart>)
            .def("Save",SerializerSave<ModelPart>)

            .def("Load",SerializerLoad<Parameters>)
            .def("LoadFromBeginning",SerializerLoadFromBeginning<Parameters>)
            .def("Save",SerializerSave<Parameters>)

            .def("Load",SerializerLoad<Model>)
            .def("LoadFromBeginning",SerializerLoadFromBeginning<Model>)
            .def("Save",SerializerSave<Model>)

            .def("Load",SerializerLoad<Flags>)
            .def("LoadFromBeginning",SerializerLoadFromBeginning<Flags>)
            .def("Save",SerializerSave<Flags>)

            .def("Set",   &Serializer::Set)
            .def("Print", SerializerPrint);

        serializer_py_interface.attr("MPI") = Serializer::MPI;
        serializer_py_interface.attr("SHALLOW_GLOBAL_POINTERS_SERIALIZATION") = Serializer::SHALLOW_GLOBAL_POINTERS_SERIALIZATION;
    }
}