#ifndef QUEST_GLOBAL_POINTERS_VECTOR_HPP
#define QUEST_GLOBAL_POINTERS_VECTOR_HPP

// 系统头文件
#include <vector>
#include <iostream>

// 项目头文件
#include "includes/define.hpp"
#include "includes/global_pointer.hpp"
#include "includes/serializer.hpp"

namespace Quest{

    /**
     * @class GlobalPointersVector
     * @brief 全局指针向量(Condition)
     * @tparam TDataType 数据类型
     */
    template<typename TDataType>
    class GlobalPointersVector final{
        public:
            QUEST_CLASS_POINTER_DEFINITION(GlobalPointersVector);

            using TContainerType = std::vector<GlobalPointer<TDataType>>;
            using TPointerType = GlobalPointer<TDataType>;
            using data_type = TDataType;
            using value_type = TPointerType;
            using pointer = TPointerType;
            using const_pointer = const TPointerType;
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
            GlobalPointersVector(): mData(){}

            /**
             * @brief 构造函数
             * @param rList 指针列表
             */
            GlobalPointersVector(const std::initializer_list<GlobalPointer<TDataType>>& rList): mData(rList){}

            /**
             * @brief 析构函数
             */
            ~GlobalPointersVector(){}

            /**
             * @brief 从另一个容器填充当前容器
             * @tparam TContainerType 容器类型
             * @param rContainer 源容器
             */
            template<typename TContainerType>
            void FillFromContainer(TContainerType& rContainer){
                this->reserve(rContainer.size());
                for(auto& data:rContainer){
                    this->push_back(GlobalPointer<TDataType>(&data));
                }
            }

            /**
             * @brief 对容器内所有指针进行排序
             */
            void Sort(){
                std::sort(mData.begin(),mData.end(),GlobalPointerCompare<TDataType>());
            }

            /**
             * @brief 对容器内所有指针进行去重
             */
            void unique(){
                Sort();
                auto end_it = mData.end();
                auto new_end_it = std::unique(mData.begin(),mData.end(),GlobalPointerComparor<TDataType>());
                this->erase(new_end_it,end_it);
            }

            /**
             * @brief 将容器的容量缩小到适应其大小
             */
            void shrink_to_fit(){
                mData.shrink_to_fit();
            }

            /**
             * @brief 重载赋值运算符
             */
            GlobalPointersVector& operator = (const GlobalPointersVector& rOther){
                mData = rOther.mData;
                return *this;
            }

            /**
             * @brief 下标运算符重载，返回对应的指针的对象的引用
             */
            TDataType& operator[] (const size_type& rIndex){
                return *(mData[rIndex]);
            }

            /**
             * @brief 下标运算符重载，返回对应的指针的对象的引用
             */
            const TDataType& operator[] (const size_type& rIndex) const{
                return *(mData[rIndex]);
            }

            /**
             * @brief 函数调用运算符重载，返回对应的指针对象
             */
            Pointer& operator() (const size_type& rIndex){
                return mData[rIndex];
            }

            /**
             * @brief 函数调用运算符重载，返回对应的指针对象
             */
            const Pointer& operator() (const size_type& rIndex) const{
                return mData[rIndex];
            }

            /**
             * @brief 重载比较运算符
             */
            bool operator == (const GlobalPointersVector& rOther) const{
                if(this->size()!= rOther.size()){
                    return false;
                } else {
                    return std::equal(mData.begin(),mData.end(),rOther.begin(),this->EqualKeyTo());
                }
            }

            /**
             * @brief 返回指向首指针的迭代器
             */
            iterator begin(){
                return iterator(mData.begin());
            }   

            /**
             * @brief 返回指向首指针的迭代器
             */
            const_iterator begin() const{   
                return const_iterator(mData.begin());
            }   

            /**
             * @brief 返回指向尾指针的迭代器
             */
            iterator end(){
                return iterator(mData.end());
            }   

            /**
             * @brief 返回指向尾指针的迭代器
             */
            const_iterator end() const{   
                return const_iterator(mData.end());
            }   

            /**
             * @brief 指向反序首指针的迭代器
             */
            reverse_iterator rbegin(){
                return reverse_iterator(mData.rbegin());
            }   

            /**
             * @brief 指向反序首指针的迭代器
             */
            const_reverse_iterator rbegin() const{   
                return const_reverse_iterator(mData.rbegin());
            }   

            /**
             * @brief 指向反序尾指针的迭代器
             */
            reverse_iterator rend(){
                return reverse_iterator(mData.rend());
            }   

            /**
             * @brief 指向反序尾指针的迭代器
             */
            const_reverse_iterator rend() const{   
                return const_reverse_iterator(mData.rend());
            }   

            /**
             * @brief 返回首元素中的指针的迭代器
             */
            ptr_iterator ptr_begin(){
                return mData.begin();
            }   

            /**
             * @brief 返回首元素中的指针的迭代器
             */
            ptr_const_iterator ptr_begin() const{   
                return mData.begin();
            }   

            /**
             * @brief 返回尾元素中的指针的迭代器
             */
            ptr_iterator ptr_end(){
                return mData.end();
            }   

            /**
             * @brief 返回尾元素中的指针的迭代器
             */
            ptr_const_iterator ptr_end() const{   
                return mData.end();
            }   

            /**
             * @brief 指向反序首元素中的指针的迭代器
             */
            ptr_reverse_iterator ptr_rbegin(){
                return mData.rbegin();
            }   

            /**
             * @brief 指向反序首元素中的指针的迭代器
             */
            ptr_const_reverse_iterator ptr_rbegin() const{   
                return mData.rbegin();
            }   

            /**
             * @brief 指向反序尾元素中的指针的迭代器
             */
            ptr_reverse_iterator ptr_rend(){
                return mData.rend();
            }   

            /**
             * @brief 指向反序尾元素中的指针的迭代器
             */
            ptr_const_reverse_iterator ptr_rend() const{   
                return mData.rend();
            }   

            /**
             * @brief 返回首元素的指针指向的对象引用
             */
            reference front(){
                assert(!mData.empty());
                return *(mData.front());
            }

            /**
             * @brief 返回首元素的指针指向的对象引用
             */
            const_reference front() const{
                assert(!mData.empty());
                return *(mData.front());
            }

            /**
             * @brief 返回尾元素的指针指向的对象引用
             */
            reference back(){
                assert(!mData.empty());
                return *(mData.back());
            }

            /**
             * @brief 返回尾元素的指针指向的对象引用
             */
            const_reference back() const{
                assert(!mData.empty());
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
             * @brief 与另一个容器交换元素
             */
            void swap(GlobalPointersVector& rOther){
                mData.swap(rOther.mData);
            }

            /**
             * @brief 在尾部插入元素
             */
            void push_back(TPointerType rPointer){
                mData.push_back(rPointer);
            }

            /**
             * @brief 在指定位置插入元素
             */
            iterator insert(iterator Position, const TPointerType pData){
                return iterator(mData.insert(Position,pData));
            }

            /**
             * @brief 在尾部插入一系列元素
             */
            template<typename InputIterator>
            void insert(InputIterator First, InputIterator Last){
                for(;First!=Last;First++){
                    insert(*First);
                }
            }

            /**
             * @brief 清除指定位置的元素
             */
            iterator erase(iterator Position){
                return iterator(mData.erase(Position.base()));
            }

            /**
             * @brief 清除迭代器范围内的元素
             */
            iterator erase(iterator First, iterator Last){
                return iterator(mData.erase(First.base(),Last.base()));
            }

            /**
             * @brief 清除所有元素
             */
            void clear(){
                mData.clear();
            }

            /**
             * @brief 调整容器大小
             */
            void resize(size_type new_dim) const{
                mData.resize(new_dim);
            }

            /**
             * @brief 调整容器大小
             */
            void resize(size_type new_dim){
                mData.resize(new_dim);
            }

            /**
             * @brief 预分配指定大小的内存
             */
            void reserve(int dim){
                mData.reserve(dim);
            }

            /**
             * @brief 返回当前容量
             */
            int capacity(){
                return mData.capacity();
            }

            std::string Info() const{
                std::stringstream buffer;
                buffer << "GlobalPointersVector";
                return buffer.str();
            }

            void PrintInfo(std::ostream& rOstream) const{
                rOstream << Info();
            }

            void PrintData(std::ostream& rOstream) const{}

            TContainerType& GetContainer(){
                return mData;
            }

            const TContainerType& GetContainer() const{
                return mData;
            }

        protected:

        private:
            friend class Serializer;

            void save(Serializer& rSerializer) const{
                rSerializer.save("Size",this->size());

                for(std::size_t i=0;i<this->size();i++){
                    rSerializer.save("Data",mData[i]);
                }
            }

            void load(Serializer& rSerializer){
                std::size_t size;
                rSerializer.load("Size",size);

                for(std::size_t i=0;i<size;i++){
                    GlobalPointer<TDataType> data(nullptr);
                    rSerializer.load("Data",data);
                    this->push_back(data);
                }
            }

        private:
            /**
             * @brief 存储全局指针的Vector容器
             * @details std::vector<GlobalPointer<TDataType>>
             */
            TContainerType mData;


    }; // class GlobalPointersVector

    template<typename TDataType>
    inline std::istream& operator >> (std::istream& rIstream, GlobalPointersVector<TDataType>& rVector){
        return rIstream;
    }

    template<typename TDataType>
    inline std::ostream& operator << (std::ostream& rOstream, const GlobalPointersVector<TDataType>& rVector){
        rVector.PrintInfo(rOstream);
        rOstream << std::endl;
        rVector.PrintData(rOstream);

        return rOstream;
    }

} // namespace Quest


#endif //QUEST_GLOBAL_POINTERS_VECTOR_HPP