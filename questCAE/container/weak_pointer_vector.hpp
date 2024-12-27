#ifndef QUEST_WEAK_POINTER_VECTOR_HPP
#define QUEST_WEAK_POINTER_VECTOR_HPP

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

    /**
     * @class WeakPointerVector
     * @brief 存储指针的弱引用的向量容器
     */
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

            /**
             * @brief 默认构造函数
             */
            WeakPointerVector(): mData() {}

            /**
             * @brief 构造函数
             */
            template<typename TInputIterator>
            WeakPointerVector(TInputIterator first, TInputIterator last): mData(first, last) {}

            /**
             * @brief 拷贝构造函数
             */
            WeakPointerVector(const WeakPointerVector& rOther): mData(rOther.mData) {}

            /**
             * @brief 构造函数
             */
            explicit WeakPointerVector(const TContainerType& rContainer): mData(rContainer) {}

            /**
             * @brief 析构函数
             */
            ~WeakPointerVector() {}

            /**
             * @brief 赋值运算符
             */
            WeakPointerVector& operator = (const WeakPointerVector& rOther){
                mData = rOther.mData;
                return *this;
            }

            /**
             * @brief 下标运算符重载，返回TDataType的引用
             */
            TDataType& operator[](const size_type& i){
                return *(mData[i].lock());
            }

            /**
             * @brief 下标运算符重载，返回TDataType的引用
             */
            const TDataType& operator[](const size_type& i) const{
                return *(mData[i].lock());
            }

            /**
             * @brief 函数调用运算符重载，返回TDataType的指针
             */
            pointer& operator()(const size_type& i){
                return mData[i];
            }

            /**
             * @brief 函数调用运算符重载，返回TDataType的指针
             */
            const_pointer& operator()(const size_type& i) const{
                return mData[i];
            }

            /**
             * @brief 判断两个WeakPointerVector是否相等
             */
            bool operator == (const WeakPointerVector& r) const{
                if(size() != r.size())
                    return false;
                else
                    return std::equal(mData.begin(), mData.end(), r.mData.begin());
            }

            /**
             * @brief 返回容器首元素的迭代器
             */
            iterator begin(){
                return iterator(mData.begin());
            }

            /**
             * @brief 返回容器首元素的迭代器
             */
            const_iterator begin() const{
                return const_iterator(mData.begin());
            }

            /**
             * @brief 返回容器尾元素的迭代器
             */
            iterator end(){
                return iterator(mData.end());
            }

            /**
             * @brief 返回容器尾元素的迭代器
             */
            const_iterator end() const{
                return const_iterator(mData.end());
            }

            /**
             * @brief 返回容器逆向首元素的迭代器
             */
            reverse_iterator rbegin(){
                return reverse_iterator(mData.rbegin());
            }

            /**
             * @brief 返回容器逆向首元素的迭代器
             */
            const_reverse_iterator rbegin() const{
                return const_reverse_iterator(mData.rbegin());
            }

            /**
             * @brief 返回容器逆向尾元素的迭代器
             */
            reverse_iterator rend(){
                return reverse_iterator(mData.rend());
            }

            /**
             * @brief 返回容器逆向尾元素的迭代器
             */
            const_reverse_iterator rend() const{
                return const_reverse_iterator(mData.rend());
            }

            /**
             * @brief 返回vector首元素的迭代器
             */
            ptr_iterator ptr_begin(){
                return mData.begin();
            }

            /**
             * @brief 返回vector首元素的迭代器
             */
            ptr_const_iterator ptr_begin() const{
                return mData.begin();
            }

            /**
             * @brief 返回vector尾元素的迭代器
             */
            ptr_iterator ptr_end(){
                return mData.end();
            }

            /**
             * @brief 返回vector尾元素的迭代器
             */
            ptr_const_iterator ptr_end() const{
                return mData.end();
            }

            /**
             * @brief 返回vector逆序首元素的迭代器
             */
            ptr_reverse_iterator ptr_rbegin(){
                return mData.rbegin();
            }

            /**
             * @brief 返回vector逆序首元素的迭代器
             */
            ptr_const_reverse_iterator ptr_rbegin() const{
                return mData.rbegin();
            }

            /**
             * @brief 返回vector逆序尾元素的迭代器
             */
            ptr_reverse_iterator ptr_rend(){
                return mData.rend();
            }

            /**
             * @brief 返回vector逆序尾元素的迭代器
             */
            ptr_const_reverse_iterator ptr_rend() const{
                return mData.rend();
            }

            /**
             * @brief 返回容器首元素(TDataType)的引用
             */
            reference front(){
                assert(!empty());
                return *((mData.front()).lock());
            }

            /**
             * @brief 返回容器首元素(TDataType)的引用
             */
            const_reference front() const{
                assert(!empty());
                return *((mData.front()).lock());
            }

            /**
             * @brief 返回容器尾元素(TDataType)的引用
             */
            reference back(){
                assert(!empty());
                return *((mData.back()).lock());
            }

            /**
             * @brief 返回容器尾元素(TDataType)的引用
             */
            const_reference back() const{
                assert(!empty());
                return *((mData.back()).lock());
            }

            /**
             * @brief 返回容器大小
             */
            size_type size() const{
                return mData.size();
            }

            /**
             * @brief 重新设置容器大小
             */
            void resize(size_type new_dim){
                return mData.resize(new_dim);
            }

            /**
             * @brief 返回容器最大容量
             */
            size_type max_size() const{
                return mData.max_size();
            }

            /**
             * @brief 与另一个WeakPointerVector交换元素
             */
            void swap(WeakPointerVector& rOther){
                mData.swap(rOther.mData);
            }

            /**
             * @brief 向容器尾部添加元素
             */
            void push_back(TPointerType value){
                mData.push_back(value);
            }

            /**
             * @brief 向容器尾部添加元素
             */
            void push_back(const_reference value){
                push_back(TPointerType(new TDataType(value)));
            }

            /**
             * @brief 向指定位置插入元素
             */
            iterator insert(iterator Position, const TDataType& rData){
                return iterator(mData.insert(Position, TPointerType(new TDataType(rData))));
            }

            /**
             * @brief 向指定位置插入元素
             */
            iterator insert(iterator Position, const TPointerType pData){
                return iterator(mData.insert(position, pData));
            }

            /**
             * @brief 插入一系列元素
             */
            template<typename InputIterator>
            void insert(InputIterator first, InputIterator last){
                for(;first != last; ++first)
                    insert(*first);
            }

            /**
             * @brief 删除指定位置的元素
             */
            iterator erase(iterator pos){
                return iterator(mData.erase(pos.base()));
            }

            /**
             * @brief 删除指定范围的元素
             */
            iterator erase(iterator first, iterator last){
                return iterator(mData.erase(first.base(), last.base()));
            }

            /**
             * @brief 清空容器
             */
            void clear(){
                mData.clear();
            }

            /**
             * @brief 重新分配内容
             */
            void reserve(int dim){
                mData.resever(dim);
            }

            /**
             * @brief 返回容量
             */
            int capacity(){
                return mData.capacity();
            }

            /**
             * @brief 返回内部容器
             */
            TContainerType& GetContainer(){
                return mData;
            }

            /**
             * @brief 返回内部容器
             */
            const TContainerType& GetContainer() const{
                return mData;
            }

            /**
             * @brief 判断容器是否为空
             */
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
            /**
             * @brief 存储指针的弱引用的向量容器
             * @details std::vector<TDataType::WeakPointer>
             */
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