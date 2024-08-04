# QuestCAE

# 文件组成
# “external_libraries": 第三方库
    -“bost”: boost库
        -"instrusive_ptr.hpp":智能指针
        -"ublas_interface.hpp":线性代数运算


# "questCAE": CAE求解器主要代码
    -“condition”: 


    -"container": 容器


    -"dof":自由度相关


    -"element":单元类型
    

    -"geometry":几何相关，行函数和坐标变换


    -"includes":头文件
        -"check.hpp":检查宏定义
        -“code_location.hpp":错误代码位置
        -"define.hpp":宏定义文件
        -"exceptions.hpp":异常处理类
        -"quest_export_api.hpp":符号导入导出宏定义
        -”quest.hpp":包含命名空间的定义
        -"

    -"IO":输入输出


    -"matrix":矩阵相关


    -"mesh":网格相关


    -"model":


    -"process":新算法扩展接口


    -"quadrature":数值积分算法


    -"solver":求解


    -"sources":源文件


    -"saptialContainer":空间搜索算法相关容器


    -"strategy":求解过程


    -"utilities": 存放基础组件
        -“stl_vector_io.hpp”:输出std::vector中的数据