/*-------------------------------------
用来存储指向数据对象的指针
-------------------------------------*/

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
            PointerVector():mData(){}


            template<typename TinputIteraatorType>
            PointerVector(TinputIteraatorType first, TinputIteraatorType last):mData(first, last){}


            PointerVector(const PointerVector& rOther):mData(rOther.mData){}


            explicit PointerVector(const TContainerType& rOther):mData(rOther){}


            explicit PointerVector(std::size_t NewSize):mData(NewSize){}


            ~PointerVector(){}


            PointerVector& operator = (const PointerVector& rOther){
                mData = rOther.mData;
                return *this;
            }


            TDataType& operator [] (const size_type& i){
                return *(mData[i]);
            }


            const TDataType& operator [] (const size_type& i) const{
                return *(mData[i]);
            }


            pointer& operator () (const size_type& i){
                return mData[i];
            }


            const_pointer& operator () (const size_type& i) const{
                return mData[i];
            }


            bool operator == (const PointerVector& rOther) const{
                if(size() != rOther.size()){
                    return false;
                } else {
                    return std::equal(mData.begin(), mData.end(), rOther.mData.begin(), this->EqualKeyTo());
                }
            }


            iterator begin() const{
                return iterator(mData.begin());
            }


            const_iterator begin() const{
                return const_iterator(mData.begin());
            }


            iterator end() const{
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
                return *(mData.front());
            }


            const_reference front() const{
                assert(!empty());
                return *(mData.front());
            }


            reference back(){
                assert(!empty());
                return *(mData.back());
            }


            const_reference back() const{
                assert(!empty());
                return *(mData.back());
            }


            size_type size() const{
                return mData.size();
            }


            size_type max_size() const{
                return mData.max_size();
            }


            void swap(PointerVector& rOther){
                mData.swap(rOther.mData);
            }


            void push_back(const TPointerType& x){
                mData.push_back(x);
            }


            void push_back(TPointerType&& x){
                mData.push_back(std::move(x));
            }


            template<typename... Args>
            void emplace_back(Args&&... args){
                mData.emplace_back(std::forward<Args>(args)...);
            }


            iterator insert(iteartor Position, const TPointerType pData){
                return iterator(mData.insert(Position, pData));
            }


            template<typename InputIterator>
            void insert(InputIterator First, InputIterator Last){
                for(;First != Last; ++First){
                    insert(*First);
                }
            }


            iterator erase(iterator Position){
                return iterator(mData.erase(Position.base()));
            }


            iterator erase(iterator First, iterator Last){
                return iterator(mData.erase(First.base(), Last.base()));
            }


            void clear(){
                mData.clear();
            }


            void resize(size_type NewSize){
                mData.resize(NewSize);
            }


            void reserve(size_type NewCapacity){
                mData.reserve(NewCapacity);
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