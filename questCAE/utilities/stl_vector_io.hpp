/*-----------------------------------------------------
定义了用于将std::vector对象输出到std::ostream的模板函数
------------------------------------------------------*/

#ifndef QUEST_STL_VECTOR_IO_HPP
#define QUEST_STL_VECTOR_IO_HPP

//系统头文件
#include <iostream>
#include <vector>

namespace Quest{

    //输出std::vector对象的值
    template<typename T>
    std::ostream& operator<<(std::ostream& rOstream, const std::vector<T>& rVec){

        std::size_t vector_size = rVec.size();

        rOstream << "[";
        if(vector_size>0) rOstream << rVec[0];
        for(std::size_t i=1; i<vector_size; ++i)
            rOstream<< ", "<<rVec[i];
        rOstream << "]";

        return rOstream;
    }

}//namespace quest


#endif //QUEST_STL_VECTOR_IO_HPP