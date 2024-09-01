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