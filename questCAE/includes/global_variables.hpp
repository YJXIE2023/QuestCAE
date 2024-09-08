/*--------------------------------------
存储全局变量的头文件
--------------------------------------*/

#ifndef GLOBAL_VARIABLES_HPP
#define GLOBAL_VARIABLES_HPP

namespace Quest::Globals{
    constexpr double Pi = 3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651;

    static constexpr int MaxAllowedThreads = 128;

    enum class configuration{
        Initial = 0,
        Current = 1
    };

    enum class DataLocation{
        NodeHistorical,
        NodeNonHistorical,
        Element,
        Condition,
        ModelPart,
        ProcessInfo
    };
}

#endif // GLOBAL_VARIABLES_HPP