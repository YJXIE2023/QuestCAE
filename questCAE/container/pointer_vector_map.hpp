#ifndef QUEST_POINTER_VECTOR_MAP_HPP
#define QUEST_POINTER_VECTOR_MAP_HPP

// 系统头文件
#include <vector>
#include <functional>
#include <string>
#include <iostream>
#include <sstream>
#include <cstddef>

// 项目头文件
#include "includes/define.hpp"
#include "container/pointer_vector_map_iterator.hpp"

namespace Quest{

    /**
     * @class PointerVectorMap
     * @brief 排序的关联容器，类似于 STL 的 map，但使用一个向量来存储指向其数据的指针
     */
    template<typename TKeyType, typename TDataType,
            typename TCompareType = std::less<TKeyType>,
            typename TPointerType = Quest::shared_ptr<TDataType>,
            typename TContainerType = std::vector<std::pair<TKeyType, TPointerType>>>
    class PointerVectorMap final{
        public:
            QUEST_CLASS_POINTER_DEFINITION(PointerVectorMap);

            using key_type = TKeyType;

            using data_type = TDataType;
            using value_type = std::pair<TKeyType, TPointerType>;
            using key_compare = TCompareType;
            using pointer = TPointerType;
            using reference = TPointerType&;
            using const_reference = const TPointerType&;
            using ContainerType = TContainerType;

            using iterator = PointerVectorMapIterator<typename TContainerType::iterator, TDataType>;
            using const_iterator = PointerVectorMapIterator<typename TContainerType::const_iterator, TDataType>;
            using reverse_iterator = PointerVectorMapIterator<typename TContainerType::reverse_iterator, TDataType>;
            using const_reverse_iterator = PointerVectorMapIterator<typename TContainerType::const_reverse_iterator, TDataType>;

            using size_type = typename TContainerType::size_type;
            using pair_iterator = typename TContainerType::iterator;
            using pair_const_iterator = typename TContainerType::const_iterator;
            using pair_reverse_iterator = typename TContainerType::reverse_iterator;
            using pair_const_reverse_iterator = typename TContainerType::const_reverse_iterator;

            /**
             * @brief 默认构造函数
             */
            PointerVectorMap(): mData(), mSortedPartSize(size_type()), mMaxBufferSize(100) {}

            /**
             * @brief 复制构造函数
             */
            PointerVectorMap(const PointerVectorMap& rOther): mData(rOther.mData), mSortedPartSize(rOther.mSortedPartSize), mMaxBufferSize(rOther.mMaxBufferSize) {}

            /**
             * @brief 构造函数
             * @brief rContainer 要拷贝的容器
             */
            explicit PointerVectorMap(const TContainerType& rContainer): mData(rContainer), mSortedPartSize(size_type()), mMaxBufferSize(100) {
                Sort();
                std::unique(mData.begin(), mData.end(), EqualKeyTo());
            }

            /**
             * @brief 析构函数
             */
            ~PointerVectorMap() {}

            /**
             * @brief 赋值运算符重载
             */
            PointerVectorMap& operator = (const PointerVectorMap& rOther){
                mData = rOther.mData;
                mSortedPartSize = rOther.mSortedPartSize;

                return *this;
            }

            /**
             * @brief 下标访问，返回键对应的数据引用
             */
            TDataType& operator [] (const key_type& rKey){
                pair_iterator sorted_part_end;

                if(mData.size() - mSortedPartSize >= mMaxBufferSize){
                    Sort();
                    sorted_part_end = mData.end();
                } else {
                    sorted_part_end = mData.begin() + mSortedPartSize;
                }

                pair_iterator i(std::lower_bound(mData.begin(), sorted_part_end, rKey, CompareKey()));
                if(i == sorted_part_end){
                    mSortedPartSize++;
                    return *(mData.insert(sorted_part_end, value_type(Key, TPointerType(new TDataType)))->second);
                }

                if(rKey != i->first){
                    if((i = std::find_if(sorted_part_end, mData.end(), EqualKeyTo(rKey))) == mData.end()){
                        mData.push_back(value_type(rKey, TPointerType(new TDataType)));
                        return *((--mData.end())->second);
                    }
                }

                return *(i->second);
            }

            /**
             * @brief 下标访问，返回键对应的数据指针
             */
            pointer operator [] (const key_type& Key){
                pair_iterator sorted_part_end;

                if(mData.size() - mSortedPartSize >= mMaxBufferSize){
                    Sort();
                    sorted_part_end = mData.end();
                } else {
                    sorted_part_end = mData.begin() + mSortedPartSize;
                }

                pair_iterator i(std::lower_bound(mData.begin(), sorted_part_end, Key, CompareKey()));
                if(i == sorted_part_end){
                    mSortedPartSize++;
                    return mData.insert(sorted_part_end, value_type(Key, TPointerType(new TDataType)))->second;
                }

                if(Key != i->first){
                    if((i = std::find_if(sorted_part_end, mData.end(), EqualKeyTo(Key))) == mData.end()){
                        mData.push_back(value_type(Key, TPointerType(new TDataType)));
                        return (--mData.end())->second;
                    }
                }

                return i->second;
            }

            /**
             * @brief 返回指向第一个pair的迭代器
             */
            iterator begin(){
                return iterator(mData.beign());
            }

            /**
             * @brief 返回指向第一个pair的迭代器
             */
            const_iterator begin() const{
                return const_iterator(mData.beign());
            }

            /**
             * @brief 返回指向最后一个pair的迭代器
             */
            iterator end(){
                return iterator(mData.end());
            }

            /**
             * @brief 返回指向最后一个pair的迭代器
             */
            const_iterator end() const{
                return const_iterator(mData.end());
            }

            /**
             * @brief 返回指向第一个pair的逆向迭代器
             */
            reverse_iterator rbegin(){
                return reverse_iterator(mData.rbegin());
            }

            /**
             * @brief 返回指向第一个pair的逆向迭代器
             */
            const_reverse_iterator rbegin() const{
                return const_reverse_iterator(mData.rbegin());
            }

            /**
             * @brief 返回指向最后一个pair的逆向迭代器
             */
            reverse_iterator rend(){
                return reverse_iterator(mData.rend());
            }

            /**
             * @brief 返回指向最后一个pair的逆向迭代器
             */
            const_reverse_iterator rend() const{
                return const_reverse_iterator(mData.rend());
            }

            /**
             * @brief 返回第一个pair对象的迭代器
             */
            pair_iterator pair_begin(){
                return mData.begin();
            }

            /**
             * @brief 返回第一个pair对象的迭代器
             */
            pair_const_iterator pair_begin() const{
                return mData.begin();
            }

            /**
             * @brief 返回最后一个pair对象的迭代器
             */
            pair_iterator pair_end(){
                return mData.end();
            }

            /**
             * @brief 返回最后一个pair对象的迭代器
             */
            pair_const_iterator pair_end() const{
                return mData.end();
            }

            /**
             * @brief 返回反序第一个pair对象的反向迭代器
             */
            pair_reverse_iterator pair_rbegin(){
                return mData.rbegin();
            }

            /**
             * @brief 返回反序第一个pair对象的反向迭代器
             */
            pair_const_reverse_iterator pair_rbegin() const{
                return mData.rbegin();
            }

            /**
             * @brief 返回反序最后一个pair对象的反向迭代器
             */
            pair_reverse_iterator pair_rend(){
                return mData.rend();
            }

            /**
             * @brief 返回反序最后一个pair对象的反向迭代器
             */
            pair_const_reverse_iterator pair_rend() const{
                return mData.rend();
            }

            /**
             * @brief 返回第一个pair对象的指针的值
             */
            reference front(){
                assert(!empty());
                return *(mData.front()->second);
            }   

            /**
             * @brief 返回第一个pair对象的指针的值
             */
            const_reference front() const{
                assert(!empty());
                return *(mData.front()->second);
            }   

            /**
             * @brief 返回最后一个pair对象的指针的值
             */
            reference back(){
                assert(!empty());
                return *(mData.back()->second);
            }   

            /**
             * @brief 返回最后一个pair对象的指针的值
             */
            const_reference back() const{
                assert(!empty());
                return *(mData.back()->second);
            }   

            /**
             * @brief 返回键值对数量
             */
            size_type size() const{
                return mData.size();
            }

            /**
             * @brief 返回最大容量
             */
            size_type max_size() const{
                return mData.max_size();
            }

            /**
             * @brief 返回键值比较器
             */
            key_compare key_comp() const{
                return TCompareType();
            }

            /**
             * @brief 与另一个容器交换内容
             */
            void swap(PointerVectorMap& rOther){
                mData.swap(rOther.mData);
            }

            /**
             * @brief 在尾部插入一个pair
             */
            void push_back(value_type x){
                mData.push_back(x);
            }

            /**
             * @brief 在尾部插入元素
             * @param Key 键
             * @param rData 数据
             */
            iterator insert(const key_type& Key, const TDataType& rData){
                pair_iterator sorted_part_end;

                if(mData.size() - mSortedPartSize >= mMaxBufferSize){
                    Sort();
                    sorted_part_end = mData.end();
                } else {
                    sorted_part_end = mData.begin() + mSortedPartSize;
                }

                pair_iterator i(std::lower_bound(mData.begin(), sorted_part_end, Key, CompareKey()));
                if(i == sorted_part_end){
                    mSortedPartSize++;
                    return (mData.insert(sorted_part_end, value_type(Key, TPointerType(new TDataType(rData)))));
                }

                if(Key != i->first){
                    if((i = std::find_if(sorted_part_end, mData.end(), EqualKeyTo(Key))) == mData.end()){
                        mData.push_back(value_type(Key, TPointerType(new TDataType)));
                        return iterator(--mData.end());
                    }
                }
                *(i->second) = rData;

                return i;
            }

            /**
             * @brief 在尾部插入元素
             * @param Key 键
             * @param pData 数据指针
             */
            iterator insert(const key_type& Key, const TPointerType pData){
                pair_iterator sorted_part_end;

                if(mData.size() - mSortedPartSize >= mMaxBufferSize){
                    Sort();
                    sorted_part_end = mData.end();
                } else {
                    sorted_part_end = mData.begin() + mSortedPartSize;
                }

                pair_iterator i(std::lower_bound(mData.begin(), sorted_part_end, Key, CompareKey()));
                if(i == sorted_part_end){
                    mSortedPartSize++;
                    return mData.insert(sorted_part_end, value_type(Key, pData));
                }

                if(Key != i->first){
                    if((i = std::find_if(sorted_part_end, mData.end(), EqualKeyTo(Key))) == mData.end()){
                        mData.push_back(value_type(Key, pData));
                        return iterator(--mData.end());
                    }
                }

                *(i->second) = *pData;
                return i;
            }

            /**
             * @brief 删除指定位置的pair
             */
            iterator erase(iterator pos){
                return iterator(mData.erase(pos.base()));
            }

            /**
             * @brief 删除指定范围的pair
             */
            iterator erase(iterator first, iterator last){
                return iterator(mData.erase(first.base(), last.base()));
            }

            /**
             * @brief 删除指定键对应的pair
             */
            iterator erese(const key_type& k){
                return erase(find(k));
            }
            
            /**
             * @brief 清空容器
             */
            void clear(){
                mData.clear();
            }

            /**
             * @brief 返回指向键对应的pair的迭代器
             */
            iterator find(const key_type& Key){
                pair_iterator sorted_part_end;

                if(mData.size() - mSortedPartSize >= mMaxBufferSize){
                    Sort();
                    sorted_part_end = mData.end();
                } else {
                    sorted_part_end = mData.begin() + mSortedPartSize;
                }

                pair_iterator i(std::lower_bound(mData.begin(), sorted_part_end, Key, CompareKey()));
                if(i == sorted_part_end || i->first != Key){
                    if((i = std::find_if(sorted_part_end, mData.end(), EqualKeyTo(Key))) == mData.end()){
                        return mData.end();
                    }
                }

                return i;
            }

            /**
             * @brief 返回指向键对应的pair的迭代器
             */
            const_iterator find(const key_type& Key) const{
                pair_const_iterator sorted_part_end(mData.begin() + mSortedPartSize);

                pair_const_iterator i(std::lower_bound(mData.begin(), sorted_part_end, Key, CompareKey()));
                if(i == sorted_part_end || i->first != Key){
                    if((i = std::find_if(sorted_part_end, mData.end(), EqualKeyTo(Key))) == mData.end()){
                        return mData.end();
                    }
                }

                return const_iterator(i);
            }

            /**
             * @brief 返回键值对应的pair的数量
             */
            size_type count(const key_type& Key){
                return find(Key) == mData.end() ? 0 : 1;
            }

            /**
             * @brief 返回键对应的值（非指针）
             */
            TDataType& at(const key_type& Key){
                pair_iterator sorted_part_end;

                if(mData.size() - mSortedPartSize >= mMaxBufferSize){
                    Sort();
                    sorted_part_end = mData.end();
                } else {
                    sorted_part_end = mData.begin() + mSortedPartSize;
                }

                pair_iterator i(std::lower_bound(mData.begin(), sorted_part_end, Key, CompareKey()));
                if(i == sorted_part_end){
                    if((i = std::find_if(sorted_part_end, mData.end(), EqualKeyTo(Key))) == mData.end()){
                        QUEST_ERROR << Key << " not found in this map" << std::endl;
                    }
                }

                return *(i->second);
            }

            /**
             * @brief 返回键对应的值（非指针）
             */
            TDataType& at(const key_type& Key) const{
                pair_iterator sorted_part_end;
                sorted_part_end = mData.begin() + mSortedPartSize;

                pair_iterator i(std::lower_bound(mData.begin(), sorted_part_end, Key, CompareKey()));
                if(i == sorted_part_end){
                    if((i = std::find_if(sorted_part_end, mData.end(), EqualKeyTo(Key))) == mData.end()){
                        QUEST_ERROR << Key << " not found in this map" << std::endl;
                    }
                }

                return *(i->second);
            }

            /**
             * @brief 排序
             */
            void Sort(){
                std::sort(mData.begin(), mData.end(), CompareKey());
                mSortedPartSize = mData.size();
            }

            /**
             * @brief 获取内部容器
             */
            TContainerType& GetContainer(){
                return mData;
            }

            /**
             * @brief 获取内部容器
             */
            const TContainerType& GetContainer() const{ 
                return mData;
            }

            /**
             * @Brief 获取最大缓冲区大小
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
             * @brief 判断容器是否为空
             */
            bool empty() const{
                return mData.empty();
            }

            /**
             * @brief 判断是都全部排序
             */
            bool IsSorted() const{
                return mSortedPartSize == mData.size();
            }

            std::string Info() const{
                std::stringstream buffer;
                buffer << "Pointer vector map (size = " << size() << ") : ";

                return buffer.str();
            }

            void PrintInfo(std::ostream& rOstream) const{
                rOstream << Info();
            }

            void PrintData(std::ostream& rOstream) const{
                for(typename TContainerType::const_iterator i = mData.begin(); i != mData.end(); ++i){
                    rOstream << "(" << i->first << " , " << *(i->second) << ")" << std::endl;
                }
            }


        public:
            

        protected:

        private:
            class CompareKey{
                public:
                    bool operator()(const value_type& a, key_type b) const{
                        return TCompareType()(a.first, b);
                    }

                    bool operator()(key_type a, const value_type& b) const{
                        return TCompareType()(a, b.first);
                    }

                    bool operator()(const value_type& a, const value_type& b) const{
                        return TCompareType()(a.first, b.first);
                    }

                protected:

                private:

            };


            class EqualKeyTo{
                public:
                    explicit EqualKeyTo(key_type key) : mKey(key){}

                    bool operator()(const value_type& a, const value_type& b) const{
                        return a.first == b.first;
                    }

                    bool operator()(const value_type& a) const{
                        return a.first == mKey;
                    }

                protected:

                private:
                    key_type mKey;
            };

        private:
            friend class Serializer;

            virtual void save(Serializer& rSerializer) const{
                size_type local_size = mData.size();

                rSerializer.save("size", local_size);
                for(size_type i = 0; i < local_size; ++i){
                    rSerializer.save("Key", mData[i].first);
                    rSerializer.save("Data", mData[i].second);
                }

                rSerializer.save("Sorted Part Size", mSortedPartSize);
                rSerializer.save("Max Buffer Size", mMaxBufferSize);
            }

            virtual void load(Serializer& rSerializer){
                size_type local_size;
                rSerializer.load("size", local_size);
                mData.resize(local_size);

                for(size_type i = 0; i < local_size; ++i){
                    rSerializer.load("Key", mData[i].first);
                    rSerializer.load("Data", mData[i].second);
                }

                rSerializer.load("Sorted Part Size", mSortedPartSize);
                rSerializer.load("Max Buffer Size", mMaxBufferSize);
            }

        private:
            /**
             * @brief 实际存储数据的容器
             * @details std::vector<std::pair<TKeyType, TPointerType>>
             */
            TContainerType mData;

            /**
             * @brief 排序部分的大小
             */
            size_type mSortedPartSize;

            /**
             * @brief 最大缓冲区大小(当未排序部分的大小超过该值时，将进行排序)
             */
            size_type mMaxBufferSize;

    };

    template<typename TDataType,
            typename TGetKeyType,
            typename TCompareType,
            typename TPointerType,
            typename TContainerType>
    inline std::istream& operator >> (std::istream& rIstream, PointerVectorMap<TDataType, TGetKeyType, TCompareType, TPointerType, TContainerType>& rThis);

    template<typename TDataType,
            typename TGetKeyType,
            typename TCompareType,
            typename TPointerType,
            typename TContainerType>
    inline std::ostream& operator << (std::ostream& rOstream, const PointerVectorMap<TDataType, TGetKeyType, TCompareType, TPointerType, TContainerType>& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }
    
} // namespace Quest

#endif //QUEST_POINTER_VECTOR_MAP_HPP