/*-----------------------
弱指针数组
删除常量版resize函数
-----------------------*/

#ifndef WEAK_POINTER_VECTOR_HPP
#define WEAK_POINTER_VECTOR_HPP

// 系统头文件
#include <functional>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

// 项目头文件
#include "includes/define.hpp"
#include "container/weak_pointer_vector_iterator.hpp"

namespace Quest{
    template<typename TDataType, typename TPointerType = typename TDataType::WeakPointer,
            typename TContainerType = std::vector<TPointerType>>
    class WeakPointerVector{
        public:
            QUEST_CLASS_POINTER_DEFINITION(WeakPointerVector);
            
            typedef TDataType data_type;
            typedef TPointerType value_type;
            typedef TPointerType pointer;
            typedef const TPointerType const_pointer;
            typedef TDataType& reference;
            typedef const TDataType& const_reference;
            typedef TContainerType containerType;

            typedef WeakPointerVectorIterator<typename TContainerType::iterator, TDataType> iterator;
            typedef WeakPointerVectorIterator<typename TContainerType::const_iterator, TDataType> const_iterator;
            typedef WeakPointerVectorIterator<typename TContainerType::reverse_iterator, TDataType> reverse_iterator;
            typedef WeakPointerVectorIterator<typename TContainerType::const_reverse_iterator, TDataType> const_reverse_iterator;

            typedef typename TContainerType::size_type size_type;
            typedef typename TContainerType::iterator ptr_iterator;
            typedef typename TContainerType::const_iterator ptr_const_iterator;
            typedef typename TContainerType::reverse_iterator ptr_reverse_iterator;
            typedef typename TContainerType::const_reverse_iterator ptr_const_reverse_iterator;

            WeakPointerVector(): mData() {}

            template<typename TInputIterator>
            WeaakPointerVector(TInputIterator first, TInputIterator last): mData(first, last) {}

            WeakPointerVector(const WeakPointerVector& rOther): mData(rOther.mData) {}

            explicit WeakPointerVector(const TContainerType& rContainer): mData(rContainer) {}

            ~WeakPointerVector() {}

            WeakPointerVector& operator = (const WeakPointerVector& rOther){
                mData = rOther.mData;
                return *this;
            }

            TDataType& operator[](const size_type& i){
                return *(mData[i].lock());
            }

            const TDataType& operator[](const size_type& i) const{
                return *(mData[i].lock());
            }

            pointer& operator()(const size_type& i){
                return mData[i];
            }

            const_pointer& operator()(const size_type& i) const{
                return mData[i];
            }

            bool operator == (const WeakPointerVector& r) const{
                if(size() != r.size())
                    return false;
                else
                    return std::equal(mData.begin(), mData.end(), r.mData.begin());
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

            reference front(){
                assert(!empty());
                return *((mData.front()).lock());
            }

            const_reference front() const{
                assert(!empty());
                return *((mData.front()).lock());
            }

            reference back(){
                assert(!empty());
                return *((mData.back()).lock());
            }

            const_reference back() const{
                assert(!empty());
                return *((mData.back()).lock());
            }

            size_type size() const{
                return mData.size();
            }

            void resize(size_type new_dim){
                return mData.resize(new_dim);
            }

            size_type max_size() const{
                return mData.max_size();
            }

            void swap(WeakPointerVector& rOther){
                mData.swap(rOther.mData);
            }

            void push_back(TPointerType value){
                mData.push_back(value);
            }

            void push_back(const_reference value){
                push_back(TPointerType(new TDataType(value)));
            }

            iterator insert(iterator Position, const TDataType& rData){
                return iterator(mData.insert(Position, TPointerType(new TDataType(rData))));
            }

            iterator insert(iterator Position, const TPointerType pData){
                return iterator(mData.insert(position, pData));
            }

            template<typename InputIterator>
            void insert(InputIterator first, InputIterator last){
                for(;first != last; ++first)
                    insert(*first);
            }

            iterator erase(iterator pos){
                return iterator(mData.erase(pos.base()));
            }

            iterator erase(iterator first, iterator last){
                return iterator(mData.erase(first.base(), last.base()));
            }

            void clear(){
                mData.clear();
            }

            void reserve(int dim){
                mData.resever(dim);
            }

            int capacity(){
                return mData.capacity();
            }

            TContainerType& GetContainer(){
                return mData;
            }

            const TContainerType& GetContainer() const{
                return mData;
            }

            bool empty() const{
                return mData.empty();
            }

            std::stringt Info() const{
                std::stringstream buffer;
                buffer << "WeakPointerVector: size = " << size() << ") :";

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
            TContainerType mData;

    }; // class WeakPointerVector

    template<typename TDataType, typename TPointerType, typename TContainerType>
    inline std::istream& operator >> (std::istream& rIstream, WeakPointerVector<TDataType, TPointerType, TContainerType>& rThis);

    template<typename TDataType, typename TPointerType, typename TContainerType>
    inline std::ostream& operator << (std::ostream& rOstream, const WeakPointerVector<TDataType, TPointerType, TContainerType>& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }

} // namespace Quest

#endif // WEAK_POINTER_VECTOR_HPP