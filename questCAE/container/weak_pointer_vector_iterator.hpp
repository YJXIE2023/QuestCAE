/*---------------------------------
弱指针向量迭代器
----------------------------------*/

#ifndef WEAK_POINTER_VECTOR_ITERATOR_HPP
#define WEAK_POINTER_VECTOR_ITERATOR_HPP

// 系统头文件
#include <string>
#include <iostream>

// 外部库头文件
#include <boost/iterator/iterator_adaptor.hpp>

// 项目头文件
#include "includes/define.hpp"

namespace Quest{
    template<typename TIteratorType, typename TDataType>
    class WeakPointerVcetorItrator: public boost::iterator_adaptor<WeakPointerVectorIterator<TIterator,TDataType>,TIteratorType,TDataType>{
        public:
            QUEST_CALSS_POINTER_DEFINITION(WeakPointerVectorIterator);

            typedef boost::iterator_adaptor<WeakPointerVcetorItrator,TIteratorType,TDataType> BaseType;

            WeakPointerVcetorItrator() {}

            WeakPointerVcetorItrator(TIteratorType NewIterator):BaseType(NewIterator) {}

            WeakPointerVcetorItrator(const WeakPointerVcetorItrator& NewIterator):BaseType(NewIterator.base()) {}

        protected:

        private:
            friend class boost::iterator_core_access;

            typename BaseType::reference dereference() const{
                return *((this->base())->lock());
            }
    };


} // namespace Quest

#endif // WEAK_POINTER_VECTOR_ITERATOR_HPP