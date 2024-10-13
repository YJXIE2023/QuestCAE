/*----------------------------------
用于管理和访问分布在不同进程中的数据
----------------------------------*/

#ifndef QUEST_GLOBAL_POINTER_HPP
#define QUEST_GLOBAL_POINTER_HPP

// 系统头文件
#include <iostream>

// 项目头文件
#include "includes/define.hpp"
#include "includes/serializer.hpp"
#include "includes/key_hash.hpp"

namespace Quest{

    template<typename TDataType>
    class GlobalPointer{
        public:
            using element_type = TDataType;

            GlobalPointer(){
                mDataPointer = nullptr;
                #ifdef QUEST_USING_MPI
                    this->mRank = 0;
                #endif
            }

            GlobalPointer(TDataType Data) = delete;

            GlobalPointer(TDataType* DataPointer, int Rank = 0): mDataPointer(DataPointer)
            #ifdef QUEST_USING_MPI
                , mRank(Rank)
            #endif
            {
                #ifdef QUEST_USING_MPI
                    QUEST_DEBUG_ERROR_IF(Rank != 0) << "Trying to construct a global pointer with rank different from zero when quest is not in MPI mode " << std::endl;
                #endif
            }

            GlobalPointer(Quest::shared_ptr<TDataType> DataPointer, int Rank = 0): mDataPointer(DataPointer.get())
            #ifdef QUEST_USING_MPI
                , mRank(Rank)
            #endif
            {
                #ifdef QUEST_USING_MPI
                    QUEST_DEBUG_ERROR_IF(Rank != 0) << "Trying to construct a global pointer with rank different from zero when quest is not in MPI mode " << std::endl;
                #endif
            }

            GlobalPointer(Quest::intrusive_ptr<TDataType> DataType, int Rank = 0): mDataPointer(DataType.get())
            #ifdef QUEST_USING_MPI
                , mRank(Rank)
            #endif
            {
                #ifdef QUEST_USING_MPI
                    QUEST_DEBUG_ERROR_IF(Rank != 0) << "Trying to construct a global pointer with rank different from zero when quest is not in MPI mode " << std::endl;
                #endif
            }

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

            GlobalPointer(const GlobalPointer<TDataType>& rOther): mDataPointer(rOther.mDataPointer)
            #ifdef QUEST_USING_MPI
                , mRank(rOther.mRank)
            #endif
            {}

            GlobalPointer(GlobalPointer<TDataType>&& rOther): mDataPointer(std::move(rOther.mDataPointer))
            #ifdef QUEST_USING_MPI
                , mRank(std::move(rOther.mRank))
            #endif
            {}


            ~GlobalPointer() = default;

            GlobalPointer<TDataType>& operator=(const GlobalPointer<TDataType>& rOther){
                mDataPointer = rOther.mDataPointer;
                #ifdef QUEST_USING_MPI
                    mRank = rOther.mRank;
                #endif
                return *this;
            }

            TDataType& operator*() {
                return *mDataPointer;
            }

            const TDataType& operator*() const {
                return *mDataPointer;
            }

            TDataType* operator->() {
                return mDataPointer;
            }

            const TDataType* operator->() const {
                return mDataPointer;
            }

            bool operator==(const GlobalPointer& rOther){
                #ifdef QUEST_USING_MPI
                    return this->get() == rOther.get() && this->GetRank() == rOther.GetRank();
                #else
                    return this->get() == rOther.get();
                #endif
            }

            TDataType* get(){
                return mDataPointer;
            }

            const TDataType* get() const{
                return mDataPointer;
            }

            int GetRank() const{
                #ifdef QUEST_USING_MPI
                    return mRank;
                #else
                    return 0;
                #endif
            }

            void SetRank(const int Rank){
                #ifdef QUEST_USING_MPI
                    this->mRank = Rank;
                #endif
            }

            void save(char* buffer) const{
                memcpy(buffer,this,sizeof(GlobalPointer));
            }

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
            TDataType* mDataPointer;

            #ifdef QUEST_USING_MPI
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