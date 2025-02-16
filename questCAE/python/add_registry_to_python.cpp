#include "includes/define_python.hpp"
#include "includes/registry.hpp"
#include "python/add_registry_to_python.hpp"
#include "modeler/modeler.hpp"
#include "processes/process.hpp"

namespace Quest::Python{

    namespace
    {
        pybind11::list GetRegistryItemKeys(const RegistryItem& rSelf)
        {
            pybind11::list t;
            KRATOS_ERROR_IF(!rSelf.HasItems()) << "Asking for the keys of " << rSelf.Name() << "which has no subitems." << std::endl;
            for (auto it = rSelf.cbegin(); it != rSelf.cend(); ++it) {
                t.append(it->first);
            }
            return t;
        }

        pybind11::list GetRegistryKeys()
        {
            pybind11::list t;
            for (auto it = Kratos::Registry::cbegin(); it != Kratos::Registry::cend(); ++it) {
                t.append(it->first);
            }
            return t;
        }
    }



    void AddRegistryToPython(pybind11::module& m){
        namespace py = pybind11;

        py::class_<RegistryItem, RegistryItem::Pointer>(m, "RegistryItem")
            .def("Name", &RegistryItem::Name)
            .def("HasItems", &RegistryItem::HasItems)
            .def("HasValue", &RegistryItem::HasValue)
            .def("keys", &GetRegistryItemKeys)
            .def("size", &RegistryItem::size)
            .def("__iter__", [](RegistryItem& rSelf){return py::make_iterator(rSelf.KeyConstBegin(), rSelf.KeyConstEnd());}, py::keep_alive<0,1>())
            .def("__str__", PrintObject<RegistryItem>);

        py::class_<Registry, Registry::Pointer>(m, "CppRegistry")
            .def_static("HasItem", &Registry::HasItem)
            .def_static("HasItems", &Registry::HasItems)
            .def_static("HasValue", &Registry::HasValue)
            .def_static("GetItem", &Registry::GetItem, py::return_value_policy::reference)
            .def_static("GetModeler", &Registry::GetValue<Modeler>, py::return_value_policy::reference)
            .def_static("GetOperation", &Registry::GetValue<Operation>, py::return_value_policy::reference)
            .def_static("GetProcess", &Registry::GetValue<Process>, py::return_value_policy::reference)
            .def_static("RemoveItem", &Registry::RemoveItem)
            .def_static("keys", &GetRegistryKeys)
            .def_static("size", &Registry::size);
    }

}