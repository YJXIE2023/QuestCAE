/*--------------------------------------
提供类似于 STL map 的有序关联容器
使用 vector 来存储指向其数据的指针
--------------------------------------*/

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


            PointerVectorMap(): mData(), mSortedPartSize(size_type()), mMaxBufferSize(100) {}

            PointerVectorMap(const PointerVectorMap& rOther): mData(rOther.mData), mSortedPartSize(rOther.mSortedPartSize), mMaxBufferSize(rOther.mMaxBufferSize) {}

            explicit PointerVectorMap(const TContainerType& rContainer): mData(rContainer), mSortedPartSize(size_type()), mMaxBufferSize(100) {
                Sort();
                std::unique(mData.begin(), mData.end(), EqualKeyTo());
            }

            ~PointerVectorMap() {}

            PointerVectorMap& operator = (const PointerVectorMap& rOther){
                mData = rOther.mData;
                mSortedPartSize = rOther.mSortedPartSize;

                return *this;
            }

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

            iterator begin(){
                return iterator(mData.beign());
            }

            const_iterator begin() const{
                return const_iterator(mData.beign());
            }

            iterator end(){
                return iterator(mData.end());
            }

            const_iterator end() const{
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

            pair_iterator pair_begin(){
                return mData.begin();
            }

            pair_const_iterator pair_begin() const{
                return mData.begin();
            }

            pair_iterator pair_end(){
                return mData.end();
            }

            pair_const_iterator pair_end() const{
                return mData.end();
            }

            pair_reverse_iterator pair_rbegin(){
                return mData.rbegin();
            }

            pair_const_reverse_iterator pair_rbegin() const{
                return mData.rbegin();
            }

            pair_reverse_iterator pair_rend(){
                return mData.rend();
            }

            pair_const_reverse_iterator pair_rend() const{
                return mData.rend();
            }

            reference front(){
                assert(!empty());
                return *(mData.front()->second);
            }   

            const_reference front() const{
                assert(!empty());
                return *(mData.front()->second);
            }   

            reference back(){
                assert(!empty());
                return *(mData.back()->second);
            }   

            const_reference back() const{
                assert(!empty());
                return *(mData.back()->second);
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

            void swap(PointerVectorMap& rOther){
                mData.swap(rOther.mData);
            }

            void push_back(value_type x){
                mData.push_back(x);
            }

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

            iterator erase(iterator pos){
                return iterator(mData.erase(pos.base()));
            }

            iterator erase(iterator first, iterator last){
                return iterator(mData.erase(first.base(), last.base()));
            }

            iterator erese(const key_type& k){
                return erase(find(k));
            }

            void clear(){
                mData.clear();
            }

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

            size_type count(const key_type& Key){
                return find(Key) == mData.end() ? 0 : 1;
            }

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

            void Sort(){
                std::sort(mData.begin(), mData.end(), CompareKey());
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
            TContainerType mData;
            size_type mSortedPartSize;
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