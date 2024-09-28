/*----------------------------------------------
为模板参数 TCountedType 提供一个计数器机制
用于追踪某种类型的实例创建、访问或其他相关操作的次数
----------------------------------------------*/

#ifndef QUEST_COUNTER_HPP
#define QUEST_COUNTER_HPP

// 系统头文件
#include <string>
#include <iostream>

// 项目头文件
#include "includes/define.hpp"

namespace Quest{

    template<typename TCountedType>
    class Counter{
        public:
            QUEST_CLASS_POINTER_DEFINITION(Counter);
            using SizeType = unsigned int;

            static SizeType GetCounter(){
                return Counter<TCountedType>::msCounter;
            }

            static SizeType Increment(){
                return ++Counter<TCountedType>::msCounter;
            }

            virtual std::string Info() const {
                return std::string("Counter");
            }

            virtual void PrintInfo(std::ostream& rOstream) const {
                rOstream << Info();
            }

            virtual void PrintData(std::ostream& rOstream) const {
                rOstream << Counter();
            }

        protected:
            Counter() {}

            Counter(const Counter<TCountedType>& rOther) {}

            virtual ~Counter() {}

        private:
            static SizeType msCounter;

            Counter& operator=(const Counter& rOther);

    };

    template<typename TCountedType>
    inline std::istream& operator >> (std::istream& rIstream, Counter<TCountedType>& rCounter){}

    template<typename TCountedType>
    inline std::ostream& operator << (std::ostream& rOstream, const Counter<TCountedType>& rCounter){
        rThis.PrintInfo(rOstream);
        rOstream << " : ";
        rThis.PrintData(rOstream);

        return rOstream;
    }

    template<typename TCountedType>
    typename Counter<TCountedType>::SizeType Counter<TCountedType>::msCounter = 0;

} // namespace Quest

#endif // COUNTER_HPP