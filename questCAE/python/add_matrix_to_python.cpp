// 系统文件夹
#include <pybind11/complex.h>

// 项目头文件
#include "includes/define_python.hpp"
#include "includes/ublas_interface.hpp"
#include "includes/ublas_complex_interface.hpp"
#include "python/add_matrix_to_python.hpp"
#include "container/array_1d.hpp"

namespace Quest::Python{
    namespace py = pybind11;

    template<typename TMatrixType>
    py::class_<TMatrixType> CreateMatrixInterface(pybind11::module& m, std::string Name ){
        py::class_<TMatrixType, Quest::shared_ptr<TMatrixType> > binder(m,Name.c_str(),py::buffer_protocol());

        binder.def(py::init<>());
        binder.def(py::init<const TMatrixType&>());

        binder.def("Size1", [](const TMatrixType& self){return self.size1();} );
        binder.def("Size2", [](const TMatrixType& self){return self.size2();} );
        binder.def("Resize", [](TMatrixType& self, const typename TMatrixType::size_type new_size1, const typename TMatrixType::size_type new_size2)
                            {if(self.size1() != new_size1 || self.size2() != new_size2)
                                self.resize(new_size1, new_size2, false);} );
        binder.def("__len__", [](const TMatrixType& self){return self.size1() * self.size2();} );
        binder.def("__setitem__", [](TMatrixType& self, const std::pair<int,int> index, const  typename TMatrixType::value_type value)
                                    {
                                        const int index_i = index.first;
                                        const int index_j = index.second;
                                        self(index_i,index_j) = value;
                                    } );
        binder.def("__getitem__", [](const TMatrixType& self, const std::pair<int,int> index)
                                    {
                                        const int index_i = index.first;
                                        const int index_j = index.second;
                                        return self(index_i, index_j);} );

        binder.def("__add__", [](const TMatrixType& m1, const TMatrixType& m2){ return TMatrixType(m1+m2);}, py::is_operator());
        binder.def("__sub__", [](const TMatrixType& m1, const TMatrixType& m2){ return TMatrixType(m1-m2);}, py::is_operator());
        binder.def("__mul__", [](const TMatrixType& m1, const TMatrixType& m2){ return TMatrixType(prod(m1,m2));}, py::is_operator());
        binder.def("__mul__", [](const TMatrixType& m1, const typename TMatrixType::value_type& value){ return TMatrixType(m1*value);}, py::is_operator());
        binder.def("__truediv__", [](const TMatrixType& m1, const typename TMatrixType::value_type& value){ return TMatrixType(m1/value);}, py::is_operator());

        binder.def("__imul__", [](TMatrixType& m1, const typename TMatrixType::value_type& value){ m1*=value; return m1;}, py::is_operator());
        binder.def("__itruediv__", [](TMatrixType& m1, const typename TMatrixType::value_type& value){ m1/=value; return m1;}, py::is_operator());
        binder.def("__str__", PrintObject<TMatrixType>);
        return std::move(binder);
    }


    void AddMatrixToPython(pybind11::module& m){
        auto matrix_binder = CreateMatrixInterface< DenseMatrix<double> >(m,"Matrix");

        matrix_binder.def(py::init<const DenseMatrix<double>::size_type, const DenseMatrix<double>::size_type>());
        matrix_binder.def(py::init( [](py::buffer b){
            py::buffer_info info = b.request();
            QUEST_ERROR_IF( info.format != py::format_descriptor<double>::value ) << "Expected a double array\n";
            QUEST_ERROR_IF( info.ndim != 2 ) << "Buffer dimension of 2 is required, got: " << info.ndim << std::endl;
            DenseMatrix<double> matrix = DenseMatrix<double>(info.shape[0], info.shape[1]);
            std::array<std::size_t,2> strides {info.strides[0] / static_cast<std::size_t>(info.itemsize),
                                                info.strides[1] / static_cast<std::size_t>(info.itemsize)};

            for( int i=0; i<info.shape[0]; ++i ) {
                for( int j=0; j<info.shape[1]; ++j ) {
                matrix(i,j) = static_cast<double *>(info.ptr)[i * strides[0] + j * strides[1]];
                }
            }

            return matrix;
        }));

        matrix_binder.def(py::init([](const py::list& input){
            std::size_t num_rows = input.size();
            if( num_rows == 0 || ( (num_rows == 1) && (py::len(input[0]) == 0) ) )
                return DenseMatrix<double>(0,0);
            else{
                std::size_t num_cols = py::len(input[0]);
                DenseMatrix<double>matrix = DenseMatrix<double>(num_rows, num_cols);
                for(std::size_t i = 0; i < num_rows; i++){
                const auto row = py::cast<py::list>(input[i]);
                QUEST_ERROR_IF( py::len(row) != num_cols ) << "Wrong size of a row " << i << "! Expected " << num_cols << ", got " << py::len(row) << std::endl;;
                for(std::size_t j = 0; j < num_cols; j++){
                    matrix(i,j) = py::cast<double>(row[j]);
                }
                }
                return matrix;
            }
        }));

        matrix_binder.def(py::init<const DenseMatrix<double>::size_type, const DenseMatrix<double>::size_type, const DenseMatrix<double>::value_type >());
        matrix_binder.def("fill", [](DenseMatrix<double>& self, const typename DenseMatrix<double>::value_type value) { noalias(self) = DenseMatrix<double>(self.size1(),self.size2(),value); });
        matrix_binder.def("fill_identity", [](DenseMatrix<double>& self) { noalias(self) = IdentityMatrix(self.size1()); });
        matrix_binder.def("transpose", [](DenseMatrix<double>& self) { return Matrix(trans(self)); });
        matrix_binder.def(py::init<const DenseMatrix<double>& >());
        matrix_binder.def("__mul__", [](const DenseMatrix<double>& m1, const Vector& v){ return Vector(prod(m1,v));}, py::is_operator());
        matrix_binder.def("__mul__", [](const DenseMatrix<double>& m1, const array_1d<double,3>& v){ if(m1.size2() != 3) QUEST_ERROR << "matrix size2 is not 3!" << std::endl; return Vector(prod(m1,v));}, py::is_operator());
        matrix_binder.def_buffer( [](DenseMatrix<double>& self)-> py::buffer_info{
                                                                    return py::buffer_info(
                                                                    self.data().begin(),
                                                                    sizeof(double),
                                                                    py::format_descriptor<double>::format(),
                                                                    2,
                                                                    {self.size1(),self.size2()},
                                                                    {sizeof(double)*self.size2(),
                                                                    sizeof(double) }
                                                                    );
                                                                    });

        auto compressed_matrix_binder = CreateMatrixInterface< CompressedMatrix >(m,"CompressedMatrix");
        compressed_matrix_binder.def(py::init<const CompressedMatrix::size_type, const CompressedMatrix::size_type>());
        compressed_matrix_binder.def(py::init<const CompressedMatrix& >());
        compressed_matrix_binder.def("value_data", [](const CompressedMatrix& rA) ->  std::vector<double>
                                                    {return std::vector<double>(
                                                        rA.value_data().begin(),
                                                        rA.value_data().end()
                                                        ) ;});
        compressed_matrix_binder.def("index1_data", [](const CompressedMatrix& rA) -> std::vector<std::size_t>
                                                    {return std::vector<std::size_t>(
                                                        rA.index1_data().begin(),
                                                        rA.index1_data().end()
                                                        ) ;});
        compressed_matrix_binder.def("index2_data", [](const CompressedMatrix& rA) -> std::vector<std::size_t>
                                                    {return std::vector<std::size_t>(
                                                        rA.index2_data().begin(),
                                                        rA.index2_data().end()
                                                        ) ;});

        auto cplx_matrix_binder = CreateMatrixInterface< ComplexMatrix >(m,"ComplexMatrix");
        cplx_matrix_binder.def(py::init<const ComplexMatrix::size_type, const ComplexMatrix::size_type>());
        cplx_matrix_binder.def(py::init( [](py::buffer b){
            py::buffer_info info = b.request();
            QUEST_ERROR_IF( info.format != py::format_descriptor<std::complex<double>>::value &&
                info.format != py::format_descriptor<double>::value ) << "Expected a double or complex array" << std::endl;
            QUEST_ERROR_IF( info.ndim != 2 ) << "Buffer dimension of 2 is required, got: " << info.ndim << std::endl;
            ComplexMatrix matrix = ComplexMatrix(info.shape[0], info.shape[1]);

            std::size_t count = 0;
            //if the python data is complex, copy the values
            if( info.format == py::format_descriptor<std::complex<double>>::value )
            {
                for( int i=0; i<info.shape[0]; ++i ) {
                for( int j=0; j<info.shape[1]; ++j ) {
                    matrix(i,j) = static_cast<std::complex<double> *>(info.ptr)[count];
                    count++;
                }
                }
            }
            //if the python data is real, copy the values to the real part and initialize the imaginary part
            else if( info.format == py::format_descriptor<double>::value )
            {
                for( int i=0; i<info.shape[0]; ++i ) {
                for( int j=0; j<info.shape[1]; ++j ) {
                    matrix(i,j) = std::complex<double>(static_cast<double *>(info.ptr)[count], 0.0);
                    count++;
                }
                }
            }

            return matrix;
        }));

        py::implicitly_convertible<py::buffer, DenseMatrix<double>>();

        cplx_matrix_binder.def(py::init<const ComplexMatrix::size_type, const ComplexMatrix::size_type, const ComplexMatrix::value_type >());
        cplx_matrix_binder.def("fill", [](ComplexMatrix& self, const typename ComplexMatrix::value_type value) { noalias(self) = ComplexMatrix(self.size1(),self.size2(),value); });
        cplx_matrix_binder.def("fill_identity", [](ComplexMatrix& self) { noalias(self) = IdentityMatrix(self.size1()); });
        cplx_matrix_binder.def(py::init<const ComplexMatrix& >());
        cplx_matrix_binder.def("__mul__", [](const ComplexMatrix& m1, const ComplexVector& v){ return ComplexVector(prod(m1,v));}, py::is_operator());
        cplx_matrix_binder.def_buffer( [](ComplexMatrix& self)-> py::buffer_info{
                                                                    return py::buffer_info(
                                                                    self.data().begin(),
                                                                    sizeof(std::complex<double>),
                                                                    py::format_descriptor<std::complex<double>>::format(),
                                                                    2,
                                                                    {self.size1(),self.size2()},
                                                                    {sizeof(std::complex<double>)*self.size2(),
                                                                    sizeof(std::complex<double>) }
                                                                    );
                                                                    });

        auto cplx_compressed_matrix_binder = CreateMatrixInterface< ComplexCompressedMatrix >(m,"ComplexCompressedMatrix");
        cplx_compressed_matrix_binder.def(py::init< const ComplexCompressedMatrix::size_type, const ComplexCompressedMatrix::size_type >());
        cplx_compressed_matrix_binder.def(py::init< const CompressedMatrix& >());
        cplx_compressed_matrix_binder.def(py::init< const ComplexCompressedMatrix& >());
        cplx_compressed_matrix_binder.def("value_data", [](const ComplexCompressedMatrix& rA) ->  std::vector<std::complex<double>>
                                                    {return std::vector<std::complex<double>>(
                                                        rA.value_data().begin(),
                                                        rA.value_data().end()
                                                        ) ;});
        cplx_compressed_matrix_binder.def("index1_data", [](const ComplexCompressedMatrix& rA) -> std::vector<std::size_t>
                                                    {return std::vector<std::size_t>(
                                                        rA.index1_data().begin(),
                                                        rA.index1_data().end()
                                                        ) ;});
        cplx_compressed_matrix_binder.def("index2_data", [](const ComplexCompressedMatrix& rA) -> std::vector<std::size_t>
                                                    {return std::vector<std::size_t>(
                                                        rA.index2_data().begin(),
                                                        rA.index2_data().end()
                                                        ) ;});
    }
}