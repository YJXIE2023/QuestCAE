/*-------------------------------------------
定义与Boost库相关的接口
-------------------------------------------*/

#ifndef UBLAS_INTERFACE_HPP
#define UBLAS_INTERFACE_HPP

//系统头文件
#include <string>
#include <iostream>

//Boost库头文件
#include <boost/numeric/ublas/matrix.hpp>      //普通稠密矩阵
#include <boost/numeric/ublas/vector.hpp>      //普通稠密向量
#include <boost/numeric/ublas/vector_proxy.hpp>      //向量操作
#include <boost/numeric/ublas/vector_sparse.hpp>      //稀疏向量
#include <boost/numeric/ublas/matrix_expression.hpp>      //向量表达式
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


#endif // UBLAS_INTERFACE_HPP