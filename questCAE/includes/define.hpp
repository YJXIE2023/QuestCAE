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
exception
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


#endif // DEFINE_H