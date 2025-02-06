// 系统头文件
#include <pybind11/numpy.h>

// 项目头文件
#include "includes/define_python.hpp"
#include "includes/ublas_interface.hpp"
#include "python/add_sparse_matrices_to_python.hpp"
#include "containers/sparse_graph.hpp"
#include "containers/sparse_contiguous_row_graph.hpp"
#include "containers/csr_matrix.hpp"
#include "containers/system_vector.hpp"


namespace Quest::Python{
    namespace py = pybind11;

    void AddSparseMatricesToPython(pybind11::module& m){
        namespace py = pybind11;
        using IndexType = std::size_t;


        py::class_<SparseGraph<IndexType>, SparseGraph<IndexType>::Pointer >(m, "SparseGraph")
            .def(py::init<>())
            .def(py::init<IndexType>())
            .def("GetComm", &SparseGraph<IndexType>::GetComm, py::return_value_policy::reference_internal)
            .def("Size", &SparseGraph<IndexType>::Size)
            .def("IsEmpty", &SparseGraph<IndexType>::IsEmpty)
            .def("AddEntry", &SparseGraph<IndexType>::AddEntry)
            .def("Finalize", &SparseGraph<IndexType>::Finalize)
            .def("GetGraph", &SparseGraph<IndexType>::GetGraph, py::return_value_policy::reference_internal)
            .def("AddEntries", [](SparseGraph<IndexType>& self, std::vector<IndexType>& indices){
                self.AddEntries(indices);
            })
            .def("AddEntries", [](SparseGraph<IndexType>& self, std::vector<IndexType>& row_indices, std::vector<IndexType>& col_indices){
                self.AddEntries(row_indices, col_indices);
            })
            .def("ExportSingleVectorRepresentation", &SparseGraph<IndexType>::ExportSingleVectorRepresentation)
            .def("__str__", PrintObject<SparseGraph<IndexType>>);



        py::class_<SparseContiguousRowGraph<IndexType>, SparseContiguousRowGraph<IndexType>::Pointer >(m, "SparseContiguousRowGraph")
            .def(py::init<IndexType>())
            .def("GetComm", &SparseContiguousRowGraph<IndexType>::GetComm, py::return_value_policy::reference_internal)
            .def("Size", &SparseContiguousRowGraph<IndexType>::Size)
            .def("AddEntry", &SparseContiguousRowGraph<IndexType>::AddEntry)
            .def("AddEntries", [](SparseContiguousRowGraph<IndexType>& self, std::vector<IndexType>& indices){
                self.AddEntries(indices);
            })
            .def("AddEntries", [](SparseContiguousRowGraph<IndexType>& self, std::vector<IndexType>& row_indices, std::vector<IndexType>& col_indices){
                self.AddEntries(row_indices, col_indices);
            })
            .def("Finalize", &SparseContiguousRowGraph<IndexType>::Finalize)
            .def("GetGraph", &SparseContiguousRowGraph<IndexType>::GetGraph)
            .def("ExportSingleVectorRepresentation", &SparseContiguousRowGraph<IndexType>::ExportSingleVectorRepresentation)
            .def("__str__", PrintObject<SparseContiguousRowGraph<IndexType>>);


        py::class_<CsrMatrix<double,IndexType>, CsrMatrix<double,IndexType>::Pointer >(m, "CsrMatrix")
            .def(py::init<>())
            .def(py::init<const DataCommunicator&>())
            .def(py::init<const CsrMatrix<double,IndexType>&>())
            .def(py::init<SparseGraph<IndexType>&>())
            .def(py::init<SparseContiguousRowGraph<IndexType>&>())
            .def("copy", [](const CsrMatrix<double,IndexType>& self){
                return Kratos::make_shared<CsrMatrix<double,IndexType>>(self);
            })
            .def("GetComm", &CsrMatrix<double,IndexType>::GetComm, py::return_value_policy::reference_internal)
            .def("SetValue", &CsrMatrix<double,IndexType>::SetValue)
            .def("Size1", &CsrMatrix<double,IndexType>::size1)
            .def("Size2", &CsrMatrix<double,IndexType>::size2)
            .def("size1", &CsrMatrix<double,IndexType>::size1)
            .def("size2", &CsrMatrix<double,IndexType>::size2)
            .def("nnz", &CsrMatrix<double,IndexType>::nnz)
            .def("index1_data", [](CsrMatrix<double,IndexType>& self){
                    return py::array_t<IndexType>(
                        self.index1_data().size(),
                        self.index1_data().begin(),
                        py::cast(self) 
                    );
            }, py::return_value_policy::reference_internal)
            .def("index2_data", [](CsrMatrix<double,IndexType>& self){
                    return py::array_t<IndexType>(
                        self.index2_data().size(),
                        self.index2_data().begin(),
                        py::cast(self)
                    );
            }, py::return_value_policy::reference_internal)
            .def("value_data", [](CsrMatrix<double,IndexType>& self){
                    return py::array_t<double>(
                        self.value_data().size(),
                        self.value_data().begin(),
                        py::cast(self)
                    );
            }, py::return_value_policy::reference_internal)
            .def("__setitem__", [](CsrMatrix<double,IndexType>& self, const std::pair<int,int> index, const  double value)
                {
                    const int index_i = index.first;
                    const int index_j = index.second;
                    self(index_i,index_j) = value;
                })
            .def("__getitem__", [](const CsrMatrix<double,IndexType>& self, const std::pair<int,int> index)
                {
                    const int index_i = index.first;
                    const int index_j = index.second;
                    return self(index_i, index_j);
                })        
            .def("SpMV", [](const CsrMatrix<double,IndexType>& rA,const SystemVector<double,IndexType>& x, SystemVector<double,IndexType>& y){
                rA.SpMV(x,y);
            })
            .def("SpMV", [](const CsrMatrix<double,IndexType>& rA,const Vector& x, Vector& y){
                rA.SpMV(x,y);
            })
            .def("__matmul__", [](const CsrMatrix<double,IndexType>& rA,const SystemVector<double,IndexType>& x){
                auto py  = std::make_shared<SystemVector<double,IndexType>>(rA.size1());
                py->SetValue(0.0); 
                rA.SpMV(x,*py);
                return py;
            }, py::is_operator())
            .def("__matmul__", [](const CsrMatrix<double,IndexType>& rA,const Vector& x){
                auto py  = std::make_shared<SystemVector<double,IndexType>>(rA.size1());
                py->SetValue(0.0);
                rA.SpMV(x,*py);
                return py;
            }, py::is_operator())
            .def("TransposeSpMV", [](CsrMatrix<double,IndexType>& rA,SystemVector<double,IndexType>& x, SystemVector<double,IndexType>& y){
                rA.TransposeSpMV(x,y);
            })
            .def("SpMM", [](CsrMatrix<double,IndexType>& rA,CsrMatrix<double,IndexType>& rB){
                return AmgclCSRSpMMUtilities::SparseMultiply(rA,rB);
            })
            .def("__matmul__", [](CsrMatrix<double,IndexType>& rA,CsrMatrix<double,IndexType>& rB){
                return AmgclCSRSpMMUtilities::SparseMultiply(rA,rB);
            }, py::is_operator())
            .def("Transpose", [](CsrMatrix<double,IndexType>& rA){
                return AmgclCSRConversionUtilities::Transpose<double,IndexType>(rA);
            })
            .def("BeginAssemble", &CsrMatrix<double,IndexType>::BeginAssemble)
            .def("FinalizeAssemble", &CsrMatrix<double,IndexType>::FinalizeAssemble)
            .def("Assemble", [](CsrMatrix<double,IndexType>& rA, 
                                Matrix& values, 
                                std::vector<IndexType>& indices){
            rA.Assemble(values,indices);
            })
            .def("Assemble", [](CsrMatrix<double,IndexType>& rA, 
                                Matrix& values, 
                                std::vector<IndexType>& row_indices,
                                std::vector<IndexType>& col_indices){
            rA.Assemble(values,row_indices, col_indices);
            })
            .def("AssembleEntry", [](CsrMatrix<double,IndexType>& rA, double value, IndexType I, IndexType J){
                rA.AssembleEntry(value,I,J);
            })
            .def("ApplyHomogeneousDirichlet", &CsrMatrix<double,IndexType>::ApplyHomogeneousDirichlet<Vector>)
            .def("ApplyHomogeneousDirichlet", &CsrMatrix<double,IndexType>::ApplyHomogeneousDirichlet<SystemVector<double,IndexType>>)
            .def("__str__", PrintObject<CsrMatrix<double,IndexType>>);


        py::class_<SystemVector<double,IndexType>, SystemVector<double,IndexType>::Pointer >(m, "SystemVector", py::buffer_protocol())
            .def(py::init<IndexType>())
            .def(py::init<IndexType, DataCommunicator&>())
            .def(py::init<SparseGraph<IndexType>&>())
            .def(py::init<SparseContiguousRowGraph<IndexType>&>())
            .def(py::init<Vector&>())
            .def("copy", [](const SystemVector<double,IndexType>& self){
                return Kratos::make_shared<SystemVector<double,IndexType>>(self);
            })
            .def("GetComm", &SystemVector<double,IndexType>::GetComm, py::return_value_policy::reference_internal)
            .def("Size", &SystemVector<double,IndexType>::size)
            .def("size", &SystemVector<double,IndexType>::size)
            .def("Clear", &SystemVector<double,IndexType>::Clear)
            .def("SetValue", &SystemVector<double,IndexType>::SetValue)
            .def("__setitem__", [](SystemVector<double,IndexType>& self, const IndexType i, const double value){self[i] = value;} )
            .def("__getitem__", [](const SystemVector<double,IndexType>& self, const IndexType i){return self[i];} )
            .def("Add", &SystemVector<double,IndexType>::Add)
            .def("BeginAssemble", &SystemVector<double,IndexType>::BeginAssemble)
            .def("FinalizeAssemble", &SystemVector<double,IndexType>::FinalizeAssemble)
            .def("BeginAssemble", &SystemVector<double,IndexType>::BeginAssemble)
            .def("Data", [](SystemVector<double,IndexType>& self) -> DenseVector<double>&{
            return self.data();
            }, py::return_value_policy::reference_internal)
            .def("SetData", [](SystemVector<double,IndexType>& self, const DenseVector<double>& other_data) { noalias(self.data()) = other_data; })
            .def("Assign", [](SystemVector<double,IndexType>& self, const SystemVector<double,IndexType>& other_vec){self = other_vec; } )
            .def_buffer([](SystemVector<double,IndexType>& self) -> py::buffer_info {
                return py::buffer_info(
                    &(self.data())[0],                           
                    sizeof(double),                      
                    py::format_descriptor<double>::format(), 
                    1,                          
                    { self.size() },        
                    { sizeof(double)  }       
                );
            })
            .def("Assemble", [](SystemVector<double,IndexType>& self, 
                                Vector& values, 
                                std::vector<IndexType>& indices){
            self.Assemble(values,indices);
            })
            .def("__mul__", [](const SystemVector<double,IndexType>& self, const double factor) 
                {auto paux = std::make_shared<SystemVector<double,IndexType>>(self);
                (*paux) *= factor;
                return paux;}, py::is_operator())
            .def("__rmul__", [](const SystemVector<double,IndexType>& self, const double factor) 
                {auto paux = std::make_shared<SystemVector<double,IndexType>>(self);
                (*paux) *= factor;
                return paux;}, py::is_operator())
            .def("__iadd__", [](SystemVector<double,IndexType>& self, const SystemVector<double,IndexType>& other_vec) -> SystemVector<double,IndexType>& {self += other_vec; return self;}, py::is_operator())
            .def("__isub__", [](SystemVector<double,IndexType>& self, const SystemVector<double,IndexType>& other_vec) -> SystemVector<double,IndexType>& {self -= other_vec;  return self;}, py::is_operator())
            .def("__imul__", [](SystemVector<double,IndexType>& self, const double& value) -> SystemVector<double,IndexType>& { self*=value; return self;}, py::is_operator())
            .def("__itruediv__", [](SystemVector<double,IndexType>& self, const double& value) -> SystemVector<double,IndexType>& { self/=value; return self;}, py::is_operator())
            .def("__add__", [](const SystemVector<double,IndexType>& vec1, const SystemVector<double,IndexType>& vec2){ 
                    auto paux = std::make_shared<SystemVector<double,IndexType>>(vec1);
                    *paux += vec2;
                    return paux;}
                    , py::is_operator())
            .def("__sub__", [](const SystemVector<double,IndexType>& vec1, const SystemVector<double,IndexType>& vec2){ 
                    auto paux = std::make_shared<SystemVector<double,IndexType>>(vec1);
                    *paux -= vec2;
                    return paux;}
                    , py::is_operator())
            .def("__setitem__", [](SystemVector<double,IndexType>& self, const unsigned int i, const double value){self[i] = value;} )
            .def("__getitem__", [](const SystemVector<double,IndexType>& self, const unsigned int i){return self[i];} )
            .def("__str__", PrintObject<SystemVector<double,IndexType>>);

    }
}