/*-------------------------------------
#一维数组类
-------------------------------------*/

#ifndef ARRAY_1D_HPP
#define ARRAY_1D_HPP

//系统头文件
#include <string>
#include <iostream>
#include <array>
#include <algorithm>
#include <initializer_list>

//项目头文件
#include "defines.hpp"
#include "ublas_interface.hpp"

#include <boost/numeric/ublas/vector_expression.hpp>    //向量表达式基类
#include <boost/numeric/ublas/storage.hpp>    //不同方式存储向量
#include <boost/numeric/ublas/detail/vector_assign.hpp>    //涉及向量赋值的操作

namespace Quest{

    //一维数组类
    template<typename T, std::size_t N>
    class Array1d:public boost::numeric::ublas::vector_expression<Array1d<T, N> >{
        
        public:
    }

}


#endif // ARRAY_1D_HPP