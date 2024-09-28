/*-------------------------------------------
定义与Boost库相关的接口
-------------------------------------------*/

#ifndef QUEST_UBLAS_INTERFACE_HPP
#define QUEST_UBLAS_INTERFACE_HPP

//系统头文件
#include <string>
#include <iostream>

//Boost库头文件
#include <boost/numeric/ublas/matrix.hpp>      //普通稠密矩阵
#include <boost/numeric/ublas/vector.hpp>      //普通稠密向量
#include <boost/numeric/ublas/vector_proxy.hpp>      //向量操作
#include <boost/numeric/ublas/vector_sparse.hpp>      //稀疏向量
#include <boost/numeric/ublas/vector_expression.hpp>      //向量表达式
#include <boost/numeric/ublas/matrix_proxy.hpp>      //矩阵操作
#include <boost/numeric/ublas/symmetric.hpp>      //对称矩阵
#include <boost/numeric/ublas/hermitian.hpp>      //复对称矩阵
#include <boost/numeric/ublas/banded.hpp>      //带状矩阵
#include <boost/numeric/ublas/triangular.hpp>      //三角矩阵
#include <boost/numeric/ublas/lu.hpp>      //LU分解
#include <boost/numeric/ublas/io.hpp>      //输入输出
#include <boost/numeric/ublas/matrix_sparse.hpp>      //稀疏矩阵操作
#include <boost/numeric/ublas/operation.hpp>      //矩阵和向量的操作
#include <boost/numeric/ublas/operation_sparse.hpp>      //稀疏矩阵和向量的操作

//项目头文件
#include "includes/define.hpp"

namespace Quest{
    using namespace boost::numeric::ublas;

    template<typename TDataType> using DenseMatrix = boost::numeric::ublas::matrix<TDataType>;
    template<typename TDataType> using DenseVector = boost::numeric::ublas::vector<TDataType>;

    template<typename TDataType, std::size_t TSize1, std::size_t TSize2> using BoundedMatrix = boost::numeric::ublas::bounded_matrix<TDataType, TSize1, TSize2>;
    template<typename TDataType, std::size_t TSize> using BoundedVector = boost::numeric::ublas::bounded_vector<TDataType, TSize>;

    typedef boost::numeric::ublas::vector<double> Vector;
    typedef unit_vector<double> UnitVector;    //单位向量
    typedef zero_vector<double> ZeroVector;    //零向量
    typedef scalar_vector<double> ScalarVector;    //向量所有分量都是相同标量值
    typedef mapped_vector<double> SpareVector;    //稀疏向量，使用映射来存储非零分量

    typedef  compressed_vector<double> CompressedVector;    //压缩存储稀疏向量
    typedef coordinate_vector<double> CoordinateVector;    //存储坐标的稀疏向量
    typedef vector_range<Vector> VectorRange;     //向量的范围子集
    typedef vector_slice<Vector> VectorSlice;    //向量的切片

    typedef matrix<double> Matrix;    //普通矩阵
    typedef identity_matrix<double> IdentityMatrix;    //单位矩阵
    typedef zero_matrix<double> ZeroMatrix;    //零矩阵
    typedef scalar_matrix<double> ScalarMatrix;    //所有元素都是相同标量值的矩阵
    typedef triangular_matrix<double> TriangularMatrix;    //三角矩阵
    typedef symmetric_matrix<double> SymmetricMatrix;    //对称矩阵
    typedef hermitian_matrix<double> HermitianMatrix;    //厄米特尔矩阵
    typedef banded_matrix<double> BandedMatrix;    //带状矩阵
    typedef mapped_matrix<double> SparseMatrix;    //稀疏矩阵，使用映射来存储非零元素
    typedef coordinate_matrix<double> CoordinateMatrix;    //存储坐标的稀疏矩阵
    typedef matrix_column<Matrix> MatrixColumn;    //表示矩阵中的一列
    typedef matrix_vector_range<Matrix> MatrixVectorRange;    //矩阵的一部分区域
    typedef matrix_vector_slice<Matrix> MatrixVectorSlice;    //矩阵的一部分切片
    typedef matrix_range<Matrix> MatrixRange;    //矩阵的一部分区域
    typedef matrix_slice<Matrix> MatrixSlice;    //矩阵的一部分切片

    template <typename TExpressionType> using MatrixRow = matrix_row<TExpressionType>;    //表示矩阵中的一行

    typedef boost::numeric::ublas::compressed_matrix<double> CompressedMatrix;     //压缩存储稀疏矩阵


}//namespace Quest


#endif // UBLAS_INTERFACE_HPP