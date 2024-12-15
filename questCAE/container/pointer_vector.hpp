#ifndef QUEST_POINTER_VECTOR_HPP
#define QUEST_POINTER_VECTOR_HPP

// 系统头文件
#include <functional>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

// 第三方头文件
#include <boost/iterator/indirect_iterator.hpp>

// 项目头文件
#include "includes/define.hpp"
#include "includes/serializer.hpp"

namespace Quest{

    /**
     * @brief 存储指针的容器类（几何对象中存储节点）
     * @tparam TDataType 存储的数据类型
     * @tparam TPointerType 数据指针类型
     * @tparam TContainerType 容器类型
     */
    template<typename TDataType,
            typename TPointerType = typename TDataType::Pointer,
            typename TContainerType = std::vector<TPointerType>>
    class PointerVector final{
        public:
            QUEST_CLASS_POINTER_DEFINITION(PointerVector);

            using data_type = TDataType;
            using value_type = TDataType;
            using pointer = TPointerType;
            using const_pointer = const TPointerType;
            using reference = TDataType&;
            using const_reference = const TDataType&;
            using ContainerType = TContainerType;

            using iterator = boost::indirect_iterator<typename ContainerType::iterator>;
            using const_iterator = boost::indirect_iterator<typename ContainerType::const_iterator>;
            using reverse_iterator = boost::indirect_iterator<typename ContainerType::reverse_iterator>;
            using const_reverse_iterator = boost::indirect_iterator<typename ContainerType::const_reverse_iterator>;

            using size_type = typename TContainerType::size_type;
            using ptr_iterator = typename TContainerType::iterator;
            using ptr_const_iterator = typename TContainerType::const_iterator;
            using ptr_reverse_iterator = typename TContainerType::reverse_iterator;
            using ptr_const_reverse_iterator = typename TContainerType::const_reverse_iterator;
            using difference_type = typename TContainerType::difference_type;

        public:
            /**
             * @brief 默认构造函数
             */
            PointerVector():mData(){}

            /**
             * @brief 构造函数
             * @param first 输入迭代器
             * @param last 输入迭代器
             */
            template<typename TinputIteraatorType>
            PointerVector(TinputIteraatorType first, TinputIteraatorType last):mData(first, last){}

            /**
             * @brief 复制构造函数
             */
            PointerVector(const PointerVector& rOther):mData(rOther.mData){}

            /**
             * @brief 构造函数
             * @param rOther 具体容器对象
             */
            explicit PointerVector(const TContainerType& rOther):mData(rOther){}

            /**
             * @brief 构造函数
             * @param NewSize 容器大小
             */
            explicit PointerVector(std::size_t NewSize):mData(NewSize){}

            /**
             * @brief 析构函数
             */
            ~PointerVector(){}

            /**
             * @brief 赋值运算符
             */
            PointerVector& operator = (const PointerVector& rOther){
                mData = rOther.mData;
                return *this;
            }

            /**
             * @brief 下标运算符重载，返回指针指向的数据
             * @param i 下标
             */
            TDataType& operator [] (const size_type& i){
                return *(mData[i]);
            }

            /**
             * @brief 下标运算符重载，返回指针指向的数据
             * @param i 下标
             */
            const TDataType& operator [] (const size_type& i) const{
                return *(mData[i]);
            }

            /**
             * @brief 函数调用运算符重载，返回指针
             * @param i 下标
             */
            pointer& operator () (const size_type& i){
                return mData[i];
            }

            /**
             * @brief 函数调用运算符重载，返回指针
             * @param i 下标
             */
            const_pointer& operator () (const size_type& i) const{
                return mData[i];
            }

            /**
             * @brief 相等运算符重载
             * @param rOther 另一个PointerVector对象
             */
            bool operator == (const PointerVector& rOther) const{
                if(size() != rOther.size()){
                    return false;
                } else {
                    return std::equal(mData.begin(), mData.end(), rOther.mData.begin(), this->EqualKeyTo());
                }
            }

            /**
             * @brief 返回容器头指针
             */
            iterator begin() const{
                return iterator(mData.begin());
            }

            /**
             * @brief 返回容器头指针
             */
            const_iterator begin() const{
                return const_iterator(mData.begin());
            }

            /**
             * @brief 返回容器尾指针
             */
            iterator end() const{
                return iterator(mData.end());
            }

            /**
             * @brief 返回容器尾指针
             */
            const_iterator end() const{
                return const_iterator(mData.end());
            }

            /**
             * @brief 返回容器反序头指针
             */
            reverse_iterator rbegin(){
                return reverse_iterator(mData.rbegin());
            }

            /**
             * @brief 返回容器反序头指针
             */
            const_reverse_iterator rbegin() const{
                return const_reverse_iterator(mData.rbegin());
            }

            /**
             * @brief 返回容器反序尾指针
             */
            reverse_iterator rend(){
                return reverse_iterator(mData.rend());
            }

            /**
             * @brief 返回容器反序尾指针
             */
            const_reverse_iterator rend() const{
                return const_reverse_iterator(mData.rend());
            }

            /**
             * @brief 获取头元素
             */
            ptr_iterator ptr_begin(){
                return mData.begin();
            }

            /**
             * @brief 获取头元素
             */
            ptr_const_iterator ptr_begin() const{
                return mData.begin();
            }

            /**
             * @brief 获取尾元素
             */
            ptr_iterator ptr_end(){
                return mData.end();
            }

            /**
             * @brief 获取尾元素
             */
            ptr_const_iterator ptr_end() const{
                return mData.end();
            }

            /**
             * @brief 获取反序头元素
             */
            ptr_reverse_iterator ptr_rbegin(){
                return mData.rbegin();
            }

            /**
             * @brief 获取反序头元素
             */
            ptr_const_reverse_iterator ptr_rbegin() const{
                return mData.rbegin();
            }

            /**
             * @brief 获取反序尾元素
             */
            ptr_reverse_iterator ptr_rend(){
                return mData.rend();
            }

            /**
             * @brief 获取反序尾元素
             */
            ptr_const_reverse_iterator ptr_rend() const{
                return mData.rend();
            }

            /**
             * @brief 返回第一个元素
             */
            reference front(){
                assert(!empty());
                return *(mData.front());
            }

            /**
             * @brief 返回第一个元素
             */
            const_reference front() const{
                assert(!empty());
                return *(mData.front());
            }

            /**
             * @brief 返回最后一个元素
             */
            reference back(){
                assert(!empty());
                return *(mData.back());
            }

            /**
             * @brief 返回最后一个元素
             */
            const_reference back() const{
                assert(!empty());
                return *(mData.back());
            }

            /**
             * @brief 返回容器大小
             */
            size_type size() const{
                return mData.size();
            }

            /**
             * @brief 返回容器最大容量
             */
            size_type max_size() const{
                return mData.max_size();
            }

            /**
             * @brief 交换容器数据
             * @param rOther 另一个PointerVector对象
             */
            void swap(PointerVector& rOther){
                mData.swap(rOther.mData);
            }

            /**
             * @brief 在尾部添加元素
             */
            void push_back(const TPointerType& x){
                mData.push_back(x);
            }

            /**
             * @brief 在尾部添加元素
             */
            void push_back(TPointerType&& x){
                mData.push_back(std::move(x));
            }

            /**
             * @brief 在尾部添加元素
             */
            template<typename... Args>
            void emplace_back(Args&&... args){
                mData.emplace_back(std::forward<Args>(args)...);
            }

            /**
             * @brief 在指定位置添加元素
             */
            iterator insert(iteartor Position, const TPointerType pData){
                return iterator(mData.insert(Position, pData));
            }

            /**
             * @brief 在容器尾部添加元素
             */
            template<typename InputIterator>
            void insert(InputIterator First, InputIterator Last){
                for(;First != Last; ++First){
                    mdata.insert(*First);
                }
            }

            /**
             * @brief 删除指定位置元素
             */
            iterator erase(iterator Position){
                return iterator(mData.erase(Position.base()));
            }

            /**
             * @brief 删除指定范围元素
             */
            iterator erase(iterator First, iterator Last){
                return iterator(mData.erase(First.base(), Last.base()));
            }

            /**
             * @brief 清除数据
             */
            void clear(){
                mData.clear();
            }

            /**
             * @brief 重新设置容器大小
             */
            void resize(size_type NewSize){
                mData.resize(NewSize);
            }

            /**
             * @brief 重新设置容器内存空间
             */
            void reserve(size_type NewCapacity){
                mData.reserve(NewCapacity);
            }

            /**
             * @brief 返回可以储存的最大元素数量
             */
            int capacity(){
                return mData.capacity();
            }

            /**
             * @brief 获取具体容器对象
             */
            TContainerType& GetContainer(){
                return mData;
            }

            /**
             * @brief 获取具体容器对象
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


            std::string Info() const{
                std::stringstream buffer;
                buffer << "PointerVector (size = " << size() << ") : ";

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
            friend class Serializer;

            virtual void save(Serializer& rSerializer) const{
                size_type local_size = mData.size();

                rSerializer.save("size", local_size);

                for(size_type i = 0; i < local_size; ++i){
                    rSerializer.save("E", mData[i]);
                }
            }

            virtual void load(Serializer& rSerializer){
                size_type local_size;
                rSerializer.load("size", local_size);

                mData.resize(local_size);

                for(size_type i = 0; i < local_size; ++i){
                    rSerializer.load("E", mData[i]);
                }
            }

        private:
            /**
             * @brief 具体容器
             * @details 默认为std::vector<TDataType::Pointer>
             */
            TContainerType mData;

    };

    template<typename TDataType, typename TPointerType, typename TContainerType>
    inline std::istream& operator >> (std::istream& rIstream, PointerVector<TDataType, TPointerType, TContainerType>& rThis);

    template<typename TDataType, typename TPointerType, typename TContainerType>
    inline std::ostream& operator << (std::ostream& rOstream, const PointerVector<TDataType, TPointerType, TContainerType>& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }

} // namespace Quest

#endif //QUEST_POINTER_VECTOR_HPP