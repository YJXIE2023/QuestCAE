/*--------------------------------------------
intrusive_ptr智能指针
--------------------------------------------*/

#ifndef INTRUSIVE_PTR_HPP
#define INTRUSIVE_PTR_HPP

// 系统依赖
#include <cstddef>
#include <functional>
#include <iostream>

namespace Quest{

    namespace detail{
        template<typename Y, typename X>
        struct sp_convertible{
            typedef char (&yes) [1];
            typedef char (&no) [2];
            static yes f(T*);
            static no f(...);
            enum _vt {value = size((f)(static_cast<Y*>(0))) == sizeof(yes)};
        };
    
        template<typename Y, typename T>
        struct sp_convertible<Y,T[]>{
            enum _vt{value = false};
        };

        template<typename Y, typename T>
        struct sp_convertible<Y[], T[]>{
            enum _vt{value = sp_convertible<Y[1], T[1]>::value};
        };

        template<typename Y, std::size_t N,typename T>
        struct sp_convertible<Y[N], T[]>{
            enum _vt{value = sp_convertible<Y[1], T[1]>::value};
        };

        struct sp_empty{};

        template<bool> struct sp_enable_if_convertible_impl;

        template<> 
        struct sp_enable_if_convertible_impl<true>{
            typedef sp_empty type;
        };

        template<>
        struct sp_enable_if_convertible_impl<false>{};

        template<typename Y,typename T>
        struct sp_enble_if_convertible:public sp_enable_if_convertible_impl<sp_convertible<Y,T>::value>{};
    }

    template<typename T>
    class intrusive_ptr{
        private:
            typedef intrusive_ptr this_type;

        public: 
            typedef T element_type;

            constexpr intrusive_ptr() noexcept:px(nullptr){};

            intrusive_ptr(T* p,bool add_ref=true):px(p){
                if(px != nullptr && add_ref)
                    intrusive_ptr_add_ref(px);
            }

            template<typename U>
            intrusive_ptr(const intrusive_ptr<U>& rhs, typename Quest::detail::sp_enable_if_convertible<U,T>::type = Quest::detail::sp_empty()):px(rhs.get()){
                if(px != nullptr)
                    rhs.px = nullptr;
            }

            template<typename U>
            intrusive_ptr& operator=(intrusive_ptr<U> && rhs) noexcept{
                this_type(static_cast<intrusive_ptr<U> &&>(rhs)).swap(*this);
                return *this;
            }

            intrusive_ptr& operator=(const intrusive_ptr& rhs){
                this_type(rhs).swap(*this);
                return *this;
            }

            intrusive_ptr& operator=(T* rhs){
                this_type(rhs).swap(*this);
                return *this;
            }

            void reset(){
                this_type().swap(*this);
            }

            void reset(T* rhs){
                this_type(rhs).swap(*this);
            }

            void reset(T* rhs,bool add_ref){
                this_type(rhs,add_ref).swap(*this);
            }

            T* get() const noexcept{
                return px;
            }

            T* detach() noexcept{
                T* ret = px;
                px = nullptr;
                return ret;
            }

            T& operator*() const noexcept{
                (static_cast<void>(0));
                return *px;
            }

            T* operator->() const noexcept{
                (static_cast<void>(0));
                return px;
            }

            explicit operator bool() const noexcept{
                return px != nullptr;
            }

            bool operator!() const noexcept{
                return px == nullptr;
            }
            
            void swap(intrusive_ptr& rhs) noexcept{
                T* tmp = px;
                px = rhs.px;
                rhs.px = tmp;
            }

        private:
            T* px;
    };

    template<typename T,typename U>
    inline bool operator==(const intrusive_ptr<T>& a,const intrusive_ptr<U>& b)noexcept{
        return a.get() == b.get();
    }

    template<typename T,typename U>
    inline bool operator!=(const intrusive_ptr<T>& a,const intrusive_ptr<U>& b)noexcept{
        return a.get()!= b.get();
    }

    template<typename T,typename U>
    inline bool operator==(const intrusive_ptr<T>& a,U* b)noexcept{
        return a.get() == b;
    }

    template<typename T,typename U>
    inline bool operator!=(const intrusive_ptr<T>& a,U* b)noexcept{
        return a.get()!= b;
    }

    template<typename T,typename U>
    inline bool operator==(T* a,const intrusive_ptr<U>& b)noexcept{
        return a == b.get();
    }

    template<typename T,typename U>
    inline bool operator!=(T* a,const intrusive_ptr<U>& b)noexcept{
        return a != b.get();
    }

    template<typename T>
    inline bool operator==(const intrusive_ptr<T>& a,std::nullptr_t)noexcept{
        return a.get() == nullptr;
    }

    template<typename T>
    inline bool operator!=(const intrusive_ptr<T>& a,std::nullptr_t)noexcept{
        return a.get()!= nullptr;
    }

    template<typename T>
    inline bool operator==(std::nullptr_t,const intrusive_ptr<T>& b)noexcept{
        return nullptr == b.get();
    }

    template<typename T>
    inline bool operator!=(std::nullptr_t,const intrusive_ptr<T>& b)noexcept{
        return nullptr != b.get();
    }

    template<typename T>
    inline bool operator<(const intrusive_ptr<T>& a,const intrusive_ptr<T>& b)noexcept{
        return std::less<T*>()(a.get(),b.get());
    }

    template<typename T>
    void swap(intrusive_ptr<T>& lhs,intrusive_ptr<T>& rhs) noexcept{
        lhs.swap(rhs);
    }

    template<typename T>
    T* get_pointer(const intrusive_ptr<T>& p) noexcept{
        return p.get();
    }

    template<typename T,typename U>
    intrusive_ptr<T> static_pointer_cast(const intrusive_ptr<U>& p) {
        return static_cast<T*>(p.get());
    }

    template<typename T,typename U>
    intrusive_ptr<T> const_pointer_cast(const intrusive_ptr<U>& p) {
        return const_cast<T*>(p.get());
    }

    template<typename T,typename U>
    intrusive_ptr<T> dynamic_pointer_cast(const intrusive_ptr<U>& p) {
        return dynamic_cast<T*>(p.get());
    }

    template<typename E,typename T,typename Y>
    std::basic_ostream<E,T>& operator<<(std::basic_ostream<E,T>& os,const intrusive_ptr<Y>& p){
        os << p.get();
        return os;
    }

    template<typename T> struct hash;

    template<typename T>
    std::size_t hash_value(Quest::intrusive_ptr<T> const& p) noexcept{
        return std::hash<T*>()(p.get());
    }
}


#endif // INTRUSIVE_PTR_HPP