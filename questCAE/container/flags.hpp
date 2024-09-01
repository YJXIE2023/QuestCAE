/*---------------------------------------------------------
标志类
用于标记特定的边界条件、求解器状态和节点或元素的特殊属性
----------------------------------------------------------*/

#ifndef FLAGES_HPP
#define FLAGES_HPP

//系统头文件
#include <string>
#include <iostream>

//项目头文件
#include "includes/define.hpp"

namespace Quest{

    class Serializer;

    class QUEST_API Flags{
        public:
            
            QUEST_CLASS_POINTER_DEFINITION(Flags);

            #ifdef __WIN32
                typedef __int64 int64_t;
            #endif

            using BlockType = int64_t;
            using FlagType = int64_t;
            using IndexType = std::size_t;

            enum FlagsList{
                Flag0 = BlockType(1),
                Flag1 = BlockType(1) << 1,
                Flag2 = BlockType(1) << 2,
                Flag3 = BlockType(1) << 3,
                Flag4 = BlockType(1) << 4,
                Flag5 = BlockType(1) << 5,
                Flag6 = BlockType(1) << 6,
                Flag7 = BlockType(1) << 7,
                Flag8 = BlockType(1) << 8,
                Flag9 = BlockType(1) << 9,
                Flag10 = BlockType(1) << 10,
                Flag11 = BlockType(1) << 11,
                Flag12 = BlockType(1) << 12,
                Flag13 = BlockType(1) << 13,
                Flag14 = BlockType(1) << 14,
                Flag15 = BlockType(1) << 15,
                Flag16 = BlockType(1) << 16,
                Flag17 = BlockType(1) << 17,
                Flag18 = BlockType(1) << 18,
                Flag19 = BlockType(1) << 19,
                Flag20 = BlockType(1) << 20,
                Flag21 = BlockType(1) << 21,
                Flag22 = BlockType(1) << 22,
                Flag23 = BlockType(1) << 23,
                Flag24 = BlockType(1) << 24,
                Flag25 = BlockType(1) << 25,
                Flag26 = BlockType(1) << 26,
                Flag27 = BlockType(1) << 27,
                Flag28 = BlockType(1) << 28,
                Flag29 = BlockType(1) << 29,
                Flag30 = BlockType(1) << 30,
            };

            Flags():mIsDefined(BlockType()),mFlags(BlockType()){}

            Flags(const Flags& rOther):mIsDefined(rOther.mIsDefined),mFlags(rOther.mFlags){}

            virtual ~Flags(){}

            static Flags Creat(IndexType ThisPosition, bool Value=true){
                Flags flags;
                flags.SetPosition(ThisPosition, Value);
                return flags;
            }

            Flags& operator=(const Flags& rOther){
                mIsDefined = rOther.mIsDefined;
                mFlags = rOther.mFlags;
                return *this;
            }

            operator bool() const{
                return mFlags;
            }

            Flags operator~() const{
                Flags result(*this);
                result.mFlags = ~mFlags;
                return result;
            }

            bool operator!() const{
                Flags result(*this);
                result.mFlags = !mFlags;
                return result;
            }

            void AssignFlags(const Flags& rOther){
                mIsDefined = rOther.mIsDefined;
                mFlags = rOther.mFlags;
            }

            void Set(const Flags ThisFlag);

            void Set(const Flags ThisFlag, bool Vlaue);

            void Reset(const Flags ThisFlag){
                mIsDefined &= (~ThisFlag.mIsDefined);
                mFlags &= (~ThisFlag.mIsDefined);
            }

            void Flip(const Flags ThisFlag){
                mIsDefined |= ThisFlag.mIsDefined;
                mFlags ^= (ThisFlag.mIsDefined);
            }

            void SetPosition(IndexType Position, const bool Value=true){
                mIsDefined |= (BlockType(1) << Position);
                mFlags &= ~(BlockType(1) << Position);
                mFlags |= (BlockType(Value) << Position);
            }

            bool GetPosition(IndexType Position) const{
                return (mFlags & (BlockType(1) << Position));
            }

            void FlipPosition(IndexType Position){
                mIsDefined |= (BlockType(1) << Position);
                mFlags ^= (BlockType(1) << Position);
            }

            void ClearPosition(IndexType Position){
                mIsDefined &= ~(BlockType(1) << Position);
                mFlags &= ~(BlockType(1) << Position);
            }

            void Clear(){
                mIsDefined = BlockType();
                mFlags = BlockType();
            }

            Flags AsFalse() const{
                Flags this_flag_false(*this);
                this_flag_false.mFlags = !((*this).mFlags);
                return this_flag_false;
            }

            static const Flags AllDefined(){
                return Flags(~0, 0);
            }

            static const Flags AllTrue(){
                return Flags(~0, ~0);
            }

            bool Is(const Flags& rOther) const{
                return (mFlags & rOther.mFlags) | ((rOther.mIsDefined ^ rOther.mFlags) & (~mFlags));
            }

            bool IsDefined(const Flags& rOther) const{
                return (mIsDefined & rOther.mIsDefined);
            }

            bool IsNot(const Flags& rOther) const{
                return !((mFlags & rOther.mFlags) | ((rOther.mIsDefined ^ rOther.mFlags) & (~mFlags)));
            }

            bool IsNotDefined(const Flags& rOther) const{
                return !((mIsDefined & rOther.mIsDefined));
            }

            virtual std::string Info() const{
                std::stringstream buffer;
                buffer << "Flags";
                return buffer.str();
            }

            virtual void PrintInfo(std::ostream& rOStream) const{
                rOStream << "Flags";
            }

            virtual void PrintData(std::ostream& rOStream) const {
                for(std::size_t i = sizeof(BlockType)*8; i > 0; --i)
                    rOStream << bool(mFlags & (BlockType(1) << i));
            }

            friend bool QUEST_API operator==(const Flags& rLeft, const Flags& rRight);

            friend bool QUEST_API operator!=(const Flags& rLeft, const Flags& rRight);

            friend Flags QUEST_API operator|(const Flags& rLeft, const Flags& rRight);

            friend Flags QUEST_API operator&(const Flags& rLeft, const Flags& rRight);

            const Flags& operator|=(const Flags& Other);

            const Flags& operator&=(const Flags& Other);


        protected:

        private:
            BlockType mIsDefined;
            BlockType mFlags;
            
            friend class MPIDataCommunicator;

            BlockType GetDefined() const;

            void SetDefined(const BlockType& rDefined);

            BlockType GetFlags() const;

            void SetFlags(const BlockType& rValues);

            friend class Serializer;

            virtual void save(Serializer& rSerializer) const;

            virtual void load(Serializer& rSerializer);

            Flags(BlockType DefinedFlags, BlockType SetFlags):mIsDefined(DefinedFlags),mFlags(SetFlags){};
    };

    inline std::istream& operator>>(std::istream& rIStream, Quest::Flags& rFlags){
        return rIStream;
    }

    inline std::ostream& operator<<(std::ostream& rOStream, const Quest::Flags& rFlags){
        rFlags.PrintInfo(rOStream);
        rOStream<<" : ";
        rFlags.PrintData(rOStream);
        return rOStream;
    }


} // namespace Quest


#endif // FLAGES_HPP