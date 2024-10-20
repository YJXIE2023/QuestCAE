/*---------------------------------------
pointer_vector_map类的迭代器类
----------------------------------------*/

#ifndef QUEST_POINTER_VECTOR_MAP_ITERATOR_HPP
#define QUEST_POINTER_VECTOR_MAP_ITERATOR_HPP

// 系统头文件
#include <string>
#include <iostream>

// 第三方头文件
#include <boost/iterator/iterator_adaptor.hpp>

// 项目头文件
#include "includes/define.hpp"


namespace Quest{

    template<typename TIterator, typename TDataType>
    class PointerVectorMapIterator: public boost::iterator_adaptor<PointerVectorMapIterator<TIteratorType, TDataType>,TIteratorType, TDataType>
    {
        public:
            QUEST_CLASS_POINTER_DEFINITION(PointerVectorMapIterator);

            using BaseType = boost::iterator_adaptor<PointerVectorMapIterator, TIteratorType, TDataType>;
            using key_type = typename TIteratorType::value_type::first_type;

            PointerVectorMapIterator() {}

            PointerVectorMapIterator(TIteratorType NewIterator): BaseType(NewIterator) {}

            PointerVectorMapIterator(const PointerVectorMapIterator& Other): BaseType(Other.base()) {}

            key_type key(){
                return this->base()->first;
            }

        protected:

        private:
            friend class boost::iterator_core_access;

            typename BaseType::reference dereference() const{
                #if BOOST_WORKAROUND(__BORLANDC__, BOOST_TESTED_AT(0x551))
                    return const_cast<BaseType::reference>(*this->base()->second);
                #else 
                    return *(this->base()->second);
                #endif
            }

    };

} // namespace Quest


#endif //QUEST_POINTER_VECTOR_MAP_ITERATOR_HPP