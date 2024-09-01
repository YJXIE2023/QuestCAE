/*-------------------------------------
#一维数组类
-------------------------------------*/

#ifndef ARRAY_1D_HPP
#define ARRAY_1D_HPP

//系统头文件
#include <string>
#include <iostream>
#include <array>
#include <algorithm>
#include <initializer_list>

//项目头文件
#include "includes/defines.hpp"
#include "boost/ublas_interface.hpp"

#include <boost/numeric/ublas/vector_expression.hpp>    //向量表达式基类
#include <boost/numeric/ublas/storage.hpp>    //不同方式存储向量
#include <boost/numeric/ublas/detail/vector_assign.hpp>    //涉及向量赋值的操作

namespace Quest{

    //一维数组类
    template<typename T, std::size_t N>
    class Array1d:public boost::numeric::ublas::vector_expression<Array1d<T, N> >{
        
        public:
            //一维数组类中类型别名
            QUEST_CLASS_POINTER_DEFINITION(Array1d);
            typedef std::size_t size_type;
            typedef std::ptrdiff_t difference_type;  //指针之间的差值
            typedef T value_type;
            typedef typename boost::numeric::ublas::type_traits<T>::const_reference const_reference;
            typedef T& reference;
            typedef std::array<T,N> array_type;
            typedef T* pointer;
            typedef Array1d<T,N> self_type;
            typedef const boost::numeric::ublas::vector_reference<const self_type> const_closure_type;
            typedef boost::numeric::ublas::vector_reference<self_type> closure_type;
            typedef self_type vector_temporary_type;
            typedef boost::numeric::ublas::dense_tag storage_category;

            //只读迭代器和迭代器的声明
            #ifdef BOOST_UBLAS_USE_INDEXED_ITERATOR
                typedef indexed_iterator<self_type, dense_random_access_iterator_tag> iterator;
                typedef indexed_const_iterator<self_type, dense_random_access_iterator_tag> const_iterator;
            #else
                class const_iterator;
                class iterator;
            #endif // BOOST_UBLAS_USE_INDEXED_ITERATOR

            //反向迭代器
            #ifdef BOOST_MSVC_STD_ITERATOR
                typedef reverse_iterator_base<const_iterator,value_type,const_reference> const_reverse_iterator;
                typedef reverse_iterator_base<iterator,value_type,reference> reverse_iterator;
            #else
                typedef boost::numeric::ublas::reverse_iterator_base<const_iterator> const_reverse_iterator;
                typedef boost::numeric::ublas::reverse_iterator_base<iterator> reverse_iterator;
            #endif // BOOST_MSVC_STD_ITERATOR

            //只读迭代器的实现
            #ifndef BOOST_UBLAS_USE_INDEXED_ITERATOR
            class const_iterator:public container_const_iterator<Array1d>,
                public random_access_iterator_base<dense_random_access_iterator_tag,const_iterator,value_type,difference_type>
            {
                public:
                    //类型别名
                    typedef dense_random_access_iterator_tag iterator_category;
                    #ifdef BOOST_MSVC_STD_ITERATOR
                        typedef const_reference reference;
                    #else
                        typedef typename Array1d::difference_type difference_type;
                        typedef typename Array1d::value_type value_type;
                        typedef typename Array1d::const_reference reference;
                        typedef const typename Array1d::const_pointer pointer;
                    #endif

                    BOOST_UBLAS_INLINE
                    const_iterator():container_const_iterator<Array1d>(),it_(){}

                    BOOST_UBLAS_INLINE
                    const_iterator(const self_type& v,const_iterator_type it_):
                        container_const_reference<self_type>(v),it_(it){}

                    #ifdef BOOST_UBALS_QUALIFIED_TYPENAME
                        const_iterator(const iterator& it):
                    #else
                        cosnt_iterator(const typename slef_type::iterator& it):
                    #endif
                        container_const_reference<self_type>(it()),it_(it.it_){}

                    BOOST_UBLAS_INLINE
                    const_iterator& operator++(){
                        ++it_;
                        return *this;
                    }

                    BOOST_UBLAS_INLINE
                    const_iterator& operator--(){
                        --it_;
                        return *this;
                    }

                    BOOST_UBLAS_INLINE
                    const_iterator& operator+=(difference_type n){
                        it_ += n;
                        return *this;
                    }

                    BOOST_UBLAS_INLINE
                    const_iterator& operator-=(difference_typee n){
                        it_ -= n;
                        return *this;
                    }

                    BOOST_UBLAS_INLINE
                    difference_type operator-(const const_iterator& it) const{
                        BOOST_UBLAS_CHECK(&(*this)()==&it(), external_logic());
                        return it_ - it.it_;
                    }

                    BOOST_UBLAS_INLINE
                    const_reference operator*() const{
                        BOOST_UBLAS_CHECK(it_->(*this)().begin().it_ && it_<(*this)().end().it_,bad_index());
                        return *it_;
                    }

                    BOOST_UBLAS_INLINE
                    size_type index() const{
                        BOOST_UBLAS_CHECK(it_>=(*this)().begin().it_ && it_<(*this)().end().it_,bad_index());
                        return it_-(*this)().begin().it_;
                    }

                    BOOST_UBLAS_INLINE
                    const_iterator& operator=(cibst cibst_iterator &it){
                        container_const_reference<self_type>::assign(&it());
                        it_ = it.it_;
                        return *this;
                    }

                    BOOST_UBLAS_INLINE
                    bool operator==(const const_iterator& it) const{
                        BOOST_UBLAS_CHECK(&(*this)()==&it(), external_logic());
                        return it_ == it.it_;
                    }

                    BOOST_UBLAS_INLINE
                    bool operator<(const const_iterator& it) const{
                        BOOST_UBLAS_CHECK(&(*this)()==&it(), external_logic());
                        return it_ < it.it_;
                    }
                protected:

                private:
                    const_iterator_type it_;
                    friend class iterator;
            };
            #endif // BOOST_UBLAS_USE_INDEXED_ITERATOR

            //迭代器的实现
            #ifndef BOOST_UBLAS_USE_INDEXED_ITERATOR
            class iterator:public container_reference<Array1d>,
                public random_access_iterator_base<dense_random_access_iterator_tag,iterator,value_type,difference_type>
            {
                public:
                    typedef dense_random_access_iterator_tag iterator_category;
                    #ifndef BOOST_MSVC_STD_ITERATOR
                        typedef typename Array1d::difference_type difference_type;
                        typedef typename Array1d::value_type value_type;
                        typedef typename Array1d::reference reference;
                        typedef typename Array1d::pointer pointer;
                    #endif

                    BOOST_UBLAS_INLINE
                    iterator&operator++(){
                        ++it_;
                        return *this;
                    }

                    BOOST_UBLAS_INLINE
                    iterator& operator--(){
                        --it_;
                        return *this;
                    }

                    BOOST_UBLAS_INLINE
                    iterator& operator +=(difference_type n){
                        it_ += n;
                        return *this;
                    }

                    BOOST_UBLAS_INLINE
                    iterator& operator-=(difference_type n){
                        it_ -= n;
                        return *this;
                    }

                    BOOST_UBLAS_INLINE
                    difference_type operator-(const iterator& it) const{
                        BOOST_UBLAS_CHECK(&(*this)()==&it(), external_logic());
                        return it_ - it.it_;
                    }

                    BOOST_UBLAS_INLINE
                    reference operator*() const{
                        BOOST_UBLAS_CHECK(it_->(*this)().begin().it_ && it_<(*this)().end().it_,bad_index());
                        return *it_;
                    }

                    BOOST_UBLAS_INLINE
                    size_type index() const{
                        BOOST_UBLAS_CHECK(it_>=(*this)().begin().it_ && it_<(*this)().end().it_,bad_index());
                        return it_-(*this)().begin().it_;
                    }

                    BOOST_UBLAS_INLINE
                    iterator& operator=(const_iterator cibst_iterator &it){
                        container_reference<self_type>::assign(&it());
                        it_ = it.it_;
                        return *this;
                    }

                    BOOST_UBLAS_INLINE
                    bool operator==(const iterator& it) const{
                        BOOST_UBLAS_CHECK(&(*this)()==&it(), external_logic());
                        return it_ == it.it_;
                    }

                    BOOST_UBLAS_INLINE
                    bool operator<(const iterator& it) const{
                        BOOST_UBLAS_CHECK(&(*this)()==&it(), external_logic());
                        return it_ < it.it_;
                    }
                protected:

                private:
                    iterator_type it_;
                    friend class const_iterator;
            };
            #endif // BOOST_UBLAS_USE_INDEXED_ITERATOR

            BOOST_UBLAS_INLINE
            Array1d():vector_expression<self_type>(){}

            explicit BOOST_UBLAS_INLINE
            Array1d(size_type array_size):vector_expression<self_type>(){}

            explicit BOOST_UBLAS_INLINE
            Array1d(size_type array_size, value_type v):vector_expression<self_type>(){
                QUEST_DEBUG_ERROR_IF(array_size>N)<<"数组越界"<<std::endl;
                std::fill(data().begin(),data().begin()+array_size,v);
            }

            explicit BOOST_UBLAS_INLINE
            Array1d(const std::initializer_list<value_type>& rInitList):vector_expression<self_type>(){
                QUEST_DEBUG_ERROR_IF(array_size>N)<<"数组越界"<<std::endl;
                std::copy(rInitList.begin(),rInitList.end(),data().begin());
            }

            BOOST_UBLAS_INLINE
            Array1d(size_type array_size,const array_type& rdata):vector_expression<self_type>(),data_(rdata){}

            BOOST_UBLAS_INLINE
            Array1d(const Array1d& v):vector_expression<self_type>(),data_(v.data_){}

            template<typename AE>
            BOOST_UBLAS_INLINE
            Array1d(const boost::numeric::ublas::vector_expression<AE>& ae){
                boost::numeric::ublas::vector_assign<boost::numeric::ublas::scalar_assign>(*this, ae);
            }

            BOOST_UBLAS_INLINE
            const_reference operator()(size_type i)const{
                QUEST_DEBUG_ERROR_IF(i>=N)<<"数组越界(i)"<<std::endl;
                return data_[i];
            }

            BOOST_UBLAS_INLINE
            reference operator()(size_type i){
                QUEST_DEBUG_ERROR_IF(i>=N)<<"数组越界(i)"<<std::endl;
                return data_[i];
            }

            BOOST_UBLAS_INLINE
            const_reference operator[](size_type i)const{
                QUEST_DEBUG_ERROR_IF(i>=N)<<"数组越界(i)"<<std::endl;
                return data_[i];
            }

            BOOST_UBLAS_INLINE
            reference operator[](size_type i){
                QUEST_DEBUG_ERROR_IF(i>=N)<<"数组越界(i)"<<std::endl;
                return data_[i];
            }

            BOOST_UBLAS_INLINE
            Array1d& operator=(const Array1d& v){
                data_ = v.data_;
                return *this;
            }

            template<typename AE>
            BOOST_UBLAS_INLINE
            Array1d& operator=(const boost::numeric::ublas::vector_expression<AE>& ae){
                return assign(self_type(ae));
            }

            template<typename AE>
            BOOST_UBLAS_INLINE
            Array1d& operator +=(const boost::numeric::ublas::vector_expression<AE>& ae){
                return assign(self_type(*this + ae));
            }

            template<typename AE>
            BOOST_UBLAS_INLINE
            Array1d& operator -=(const boost::numeric::ublas::vector_expression<AE>& ae){
                return assign(self_type(*this - ae));
            }

            template<typename AT>
            BOOST_UBLAS_INLINE
            Array1d& operator /=(const AT& at){
                vector_assign_scalar<scalar_divides_assign>(*this, at);
                return *this;
            }

            BOOST_UBLAS_INLINE
            bool operator ==(const Array1d& v)const{
                return std::equal(data_.begin(),data_.end(),v.data_.begin());
            }

            BOOST_UBLAS_INLINE
            void resize(size_type array_size,bool preserve=true){
                if(!preserve)
                    std::fill(data_.begin(),data_.end(),value_type());
            }

            BOOST_UBLAS_INLINE
            Array1d& assign_tempoary(Array1d& v){
                swap(v);
                return *this;
            }

            template<typename AT>
            BOOST_UBLAS_INLINE
            Array1d& operator *=(const AT& at){
                vector_assign_scalar<scalar_multiplies_assign>(*this, at);
                return *this;
            }

            template<typename AE>
            BOOST_UBLAS_INLINE
            Array1d& plus_assign(const boost::numeric::ublas::vector_expression<AE>& ae){
                vector_assign<scalar_plus_assign>(*this, ae);
                return *this;
            }

            template<typename AE>
            BOOST_UBLAS_INLINE
            Array1d& minus_assign(const boost::numeric::ublas::vector_expression<AE>& ae){
                vector_assign<scalar_minus_assign>(*this, ae);
                return *this;
            }

            template<typename AE>
            BOOST_UBLAS_INLINE
            Array1d& assign(const boost::numeric::ublas::vector_expression<AE>& ae){
                vector_assign<scalar_assign>(*this, ae);
                return *this;
            }

            BOOST_UBLAS_INLINE
            void swap(Array1d& v){
                if (this != &v)
                    data().swap(v.data());
            }

            #ifndef BOOST_UBLAS_NO_MEMBER_FRIENDS
                BOOST_UBLAS_INLINE
                friend void swap(Array1d& v1, Array1d& v2){
                    v1.swap(v2);
                }
            #endif

            BOOST_UBLAS_INLINE
            reference insert_element(size_type i, const_reference t){
                BOOST_UBLAS_CHECK(i<N, bad_index());
                return (data_[i] = t)
            }

            BOOST_UBLAS_INLINE
            void erase_element(size_type i){
                BOOST_UBLAS_CHECK(i<N, bad_index());
                data_[i] = value_type();
            }

            BOOST_UBLAS_INLINE
            void clear(){
                std::fill(data_.begin(),data_.end(),value_type(0));
            }

            BOOST_UBLAS_INLINE
            const_iterator find(size_type i)const{
                #ifndef BOOST_UBLAS_USE_INDEXED_ITERATOR
                    return const_iterator(*this,data().begin()+i);
                #else
                    return const_iterator(*this,i);
                #endif
            }

            BOOST_UBLAS_INLINE
            iterator find(size_type i){
                #ifndef BOOST_UBLAS_USE_INDEXED_ITERATOR
                    return iterator(*this,data().begin()+i);
                #else
                    return iterator(*this,i);
                #endif
            }

            BOOST_UBLAS_INLINE
            size_type size()const{
                return N;
            }

            BOOST_UBLAS_INLINE
            const array_type& data()const{
                return data_;
            }

            BOOST_UBLAS_INLINE
            array_type& data(){
                return data_;
            }

            BOOST_UBLAS_INLINE
            const_iterator begin()const{
                return find(0);
            }

            BOOST_UBLAS_INLINE
            const_iterator end()const{
                return find(data_.size());
            }

            BOOST_UBLAS_INLINE
            iterator begin(){
                return find(0);
            }

            BOOST_UBLAS_INLINE
            iterator end(){
                return find(data_.size());
            }

            BOOST_UBLAS_INLINE
            const_reverse_iterator rbegin()const{
                return const_reverse_iterator(end());
            }

            BOOST_UBLAS_INLINE
            const_reverse_iterator rend()const{
                return const_reverse_iterator(begin());
            }

            BOOST_UBLAS_INLINE
            reverse_iterator rbegin(){
                return reverse_iterator(end());
            }

            BOOST_UBLAS_INLINE
            reverse_iterator rend(){
                return reverse_iterator(begin());
            }

        protected:

        
        private:
            typedef typename array_type::const_iterator const_iterator_type;
            typedef typename array_type::iterator iterator_type;
            array_type data_;
    }
}

namespace AuxiliaryHashCombine{
    template<typename TClassType>
    inline void HashCombine(std::size_t& seed,const TClassType& value){
        std::hash<TClassType> hasher;
        seed ^= hasher(value) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    }
}

namespace std{
    template<typename T,std::size_t N>
    struct hash<Quest::Array1d<T,N>>{
        std::size_t operator()(const Quest::Array1d<T,N>& array){
            std::size_t seed = 0;
            for (auto component : array){AuxiliaryHashCombine::HashCombine(seed,component);}
            return seed;
        }
    };
}


#endif // ARRAY_1D_HPP