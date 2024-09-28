/*-------------------------------------------
复数向量与矩阵的接口
--------------------------------------------*/

#ifndef QUEST_UBLAS_COMPLEX_INTERFACE_HPP
#define QUEST_UBLAS_COMPLEX_INTERFACE_HPP

// 系统依赖
#include <string>
#include <iostream>

// 第三方依赖
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/vector_sparse.hpp>
#include <boost/numeric/ublas/vector_expression.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/symmetric.hpp>
#include <boost/numeric/ublas/hermitian.hpp>
#include <boost/numeric/ublas/banded.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/operation.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/operation_sparse.hpp>

// 项目依赖
#include "questCAE/includes/define.hpp"
#include "includes/ublas_interface.hpp"

namespace Quest{
    
    using namespace boost::numeric::ublas;

    typedef DenseVector<std::complex<double>> ComplexVector;
    typedef unit_vector<std::complex<double>> ComplexUnitVector;
    typedef zero_vector<std::complex<double>> ComplexZeroVector;
    typedef scalar_vector<std::complex<double>> ComplexScalarVector;
    typedef mapped_vector<std::complex<double>> ComplexSparseVector;
    typedef compressed_vector<std::complex<double>> ComplexCompressedVector;
    typedef coordinate_vector<std::complex<double>> ComplexCoordinateVector;
    typedef vector_range<ComplexVector> ComplexVectorRange;
    typedef vector_slice<ComplexVector> ComplexVectorSlice;
    typedef DenseMatrix<std::complex<double>> ComplexMatrix;
    typedef identity_matrix<std::complex<double>> ComplexIdentityMatrix;
    typedef zero_matrix<std::complex<double>> ComplexZeroMatrix;
    typedef scalar_matrix<std::complex<double>> ComplexScalarMatrix;
    typedef triangular_matrix<std::complex<double>> ComplexTriangularMatrix;
    typedef symmetric_matrix<std::complex<double>> ComplexSymmetricMatrix;
    typedef hermitian_matrix<std::complex<double>> ComplexHermitianMatrix;
    typedef banded_matrix<std::complex<double>> ComplexBandedMatrix;
    typedef mapped_matrix<std::complex<double>> ComplexSparseMatrix;
    typedef compressed_matrix<std::complex<double>> ComplexCompressedMatrix;
    typedef coordinate_matrix<std::complex<double>> ComplexCoordinateMatrix;
    typedef matrix_row<ComplexMatrix> ComplexMatrixRow;
    typedef matrix_column<ComplexMatrix> ComplexMatrixColumn;
    typedef matrix_vector_range<ComplexMatrix> ComplexMatrixVectorRange;
    typedef matrix_vector_slice<ComplexMatrix> ComplexMatrixVectorSlice;
    typedef matrix_range<ComplexMatrix> ComplexMatrixRange;
    typedef matrix_slice<ComplexMatrix> ComplexMatrixSlice;

}  // namespace Quest




#endif // UBLAS_COMPLEX_INTERFACE_HPP