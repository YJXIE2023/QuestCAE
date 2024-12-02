#ifndef QUEST_SET_IDENTITY_FUNCTION_HPP
#define QUEST_SET_IDENTITY_FUNCTION_HPP

// 系统头文件
#include <functional>

namespace Quest{

    /**
     * @class SetIdentityFunction
     * @brief 一个作为恒等函数的函数对象
     * @details 这个函数对象的目的是使对象能做做诶几何或其他需要比较的容器中的键使用
     * 当对象本身可以作为比较标准时，可以直接使用这个函数对象，而不需要自定义比较函数或操作符。
     */
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