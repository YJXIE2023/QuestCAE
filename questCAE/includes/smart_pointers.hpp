/*----------------------------------
包含智能指针代码的头文件
------------------------------------*/

#ifndef QUEST_SMART_POINTERS_HPP
#define QUEST_SMART_POINTERS_HPP


// 标准头文件
#include <iostream>
#include <utility>
#include <memory>

// 自定义头文件
#include "intrusive_ptr/intrusive_ptr.hpp"

namespace Quest{

    template<typename T>
    using shared_ptr = std::shared_ptr<T>;

    template<typename T>
    using weak_ptr = std::weak_ptr<T>;

    template<typename T>
    using unique_ptr = std::unique_ptr<T>;

    template<typename C, typename ...Args>
    intrusive_ptr<C> make_intrusive(Args&&... args){
        return intrusive_ptr<C>(new C(std::forward<Args>(args)...));
    }

    template<typename C, typename ...Args>
    shared_ptr<C> make_shared(Args&&... args){
        return std::make_shared<C>(std::forward<Args>(args)...);
    }

    template<typename C, typename ...Args>
    unique_ptr<C> make_unique(Args&&... args){
        return std::make_unique<C>(std::forward<Args>(args)...);
    }

    template<typename T>
    std::ostream& operator<<(std::ostream& os,const Quest::weak_ptr<T>& rData){
        if(!rData.expired())
            os<<*rData.lock().get();
        else
            os<<"空指针";
        return os;
    }

    template<typename T>
    std::ostream& operator<<(std::ostream& os,const Quest::shared_ptr<T>& rData){
        os<<*rData.get();
        return os;
    }
} // namespace Quest


#define QUEST_CLASS_POINTER_DEFINITION(ClassName)    \
    typedef Quest::shared_ptr<ClassName> Pointer;   \
    typedef Quest::shared_ptr<ClassName> SharedPointer;   \
    typedef Quest::weak_ptr<ClassName> WeakPointer;   \
    typedef Quest::unique_ptr<ClassName> UniquePointer;   

namespace Quest{
    template<typename T>
    class GlobalPointer;
}

#define QUEST_CLASS_INTRUSIVE_POINTER_DEFINITION(ClassName)    \
    typedef typename Quest::intrusive_ptr<ClassName> Pointer;   \
    typedef Quest::GlobalPointer<ClassName> WeakPointer;   \
    typedef Quest::unique_ptr<ClassName> UniquePointer;    \
    typename ClassName::Pointer shared_from_this(){return ClassName::Pointer(this);}


#endif // SMART_POINTERS_HPP