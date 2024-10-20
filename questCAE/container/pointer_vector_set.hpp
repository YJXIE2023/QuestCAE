/*------------------------------------------------
提供一个类似于 STL 集合的有序关联容器
使用向量（std::vector）来存储指向其数据的指针
------------------------------------------------*/

#ifndef QUEST_POINTER_VECTOR_SET_HPP
#define QUEST_POINTER_VECTOR_SET_HPP

// 系统头文件
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <cstddef>
#include <utility>

// 第三方头文件
#include <boost/iterator/indirect_iterator.hpp>

// 项目头文件
#include "includes/define.hpp"
#include "includes/serializer.hpp"
#include "container/set_identity_function.hpp"

namespace Quest{

    template<typename TDataType,
            typename TGetKeyType = SetIdentityFunction<TDataType>,
            typename TCompareType = std::less<decltype(std::declval<TGetKeyType>()(std::declval<TDataType>()))>,
            typename TEqualType = std::equal_to<decltype(std::declval<TGetKeyType>()(std::declval<TDataType>()))>,
            typename TPointerType = typename TDataType::Pointer,
            typename TContainerType = std::vector<TPointerType>>
    class PointerVectorSet final{
        public:
            QUEST_CLASS_POINTER_DEFINITION(PointerVectorSet);

            using key_type = typename std::remove_reference<decltype(std::declval<TGetKeyType>()(std::declval<TDataType>()))>::type;
            using data_type = TDataType;
            using value_type = TDataType;
            using key_compare = TCompareType;
            using pointer = TPointerType;
            using reference = TDataType&;
            using const_reference = const TDataType&;
            using ContainerType = TContainerType;

            using iterator = boost::indirect_iterator<typename TContainerType::iterator>;
            using const_iterator = boost::indirect_iterator<typename TContainerType::const_iterator>;
            using reverse_iterator = boost::indirect_iterator<typename TContainerType::reverse_iterator>;
            using const_reverse_iterator = boost::indirect_iterator<typename TContainerType::const_reverse_iterator>;

            using size_type = typename TContainerType::size_type;
            using ptr_iterator = typename TContainerType::iterator;
            using ptr_const_iterator = typename TContainerType::const_iterator;
            using ptr_reverse_iterator = typename TContainerType::reverse_iterator;
            using ptr_const_reverse_iterator = typename TContainerType::const_reverse_iterator;
            using difference_type = typename TContainerType::difference_type;


            PointerVectorSet():mData(),mSortedPartSize(size_type()),mMaxBufferSize(1) {}


            template<typename TInputIteratorType>
            PointerVectorSet(TInputIteratorType first, TInputIteratorType last, size_type NewMaxBufferSize = 1):
                mSortedPartSize(size_type()),
                mMaxBufferSize(NewMaxBufferSize)
            {
                for(; first!= last; ++first){
                    insert(begin(), *first);
                }
            }


            PointerVectorSet(const PointerVectorSet& rOther)
                :mData(rOther.mData),
                mSortedPartSize(rOther.mSortedPartSize),
                mMaxBufferSize(rOther.mMaxBufferSize)
            {}


            explicit PointerVectorSet(const TContainerType& rContainer):
                mData(rContainer),
                mSortedPartSize(size_type()),
                mMaxBufferSize(1)
            {
                Sort();
                std::unique(mData.begin(), mData.end(), EqualKeyTo());
            }


            ~PointerVectorSet() {}


            PointerVectorSet& operator = (const PointerVectorSet& rOther){
                mData = rOther.mData;
                mSortedPartSize = rOther.mSortedPartSize;

                return *this;
            }


            TDataType& operator[](const key_type& rKey){
                ptr_iterator sort_part_end;

                if(mData.size() - mSortedPartSize >= mMaxBufferSize){
                    Sort();
                    sorted_part_end = mData.end();
                } else {
                    sorted_part_end = mData.begin() + mSortedPartSize;
                }

                ptr_iterator i(std::lower_bound(mData.begin(), sorted_part_end, rKey, CompareKey()));
                if(i == sort_part_end){
                    mSortedPartSize++;
                    return **mData.insert(sort_part_end, TPointerType(new TDataType(rKey)));
                }

                if(!EqualKeyTo(rKey)(*i)){
                    if ((i = std::find_if(sort_part_end, mData.end(), EqualKeyTo(rKey))) == mData.end()){
                        mData.push_back(TPointerType(new TDataType(rKey)));
                        return **(mData.end() - 1);
                    }
                }

                return **i;
            }


            pointer& operator()(const key_type& rKey){
                ptr_iterator sort_part_end;

                if(mData.size() - mSortedPartSize >= mMaxBufferSize){
                    Sort();
                    sorted_part_end = mData.end();
                } else {
                    sorted_part_end = mData.begin() + mSortedPartSize;
                }

                ptr_iterator i(std::lower_bound(mData.begin(), sorted_part_end, rKey, CompareKey()));
                if(i == sort_part_end){
                    mSortedPartSize++;
                    return *mData.insert(sort_part_end, TPointerType(new TDataType(rKey)));
                }

                if(!EqualKeyTo(rKey)(*i)){
                    if ((i = std::find_if(sort_part_end, mData.end(), EqualKeyTo(rKey))) == mData.end()){
                        mData.push_back(TPointerType(new TDataType(rKey)));
                        return *(mData.end() - 1);
                    }
                }

                return *i;
            }


            bool operator == (const PointerVectorSet& rOther) const noexcept{
                assert(!empty());
                if(size() != rOther.size()){
                    return false;
                } else {
                    return std::equal(mData.begin(), mData.end(), rOther.mData.begin(), rOther.mData.end(), EqualKeyTo());
                }
            }


            bool operator < (const PointerVectorSet& rOther) const noexcept{
                assert(!empty());
                return std::lexicographical_compare(mData.begin(), mData.end(), rOther.mData.begin(), rOther.mData.end(), CompareKey());
            }


            iterator begin(){
                return iterator(mData.begin());
            }


            const_iterator begin() const{
                return const_iterator(mData.begin());
            }


            const_iterator cbegin(){
                return const_iterator(mData.begin());
            }


            const_iterator cbegin() const{
                return const_iterator(mData.begin());
            }


            iterator end(){
                return iterator(mData.end());
            }

            
            const_iterator end() const{
                return const_iterator(mData.end());
            }


            const_iterator cend(){
                return const_iterator(mData.end());
            }


            const_iterator cend() const{
                return const_iterator(mData.end());
            }


            reverse_iterator rbegin(){
                return reverse_iterator(mData.rbegin());
            }


            const_reverse_iterator rbegin() const{
                return const_reverse_iterator(mData.rbegin());
            }


            reverse_iterator rend(){
                return reverse_iterator(mData.rend());
            }

            
            const_reverse_iterator rend() const{
                return const_reverse_iterator(mData.rend());
            }


            ptr_iterator ptr_begin(){   
                return mData.begin();
            }


            ptr_const_iterator ptr_begin() const{
                return mData.begin();
            }


            ptr_iterator ptr_end(){
                return mData.end();
            }


            ptr_const_iterator ptr_end() const{
                return mData.end();
            }


            ptr_reverse_iterator ptr_rbegin(){
                return mData.rbegin();
            }


            ptr_const_reverse_iterator ptr_rbegin() const{
                return mData.rbegin();
            }


            ptr_reverse_iterator ptr_rend(){
                return mData.rend();
            }


            ptr_const_reverse_iterator ptr_rend() const{
                return mData.rend();
            }


            reference front() noexcept{
                assert(!empty());
                return *(mData.front());
            }


            const_reference front() const noexcept{
                assert(!empty());
                return *(mData.front());
            }


            reference back() noexcept{
                assert(!empty());
                return *(mData.back());
            }


            const_reference back() const noexcept{
                assert(!empty());
                return *(mData.back());
            }


            size_type size() const{
                return mData.size();
            }


            size_type max_size() const{
                return mData.max_size();
            }


            key_compare key_comp() const{
                return TCompareType();
            }


            void swap(PointerVectorSet& rOther){
                std::swap(mSortedPartSize, rOther.mSortedPartSize);
                std::swap(mMaxBufferSize, rOther.mMaxBufferSize);
                mData.swap(rOther.mData);
            }


            void push_back(TPointerType x){
                mData.push_back(x);
            }


            void pop_back(){
                mData.pop_back();
                if(mSortedPartSize > mData.size()){
                    mSortedPartSize = mData.size();
                }
            }


            iterator insert(iterator position, const TDataType& pData){
                ptr_iterator sort_part_end;

                key_type key = KeyOf(*pData);
                if(mData.size() - mSortedPartSize >= mMaxBufferSize){
                    Sort();
                    sorted_part_end = mData.end();
                } else {
                    sorted_part_end = mData.begin() + mSortedPartSize;
                }

                ptr_iterator i(std::lower_bound(mData.begin(), sorted_part_end, key, CompareKey()));
                if(i == sort_part_end){
                    mSortedPartSize++;
                    return mData.insert(sort_part_end, pData);
                }

                if(!EqualKeyTo(key)(*i)){
                    if ((i = std::find_if(sort_part_end, mData.end(), EqualKeyTo(key))) == mData.end()){
                        mData.push_back(pData);
                        return iterator(mData.end() - 1);
                    }
                }

                *i = pData;
                return i;
            }


            template<typename TInputIterator>
            void insert(InputIterator first, InputIterator last){
                for(; first!= last; ++first){   
                    insert(begin(), *first);
                }
            }


            iterator erase(iterator position){
                if(position.base() == mData.end()){
                    return mData.end();
                }

                iterator new_end = iterator(mData.erase(position.base()));
                mSortedPartSize = mData.size();
                return new_end;
            }


            iterator erase(iterator first, iterator last){
                iterator new_end = iterator(mData.erase(first.base(), last.base()));
                mSortedPartSize = mData.size();
                return new_end;
            }


            iterator erase(const key_type& rKey){
                return erase(find(rKey));
            }


            void clear(){
                mData.clear();
                mSortedPartSize = size_type();  
                mMaxBufferSize = 1;
            }


            iterator find(const key_type& rKey){
                ptr_iterator sort_part_end;

                if(mData.size() - mSortedPartSize >= mMaxBufferSize){
                    Sort();
                    sorted_part_end = mData.end();
                } else {    
                    sorted_part_end = mData.begin() + mSortedPartSize;
                }

                ptr_iterator i(std::lower_bound(mData.begin(), sorted_part_end, rKey, CompareKey()));
                if(i == sort_part_end || (!EqualKeyTo(rKey)(*i))){
                    if ((i = std::find_if(sort_part_end, mData.end(), EqualKeyTo(rKey))) == mData.end()){
                        return end();
                    }
                }

                return i;
            }


            const_iterator find(const key_type& rKey) const{
                ptr_const_iterator sorted_part_end(mData.begin() + mSortedPartSize);

                ptr_const_iterator i(std::lower_bound(mData.begin(), sorted_part_end, rKey, CompareKey()));
                if(i == sorted_part_end || (!EqualKeyTo(rKey)(*i))){
                    if ((i = std::find_if(sorted_part_end, mData.end(), EqualKeyTo(rKey))) == mData.end()){
                        return mData.end();
                    }
                }

                return const_iterator(i);
            }


            size_type count(const key_type& rKey){
                return find(rKey) == mData.end() ? 0 : 1;
            }


            void reserve(int reservedsize){
                mData.reserve(reservedsize);
            }


            int capacity(){
                return mData.capacity();
            }


            void Sort(){
                std::sort(mData.beign(), mData.end(), CompareKey());
                mSortedPartSize = mData.size();
            }


            void Unique(){
                typename TContainerType::iterator end_it = mData.end();
                std::sort(mData.begin(), end_it, CompareKey());
                typename TContainerType::iterator new_end_it = std::unique(mData.begin(), end_it, EqualKeyTo());
                mData.erase(new_end_it, end_it);
                mSortedPartSize = mData.size();
            }


            TContainerType& GetContainer(){
                return mData;
            }


            const TContainerType& GetContainer() const{
                return mData;
            }


            size_type GetMaxBufferSize() const{
                return mMaxBufferSize;
            }


            void SetMaxBufferSize(const size_type NewSize){
                mMaxBufferSize = NewSize;
            }


            size_type GetSortedPartSize() const{
                return mSortedPartSize;
            }


            void SetSortedPartSize(const size_type NewSize){
                mSortedPartSize = NewSize;
            }


            bool empty() const{
                return mData.empty();
            }


            bool IsSorted() const{
                return mSortedPartSize == mData.size();
            }


            std::string Info() const{
                std::stringstream buffer;
                buffer << "Pointer vector set (size = " << size() << ") : ";
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
            class CompareKey{
                public:
                    bool operator()(key_type a, key_type b) const{
                        return TCompareType()(a,TGetKeyType()(*b));
                    }

                    bool operator()(TPointerType a, key_type b) const{
                        return TCompareType()(TGetKeyType()(*a),b);
                    }

                    bool operator()(TPointerType a, TPointerType b) const{
                        return TCompareType()(TGetKeyType()(*a),TGetKeyType()(*b));
                    }

                protected:

                private:

            };

            class EqualKeyTo{
                public:
                    EqualKeyTo():mKey(){}

                    explicit EqualKeyTo(key_type key):mKey(key){}

                    bool operator()(TPointerType a) const{
                        return TEqualType()(mKey,TGetKeyType()(*a));
                    }

                    bool operator()(TPointerType a, TPointerType b) const{
                        return TEqualType()(TGetKeyType()(*a),TGetKeyType()(*b));
                    }

                protected:

                private:
                    key_type mKey;

            };

        private:
            key_type KeyOf(iterator i){
                return TGetKeyType()(*i);
            }

            key_type KeyOf(ptr_iterator i){
                return TGetKeyType()(**i);
            }

            key_type KeyOf(const TDataType& i){
                return TGetKeyType()(i);
            }

            friend class Serializer;

            virtual void save(Serializer& rSerializer) const{
                size_type local_size = mData.size();

                rSerializer.save("size", local_size);
                for(size_type i = 0; i < local_size; ++i){
                    rSerializer.save("E", mData[i])
                }

                rSerializer.save("Sorted Part Size", mSortedPartSize);
                rSerializer.save("Max Buffer Size", mMaxBufferSize);
            }

            virtual void load(Serializer& rSerializer){
                size_type local_size;
                rSerializer.load("size", local_size);

                mData.resize(local_size);
                for(size_type i = 0; i < local_size; ++i){
                    rSerializer.load("E", mData[i])
                }

                rSerializer.load("Sorted Part Size", mSortedPartSize);
                rSerializer.load("Max Buffer Size", mMaxBufferSize);
            }

        private:
            TContainerType mData;
            size_type mSortedPartSize;
            size_type mMaxBufferSize;

    };

    template<typename TDataType,
            typename TGetKeyType,
            typename TCompareType,
            typename TEqualType,
            typename TPointerType,
            typename TContainerType>
    inline std::istream& operator >> (std::istream& rIstream, PointerVectorSet<TDataType, TGetKeyType, TCompareType, TEqualType, TPointerType, TContainerType>& rThis);

    template<typename TDataType,
            typename TGetKeyType,
            typename TCompareType,
            typename TEqualType,
            typename TPointerType,
            typename TContainerType>
    inline std::ostream& operator << (std::ostream& rOstream, const PointerVectorSet<TDataType, TGetKeyType, TCompareType, TEqualType, TPointerType, TContainerType>& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }

} // namespace Quest


#endif //QUEST_POINTER_VECTOR_SET_HPP