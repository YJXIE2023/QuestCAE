/*-------------------------------------
variable_data.hpp类的实现
-------------------------------------*/

// 项目头文件
#include "includes/define.hpp"
#include "container/variable_data.hpp"
#include "IO/logger.hpp"
#include "includes/fnv_1a_hash.hpp"

namespace Quest{

    VariableData::VariableData(const std::string& NewName, std::size_t NewSize)
        : mName(NewName), mKey(0), mSize(NewSize), mpSourceVariable(this), mIsComponent(false)
    {
        mKey = GenerateKey(mName, mSize, mIsComponent, 0);
    }

    VariableData::VariableData(const std::string& NewName, std::size_t NewSize, const VariableData* pSourceVariable, char ComponentIndex)
        : mName(NewName), mKey(0), mSize(NewSize), mpSourceVariable(pSourceVariable), mIsComponent(true)
    {
        mKey = GenerateKey(mpSourceVariable->Name(), mSize, mIsComponent, ComponentIndex);
    }

    VariableData::VariableData(const VariableData& rOtherVariable)
        : mName(rOtherVariable.mName), mKey(rOtherVariable.mKey), mSize(rOtherVariable.mSize), mpSourceVariable(rOtherVariable.mpSourceVariable), mIsComponent(rOtherVariable.mIsComponent){}
    
    VariableData::KeyType VariableData::GenerateKey(const std::string& Name, std::size_t Size, bool IsComponent, char ComponentIndex){
        std::uint64_t key = Size;
        key = key << 32;
        key += FNV1a32Hash::CalculateHash(Name.c_str());

        key <<= 1;
        key += IsComponent;
        key <<= 7;
        key += ComponentIndex;

        return key;
    }

    void* VariableData::Clone(const void* pData) const{
        return 0;
    }

    void* VariableData::Copy(const void* pData, void* pDest) const{
        return 0;
    }

    void VariableData::Assign(const void* pData, void* pDest) const{}

    void VariableData::AssignZero(void* pDestination) const {}

    void VariableData::Destruct(void* pData) const{}

    void VariableData::Delete(void* pData) const{}

    void VariableData::Print(const void* pData, std::ostream& rOStream) const{}

    void VariableData::PrintData(const void* pSource, std::ostream& rOStream) const{}

    void VariableData::Allocate(void** pData) const{}

    void VariableData::Save(Serializer& rSerializer, void* pData) const{}

    void VariableData::Load(Serializer& rSerializer, void* pData){}

    void VariableData::SetKey(KeyType NewKey){
        mKey = NewKey;
    }

    std::string VariableData::Info() const{
        std::stringstream buffer;
        buffer << mName << " variable data" <<" #" << static_cast<unsigned int>(mKey);
        return buffer.str();
    }

    void VariableData::PrintInfo(std::ostream& rOstream) const{
        rOstream << mName << " variable data";
    }

    void VariableData::PrintData(std::ostream& rOstream) const {
        rOstream <<" #" << static_cast<unsigned int>(mKey);
    }

    void VariableData::save(Serializer& rSerializer) const{
        rSerializer.save("name", mName);
        rSerializer.save("key", mKey);
        rSerializer.save("IsComponent", mIsComponent);
    }

    void VariableData::load(Serializer& rSerializer){
        rSerializer.load("name", mName);
        rSerializer.load("key", mKey);
        rSerializer.load("IsComponent", mIsComponent);
    }

} // namespace Quest