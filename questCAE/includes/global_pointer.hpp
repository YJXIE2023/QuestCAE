#ifndef QUEST_GLOBAL_POINTER_HPP
#define QUEST_GLOBAL_POINTER_HPP

// 系统头文件
#include <iostream>

// 项目头文件
#include "includes/define.hpp"
#include "includes/serializer.hpp"
#include "includes/key_hash.hpp"

namespace Quest{

    /**
     * @class GlobalPointer
     * @brief 指针的封装，指向位域不同 rank 的数据
     * @tparam TDataType 指针指向的数据类型
     */
    template<typename TDataType>
    class GlobalPointer{
        public:
            using element_type = TDataType;

            /**
             * @brief 默认构造函数
             */
            GlobalPointer(){
                mDataPointer = nullptr;
                #ifdef QUEST_USING_MPI
                    this->mRank = 0;
                #endif
            }

            GlobalPointer(TDataType Data) = delete;

            /**
             * @brief 构造函数
             * @param DataPointer 指向的数据指针
             * @param Rank 线程号
             */
            GlobalPointer(TDataType* DataPointer, int Rank = 0): mDataPointer(DataPointer)
            #ifdef QUEST_USING_MPI
                , mRank(Rank)
            #endif
            {
                #ifdef QUEST_USING_MPI
                    QUEST_DEBUG_ERROR_IF(Rank != 0) << "Trying to construct a global pointer with rank different from zero when quest is not in MPI mode " << std::endl;
                #endif
            }

            /**
             * @brief 构造函数
             * @param DataPointer 指向的数据指针
             * @param Rank 线程号
             */
            GlobalPointer(Quest::shared_ptr<TDataType> DataPointer, int Rank = 0): mDataPointer(DataPointer.get())
            #ifdef QUEST_USING_MPI
                , mRank(Rank)
            #endif
            {
                #ifdef QUEST_USING_MPI
                    QUEST_DEBUG_ERROR_IF(Rank != 0) << "Trying to construct a global pointer with rank different from zero when quest is not in MPI mode " << std::endl;
                #endif
            }

            /**
             * @brief 构造函数
             * @param DataType 指向的数据指针
             * @param Rank 线程号
             */
            GlobalPointer(Quest::intrusive_ptr<TDataType> DataType, int Rank = 0): mDataPointer(DataType.get())
            #ifdef QUEST_USING_MPI
                , mRank(Rank)
            #endif
            {
                #ifdef QUEST_USING_MPI
                    QUEST_DEBUG_ERROR_IF(Rank != 0) << "Trying to construct a global pointer with rank different from zero when quest is not in MPI mode " << std::endl;
                #endif
            }

            /**
             * @brief 构造函数
             * @param DataPointer 指向的数据指针
             * @param Rank 线程号
             */
            GlobalPointer(Quest::weak_ptr<TDataType> DataPointer, int Rank = 0): mDataPointer(DataPointer.lock().get())
            #ifdef QUEST_USING_MPI
                , mRank(Rank)
            #endif
            {
                #ifdef QUEST_USING_MPI
                    QUEST_DEBUG_ERROR_IF(Rank != 0) << "Trying to construct a global pointer with rank different from zero when quest is not in MPI mode " << std::endl;
                #endif
            }

            GlobalPointer(std::unique_ptr<TDataType> DataPointer, int Rank = 0) = delete;

            /**
             * @brief 拷贝构造函数
             */
            GlobalPointer(const GlobalPointer<TDataType>& rOther): mDataPointer(rOther.mDataPointer)
            #ifdef QUEST_USING_MPI
                , mRank(rOther.mRank)
            #endif
            {}

            /**
             * @brief 移动构造函数
             */
            GlobalPointer(GlobalPointer<TDataType>&& rOther): mDataPointer(std::move(rOther.mDataPointer))
            #ifdef QUEST_USING_MPI
                , mRank(std::move(rOther.mRank))
            #endif
            {}

            /**
             * @brief 析构函数
             */
            ~GlobalPointer() = default;

            /**
             * @brief 赋值运算符
             */
            GlobalPointer<TDataType>& operator=(const GlobalPointer<TDataType>& rOther){
                mDataPointer = rOther.mDataPointer;
                #ifdef QUEST_USING_MPI
                    mRank = rOther.mRank;
                #endif
                return *this;
            }

            /**
             * @brief 解引用运算符重载，返回指向的数据
             */
            TDataType& operator*() {
                return *mDataPointer;
            }

            /**
             * @brief 解引用运算符重载，返回指向的数据
             */
            const TDataType& operator*() const {
                return *mDataPointer;
            }

            /**
             * @brief 指针运算符重载，返回指向的数据指针
             */
            TDataType* operator->() {
                return mDataPointer;
            }

            /**
             * @brief 指针运算符重载，返回指向的数据指针
             */
            const TDataType* operator->() const {
                return mDataPointer;
            }

            /**
             * @brief 相等运算符重载
             */
            bool operator==(const GlobalPointer& rOther){
                #ifdef QUEST_USING_MPI
                    return this->get() == rOther.get() && this->GetRank() == rOther.GetRank();
                #else
                    return this->get() == rOther.get();
                #endif
            }

            /**
             * @brief 获取数据指针
             */
            TDataType* get(){
                return mDataPointer;
            }

            /**
             * @brief 获取数据指针
             */
            const TDataType* get() const{
                return mDataPointer;
            }

            /**
             * @brief 获取线程号
             */
            int GetRank() const{
                #ifdef QUEST_USING_MPI
                    return mRank;
                #else
                    return 0;
                #endif
            }

            /**
             * @brief 设置线程号
             */
            void SetRank(const int Rank){
                #ifdef QUEST_USING_MPI
                    this->mRank = Rank;
                #endif
            }

            /**
             * @brief 将当前对象的数据（即 GlobalPointer 对象）保存到给定的内存缓冲区 buffer 中
             */
            void save(char* buffer) const{
                memcpy(buffer,this,sizeof(GlobalPointer));
            }

            /**
             * @brief 从给定的内存缓冲区 buffer 中加载数据到当前对象中
             */
            void load(char* buffer){
                memcpy(this,buffer,sizeof(GlobalPointer));
            }

            std::string Info() const{
                std::stringstream buffer;
                buffer << "GlobalPointer";
                return buffer.str();
            }

            void PrintInfo(std::ostream& rOstream) const{
                rOstream << Info();
            }

            void PrintData(std::ostream& rOstream) const{
                rOstream << "GlobalPointer from Rank: " << GetRank() << " contains: \n";
                mDataPointer->PrintData(rOstream);
            }

        protected:

        private:
            friend class Serializer;

            void save(Serializer& rSerializer) const{
                if(rSerializer.Is(Serializer::SHALLOW_GLOBAL_POINTER_SERIALIZATION)){
                    rSerializer.save("D",reinterpret_cast<std::size_t>(mDataPointer));
                } else {
                    rSerializer.save("D",mDataPointer);
                }

                #ifdef QUEST_USING_MPI
                    rSerializer.save("R",mRank);
                #endif
            }

            void load(Serializer& rSerializer){
                if(rSerializer.Is(Serializer::SHALLOW_GLOBAL_POINTER_SERIALIZATION)){
                    std::size_t data_pointer_size;
                    rSerializer.load("D",data_pointer_size);
                    mDataPointer = reinterpret_cast<TDataType*>(data_pointer_size);
                } else {
                    rSerializer.load("D",mDataPointer);
                }

                #ifdef QUEST_USING_MPI
                    rSerializer.load("R",mRank);
                #endif
            }

        private:
            /**
             * @brief 指向的数据指针
             */
            TDataType* mDataPointer;

            #ifdef QUEST_USING_MPI
                /**
                 * @brief 线程号
                 */
                int mRank;
            #endif
    };

    template<typename TDataType>
    struct GlobalPointerHasher{
        std::size_t operator()(const GlobalPointer<TDataType>& rPointer) const{
            std::size_t seed = 0;
            HashCombine(seed,&(*rPointer));
            #ifdef QUEST_USING_MPI
                HashCombine(seed,rPointer.GetRank());
            #endif  
            return seed;
        }
    };

    template<typename TDataType>
    struct GlobalPointerComparor{
        bool operator()(const GlobalPointer<TDataType>& rPointer1, const GlobalPointer<TDataType>& rPointer2) const{
            #ifdef QUEST_USING_MPI
                return (&(*rPointer1) == &(*rPointer2) && rPointer1.GetRank() == rPointer2.GetRank());
            #else
                return (&(*rPointer1) == &(*rPointer2));
            #endif
        }
    };

    template<typename TDataType>
    struct GlobalPointerCompare{
        bool operator()(const GlobalPointer<TDataType>& rPointer1, const GlobalPointer<TDataType>& rPointer2) const{
            #ifdef QUEST_USING_MPI
                return (rPointer1.GetRank() == rPointer2.GetRank()) ? (rPointer1.get() < rPointer2.get()) : (rPointer1.GetRank() < rPointer2.GetRank());
            #else
                return (rPointer1.get() < rPointer2.get());
            #endif
        }
    };

    template<typename TDataType>
    inline std::istream& operator>>(std::istream& rIstream, GlobalPointer<TDataType>& rPointer){
        return rIstream;
    }

    template<typename TDataType>
    inline std::ostream& operator<<(std::ostream& rOstream, const GlobalPointer<TDataType>& rPointer){
        rOstream << reinterpret_cast<const std::size_t>(&*rPointer) << " : " << rPointer.GetRank();
        return rOstream;
    }

} // namespace Quest


#endif //QUEST_GLOBAL_POINTER_HPP