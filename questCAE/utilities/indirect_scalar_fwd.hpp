/*------------------------------------------------------------------------------
IndirectScalar 类提供一个前向声明，并确保它只在未定义的情况下包含此文件，以避免重复定义
它引入了类型约束，确保模板参数 T 必须是算术类型（如整数或浮点数）
------------------------------------------------------------------------------*/

#ifndef QUEST_INDIRECT_SCALAR_FWD_HPP
#define QUEST_INDIRECT_SCALAR_FWD_HPP

// 系统头文件
#include <type_traits>

namespace Quest{

    template<typename T, typename S = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
    class IndirectScalar;

}


#endif //QUEST_INDIRECT_SCALAR_FWD_HPP