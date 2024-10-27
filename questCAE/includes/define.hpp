/*---------------------------------------------------
包含全局的宏定义、常量、类型定义以及其他项目所需的基本配置
----------------------------------------------------*/

#ifndef QUEST_DEFINE_H
#define QUEST_DEFINE_H

//系统头文件
#include <stdexcept>
#include <sstream>

//自定义头文件
#include "includes/quest_export_api.hpp"
#include "includes/exceptions.hpp"
#include "includes/smart_pointers.hpp"

/*---------------------------------------------------
异常处理相关宏定义
----------------------------------------------------*/
#define QUEST_CATCH_AND_THROW(ExceptionType,MoreInfo,Block) \
catch(ExceptionType& e)  \
{  \
Block \
QUEST_ERROR << e.what(); \
}

#define QUEST_THROW_ERROR(ExceptionType,ErrorMessage,MoreInfo) \
{ \
QUEST_ERROR << ErrorMessage << MoreInfo << std::endl; \
}

#define QUEST_CATCH_WITH_BLOCK(MoreInfo,Block)\
} \
QUEST_CATCH_AND_THROW(std::overflow_error,MoreInfo,Block) \
QUEST_CATCH_AND_THROW(std::underflow_error,MoreInfo,Block) \
QUEST_CATCH_AND_THROW(std::range_error,MoreInfo,Block) \
QUEST_CATCH_AND_THROW(std::out_of_range,MoreInfo,Block) \
QUEST_CATCH_AND_THROW(std::length_error,MoreInfo,Block) \
QUEST_CATCH_AND_THROW(std::invalid_argument,MoreInfo,Block) \
QUEST_CATCH_AND_THROW(std::domain_error,MoreInfo,Block) \
QUEST_CATCH_AND_THROW(std::logic_error,MoreInfo,Block) \
QUEST_CATCH_AND_THROW(std::runtime_error,MoreInfo,Block) \
catch(Exception& e){Block throw Exception(e) << QUEST_CODE_LOCATION << MoreInfo << std::endl;} \
catch(std::exception& e){ Block QUEST_THROW_ERROR(std::runtime_error,e.what(),MoreInfo)} \
catch(...) { Block QUEST_THROW_ERROR(std::runtime_error,"Unknown exception",MoreInfo)}

#define QUEST_CATCH_BLOCK_DEBUG class ExceptionBlock{ public: void operator()(void)}{
#define QUEST_CATCH_BLOCK_END }} exception_block; exception_block();

#ifndef QUEST_NO_TRY_CATCH
    #define QUEST_TRY_IMPL try {
    #define QUEST_CATCH_IMPL(MoreInfo) QUEST_CATCH_WITH_BLOCK(MoreInfo,{})
#else
    #define QUEST_TRY_IMPL {};
    #define QUEST_CATCH_IMPL(MoreInfo) {};
#endif

#ifndef __SUNPRO_CC
    #define QUEST_TRY QUEST_TRY_IMPL
    #define QUEST_CATCH(MoreInfo) QUEST_CATCH_IMPL(MoreInfo)
#else
    #define QUEST_TRY {};
    #define QUEST_CATCH(MoreInfo) {};
#endif


/*---------------------------------------------------
变量相关宏定义
----------------------------------------------------*/

#define QUEST_EXPORT_MACRO QUEST_NO_EXPORT

#ifdef QUEST_DEFINE_VARIABLE_IMPLEMENTATION
#undef QUEST_DEFINE_VARIABLE_IMPLEMENTATION
#endif
#define QUEST_DEFINE_VARIABLE_IMPLEMENTATION(module,type,name) \
    extern QUEST_EXPORT_MACRO(module) Variable<type> name;

#ifdef QUEST_DEFINE_VARIABLE
#undef QUEST_DEFINE_VARIABLE
#endif
#define QUEST_DEFINE_VARIABLE(type,name) \
    QUEST_DEFINE_VARIABLE_IMPLEMENTATION(QUEST_CORE, type, name)

#ifdef QUEST_DEFINE_APPLICATION_VARIABLE
#undef QUEST_DEFINE_APPLICATION_VARIABLE
#endif
#define QUEST_DEFINE_APPLICATION_VARIABLE(application,type,name)    \
    QUEST_API(application) extern Variable<type> name;

#ifdef QUEST_DEFINE_3D_VARIABLE_WITH_COMPONENTS
#undef QUEST_DEFINE_3D_VARIABLE_WITH_COMPONENTS
#endif
#define QUEST_DEFINE_3D_VARIABLE_WITH_COMPONENTS_IMPLEMENTATION(module,name)  \
    QUEST_EXPORT_MACRO(module) extern Quest::Variable<Quest::Array1d<double,3> > name;    \
    QUEST_EXPORT_MACRO(module) extern Quest::Variable<double> name##_X;    \    
    QUEST_EXPORT_MACRO(module) extern Quest::Variable<double> name##_Y;    \
    QUEST_EXPORT_MACRO(module) extern Quest::Variable<double> name##_Z;

#ifdef QUEST_DEFINE_3D_VARIABLE_WITH_COMPONENTS
#undef QUEST_DEFINE_3D_VARIABLE_WITH_COMPONENTS
#endif
#define QUEST_DEFINE_3D_VARIABLE_WITH_COMPONENTS(name) \
    QUEST_DEFINE_3D_VARIABLE_WITH_COMPONENTS_IMPLEMENTATION(QUEST_CORE, name)

#ifdef QUEST_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS
#undef QUEST_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS
#endif
#define QUEST_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(application,name) \
    QUEST_API(application) extern Quest::Variable<Quest::Array1d<double,3> > name;    \
    QUEST_API(application) extern Quest::Variable<double> name##_X;    \    
    QUEST_API(application) extern Quest::Variable<double> name##_Y;    \
    QUEST_API(application) extern Quest::Variable<double> name##_Z;

#ifdef QUEST_DEFINE_SYMMETRIC_2D_TENSOR_VARIABLE_WITH_COMPONENTS
#undef QUEST_DEFINE_SYMMETRIC_2D_TENSOR_VARIABLE_WITH_COMPONENTS
#endif
#define QUEST_DEFINE_SYMMETRIC_2D_TENSOR_VARIABLE_WITH_COMPONENTS_IMPLEMENTATION(module,name)  \
    QUEST_EXPORT_MACRO(module) extern Quest::Variable<Quest::Array1d<double,3> > name;    \
    QUEST_EXPORT_MACRO(module) extern Quest::Variable<double> name##_XX;    \
    QUEST_EXPORT_MACRO(module) extern Quest::Variable<double> name##_YY;    \
    QUEST_EXPORT_MACRO(module) extern Quest::Variable<double> name##_XY;

#ifdef QUEST_DEFINE_SYMMETRIC_2D_TENSOR_VARIABLE_WITH_COMPONENTS
#undef QUEST_DEFINE_SYMMETRIC_2D_TENSOR_VARIABLE_WITH_COMPONENTS
#endif
#define QUEST_DEFINE_SYMMETRIC_2D_TENSOR_VARIABLE_WITH_COMPONENTS(name) \
    QUEST_DEFINE_SYMMETRIC_2D_TENSOR_VARIABLE_WITH_COMPONENTS_IMPLEMENTATION(QUEST_CORE, name)

#ifdef QUEST_DEFINE_SYMMETRIC_2D_TENSOR_APPLICATION_VARIABLE_WITH_COMPONENTS
#undef QUEST_DEFINE_SYMMETRIC_2D_TENSOR_APPLICATION_VARIABLE_WITH_COMPONENTS
#endif
#define QUEST_DEFINE_SYMMETRIC_2D_TENSOR_APPLICATION_VARIABLE_WITH_COMPONENTS(application,name) \
    QUEST_API(application) extern Quest::Variable<Quest::Array1d<double,3> > name;    \
    QUEST_API(application) extern Quest::Variable<double> name##_XX;    \
    QUEST_API(application) extern Quest::Variable<double> name##_YY;    \
    QUEST_API(application) extern Quest::Variable<double> name##_XY;

#ifdef QUEST_DEFINE_SYMMETRIC_3D_TENSOR_VARIABLE_WITH_COMPONENTS
#undef QUEST_DEFINE_SYMMETRIC_3D_TENSOR_VARIABLE_WITH_COMPONENTS
#endif
#define QUEST_DEFINE_SYMMETRIC_3D_TENSOR_VARIABLE_WITH_COMPONENTS_IMPLEMENTATION(module,name)  \
    QUEST_EXPORT_MACRO(module) extern Quest::Variable<Quest::Array1d<double,6> > name;    \
    QUEST_EXPORT_MACRO(module) extern Quest::Variable<double> name##_XX;    \
    QUEST_EXPORT_MACRO(module) extern Quest::Variable<double> name##_YY;    \
    QUEST_EXPORT_MACRO(module) extern Quest::Variable<double> name##_ZZ;    \
    QUEST_EXPORT_MACRO(module) extern Quest::Variable<double> name##_XY;    \
    QUEST_EXPORT_MACRO(module) extern Quest::Variable<double> name##_YZ;    \
    QUEST_EXPORT_MACRO(module) extern Quest::Variable<double> name##_XZ;

#ifdef QUEST_DEFINE_SYMMETRIC_3D_TENSOR_VARIABLE_WITH_COMPONENTS
#undef QUEST_DEFINE_SYMMETRIC_3D_TENSOR_VARIABLE_WITH_COMPONENTS
#endif
#define QUEST_DEFINE_SYMMETRIC_3D_TENSOR_VARIABLE_WITH_COMPONENTS(name) \
    QUEST_DEFINE_SYMMETRIC_3D_TENSOR_VARIABLE_WITH_COMPONENTS_IMPLEMENTATION(QUEST_CORE, name)

#ifdef QUEST_DEFINE_SYMMETRIC_3D_TENSOR_APPLICATION_VARIABLE_WITH_COMPONENTS
#undef QUEST_DEFINE_SYMMETRIC_3D_TENSOR_APPLICATION_VARIABLE_WITH_COMPONENTS
#endif
#define QUEST_DEFINE_SYMMETRIC_3D_TENSOR_APPLICATION_VARIABLE_WITH_COMPONENTS(application,name) \
    QUEST_API(application) extern Quest::Variable<Quest::Array1d<double,6> > name;    \
    QUEST_API(application) extern Quest::Variable<double> name##_XX;    \
    QUEST_API(application) extern Quest::Variable<double> name##_YY;    \
    QUEST_API(application) extern Quest::Variable<double> name##_ZZ;    \
    QUEST_API(application) extern Quest::Variable<double> name##_XY;    \
    QUEST_API(application) extern Quest::Variable<double> name##_YZ;    \
    QUEST_API(application) extern Quest::Variable<double> name##_XZ;

#ifdef QUEST_DEFINE_2D_TENSOR_VARIABLE_WITH_COMPONENTS
#undef QUEST_DEFINE_2D_TENSOR_VARIABLE_WITH_COMPONENTS
#endif
#define QUEST_DEFINE_2D_TENSOR_VARIABLE_WITH_COMPONENTS_IMPLEMENTATION(module,name)  \
    QUEST_EXPORT_MACRO(module) extern Quest::Variable<Quest::Array1d<double,4> > name;    \
    QUEST_EXPORT_MACRO(module) extern Quest::Variable<double> name##_XX;    \
    QUEST_EXPORT_MACRO(module) extern Quest::Variable<double> name##_XY;    \    
    QUEST_EXPORT_MACRO(module) extern Quest::Variable<double> name##_YX;    \
    QUEST_EXPORT_MACRO(module) extern Quest::Variable<double> name##_YY;

#ifdef QUEST_DEFINE_2D_TENSOR_VARIABLE_WITH_COMPONENTS
#undef QUEST_DEFINE_2D_TENSOR_VARIABLE_WITH_COMPONENTS
#endif
#define QUEST_DEFINE_2D_TENSOR_VARIABLE_WITH_COMPONENTS(name) \
    QUEST_DEFINE_2D_TENSOR_VARIABLE_WITH_COMPONENTS_IMPLEMENTATION(QUEST_CORE, name)

#ifdef QUEST_DEFINE_2D_TENSOR_APPLICATION_VARIABLE_WITH_COMPONENTS
#undef QUEST_DEFINE_2D_TENSOR_APPLICATION_VARIABLE_WITH_COMPONENTS
#endif
#define QUEST_DEFINE_2D_TENSOR_APPLICATION_VARIABLE_WITH_COMPONENTS(application,name) \
    QUEST_API(application) extern Quest::Variable<Quest::Array1d<double,4> > name;    \
    QUEST_API(application) extern Quest::Variable<double> name##_XX;    \
    QUEST_API(application) extern Quest::Variable<double> name##_XY;    \    
    QUEST_API(application) extern Quest::Variable<double> name##_YX;    \
    QUEST_API(application) extern Quest::Variable<double> name##_YY;

#ifdef QUEST_DEFINE_3D_TENSOR_VARIABLE_WITH_COMPONENTS
#undef QUEST_DEFINE_3D_TENSOR_VARIABLE_WITH_COMPONENTS
#endif
#define QUEST_DEFINE_3D_TENSOR_VARIABLE_WITH_COMPONENTS_IMPLEMENTATION(module,name)  \
    QUEST_EXPORT_MACRO(module) extern Quest::Variable<Quest::Array1d<double,9> > name;    \
    QUEST_EXPORT_MACRO(module) extern Quest::Variable<double> name##_XX;    \
    QUEST_EXPORT_MACRO(module) extern Quest::Variable<double> name##_XY;    \
    QUEST_EXPORT_MACRO(module) extern Quest::Variable<double> name##_XZ;    \
    QUEST_EXPORT_MACRO(module) extern Quest::Variable<double> name##_YX;    \
    QUEST_EXPORT_MACRO(module) extern Quest::Variable<double> name##_YY;    \
    QUEST_EXPORT_MACRO(module) extern Quest::Variable<double> name##_YZ;    \
    QUEST_EXPORT_MACRO(module) extern Quest::Variable<double> name##_ZX;    \
    QUEST_EXPORT_MACRO(module) extern Quest::Variable<double> name##_ZY;    \
    QUEST_EXPORT_MACRO(module) extern Quest::Variable<double> name##_ZZ;

#ifdef QUEST_DEFINE_3D_TENSOR_VARIABLE_WITH_COMPONENTS
#undef QUEST_DEFINE_3D_TENSOR_VARIABLE_WITH_COMPONENTS
#endif
#define QUEST_DEFINE_3D_TENSOR_VARIABLE_WITH_COMPONENTS(name) \
    QUEST_DEFINE_3D_TENSOR_VARIABLE_WITH_COMPONENTS_IMPLEMENTATION(QUEST_CORE, name)

#ifdef QUEST_DEFINE_3D_TENSOR_APPLICATION_VARIABLE_WITH_COMPONENTS
#undef QUEST_DEFINE_3D_TENSOR_APPLICATION_VARIABLE_WITH_COMPONENTS
#endif
#define QUEST_DEFINE_3D_TENSOR_APPLICATION_VARIABLE_WITH_COMPONENTS(application,name) \
    QUEST_API(application) extern Quest::Variable<Quest::Array1d<double,9> > name;    \
    QUEST_API(application) extern Quest::Variable<double> name##_XX;    \
    QUEST_API(application) extern Quest::Variable<double> name##_XY;    \
    QUEST_API(application) extern Quest::Variable<double> name##_XZ;    \
    QUEST_API(application) extern Quest::Variable<double> name##_YX;    \
    QUEST_API(application) extern Quest::Variable<double> name##_YY;    \
    QUEST_API(application) extern Quest::Variable<double> name##_YZ;    \
    QUEST_API(application) extern Quest::Variable<double> name##_ZX;    \
    QUEST_API(application) extern Quest::Variable<double> name##_ZY;    \
    QUEST_API(application) extern Quest::Variable<double> name##_ZZ;

#ifdef QUEST_CREATE_VARIABLE
#undef QUEST_CREATE_VARIABLE
#endif
#define QUEST_CREATE_VARIABLE(name, value) \
    Quest::Variable<type> name(#name);

#ifdef QUEST_CREATE_VARIABLE_WITH_ZERO
#undef QUEST_CREATE_VARIABLE_WITH_ZERO
#endif
#define QUEST_CREATE_VARIABLE_WITH_ZERO(type,name,zero) \
    Quest::Variable<type> name(#name,zero);

#ifdef QUEST_CREATE_3D_VARIABLE_WITH_THIS_COMPONENTS
#undef QUEST_CREATE_3D_VARIABLE_WITH_THIS_COMPONENTS
#endif
#define QUEST_CREATE_3D_VARIABLE_WITH_THIS_COMPONENTS(name, component1, component2, component3) \
    Quest::Variable<Quest::Array1d<double,3> > name(#name, Quest::Array1d<double,3>(Quest::ZeroVector(3)));    \
    Quest::Variable<double> component1(#component1,&name,0);    \
    Quest::Variable<double> component2(#component2,&name,1);    \
    Quest::Variable<double> component3(#component3,&name,2);

#ifdef QUEST_CREATE_3D_VARIABLE_WITH_COMPONENTS
#undef QUEST_CREATE_3D_VARIABLE_WITH_COMPONENTS
#endif
#define QUEST_CREATE_3D_VARIABLE_WITH_COMPONENTS(name)  \
    QUEST_CREATE_3D_VARIABLE_WITH_THIS_COMPONENTS(name, name##_X, name##_Y, name##_Z)

#ifdef QUEST_CREATE_SYMMETRIC_2D_TENSOR_VARIABLE_WITH_THIS_COMPONENTS
#undef QUEST_CREATE_SYMMETRIC_2D_TENSOR_VARIABLE_WITH_THIS_COMPONENTS
#endif
#define QUEST_CREATE_SYMMETRIC_2D_TENSOR_VARIABLE_WITH_THIS_COMPONENTS(name, component1, component2, component3) \
    Quest::Variable<Quest::Array1d<double,3> > name(#name, Quest::zero_vector<double>(3));    \        
    Quest::Variable<double> component1(#component1,&name,0);    \    
    Quest::Variable<double> component2(#component2,&name,1);    \
    Quest::Variable<double> component3(#component3,&name,2);

#ifdef QUEST_CREATE_SYMMETRIC_2D_TENSOR_VARIABLE_WITH_COMPONENTS
#undef QUEST_CREATE_SYMMETRIC_2D_TENSOR_VARIABLE_WITH_COMPONENTS
#endif
#define QUEST_CREATE_SYMMETRIC_2D_TENSOR_VARIABLE_WITH_COMPONENTS(name)  \
    QUEST_CREATE_SYMMETRIC_2D_TENSOR_VARIABLE_WITH_THIS_COMPONENTS(name, name##_XX, name##_YY, name##_XY)

#ifdef QUEST_CREATE_SYMMETRIC_3D_TENSOR_VARIABLE_WITH_THIS_COMPONENTS
#undef QUEST_CREATE_SYMMETRIC_3D_TENSOR_VARIABLE_WITH_THIS_COMPONENTS
#endif
#define QUEST_CREATE_SYMMETRIC_3D_TENSOR_VARIABLE_WITH_THIS_COMPONENTS(name, component1, component2, component3, component4, component5, component6) \
    Quest::Variable<Quest::Array1d<double,6> > name(#name, Quest::zero_vector<double>(6));    \
    Quest::Variable<double> component1(#component1,&name,0);    \
    Quest::Variable<double> component2(#component2,&name,1);    \
    Quest::Variable<double> component3(#component3,&name,2);    \
    Quest::Variable<double> component4(#component4,&name,3);    \
    Quest::Variable<double> component5(#component5,&name,4);    \
    Quest::Variable<double> component6(#component6,&name,5);

#ifdef QUEST_CREATE_SYMMETRIC_3D_TENSOR_VARIABLE_WITH_COMPONENTS
#undef QUEST_CREATE_SYMMETRIC_3D_TENSOR_VARIABLE_WITH_COMPONENTS
#endif
#define QUEST_CREATE_SYMMETRIC_3D_TENSOR_VARIABLE_WITH_COMPONENTS(name)  \
    QUEST_CREATE_SYMMETRIC_3D_TENSOR_VARIABLE_WITH_THIS_COMPONENTS(name, name##_XX, name##_YY, name##_ZZ, name##_XY, name##_YZ, name##_XZ)

#ifdef QUEST_CREATE_2D_TENSOR_VARIABLE_WITH_THIS_COMPONENTS
#undef QUEST_CREATE_2D_TENSOR_VARIABLE_WITH_THIS_COMPONENTS
#endif
#define QUEST_CREATE_2D_TENSOR_VARIABLE_WITH_THIS_COMPONENTS(name, component1, component2, component3, component4) \
    Quest::Variable<Quest::Array1d<double,4> > name(#name, Quest::zero_vector<double>(4));    \
    Quest::Variable<double> component1(#component1,&name,0);    \
    Quest::Variable<double> component2(#component2,&name,1);    \
    Quest::Variable<double> component3(#component3,&name,2);    \
    Quest::Variable<double> component4(#component4,&name,3);

#ifdef QUEST_CREATE_2D_TENSOR_VARIABLE_WITH_COMPONENTS
#undef QUEST_CREATE_2D_TENSOR_VARIABLE_WITH_COMPONENTS
#endif
#define QUEST_CREATE_2D_TENSOR_VARIABLE_WITH_COMPONENTS(name)  \
    QUEST_CREATE_2D_TENSOR_VARIABLE_WITH_THIS_COMPONENTS(name, name##_XX, name##_XY, name##_YX, name##_YY)

#ifdef QUEST_CREATE_3D_TENSOR_VARIABLE_WITH_THIS_COMPONENTS
#undef QUEST_CREATE_3D_TENSOR_VARIABLE_WITH_THIS_COMPONENTS
#endif
#define QUEST_CREATE_3D_TENSOR_VARIABLE_WITH_THIS_COMPONENTS(name, component1, component2, component3, component4, component5, component6, component7, component8, component9) \
    Quest::Variable<Quest::Array1d<double,9> > name(#name, Quest::zero_vector<double>(9));    \
    Quest::Variable<double> component1(#component1,&name,0);    \
    Quest::Variable<double> component2(#component2,&name,1);    \
    Quest::Variable<double> component3(#component3,&name,2);    \
    Quest::Variable<double> component4(#component4,&name,3);    \
    Quest::Variable<double> component5(#component5,&name,4);    \
    Quest::Variable<double> component6(#component6,&name,5);    \
    Quest::Variable<double> component7(#component7,&name,6);    \
    Quest::Variable<double> component8(#component8,&name,7);    \
    Quest::Variable<double> component9(#component9,&name,8);

#ifdef QUEST_CREATE_3D_TENSOR_VARIABLE_WITH_COMPONENTS
#undef QUEST_CREATE_3D_TENSOR_VARIABLE_WITH_COMPONENTS
#endif
#define QUEST_CREATE_3D_TENSOR_VARIABLE_WITH_COMPONENTS(name)  \
    QUEST_CREATE_3D_TENSOR_VARIABLE_WITH_THIS_COMPONENTS(name, name##_XX, name##_XY, name##_XZ, name##_YX, name##_YY, name##_YZ, name##_ZX, name##_ZY, name##_ZZ)

#ifdef QUEST_REGISTER_VARIABLE
#undef QUEST_REGISTER_VARIABLE
#endif
#define QUEST_REGISTER_VARIABLE(name) \
    AddQuestComponent(name.Name(), name);    \
    QuestComponents<VariableData>::Add(name.Name(), name);

#ifdef QUEST_REGISTER_3D_VARIABLE_WITH_COMPONENTS
#undef QUEST_REGISTER_3D_VARIABLE_WITH_COMPONENTS
#endif
#define QUEST_REGISTER_3D_VARIABLE_WITH_COMPONENTS(name)  \
    QUEST_REGISTER_VARIABLE(name)    \
    QUEST_REGISTER_VARIABLE(name##_X)    \
    QUEST_REGISTER_VARIABLE(name##_Y)    \
    QUEST_REGISTER_VARIABLE(name##_Z)

#ifdef QUEST_REGISTER_SYMMETRIC_2D_TENSOR_VARIABLE_WITH_COMPONENTS
#undef QUEST_REGISTER_SYMMETRIC_2D_TENSOR_VARIABLE_WITH_COMPONENTS
#endif
#define QUEST_REGISTER_SYMMETRIC_2D_TENSOR_VARIABLE_WITH_COMPONENTS(name)  \
    QUEST_REGISTER_VARIABLE(name)    \
    QUEST_REGISTER_VARIABLE(name##_XX)    \
    QUEST_REGISTER_VARIABLE(name##_YY)    \
    QUEST_REGISTER_VARIABLE(name##_XY)

#ifdef QUEST_REGISTER_SYMMETRIC_3D_TENSOR_VARIABLE_WITH_COMPONENTS
#undef QUEST_REGISTER_SYMMETRIC_3D_TENSOR_VARIABLE_WITH_COMPONENTS
#endif
#define QUEST_REGISTER_SYMMETRIC_3D_TENSOR_VARIABLE_WITH_COMPONENTS(name)  \
    QUEST_REGISTER_VARIABLE(name)    \
    QUEST_REGISTER_VARIABLE(name##_XX)    \
    QUEST_REGISTER_VARIABLE(name##_YY)    \
    QUEST_REGISTER_VARIABLE(name##_ZZ)    \
    QUEST_REGISTER_VARIABLE(name##_XY)    \
    QUEST_REGISTER_VARIABLE(name##_YZ)    \
    QUEST_REGISTER_VARIABLE(name##_XZ)

#ifdef QUEST_REGISTER_2D_TENSOR_VARIABLE_WITH_COMPONENTS
#undef QUEST_REGISTER_2D_TENSOR_VARIABLE_WITH_COMPONENTS
#endif
#define QUEST_REGISTER_2D_TENSOR_VARIABLE_WITH_COMPONENTS(name)  \
    QUEST_REGISTER_VARIABLE(name)    \
    QUEST_REGISTER_VARIABLE(name##_XX)    \
    QUEST_REGISTER_VARIABLE(name##_XY)    \
    QUEST_REGISTER_VARIABLE(name##_YX)    \
    QUEST_REGISTER_VARIABLE(name##_YY)

#ifdef QUEST_REGISTER_3D_TENSOR_VARIABLE_WITH_COMPONENTS
#undef QUEST_REGISTER_3D_TENSOR_VARIABLE_WITH_COMPONENTS
#endif
#define QUEST_REGISTER_3D_TENSOR_VARIABLE_WITH_COMPONENTS(name)  \
    QUEST_REGISTER_VARIABLE(name)    \
    QUEST_REGISTER_VARIABLE(name##_XX)    \
    QUEST_REGISTER_VARIABLE(name##_XY)    \
    QUEST_REGISTER_VARIABLE(name##_XZ)    \
    QUEST_REGISTER_VARIABLE(name##_YX)    \
    QUEST_REGISTER_VARIABLE(name##_YY)    \
    QUEST_REGISTER_VARIABLE(name##_YZ)    \
    QUEST_REGISTER_VARIABLE(name##_ZX)    \
    QUEST_REGISTER_VARIABLE(name##_ZY)    \
    QUEST_REGISTER_VARIABLE(name##_ZZ)


/*-------------------------------------------------
与时间相关变量的宏定义
-------------------------------------------------*/

#ifdef QUEST_CREATE_VARIABLE_WITH_TIME_DERIVATIVE
#undef QUEST_CREATE_VARIABLE_WITH_TIME_DERIVATIVE
#endif
#define QUEST_CREATE_VARIABLE_WITH_TIME_DERIVATIVE(type,name,variable_derivative) \
    Quest::Variable<type> name(#name,&variable_derivative);

#ifdef QUEST_CREATE_3D_VARIABLE_WITH_THIS_COMPONENTS_WITH_TIME_DERIVATIVE
#undef QUEST_CREATE_3D_VARIABLE_WITH_THIS_COMPONENTS_WITH_TIME_DERIVATIVE
#endif
#define QUEST_CREATE_3D_VARIABLE_WITH_THIS_COMPONENTS_WITH_TIME_DERIVATIVE(name, component1, component2, component3, variable_derivative) \
    Quest::Variable<Quest::Array1d<double,3> > name(#name, Quest::Array1d<double,3>(Quest::ZeroVector(3)), &variable_derivative);    \
    Quest::Variable<double> component1(#component1,&name,0, &variable_derivative##_X);    \    
    Quest::Variable<double> component2(#component2,&name,1, &variable_derivative##_Y);    \
    Quest::Variable<double> component3(#component3,&name,2, &variable_derivative##_Z);

#ifdef QUEST_CREATE_3D_VARIABLE_WITH_COMPONENTS_WITH_TIME_DERIVATIVE
#undef QUEST_CREATE_3D_VARIABLE_WITH_COMPONENTS_WITH_TIME_DERIVATIVE
#endif
#define QUEST_CREATE_3D_VARIABLE_WITH_COMPONENTS_WITH_TIME_DERIVATIVE(name, variable_derivative)  \
    QUEST_CREATE_3D_VARIABLE_WITH_THIS_COMPONENTS_WITH_TIME_DERIVATIVE(name, name##_X, name##_Y, name##_Z, variable_derivative)

#ifdef QUEST_CREATE_SYMMETRIC_2D_TENSOR_VARIABLE_WITH_THIS_COMPONENTS_WITH_TIME_DERIVATIVE
#undef QUEST_CREATE_SYMMETRIC_2D_TENSOR_VARIABLE_WITH_THIS_COMPONENTS_WITH_TIME_DERIVATIVE
#endif
#define QUEST_CREATE_SYMMETRIC_2D_TENSOR_VARIABLE_WITH_THIS_COMPONENTS_WITH_TIME_DERIVATIVE(name, component1, component2, component3, variable_derivative) \
    Quest::Variable<Quest::Array1d<double,3> > name(#name, Quest::zero_vector<double>(3), &variable_derivative);    \        
    Quest::Variable<double> component1(#component1,&name,0, &variable_derivative##_XX);    \    
    Quest::Variable<double> component2(#component2,&name,1, &variable_derivative##_YY);    \
    Quest::Variable<double> component3(#component3,&name,2, &variable_derivative##_XY);

#ifdef QUEST_CREATE_SYMMETRIC_2D_TENSOR_VARIABLE_WITH_COMPONENTS_WITH_TIME_DERIVATIVE
#undef QUEST_CREATE_SYMMETRIC_2D_TENSOR_VARIABLE_WITH_COMPONENTS_WITH_TIME_DERIVATIVE
#endif
#define QUEST_CREATE_SYMMETRIC_2D_TENSOR_VARIABLE_WITH_COMPONENTS_WITH_TIME_DERIVATIVE(name, variable_derivative)  \
    QUEST_CREATE_SYMMETRIC_2D_TENSOR_VARIABLE_WITH_THIS_COMPONENTS_WITH_TIME_DERIVATIVE(name, name##_XX, name##_YY, name##_XY, variable_derivative)

#ifdef QUEST_CREATE_SYMMETRIC_3D_TENSOR_VARIABLE_WITH_THIS_COMPONENTS_WITH_TIME_DERIVATIVE
#undef QUEST_CREATE_SYMMETRIC_3D_TENSOR_VARIABLE_WITH_THIS_COMPONENTS_WITH_TIME_DERIVATIVE
#endif
#define QUEST_CREATE_SYMMETRIC_3D_TENSOR_VARIABLE_WITH_THIS_COMPONENTS_WITH_TIME_DERIVATIVE(name, component1, component2, component3, component4, component5, component6, variable_derivative) \
    Quest::Variable<Quest::Array1d<double,6> > name(#name, Quest::zero_vector<double>(6), &variable_derivative);    \
    Quest::Variable<double> component1(#component1,&name,0, &variable_derivative##_XX);    \
    Quest::Variable<double> component2(#component2,&name,1, &variable_derivative##_YY);    \
    Quest::Variable<double> component3(#component3,&name,2, &variable_derivative##_ZZ);    \
    Quest::Variable<double> component4(#component4,&name,3, &variable_derivative##_XY);    \
    Quest::Variable<double> component5(#component5,&name,4, &variable_derivative##_YZ);    \
    Quest::Variable<double> component6(#component6,&name,5, &variable_derivative##_XZ);

#ifdef QUEST_CREATE_SYMMETRIC_3D_TENSOR_VARIABLE_WITH_COMPONENTS_WITH_TIME_DERIVATIVE
#undef QUEST_CREATE_SYMMETRIC_3D_TENSOR_VARIABLE_WITH_COMPONENTS_WITH_TIME_DERIVATIVE
#endif
#define QUEST_CREATE_SYMMETRIC_3D_TENSOR_VARIABLE_WITH_COMPONENTS_WITH_TIME_DERIVATIVE(name, variable_derivative)  \
    QUEST_CREATE_SYMMETRIC_3D_TENSOR_VARIABLE_WITH_THIS_COMPONENTS_WITH_TIME_DERIVATIVE(name, name##_XX, name##_YY, name##_ZZ, name##_XY, name##_YZ, name##_XZ, variable_derivative)

#ifdef QUEST_CREATE_2D_TENSOR_VARIABLE_WITH_THIS_COMPONENTS_WITH_TIME_DERIVATIVE
#undef QUEST_CREATE_2D_TENSOR_VARIABLE_WITH_THIS_COMPONENTS_WITH_TIME_DERIVATIVE
#endif
#define QUEST_CREATE_2D_TENSOR_VARIABLE_WITH_THIS_COMPONENTS_WITH_TIME_DERIVATIVE(name, component1, component2, component3, component4, variable_derivative) \
    Quest::Variable<Quest::Array1d<double,4> > name(#name, Quest::zero_vector<double>(4), &variable_derivative);    \
    Quest::Variable<double> component1(#component1,&name,0, &variable_derivative##_XX);    \
    Quest::Variable<double> component2(#component2,&name,1, &variable_derivative##_XY);    \
    Quest::Variable<double> component3(#component3,&name,2, &variable_derivative##_YX);    \
    Quest::Variable<double> component4(#component4,&name,3, &variable_derivative##_YY);

#ifdef QUEST_CREATE_2D_TENSOR_VARIABLE_WITH_COMPONENTS_WITH_TIME_DERIVATIVE
#undef QUEST_CREATE_2D_TENSOR_VARIABLE_WITH_COMPONENTS_WITH_TIME_DERIVATIVE
#endif
#define QUEST_CREATE_2D_TENSOR_VARIABLE_WITH_COMPONENTS_WITH_TIME_DERIVATIVE(name, variable_derivative)  \
    QUEST_CREATE_2D_TENSOR_VARIABLE_WITH_THIS_COMPONENTS_WITH_TIME_DERIVATIVE(name, name##_XX, name##_XY, name##_YX, name##_YY, variable_derivative)

#ifdef QUEST_CREATE_3D_TENSOR_VARIABLE_WITH_THIS_COMPONENTS_WITH_TIME_DERIVATIVE
#undef QUEST_CREATE_3D_TENSOR_VARIABLE_WITH_THIS_COMPONENTS_WITH_TIME_DERIVATIVE
#endif
#define QUEST_CREATE_3D_TENSOR_VARIABLE_WITH_THIS_COMPONENTS_WITH_TIME_DERIVATIVE(name, component1, component2, component3, component4, component5, component6, component7, component8, component9, variable_derivative) \
    Quest::Variable<Quest::Array1d<double,9> > name(#name, Quest::zero_vector<double>(9), &variable_derivative);    \
    Quest::Variable<double> component1(#component1,&name,0, &variable_derivative##_XX);    \
    Quest::Variable<double> component2(#component2,&name,1, &variable_derivative##_XY);    \
    Quest::Variable<double> component3(#component3,&name,2, &variable_derivative##_XZ);    \
    Quest::Variable<double> component4(#component4,&name,3, &variable_derivative##_YX);    \
    Quest::Variable<double> component5(#component5,&name,4, &variable_derivative##_YY);    \
    Quest::Variable<double> component6(#component6,&name,5, &variable_derivative##_YZ);    \
    Quest::Variable<double> component7(#component7,&name,6, &variable_derivative##_ZX);    \
    Quest::Variable<double> component8(#component8,&name,7, &variable_derivative##_ZY);    \
    Quest::Variable<double> component9(#component9,&name,8, &variable_derivative##_ZZ);

#ifdef QUEST_CREATE_3D_TENSOR_VARIABLE_WITH_COMPONENTS_WITH_TIME_DERIVATIVE
#undef QUEST_CREATE_3D_TENSOR_VARIABLE_WITH_COMPONENTS_WITH_TIME_DERIVATIVE
#endif
#define QUEST_CREATE_3D_TENSOR_VARIABLE_WITH_COMPONENTS_WITH_TIME_DERIVATIVE(name, variable_derivative)  \
    QUEST_CREATE_3D_TENSOR_VARIABLE_WITH_THIS_COMPONENTS_WITH_TIME_DERIVATIVE(name, name##_XX, name##_XY, name##_XZ, name##_YX, name##_YY, name##_YZ, name##_ZX, name##_ZY, name##_ZZ, variable_derivative)


/*---------------------------------------------------
Flags
----------------------------------------------------*/

#ifndef QUEST_CREATE_FLAG
#undef QUEST_CREATE_FLAG
#endif
#define QUEST_CREATE_FLAG(name, positon)        \
    const Quest::Flags name(Quest::Flags::Create(positon));

#ifdef QUEST_DEFINE_LOCAL_FLAG
#undef QUEST_DEFINE_LOCAL_FLAG
#endif 
#define QUEST_DEFINE_LOCAL_FLAG(name) \
    static const Quest::Flags name;

#ifdef QUEST_CREATE_LOCAL_FLAG
#undef QUEST_CREATE_LOCAL_FLAG
#endif
#define QUEST_CREATE_LOCAL_FLAG(class_name, name, position)   \
    const Quest::Flags class_name::name(Quest::Flags::Create(position));


#ifdef QUEST_CREATE_LOCAL_FLAG
#undef QUEST_CREATE_LOCAL_FLAG
#endif
#define QUEST_CREATE_LOCAL_FLAG(class_name, name, position)   \
    const Quest::Flags class_name::name(Quest::Flags::Create(position));

/*---------------------------------------------------
components
----------------------------------------------------*/


#define QUEST_DEPRECATED [[deprecated]]
#define QUEST_DEPRECATED_MSG(msg) [[deprecated(msg)]]

#define QUEST_SERIALIZE_SAVE_BASE_CLASS(Serializer,BaseType) \
    Serializer.save_base("BaseClass", *static_cast<const BaseType*>(this));

#define QUEST_SERIALIZE_LOAD_BASE_CLASS(Serializer,BaseType) \
    Serializer.load_base("BaseClass", *static_cast<BaseType*>(this));


#define QUEST_SERIALIZE_SAVE_BASE_CLASS(Serializer,BaseType)  \
    Serializer.save_base("BaseClass", *static_cast<const BaseType*>(this));

#define QUEST_SERIALIZE_LOAD_BASE_CLASS(Serializer,BaseType)  \
    Serializer.load_base("BaseClass", *static_cast<BaseType*>(this));


namespace Quest{


    // 用于调试和输出变量值的工具，方便在程序运行时打印变量的值和信息
    #define QUEST_WATCH(variable) std::cout << #variable << " : " << variable << std::endl;
    #define QUEST_WAATCH_CERR(variable) std::cerr << #variable << " : " << variable << std::endl;
    #define QUEST_WATCH_MPI(variable, mpi_data_comm) std::cout << "RANK " << mpi_data_comm.Rank() << "/" << mpi_data_comm.Size() << " : "; QUEST_WATCH(variable);
    
}


#endif // DEFINE_H