// 系统头文件
#include <pybind11/pybind11.h>

// 项目头文件
#include "includes/define_python.hpp"
#include "includes/mesh.hpp"
#include "includes/properties.hpp"
#include "includes/element.hpp"
#include "includes/condition.hpp"
#include "python/container_interface.hpp"
#include "python/add_mesh_to_python.hpp"

namespace Quest::Python{
    namespace py = pybind11;

    template<class TVariableType> inline
    void NodeFix(Node& rThisNode, TVariableType const& rThisVariable){
        rThisNode.Fix(rThisVariable);
    }

    template<class TVariableType> inline
    void NodeFree(Node& rThisNode, TVariableType const& rThisVariable){
        rThisNode.Free(rThisVariable);
    }

    template<class TVariableType> inline
    bool NodeIsFixed(Node& rThisNode, TVariableType const& rThisVariable){
        return rThisNode.IsFixed(rThisVariable);
    }

    template<class TVariableType> inline
    bool NodeHasDofFor(Node& rThisNode, TVariableType const& rThisVariable){
        return rThisNode.HasDofFor(rThisVariable);
    }

    template<class TVariableType> inline
    void NodeAddDof(Node& rThisNode, TVariableType const& rThisVariable){
        rThisNode.pAddDof(rThisVariable);
    }

    template<class TVariableType> inline
    void NodeAddDofwithReaction(Node& rThisNode, TVariableType const& rThisVariable,  TVariableType const& rReactionVariable){
        rThisNode.pAddDof(rThisVariable, rReactionVariable);
    }

    template<class TVariableType>
    bool NodeSolutionStepsDataHas(Node& rThisNode, const TVariableType& rThisVariable){
        return rThisNode.SolutionStepsDataHas(rThisVariable);
    }

    void PointSetX0(Node& ThisPoint, double Value){
        ThisPoint.X0() = Value;
    }

    void PointSetY0(Node& ThisPoint, double Value){
        ThisPoint.Y0() = Value;
    }

    void PointSetZ0(Node& ThisPoint, double Value){
        ThisPoint.Z0() = Value;
    }

    double PointGetX0(Node& ThisPoint){
        return ThisPoint.X0();
    }

    double PointGetY0(Node& ThisPoint){
        return ThisPoint.Y0();
    }

    double PointGetZ0(Node& ThisPoint){
        return ThisPoint.Z0();
    }


    template<typename TBinderType, typename TContainerType, typename TVariableType >
    void IndexingUtility(TBinderType& binder){
        binder.def("__contains__", [](const TContainerType& node, const TVariableType& rV){return node.Has(rV);} );
        binder.def("__setitem__", [](TContainerType& node, const TVariableType& rV, const typename TVariableType::Type rValue){node.SetValue(rV, rValue);} );
        binder.def("__getitem__", [](TContainerType& node, const TVariableType& rV){return node.GetValue(rV);} );
        binder.def("Has", [](const TContainerType& node, const TVariableType& rV){return node.Has(rV);} );
        binder.def("SetValue",  [](TContainerType& node, const TVariableType& rV, const typename TVariableType::Type& rValue){node.SetValue(rV, rValue);} );
        binder.def("GetValue", [](TContainerType& node, const TVariableType& rV){return node.GetValue(rV);} );

        binder.def("HasSolutionStepValue", [](const TContainerType& node, const TVariableType& rV){return node.SolutionStepsDataHas(rV);} );
        binder.def("SetSolutionStepValue",  [](TContainerType& node, const TVariableType& rV,const typename TVariableType::Type rValue){node.GetSolutionStepValue(rV) = rValue;} );
        binder.def("SetSolutionStepValue",  [](TContainerType& node,
                                               const TVariableType& rV,
                                               typename TContainerType::IndexType SolutionStepIndex,
                                               const typename TVariableType::Type rValue){node.GetSolutionStepValue(rV, SolutionStepIndex) = rValue;} );
        binder.def("GetSolutionStepValue", [](TContainerType& node, const TVariableType& rV){return node.GetSolutionStepValue(rV);} );
        binder.def("GetSolutionStepValue", [](TContainerType& node, const TVariableType& rV, typename TContainerType::IndexType SolutionStepIndex ){return node.GetSolutionStepValue(rV, SolutionStepIndex);} );
    }


    void AddNodeToPython(pybind11::module& m){
        using MeshType = Mesh<Node, Properties, Element, Condition>;

        py::class_<IndexedObject, IndexedObject::Pointer>(m,"IndexedObject")
            .def_property("Id", &IndexedObject::GetId, &IndexedObject::SetId)
            .def("__str__", PrintObject<IndexedObject>);

        using NodeBinderType = py::class_<Node, Node::Pointer, Node::BaseType, Flags >
        NodeBinderType node_binder(m,"Node");
        node_binder.def(py::init<Node::IndexType, double, double, double>());
        node_binder.def(py::init<Node::IndexType, const Point& >());

        IndexingUtility<NodeBinderType,Node,Variable<bool> >(node_binder);
        IndexingUtility<NodeBinderType,Node,Variable<int> >(node_binder);
        IndexingUtility<NodeBinderType,Node,Variable<double> >(node_binder);
        IndexingUtility<NodeBinderType,Node,Variable<Array1d<double, 3> > >(node_binder);
        IndexingUtility<NodeBinderType,Node,Variable<Array1d<double, 4> > >(node_binder);
        IndexingUtility<NodeBinderType,Node,Variable<Array1d<double, 6> > >(node_binder);
        IndexingUtility<NodeBinderType,Node,Variable<Array1d<double, 9> > >(node_binder);
        IndexingUtility<NodeBinderType,Node,Variable<Quaternion<double> > >(node_binder);
        IndexingUtility<NodeBinderType,Node,Variable<Vector > >(node_binder);
        IndexingUtility<NodeBinderType,Node,Variable<Matrix > >(node_binder);

        node_binder.def("GetBufferSize", &Node::GetBufferSize);
        node_binder.def("AddDof", NodeAddDof<Variable<double> >);
        node_binder.def("AddDof", NodeAddDofwithReaction<Variable<double> >);
        node_binder.def("GetDof",
            [](const Node& rNode, const Variable<double>& rVar) -> Node::DofType& {return *rNode.pGetDof(rVar); }
            ,py::return_value_policy::reference_internal
        );
        node_binder.def("Fix", NodeFix<Variable<double> >);
        node_binder.def("Free", NodeFree<Variable<double> >);
        node_binder.def("IsFixed", NodeIsFixed<Variable<double> >);
        node_binder.def("HasDofFor", NodeHasDofFor<Variable<double> >);
        node_binder.def("SolutionStepsDataHas", &NodeSolutionStepsDataHas<Variable<bool> >);
        node_binder.def("SolutionStepsDataHas", &NodeSolutionStepsDataHas<Variable<int> >);
        node_binder.def("SolutionStepsDataHas", &NodeSolutionStepsDataHas<Variable<double> >);
        node_binder.def("SolutionStepsDataHas", &NodeSolutionStepsDataHas<Variable<Array1d<double, 3> > >);
        node_binder.def("SolutionStepsDataHas", &NodeSolutionStepsDataHas<Variable<Array1d<double, 4> > >);
        node_binder.def("SolutionStepsDataHas", &NodeSolutionStepsDataHas<Variable<Array1d<double, 6> > >);
        node_binder.def("SolutionStepsDataHas", &NodeSolutionStepsDataHas<Variable<Array1d<double, 9> > >);
        node_binder.def("SolutionStepsDataHas", &NodeSolutionStepsDataHas<Variable<Vector> >);
        node_binder.def("SolutionStepsDataHas", &NodeSolutionStepsDataHas<Variable<DenseMatrix<double> > >);
        node_binder.def("__str__", PrintObject<Node>);
        node_binder.def("OverwriteSolutionStepData", &Node::OverwriteSolutionStepData);
        node_binder.def_property("X0", PointGetX0, PointSetX0);
        node_binder.def_property("Y0", PointGetY0, PointSetY0);
        node_binder.def_property("Z0", PointGetZ0, PointSetZ0);
        node_binder.def_property("Id", &Node::GetId, &Node::SetId);

        PointerVectorSetPythonInterface<MeshType::NodesContainerType>().CreateInterface(m,"NodesArray");
    }
}