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

    /**
     * @brief 一个类似于STL几何有序关联容器，但使用向量存储数据的指针(属性模块)
     * @details 其行为类似于STL中的set，但是采用vector来存储指向其数据元素的指针
     * @tparam TDataType 存储的数据类型
     * @tparam TGetKeyType 用于获取数据的键类型，默认为SetIdentityFunction，即直接使用数据本身作为键
     * @tparam TCompareType 用于比较键的比较类型，默认为std::less<decltype(std::declval<TGetKeyType>()(std::declval<TDataType>()))>
     * @tparam TEqualType 用于判断两个键是否相等的判断类型，默认为std::equal_to<decltype(std::declval<TGetKeyType>()(std::declval<TDataType>()))>
     * @tparam TPointerType 指针类型，默认为typename TDataType::Pointer
     * @tparam TContainerType 容器类型，默认为std::vector<TPointerType>
     */
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

            /**
             * @brief 默认构造函数
             */
            PointerVectorSet():mData(),mSortedPartSize(size_type()),mMaxBufferSize(1) {}

            /**
             * @brief 构造函数
             * @param first 输入迭代器，指向容器中第一个元素
             * @param last 输入迭代器，指向容器中最后一个元素
             */
            template<typename TInputIteratorType>
            PointerVectorSet(TInputIteratorType first, TInputIteratorType last, size_type NewMaxBufferSize = 1):
                mSortedPartSize(size_type()),
                mMaxBufferSize(NewMaxBufferSize)
            {
                for(; first!= last; ++first){
                    insert(begin(), *first);
                }
            }

            /**
             * @brief 复制构造函数
             */
            PointerVectorSet(const PointerVectorSet& rOther)
                :mData(rOther.mData),
                mSortedPartSize(rOther.mSortedPartSize),
                mMaxBufferSize(rOther.mMaxBufferSize)
            {}

            /**
             * @brief 构造函数
             * @param rContainer 实际存储数据的容器类型对象
             */
            explicit PointerVectorSet(const TContainerType& rContainer):
                mData(rContainer),
                mSortedPartSize(size_type()),
                mMaxBufferSize(1)
            {
                Sort();
                std::unique(mData.begin(), mData.end(), EqualKeyTo());
            }

            /**
             * @brief 析构函数
             */
            ~PointerVectorSet() {}

            /**
             * @brief 重载赋值运算符
             */
            PointerVectorSet& operator = (const PointerVectorSet& rOther){
                mData = rOther.mData;
                mSortedPartSize = rOther.mSortedPartSize;

                return *this;
            }

            /**
             * @brief 下标访问
             * @param rKey 键值
             */
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

            /**
             * @brief 函数调用运算符重载，获取对象
             */
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

            /**
             * @brief 重载比较运算符
             */
            bool operator == (const PointerVectorSet& rOther) const noexcept{
                assert(!empty());
                if(size() != rOther.size()){
                    return false;
                } else {
                    return std::equal(mData.begin(), mData.end(), rOther.mData.begin(), rOther.mData.end(), EqualKeyTo());
                }
            }

            /**
             * @brief 重载比较运算符
             */
            bool operator < (const PointerVectorSet& rOther) const noexcept{
                assert(!empty());
                return std::lexicographical_compare(mData.begin(), mData.end(), rOther.mData.begin(), rOther.mData.end(), CompareKey());
            }

            /**
             * @brief 返回指向首元素的迭代器
             */
            iterator begin(){
                return iterator(mData.begin());
            }

            /**
             * @brief 返回指向首元素的迭代器
             */
            const_iterator begin() const{
                return const_iterator(mData.begin());
            }

            /**
             * @brief 返回指向首元素的迭代器
             */
            const_iterator cbegin(){
                return const_iterator(mData.begin());
            }

            /**
             * @brief 返回指向首元素的迭代器
             */
            const_iterator cbegin() const{
                return const_iterator(mData.begin());
            }

            /**
             * @brief 返回指向尾元素的迭代器
             */
            iterator end(){
                return iterator(mData.end());
            }

            /**
             * @brief 返回指向尾元素的迭代器
             */
            const_iterator end() const{
                return const_iterator(mData.end());
            }

            /**
             * @brief 返回指向尾元素的迭代器
             */
            const_iterator cend(){
                return const_iterator(mData.end());
            }

            /**
             * @brief 返回指向尾元素的迭代器
             */
            const_iterator cend() const{
                return const_iterator(mData.end());
            }

            /**
             * @brief 返回反序首元素的迭代器
             */
            reverse_iterator rbegin(){
                return reverse_iterator(mData.rbegin());
            }

            /**
             * @brief 返回反序首元素的迭代器
             */
            const_reverse_iterator rbegin() const{
                return const_reverse_iterator(mData.rbegin());
            }

            /**
             * @brief 返回反序尾元素的迭代器
             */
            reverse_iterator rend(){
                return reverse_iterator(mData.rend());
            }

            /**
             * @brief 返回反序尾元素的迭代器
             */
            const_reverse_iterator rend() const{
                return const_reverse_iterator(mData.rend());
            }

            /**
             * @brief 返回实际数据容器的首元素的迭代器
             */
            ptr_iterator ptr_begin(){   
                return mData.begin();
            }

            /**
             * @brief 返回实际数据容器的首元素的迭代器
             */
            ptr_const_iterator ptr_begin() const{
                return mData.begin();
            }

            /**
             * @brief 返回实际数据容器的尾元素的迭代器
             */
            ptr_iterator ptr_end(){
                return mData.end();
            }

            /**
             * @brief 返回实际数据容器的尾元素的迭代器
             */
            ptr_const_iterator ptr_end() const{
                return mData.end();
            }

            /**
             * @brief 返回实际数据容器的反序首元素的迭代器
             */
            ptr_reverse_iterator ptr_rbegin(){
                return mData.rbegin();
            }

            /**
             * @brief 返回实际数据容器的反序首元素的迭代器
             */
            ptr_const_reverse_iterator ptr_rbegin() const{
                return mData.rbegin();
            }

            /**
             * @brief 返回实际数据容器的反序尾元素的迭代器
             */
            ptr_reverse_iterator ptr_rend(){
                return mData.rend();
            }

            /**
             * @brief 返回实际数据容器的反序尾元素的迭代器
             */
            ptr_const_reverse_iterator ptr_rend() const{
                return mData.rend();
            }

            /**
             * @brief 返回首元素的引用
             */
            reference front() noexcept{
                assert(!empty());
                return *(mData.front());
            }

            /**
             * @brief 返回首元素的引用
             */
            const_reference front() const noexcept{
                assert(!empty());
                return *(mData.front());
            }

            /**
             * @brief 返回尾元素的引用
             */
            reference back() noexcept{
                assert(!empty());
                return *(mData.back());
            }

            /**
             * @brief 返回尾元素的引用
             */
            const_reference back() const noexcept{
                assert(!empty());
                return *(mData.back());
            }

            /**
             * @brief 返回存储的元素的数量
             */
            size_type size() const{
                return mData.size();
            }

            /**
             * @brief 返回存储的元素的最大数量
             */
            size_type max_size() const{
                return mData.max_size();
            }

            /**
             * @brief 返回比较函数
             */
            key_compare key_comp() const{
                return TCompareType();
            }

            /**
             * @brief 和另一个容器交换元素
             */
            void swap(PointerVectorSet& rOther){
                std::swap(mSortedPartSize, rOther.mSortedPartSize);
                std::swap(mMaxBufferSize, rOther.mMaxBufferSize);
                mData.swap(rOther.mData);
            }

            /**
             * @brief 直接向尾部插入一个元素
             * @param x 元素值
             */
            void push_back(TPointerType x){
                mData.push_back(x);
            }

            /**
             * @brief 删除尾部元素
             */
            void pop_back(){
                mData.pop_back();
                if(mSortedPartSize > mData.size()){
                    mSortedPartSize = mData.size();
                }
            }

            /**
             * @brief 向指定位置插入一个元素
             */
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

            /**
             * @brief 向vector首部插入一系列对象
             */
            template<typename TInputIterator>
            void insert(TInputIterator first, TInputIterator last){
                for(; first!= last; ++first){   
                    insert(begin(), *first);
                }
            }

            /**
             * @brief 移除指定位置的元素
             */
            iterator erase(iterator position){
                if(position.base() == mData.end()){
                    return mData.end();
                }

                iterator new_end = iterator(mData.erase(position.base()));
                mSortedPartSize = mData.size();
                return new_end;
            }

            /**
             * @brief 移除指定范围内的元素
             */
            iterator erase(iterator first, iterator last){
                iterator new_end = iterator(mData.erase(first.base(), last.base()));
                mSortedPartSize = mData.size();
                return new_end;
            }

            /**
             * @brief 移除指定键值的元素(指定对象)
             */
            iterator erase(const key_type& rKey){
                return erase(find(rKey));
            }

            /**
             * @brief 清除所有元素
             */
            void clear(){
                mData.clear();
                mSortedPartSize = size_type();  
                mMaxBufferSize = 1;
            }

            /**
             * @brief 返回指定键值的元素(指定对象)
             */
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

            /**
             * @brief 返回指定键值的元素(指定对象)
             */
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

            /**
             * @brief 判断是否存在指定键值的元素
             */
            size_type count(const key_type& rKey){
                return find(rKey) == mData.end() ? 0 : 1;
            }

            /**
             * @brief 预先分配一定数量的内存空间
             */
            void reserve(int reservedsize){
                mData.reserve(reservedsize);
            }

            /**
             * @brief 返回当前分配的内存容量
             */
            int capacity(){
                return mData.capacity();
            }

            /**
             * @brief 排序
             */
            void Sort(){
                std::sort(mData.beign(), mData.end(), CompareKey());
                mSortedPartSize = mData.size();
            }

            /**
             * @brief 去重
             */
            void Unique(){
                typename TContainerType::iterator end_it = mData.end();
                std::sort(mData.begin(), end_it, CompareKey());
                typename TContainerType::iterator new_end_it = std::unique(mData.begin(), end_it, EqualKeyTo());
                mData.erase(new_end_it, end_it);
                mSortedPartSize = mData.size();
            }

            /**
             * @brief 获取实际数据容器
             */
            TContainerType& GetContainer(){
                return mData;
            }

            /**
             * @brief 获取实际数据容器
             */
            const TContainerType& GetContainer() const{
                return mData;
            }

            /**
             * @brief 获取最大缓冲区大小
             */
            size_type GetMaxBufferSize() const{
                return mMaxBufferSize;
            }

            /**
             * @brief 设置最大缓冲区大小
             */
            void SetMaxBufferSize(const size_type NewSize){
                mMaxBufferSize = NewSize;
            }

            /**
             * @brief 获取排序部分的大小
             */
            size_type GetSortedPartSize() const{
                return mSortedPartSize;
            }

            /**
             * @brief 设置排序部分的大小
             */
            void SetSortedPartSize(const size_type NewSize){
                mSortedPartSize = NewSize;
            }

            /**
             * @brief 判断是否为空
             */
            bool empty() const{
                return mData.empty();
            }

            /**
             * @brief 判断所有元素是否都排序
             */
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
            /**
             * @brief 实际数据存储容器
             * @details std::vector<TPointerType>
             */
            TContainerType mData;

            /**
             * @brief 排序部分的大小
             */
            size_type mSortedPartSize;

            /**
             * @brief 最大缓冲区大小（未排序元素的最大数量）
             */
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