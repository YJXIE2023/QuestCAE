/*----------------------------------------
检查宏定义
------------------------------------------*/

#ifndef QUEST_CHECK_HPP
#define QUEST_CHECK_HPP


//系统头文件
#include <cstring>
#include <limits>
#include <cmath>

//项目头文件
#include "includes/exceptions.hpp"


//宏定义
#define QUEST_CHECK(IsTrue) if(!(IsTrue)) QUEST_ERROR<<"Check failed beacause "<<#IsTrue<<" is false."<<std::endl;
#define QUEST_CHECK_IS_FALSE(IsFalse) if((IsFalse)) QUEST_ERROR<<"Check failed beacause "<<#IsFalse<<" is true."<<std::endl;

#define QUEST_CHECK_EQUAL(a,b) if((a)!= (b)) QUEST_ERROR<<"Check failed beacause "<<#a<<" is not equal to "<<#b<<"."<<std::endl;
#define QUEST_CHECK_NOT_EQUAL(a,b) if((a) == (b)) QUEST_ERROR<<"Check failed beacause "<<#a<<" is equal to "<<#b<<"."<<std::endl;

#define QUEST_CHECK_STRING_EQUAL(a,b) if(a.compare(b)!= 0) QUEST_ERROR<<"Check failed beacause \""<<#a<<"\" is not equal to \""<<#b<<"\"."<<std::endl;
#define QUEST_CHECK_STRING_NOT_EQUAL(a,b) if(a.compare(b) == 0) QUEST_ERROR<<"Check failed beacause \""<<#a<<"\" is equal to \""<<#b<<"\"."<<std::endl;

#define QUEST_CHECK_C_STRING_EQUAL(a,b) if(std::strcmp(a,b)!= 0) QUEST_ERROR<<"Check failed beacause \""<<#a<<"\" is not equal to \""<<#b<<"\"."<<std::endl;
#define QUEST_CHECK_C_STRING_NOT_EQUAL(a,b) if(std::strcmp(a,b) == 0) QUEST_ERROR<<"Check failed beacause \""<<#a<<"\" is equal to \""<<#b<<"\"."<<std::endl;

#define QUEST_CHECK_LESS(a,b) if(!(a<b)) QUEST_ERROR<<"Check failed beacause "<<#a<<" is not less than "<<#b<<"."<<std::endl;
#define QUEST_CHECK_LESS_EQUAL(a,b) if(!(a<=b)) QUEST_ERROR<<"Check failed beacause "<<#a<<" is not less than or equal to "<<#b<<"."<<std::endl;

#define QUEST_CHECK_GREATER(a,b) if(!(a>b)) QUEST_ERROR<<"Check failed beacause "<<#a<<" is not greater than "<<#b<<"."<<std::endl;
#define QUEST_CHECK_GREATER_EQUAL(a,b) if(!(a>=b)) QUEST_ERROR<<"Check failed beacause "<<#a<<" is not greater than or equal to "<<#b<<"."<<std::endl;

#define QUEST_CHECK_STRING_SONTAIN_SUB_STRING(TheString,SubString) if(TheString.find(SubString) == std::string::npos) \
    QUEST_ERROR<<"The string \""<<SubString<<"\" if not found in the given string"<<std::endl;

#define QUEST_CHECK_NEAR(a,b,tolerance) if (!(std::abs(a-b) <= tolerance)) QUEST_ERROR<<"Check failed because "<<#a<<" = "<<a<<\
    " is not near to "<<#b<<" = "<<b<<" within the tolerance "<<tolerance<<std::endl;
#define QUEST_CHECK_RELATIVE_NEAR(a,b,tolerance) if(!(std::abs(b)<=std::numeric_limits<double>::epsilon())) {QUEST_ERROR_IF(!(std::abs((a-b)/b)<=tolerance)) << "Check failed because "<<#a<<" = "<<a<<\
    " is not near to "<<#b<<" = "<<b<<" within the relative tolerance "<<tolerance<<std::endl;} else {QUEST_CHECK_NEAR(a,b,tolerance);}
#define QUEST_CHECK_DOUBLE_EQUAL(a,b) QUEST_CHECK_NEAR(a,b,std::numeric_limits<double>::epsilon())

#define QUEST_CHECK_VECTOR_NEAR(a,b,tolerance){                         \
QUEST_ERROR_IF_NOT(a.size() == b.size())                                \
<<"Check failed because vector arguments do not have the same size:"    \
<<std::endl                                                            \
<<"First argument has size "<<a.size()<<", "                            \
<<"second argument has size "<<b.size()<<"."<<std::endl;                \
for(std::size_t _i=0;_i<a.size();++_i){                                 \
    QUEST_ERROR_IF(!(std::abs(a[_i]-b[_i])<=tolerance))                    \
    <<"Check failed because vector "<<#a<<" with values"<<std::endl        \
    <<a<<std::endl                                                         \
    <<"Is not near vector "<<#b<<" with values"<<std::endl                 \
    <<b<<std::endl                                                         \
    <<"Mismatch found in component "<<_i<<":"<<std::endl                    \
    <<a[_i]<<" not near "<<b[_i]                                            \
    <<" within the tolerance "<<tolerance<<std::endl;                      \
}                                                                      \
}

#define QUEST_CHECK_VECTOR_RELATIVE_NEAR(a,b,tolerance){                 \
QUEST_ERROR_IF_NOT(a.size() == b.size())                                \
<<"Check failed because vector arguments do not have the same size:"    \
<<std::endl                                                            \
<<"First argument has size "<<a.size()<<", "                            \
<<"second argument has size "<<b.size()<<"."<<std::endl;                \
for(std::size_t _i=0;_i<a.size();++_i){                                 \
    if(std::abs(b[_i]>std::numeric_limits<double>::espilon())){        \
        QUEST_ERROR_IF(!(std::abs((a[_i]-b[_i])/b[_i])<=tolerance))    \
        <<"Check failed because vector "<<#a<<" with values"<<std::endl    \
        <<a<<std::endl                                                     \
        <<"Is not near vector "<<#b<<" with values"<<std::endl             \
        <<b<<std::endl                                                     \
        <<"Mismatch found in component "<<_i<<":"<<std::endl                \
        <<a[_i]<<" not near "<<b[_i]                                        \
        <<" within the relative tolerance "<<tolerance<<std::endl;          \
    } else {                                                            \
        QUEST_ERROR_IF(!(std::abs(a[_i]-b[_i])<=tolerance))                \
        <<"Check failed because vector "<<#a<<" with values"<<std::endl    \
        <<a<<std::endl                                                     \
        <<"Is not near vector "<<#b<<" with values"<<std::endl             \
        <<b<<std::endl                                                     \
        <<"Mismatch found in component "<<_i<<":"<<std::endl                \
        <<a[_i]<<" not near "<<b[_i]                                        \
        <<" within the tolerance "<<tolerance<<std::endl;                  \
    }                                                                    \
}                                                                      \
}

#define QUEST_CHECK_VECTOR_EQUAL(a,b) QUEST_CHECK_VECTOR_NEAR(a,b,std::numeric_limits<double>::epsilon())

#define QUEST_CHECK_MATRIX_NEAR(a,b,tolerance){                         \
QUEST_ERROR_IF_NOT((a.size1()==b.size1())&&(a.size2()==b.size2()))    \
<<"Check failed because matrix arguments do not have the same size:"    \
<<std::endl                                                            \
<<"First argument has size "<<a.size1()<<"x"<<a.size2()<<", "            \
<<"second argument has size "<<b.size1()<<"x"<<b.size2()<<"."<<std::endl;  \
for(std::size_t _i=0;_i<a.size1();++_i){                                 \
    for(std::size_t _j=0;_j<a.size2();++_j){                             \
        QUEST_ERROR_IF(!(std::abs(a(_i,_j)-b(_i,_j))<=tolerance))        \
        <<"Check failed because matrix "<<#a<<" with values"<<std::endl    \
        <<a<<std::endl                                                     \
        <<"Is not near matrix "<<#b<<" with values"<<std::endl             \
        <<b<<std::endl                                                     \
        <<"Mismatch found in component ("<<_i<<","<<_j<<"):"<<std::endl    \
        <<a(_i,_j)<<" not near "<<b(_i,_j)                                \
        <<" within the tolerance "<<tolerance<<std::endl;                  \
    }                                                                    \
}                                                                      \
}

#define QUEST_CHECK_MATRIX_RELATIVE_NEAR(a,b,tolerance){                 \
QUEST_ERROR_IF_NOT((a.size1()==b.size1())&&(a.size2()==b.size2()))    \
<<"Check failed because matrix arguments do not have the same size:"    \
<<std::endl                                                            \
<<"First argument has size "<<a.size1()<<"x"<<a.size2()<<", "            \
<<"second argument has size "<<b.size1()<<"x"<<b.size2()<<"."<<std::endl;  \
for(std::size_t _i=0;_i<a.size1();++_i){                                 \
    for(std::size_t _j=0;_j<a.size2();++_j){                             \
        if(std::abs(b(_i,_j)>std::numeric_limits<double>::epsilon())){    \
            QUEST_ERROR_IF(!(std::abs((a(_i,_j)-b(_i,_j))/b(_i,_j))<=tolerance)) \
            <<"Check failed because matrix "<<#a<<" with values"<<std::endl    \
            <<a<<std::endl                                                     \
            <<"Is not near matrix "<<#b<<" with values"<<std::endl             \
            <<b<<std::endl                                                     \
            <<"Mismatch found in component ("<<_i<<","<<_j<<"):"<<std::endl    \
            <<a(_i,_j)<<" not near "<<b(_i,_j)                                \
            <<" within the relative tolerance "<<tolerance<<std::endl;      \
        } else {                                                            \
            QUEST_ERROR_IF(!(std::abs(a(_i,_j)-b(_i,_j))<=tolerance))        \
            <<"Check failed because matrix "<<#a<<" with values"<<std::endl    \
            <<a<<std::endl                                                     \
            <<"Is not near matrix "<<#b<<" with values"<<std::endl             \
            <<b<<std::endl                                                     \
            <<"Mismatch found in component ("<<_i<<","<<_j<<"):"<<std::endl    \
            <<a(_i,_j)<<" not near "<<b(_i,_j)                                \
            <<" within the tolerance "<<tolerance<<std::endl;                  \
        }                                                                    \
    }                                                                    \
}                                                                      \
}

#define QUEST_CHECK_MATRIX_EQUAL(a,b) QUEST_CHECK_MATRIX_NEAR(a,b,std::numeric_limits<double>::epsilon())

#define QUEST_CHECK_EXCEPTION_IS_THROWN(TheStatement, TheErrorMessage)    \
try{                                                                   \
    TheStatement;                                                      \
    QUEST_ERROR<<"Check failed because no exception was thrown."<<std::endl;\
} catch(const std::exception& e){                                      \
    if(std::string(e.what()).find(TheErrorMessage)==std::string::npos)    \
        QUEST_ERROR                                                       \
            <<"Test Failed: "<<#TheStatement                              \
            << " did not throw the expected exception."<<std::endl        \
            <<"Excepted: "<<std::endl<<TheErrorMessage<<std::endl            \
            <<"Got: "<<std::endl<<e.what()<<std::endl;                     \
}

#define QUEST_CHECK_VARIABLE_IN_NODAL_DATA(TheVariable, TheNode)         \
    QUEST_ERROR_IF_NOT(TheNode.HasDofFor(TheVariable))                    \
    <<"Missing "<<TheVariable.Name()<<" variable in solution step for node "    \
    <<TheNode.Id()<<"."<<std::endl;

#define QUEST_CHECK_DOF_IN_NODE(TheVariable, TheNode)                        \
    QUEST_ERROR_IF_NOT(TheNode.HasDofFor(TheVariable))                    \
    <<"Missing Degree of Freedom for "<<TheVariable.Name()                 \
    <<" in node "<<TheNode.Id()<<"."<<std::endl;

#ifdef QUEST_DEBUG
#define QUEST_DEBUG_CHECK(IsTrue) QUEST_CHECK(IsTrue)
#define QUEST_DEBUG_CHECK_IS_FALSE(IsFalse) QUEST_CHECK_IS_FALSE(IsFalse)

#define QUEST_DEBUG_CHECK_EQUAL(a,b) QUEST_CHECK_EQUAL(a,b)
#define QUEST_DEBUG_CHECK_NOT_EQUAL(a,b) QUEST_CHECK_NOT_EQUAL(a,b)

#define QUEST_DEBUG_CHECK_C_STRING_EQUAL(a,b) QUEST_CHECK_C_STRING_EQUAL(a,b)       
#define QUEST_DEBUG_CHECK_C_STRING_NOT_EQUAL(a,b) QUEST_CHECK_C_STRING_NOT_EQUAL(a,b)

#define QUEST_DEBUG_CHECK_LESS(a,b) QUEST_CHECK_LESS(a,b)
#define QUEST_DEBUG_CHECK_LESS_EQUAL(a,b) QUEST_CHECK_LESS_EQUAL(a,b)

#define QUEST_DEBUG_CHECK_GREATER(a,b) QUEST_CHECK_GREATER(a,b)
#define QUEST_DEBUG_CHECK_GREATER_EQUAL(a,b) QUEST_CHECK_GREATER_EQUAL(a,b)

#define QUEST_DEBUG_CHECK_STRING_SONTAIN_SUB_STRING(TheString,SubString) QUEST_CHECK_STRING_SONTAIN_SUB_STRING(TheString,SubString)

#define QUEST_DEBUG_CHECK_NEAR(a,b,tolerance) QUEST_CHECK_NEAR(a,b,tolerance)
#define QUEST_DEBUG_CHECK_DOUBLE_EQUAL(a,b) QUEST_CHECK_DOUBLE_EQUAL(a,b)

#define QUEST_DEBUG_CHECK_VECTOR_NEAR(a,b,tolerance) QUEST_CHECK_VECTOR_NEAR(a,b,tolerance)
#define QUEST_DEBUG_CHECK_VECTOR_EQUAL(a,b) QUEST_CHECK_VECTOR_EQUAL(a,b)

#define QUEST_DEBUG_CHECK_MATRIX_NEAR(a,b,tolerance) QUEST_CHECK_MATRIX_NEAR(a,b,tolerance)
#define QUEST_DEBUG_CHECK_MATRIX_EQUAL(a,b) QUEST_CHECK_MATRIX_EQUAL(a,b)

#define QUEST_DEBUG_CHECK_EXCEPTION_IS_THROWN(TheStatement, TheErrorMessage) QUEST_CHECK_EXCEPTION_IS_THROWN(TheStatement, TheErrorMessage)

#define QUEST_DEBUG_CHECK_VARIABLE_IN_NODAL_DATA(TheVariable, TheNode) QUEST_CHECK_VARIABLE_IN_NODAL_DATA(TheVariable, TheNode)
#define QUEST_DEBUG_CHECK_DOF_IN_NODE(TheVariable, TheNode) QUEST_CHECK_DOF_IN_NODE(TheVariable, TheNode)

#else
#define QUEST_DEBUG_CHECK(IsTrue) if(false) QUEST_CHECK(IsTrue)
#define QUEST_DEBUG_CHECK_IS_FALSE(IsFalse) if(false) QUEST_CHECK_IS_FALSE(IsFalse)

#define QUEST_DEBUG_CHECK_EQUAL(a,b) if(false) QUEST_CHECK_EQUAL(a,b)
#define QUEST_DEBUG_CHECK_NOT_EQUAL(a,b) if(false) QUEST_CHECK_NOT_EQUAL(a,b)

#define QUEST_DEBUG_CHECK_C_STRING_EQUAL(a,b) if(false) QUEST_CHECK_C_STRING_EQUAL(a,b)       
#define QUEST_DEBUG_CHECK_C_STRING_NOT_EQUAL(a,b) if(false) QUEST_CHECK_C_STRING_NOT_EQUAL(a,b)

#define QUEST_DEBUG_CHECK_LESS(a,b) if(false) QUEST_CHECK_LESS(a,b)
#define QUEST_DEBUG_CHECK_LESS_EQUAL(a,b) if(false) QUEST_CHECK_LESS_EQUAL(a,b)

#define QUEST_DEBUG_CHECK_GREATER(a,b) if(false) QUEST_CHECK_GREATER(a,b)
#define QUEST_DEBUG_CHECK_GREATER_EQUAL(a,b) if(false) QUEST_CHECK_GREATER_EQUAL(a,b)

#define QUEST_DEBUG_CHECK_STRING_SONTAIN_SUB_STRING(TheString,SubString) if(false) QUEST_CHECK_STRING_SONTAIN_SUB_STRING(TheString,SubString)

#define QUEST_DEBUG_CHECK_NEAR(a,b,tolerance) if(false) QUEST_CHECK_NEAR(a,b,tolerance)
#define QUEST_DEBUG_CHECK_DOUBLE_EQUAL(a,b) if(false) QUEST_CHECK_DOUBLE_EQUAL(a,b)

#define QUEST_DEBUG_CHECK_VECTOR_NEAR(a,b,tolerance) if(false) QUEST_CHECK_VECTOR_NEAR(a,b,tolerance)
#define QUEST_DEBUG_CHECK_VECTOR_EQUAL(a,b) if(false) QUEST_CHECK_VECTOR_EQUAL(a,b)

#define QUEST_DEBUG_CHECK_MATRIX_NEAR(a,b,tolerance) if(false) QUEST_CHECK_MATRIX_NEAR(a,b,tolerance)
#define QUEST_DEBUG_CHECK_MATRIX_EQUAL(a,b) if(false) QUEST_CHECK_MATRIX_EQUAL(a,b)

#define QUEST_DEBUG_CHECK_EXCEPTION_IS_THROWN(TheStatement, TheErrorMessage) if(false) QUEST_CHECK_EXCEPTION_IS_THROWN(TheStatement, TheErrorMessage)

#define QUEST_DEBUG_CHECK_VARIABLE_IN_NODAL_DATA(TheVariable, TheNode) if(false) QUEST_CHECK_VARIABLE_IN_NODAL_DATA(TheVariable, TheNode)
#define QUEST_DEBUG_CHECK_DOF_IN_NODE(TheVariable, TheNode) if(false) QUEST_CHECK_DOF_IN_NODE(TheVariable, TheNode)
#endif //QUEST_DEBUG



#endif //QUEST_CHECK_HPP