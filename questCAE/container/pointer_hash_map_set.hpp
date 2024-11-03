/*-------------------------------------------
基于哈希表实现的集合容器，专门用于存储指针类型的对象
-------------------------------------------*/

#ifndef QUEST_POINTER_HASH_MAP_SET_HPP
#define QUEST_POINTER_HASH_MAP_SET_HPP

// 系统头文件
#include <utility>
#include <type_traits>
#include <unordered_map>

// 项目头文件
#include "includes/define.hpp"
#include "includes/serializer.hpp"
#include "container/set_identity_function.hpp"

namespace Quest{

    template<typename TDataType,
             typename THashType = std::hash<TDataType>,
             typename TGetKeyType = SetIdentityFunction<TDataType>,
             typename TPointerType = Quest::shared_ptr<TDataType>>
    class PointerHashMapSet final{
        public:
            QUEST_CLASS_POINTER_DEFINITION(PointerHashMapSet);

            using key_type = decltype(std::declval<TGetKeyType>()(std::declval<TDataType>()));

            using data_type = TDataType;
            using value_type = TDataType;
            using pointer_type = TPointerType;
            using reference = TDataType&;
            using const_reference = const TDataType&;

            using ContainerType = std::unordered_map<typename std::remove_reference<key_type>::type, TPointerType, THashType>;
            using size_type = typename ContainerType::size_type;
            using ptr_iterator = typename ContainerType::iterator;
            using prt_const_iterator = typename ContainerType::const_iterator;
            using difference_type = typename ContainerType::difference_type;
        
        private:
            class iterator_adaptor{
                public:
                    using iterator_category = std::forward_iterator_tag;
                    using difference_type = std::ptrdiff_t;
                    using value_type = data_type;
                    using pointer = data_type*;
                    using reference = data_type&;

                    iterator_adaptor(ptr_iterator it):mpa_iterator(it){}
                    iterator_adaptor(const iterator_adaptor& other):mpa_iterator(other.mpa_iterator){}
                    iterator_adaptor& operator++(){++mpa_iterator; return *this;}
                    iterator_adaptor operator++(int){iterator_adaptor tmp(*this); operator++(); return tmp;}
                    bool operator==(const iterator_adaptor& other) const{return mpa_iterator == other.mpa_iterator;}
                    bool operator!=(const iterator_adaptor& other) const{return mpa_iterator!= other.mpa_iterator;}
                    data_type& operator*() const{return *(mpa_iterator->second);}
                    pointer_type operator->() const{return map_iterator->second;}
                    ptr_iterator& base(){return mpa_iterator;}
                    const ptr_iterator& base() const{return mpa_iterator;}

                private:
                    ptr_iterator map_iterator;
            };


            class const_iterator_adaptor{
                public:
                    using iterator_category = std::forward_iterator_tag;
                    using difference_type = std::ptrdiff_t;
                    using value_type = data_type;
                    using pointer = data_type*;
                    using reference = data_type&;

                    const_iterator_adaptor(prt_const_iterator it):mpa_iterator(it){}
                    const_iterator_adaptor(const const_iterator_adaptor& other):mpa_iterator(other.mpa_iterator){}
                    const_iterator_adaptor& operator++(){++mpa_iterator; return *this;} 
                    const_iterator_adaptor operator++(int){const_iterator_adaptor tmp(*this); operator++(); return tmp;}
                    bool operator==(const const_iterator_adaptor& other) const{return mpa_iterator == other.mpa_iterator;}
                    bool operator!=(const const_iterator_adaptor& other) const{return mpa_iterator != other.mpa_iterator;}
                    const data_type& operator*() const{return *(mpa_iterator->second);}
                    pointer_type operator->() const{return map_iterator->second;}
                    prt_const_iterator& base(){return mpa_iterator;}
                    const prt_const_iterator& base() const{return mpa_iterator;}

                private:
                    prt_const_iterator map_iterator;
            };

        public:
            using iterator = iterator_adaptor;
            using const_iterator = const_iterator_adaptor;

        public:
            PointerHashMapSet():mData(){}


            template<typename TInputIteratorType>
            PointerHashMapSet(TInputIteratorType first, TInputIteratorType last, size_type NewMaxBufferSize = 1){
                for(; first != last; ++first){
                    insert(beign(), *first);
                }
            }


            PointerHashMapSet(const PointerHashMapSet& rOther):mData(rOther.mData){}


            PointerHashMapSet(const ContainerType& rContainer):mData(rContainer){}


            ~PointerHashMapSet(){}


            PointerHashMapSet& operator=(const PointerHashMapSet& rOther){
                mData = rOther.mData;
                return *this;
            }


            TDataType& operator[](const key_type& key){
                QUEST_DEBUG_ERROR_IF(mData.find(key) == mData.end()) << "The key: " << Key << " is not available in the map" << std::endl;
                return *(mData.find(key)->second);
            }


            pointer_type& operator()(const key_type& key){
                QUEST_DEBUG_ERROR_IF(mData.find(key) == mData.end()) << "The key: " << Key << " is not available in the map" << std::endl;
                return mData.find(key)->second;
            }


            bool operator==(const PointerHashMapSet& rOther) const{
                return mData == rOther.mData;
            }


            bool operator!=(const PointerHashMapSet& rOther) const{
                return mData != rOther.mData;
            }


            iterator begin(){
                return iterator(mData.begin());
            }


            const_iterator begin() const{
                return const_iterator(mData.begin());
            }


            iterator end(){
                return iterator(mData.end());
            }


            const_iterator end() const{
                return const_iterator(mData.end());
            }


            ptr_iterator ptr_begin(){
                return mData.begin();
            }


            prt_const_iterator ptr_begin() const{
                return mData.begin();
            }


            ptr_iterator ptr_end(){
                return mData.end();
            }


            prt_const_iterator ptr_end() const{
                return mData.end();
            }


            reference front(){
                return *(mData.front().second);
            }


            const_reference front() const{
                return *(mData.front().second);
            }


            reference back(){
                return *(mData.back().second);
            }


            const_reference back() const{
                return *(mData.back().second);
            }


            size_type size() const{
                return mData.size();
            }


            void swap(PointerHashMapSet& rOther){
                mData.swap(rOther.mData);
            }


            template<typename TOtherDataType>
            iterator insert(const TOtherDataType& rValue){
                TDataType* p_new_data = new TDataType(rValue);
                return mData.insert(ContainerType::value_type(TGetKeyOf(rValue), p_new_data));
            }


            iterator insert(TPointerType pData){
                key_type key = KeyOf(*pData);
                typename ContainerType::value_type item(key, pData);
                std::pair<typename ContainerType::iterator, bool> result = mData.insert(item);
                return result.first;
            }


            template<typename TInputIterator>
            void insert(TInputIterator first, TInputIterator last){
                for(; first != last; ++first){
                    insert(begin(), *first);
                }
            }


            iterator erase(iterator pos){
                return mData.erase(pos.base());
            }


            size_type erase(const key_type& key){
                return mData.erase(key);
            }


            iterator erase(iterator first, iterator last){
                return mData.erase(first.base(), last.base());
            }


            void clear(){
                mData.clear();
            }


            iterator find(const key_type& key){
                return mData.find(key);
            }


            const_iterator find(const key_type& key) const{
                return mData.find(key);
            }


            size_type count(const key_type& key){
                return find(key) == mData.end() ? 0 : 1;
            }


            void reserve(int reservedsize){
                mData.reserve(reservedsize);
            }


            int capacity(){
                return mData.capacity();
            }


            ContainerType& GetContainer(){
                return mData;
            }


            const ContainerType& GetContainer() const{
                return mData;
            }


            bool empty() const{
                return mData.empty();
            }


            std::string Info() const{
                std::stringstream buffer;
                buffer << "Pointer hash map set (size = " << size() << ") : ";
                return buffer.str();
            }


            void PrintInfo(std::ostream& rOstream) const{
                rOstream << Info();
            }


            void PrintData(std::ostream& rOstream) const{
                std::copy(begin(), end(), std::ostream_iterator<TDataType>(rOstream, "\n"));
            }

        protected:

        private:
            key_type KeyOf(iterator i){
                return TGetKeyType()(*i);
            }


            key_type KeyOf(const_iterator i){
                return TGetKeyType()(**i);
            }


            key_type KeyOf(const data_type& value){
                return TGetKeyType()(value);
            }


            friend class Serializer;


            virtual void save(Serializer& rSerializer) const{
                size_type size = mData.size();
                rSerializer.save("size", size);

                for(ptr_const_iterator i = ptr_begin(); i!= ptr_end(); ++i){
                    rSerializer.save("E", i->second);
                }
            }


            virtual void load(Serializer& rSerializer){
                size_type size;
                rSerializer.load("size", size);

                for(size_type i = 0; i < size; ++i){
                    pointer_type ptr = nullptr;
                    rSerializer.load("E", ptr);
                    insert(ptr);
                }
            }

        private:
            ContainerType mData;
        
    };

    template<typename TDataType, typename TGetKeyType, typename TCompareType, typename TEqualType,typename TPointerType, typename ContainerType>
    inline std::ostream& operator << (std::ostream& rOstream, const PointerHashMapSet<TDataType, TGetKeyType, TCompareType, TPointerType>& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }   

} // namespace Quest

#endif //QUEST_POINTER_HASH_MAP_SET_HPP