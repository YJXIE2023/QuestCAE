/*-------------------------------------------
flags类的实现代码
---------------------------------------------*/


#include "includes/define.hpp"
#include "container/flags.hpp"
#include "includes/serializer.hpp"

namespace Quest{

    void Flags::save(Serializer& rSerializer) const{
        rSerializer.save("IsDefined", mIsDefined);
        rSerializer.save("Flags", mFlags);
    }

    void Flags::load(Serializer& rSerializer){
        rSerializer.load("IsDefined", mIsDefined);
        rSerializer.load("Flags", mFlags);
    }

    void Flags::Set(const Flags ThisFlag){
        mIsDefined |= ThisFlag.mIsDefined;
        mFlags = (mFlags &~ ThisFlag.mIsDefined) | (ThisFlag.mIsDefined & ThisFlag.mFlags);
    }

    void Flags::Set(const Flags ThisFlag, bool value){
        mIsDefined |= ThisFlag.mIsDefined;
        mFlags = (mFlags &~ ThisFlag.mIsDefined) | (ThisFlag.mIsDefined * BlockType(value));
    }

    bool operator==(const Flags& Left, const Flags& Right){
        return (Left.mFlags == Right.mFlags);
    }

    bool operator!=(const Flags& Left, const Flags& Right){
        return (Left.mFlags !=  Right.mFlags);
    }

    Flags QUEST_API(QUEST_CORE) operator|(const Flags& Left, const Flags& Right){
        Flags result(Left);
        result |= Right;
        return result;
    }

    Flags QUEST_API(QUEST_CORE) operator&(const Flags& Left, const Flags& Right){
        Flags result(Left);
        result &= Right;
        return result;
    }

    const Flags& Flags::operator|=(const Flags& other){
        mIsDefined |= other.mIsDefined;
        mFlags |= other.mFlags;
        return *this;
    }

    const Flags& Flags::operator&=(const Flags& other){
        mIsDefined |= other.mIsDefined;
        mFlags &= other.mFlags;
        return *this;
    }

    Flags::BlockType Flags::GetDefined() const{
        return mIsDefined;
    }

    void Flags::SetDefined(const BlockType& rDefined){
        mIsDefined = rDefined;
    }

    Flags::BlockType Flags::GetFlags() const{
        return mFlags;
    }

    void Flags::SetFlags(const BlockType& rValue){
        mFlags = rValue;
    }

}