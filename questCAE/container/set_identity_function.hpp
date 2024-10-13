/*-----------------------------------------------
定义一个恒等函数，提供一种方式来直接返回输入的对象
通常用于集合（set）或其他需要对象比较的容器中，
避免手动定义自定义的比较函数或操作符。
-----------------------------------------------*/

#ifndef QUEST_SET_IDENTITY_FUNCTION_HPP
#define QUEST_SET_IDENTITY_FUNCTION_HPP

// 系统头文件
#include <functional>

namespace Quest{

    template<typename TDataType>
    class SetIdentityFunction{
        public:
            TDataType& operator()(TDataType& data){
                return data;
            }

            const TDataType& operator()(const TDataType& data) const{
                return data;
            }

        protected:

        private:

    };

} // namespace Quest


#endif //QUEST_SET_IDENTITY_FUNCTION_HPP