#ifndef QUEST_FLAGES_HPP
#define QUEST_FLAGES_HPP

//系统头文件
#include <string>
#include <iostream>

//项目头文件
#include "includes/define.hpp"

namespace Quest{

    class Serializer;


    /**
     * @class Flags
     * @brief 用于高效存储和操作状态信息的标志
     * @details 标志用于位运算逻辑操作，为基础类
     */
    class QUEST_API(QUEST_CORE) Flags{
        public:
            QUEST_CLASS_POINTER_DEFINITION(Flags);

            #ifdef __WIN32
                typedef __int64 int64_t;
            #endif

            using BlockType = int64_t;
            using FlagType = int64_t;
            using IndexType = std::size_t;

            /**
             * @enum FlagsList
             * @brief 定义了一组标志位
             * @details Flag1表示将第一位设置为1
             */
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


            /**
             * @brief 默认构造函数
             */
            Flags():mIsDefined(BlockType()),mFlags(BlockType()){}


            /**
             * @brief 复制构造函数
             */
            Flags(const Flags& rOther):mIsDefined(rOther.mIsDefined),mFlags(rOther.mFlags){}


            /**
             * @brief 析构函数
             */
            virtual ~Flags(){}


            /**
             * @brief 创建一个新的Flags对象，并设置指定位置的值
             * @param ThisPosition 指定位置
             * @param Value 值
             */
            static Flags Create(IndexType ThisPosition, bool Value=true){
                Flags flags;
                flags.SetPosition(ThisPosition, Value);
                return flags;
            }


            /**
             * @brief 赋值运算符重载
             */
            Flags& operator=(const Flags& rOther){
                mIsDefined = rOther.mIsDefined;
                mFlags = rOther.mFlags;
                return *this;
            }


            /**
             * @brief 类型转换运算符重载
             * @details 如果没有设置任何标志位，则返回false，否则返回true
             */
            operator bool() const{
                return mFlags;
            }


            /**
             * @brief 按位取反运算符重载
             */
            Flags operator~() const{
                Flags result(*this);
                result.mFlags = ~mFlags;
                return result;
            }


            /**
             * @brief 将mFlags的值进行逻辑取反
             * @details 如果没有设置任何标志位，则返回true，否则返回false
             */
            bool operator!() const{
                Flags result(*this);
                result.mFlags = !mFlags;
                return result;
            }


            /**
             * @brief 将另一个Flags对象的值赋值给当前对象
             * @param rOther 另一个Flags对象
             */
            void AssignFlags(const Flags& rOther){
                mIsDefined = rOther.mIsDefined;
                mFlags = rOther.mFlags;
            }


            /**
             * @brief 设置标志值
             * @details 将ThisFlag的标志合并到当前对象中
             * @param ThisFlag 标志对象
             */
            void Set(const Flags ThisFlag);


            /**
             * @brief 设置标志值
             * @details 将ThisFlag的标志合并到当前对象中，并通过Value设置标志的值
             * @param ThisFlag 标志对象
             * @param Value 值
             */
            void Set(const Flags ThisFlag, bool Value);


            /**
             * @brief 将ThisFlag的标志从当前对象中移除
             */
            void Reset(const Flags ThisFlag){
                mIsDefined &= (~ThisFlag.mIsDefined);
                mFlags &= (~ThisFlag.mIsDefined);
            }


            /**
             * @brief 翻转当前对象与ThisFlag对象中相同的标志位
             */
            void Flip(const Flags ThisFlag){
                mIsDefined |= ThisFlag.mIsDefined;
                mFlags ^= (ThisFlag.mIsDefined);
            }


            /**
             * @brief 设置指定位置的值
             * @param Position 指定位置
             * @param Value 值
             */
            void SetPosition(IndexType Position, const bool Value=true){
                mIsDefined |= (BlockType(1) << Position);
                mFlags &= ~(BlockType(1) << Position);
                mFlags |= (BlockType(Value) << Position);
            }


            /**
             * @brief 获取指定位置的值
             * @param Position 指定位置
             */
            bool GetPosition(IndexType Position) const{
                return (mFlags & (BlockType(1) << Position));
            }


            /**
             * @brief 翻转指定位置的值
             */
            void FlipPosition(IndexType Position){
                mIsDefined |= (BlockType(1) << Position);
                mFlags ^= (BlockType(1) << Position);
            }


            /**
             * @brief 清除指定位置的值
             */
            void ClearPosition(IndexType Position){
                mIsDefined &= ~(BlockType(1) << Position);
                mFlags &= ~(BlockType(1) << Position);
            }


            /**
             * @brief 清除所有标志位
             */
            void Clear(){
                mIsDefined = BlockType();
                mFlags = BlockType();
            }


            /**
             * @brief 返回一个新的Flags对象，其值与当前对象相反
             */
            Flags AsFalse() const{
                Flags this_flag_false(*this);
                this_flag_false.mFlags = !((*this).mFlags);
                return this_flag_false;
            }


            /**
             * @brief 将所有标志设置为已定义，并将所有值设置为false
             */
            static const Flags AllDefined(){
                return Flags(~0, 0);
            }


            /**
             * @brief 将所有标志设置为已定义，并将所有值设置为true
             */
            static const Flags AllTrue(){
                return Flags(~0, ~0);
            }


            /**
             * @breif 判断当前对象是否包含指定标志位
             * @param ThisFlag 需要判断的标志位
             */
            bool Is(const Flags& rOther) const{
                return (mFlags & rOther.mFlags) | ((rOther.mIsDefined ^ rOther.mFlags) & (~mFlags));
            }


            /**
             * @brief 判断当前对象中指定的标志位是否已定义
             * @param ThisFlag 需要判断的标志位
             */
            bool IsDefined(const Flags& rOther) const{
                return (mIsDefined & rOther.mIsDefined);
            }


            /**
             * @brief 判断当前对象中指定的标志位是否为false
             * @param ThisFlag 需要判断的标志位
             */
            bool IsNot(const Flags& rOther) const{
                return !((mFlags & rOther.mFlags) | ((rOther.mIsDefined ^ rOther.mFlags) & (~mFlags)));
            }


            /**
             * @brief 判断当前对象中指定的标志位是否未定义
             * @param ThisFlag 需要判断的标志位
             */
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


            /**
             * @brief 重载运算符==
             */
            friend bool QUEST_API(QUEST_CORE) operator==(const Flags& rLeft, const Flags& rRight);


            /**
             * @brief 重载运算符!=
             */
            friend bool QUEST_API(QUEST_CORE) operator!=(const Flags& rLeft, const Flags& rRight);


            /**
             * @brief 重载运算符|
             * @details 按位或运算符，返回两个Flags对象的并集
             */
            friend Flags QUEST_API(QUEST_CORE) operator|(const Flags& rLeft, const Flags& rRight);


            /**
             * @brief 重载运算符&
             * @details 按位与运算符，返回两个Flags对象的交集
             */
            friend Flags QUEST_API(QUEST_CORE) operator&(const Flags& rLeft, const Flags& rRight);


            /**
             * @brief 重载运算符 |=
             * @details 将Other的标志合并到当前对象中
             */
            const Flags& operator|=(const Flags& Other);


            /**
             * @brief 重载运算符 &=
             * @details 获得两个Flags对象的交集 
             */
            const Flags& operator&=(const Flags& Other);


        protected:

        private:
            friend class MPIDataCommunicator;

            /**
             * @brief 获取已定义的标志位
             */
            BlockType GetDefined() const;


            /**
             * @brief 设置已定义的标志位
             */
            void SetDefined(const BlockType& rDefined);


            /**
             * @brief 获取所有标志位的值
             */
            BlockType GetFlags() const;


            /**
             * @brief 设置标志位
             */
            void SetFlags(const BlockType& rValues);

            friend class Serializer;

            virtual void save(Serializer& rSerializer) const;

            virtual void load(Serializer& rSerializer);

            Flags(BlockType DefinedFlags, BlockType SetFlags):mIsDefined(DefinedFlags),mFlags(SetFlags){};

        private:
            /**
             * @brief 64位掩码，管理某些属性是否被定义
             */
            BlockType mIsDefined;

            /**
             * @brief 64位掩码，用于存储各种标志位的值
             */
            BlockType mFlags;
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