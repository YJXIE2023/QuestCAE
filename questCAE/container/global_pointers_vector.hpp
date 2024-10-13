/*--------------------------------------
存储全局指针
--------------------------------------*/

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

    template<typename TDataType>
    class GlobalPointerVector final{
        public:
            QUEST_CLASS_POINTER_DEFINITION(GlobalPointerVector);

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


            GlobalPointerVector(): mData(){}

            GlobalPointerVector(const std::initializer_list<GlobalPointer<TDataType>>& rList): mData(rList){}

            ~GlobalPointerVector(){}

            template<typename TContainerType>
            void FillFromContainer(TContainerType& rContainer){
                this->reserve(rContainer.size());
                for(auto& data:rContainer){
                    this->push_back(GlobalPointer<TDataType>(&data));
                }
            }

            void Sort(){
                std::sort(mData.begin(),mData.end(),GlobalPointerCompare<TDataType>());
            }

            void unique(){
                Sort();
                auto end_it = mData.end();
                auto new_end_it = std::unique(mData.begin(),mData.end(),GlobalPointerComparor<TDataType>());
                this->erase(new_end_it,end_it);
            }

            void shrink_to_fit(){
                mData.shrink_to_fit();
            }

            GlobalPointerVector& operator = (const GlobalPointerVector& rOther){
                mData = rOther.mData;
                return *this;
            }

            TDataType& operator[] (const size_type& rIndex){
                return *(mData[rIndex]);
            }

            const TDataType& operator[] (const size_type& rIndex) const{
                return *(mData[rIndex]);
            }

            Pointer& operator() (const size_type& rIndex){
                return mData[rIndex];
            }

            const Pointer& operator() (const size_type& rIndex) const{
                return mData[rIndex];
            }

            bool operator == (const GlobalPointerVector& rOther) const{
                if(this->size()!= rOther.size()){
                    return false;
                } else {
                    return std::equal(mData.begin(),mData.end(),rOther.begin(),this->EqualKeyTo());
                }
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
                assert(!mData.empty());
                return *(mData.front());
            }

            const_reference front() const{
                assert(!mData.empty());
                return *(mData.front());
            }

            reference back(){
                assert(!mData.empty());
                return *(mData.back());
            }

            const_reference back() const{
                assert(!mData.empty());
                return *(mData.back());
            }

            size_type size() const{
                return mData.size();
            }

            size_type max_size() const{
                return mData.max_size();
            }

            void swap(GlobalPointerVector& rOther){
                mData.swap(rOther.mData);
            }

            void push_back(TPointerType rPointer){
                mData.push_back(rPointer);
            }

            iterator insert(iterator Position, const TPointerType pData){
                return iterator(mData.insert(Position,pData));
            }

            template<typename InputIterator>
            void insert(InputIterator First, InputIterator Last){
                for(;First!=Last;First++){
                    insert(*First);
                }
            }

            iterator erase(iterator Position){
                return iterator(mData.erase(Position.base()));
            }

            iterator erase(iterator First, iterator Last){
                return iterator(mData.erase(First.base(),Last.base()));
            }

            void clear(){
                mData.clear();
            }

            void resize(size_type new_dim) const{
                mData.resize(new_dim);
            }

            void resize(size_type new_dim){
                mData.resize(new_dim);
            }

            void reserve(int dim){
                mData.reserve(dim);
            }

            int capacity(){
                return mData.capacity();
            }

            std::string Info() const{
                std::stringstream buffer;
                buffer << "GlobalPointerVector";
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
            TContainerType mData;


    }; // class GlobalPointerVector

    template<typename TDataType>
    inline std::istream& operator >> (std::istream& rIstream, GlobalPointerVector<TDataType>& rVector){
        return rIstream;
    }

    template<typename TDataType>
    inline std::ostream& operator << (std::ostream& rOstream, const GlobalPointerVector<TDataType>& rVector){
        rVector.PrintInfo(rOstream);
        rOstream << std::endl;
        rVector.PrintData(rOstream);

        return rOstream;
    }

} // namespace Quest


#endif //QUEST_GLOBAL_POINTERS_VECTOR_HPP