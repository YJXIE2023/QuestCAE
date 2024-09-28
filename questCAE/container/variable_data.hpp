/*-----------------------------------------------------
专门用于处理 变量 和 变量组件，它包含了变量的通用数据
管理变量的名称、键值（key），以及为变量的组件提供适配器类型
-----------------------------------------------------*/

#ifndef QUEST_VARIABLE_DATA_HPP
#define QUEST_VARIABLE_DATA_HPP

// 系统头文件
#include <string>
#include <iostream>
#include <cstddef>

// 项目头文件
#include "includes/define.hpp"
#include "utilities/counter.hpp"
#include "includes/serializer.hpp"

namespace Quest{

    class QUEST_API VariableData{
        public:
            QUEST_CLASS_POINTER_DEFINITION(VariableData);
            using KeyType = std::size_t;

            VariableData(const VariableData& rOtherVariable);

            virtual ~VariableData() {}

            operator size_t() const { return mKey; }

            // 通过调用对象的拷贝构造函数来克隆对象
            virtual void* Clone(const void* pSource) const;

            // 将一个对象的数据复制到另一个对象中
            // 接收源对象的指针和目标对象的指针，并将源对象的数据复制到目标对象中
            virtual void* Copy(const void* pSource, void* pDestination) const;

            // 将源对象的值赋给目标对象，而不创建新对象
            virtual void Assign(const void* pSource, void* pDestination) const;

            // 将目标对象的值重置为零，而不依赖源对象的值
            // 专门用于初始化或重置对象，尤其在处理数组或内存值时非常有用
            virtual void AssignZero(void* pDestination) const;

            // 从内存上删除对象
            virtual void Delete(void* pSource) const;

            // 销毁一个对象，但不会释放该对象所占用的内存
            virtual void Destruct(void* pSource) const;

            // 输出给定变量的信息
            virtual void Print(const void* pSource, std::ostream& rOstream) const;

            // 输出给定变量的值
            virtual void PrintData(const void* pSource, std::ostream& rOstream) const;

            // 为变量数据分配内存
            virtual void Allocate(void** pData) const;

            // 将类的数据序列化到一个流中
            virtual void Save(Serializer& rSerializer, void* pData) const;

            // 从流中反序列化类的数据
            virtual void Load(Serializer& rSerializer, void* pData) const;

            KeyType HashKey() const {
                KeyType key = mKey;
                key &= 0xFFFFFFFF00;

                return key;
            }

            KeyType Key() const { return mKey; }

            KeyType SourceKey() const {
                return mpSourceVariable->mKey;
            }

            void SetKey(KeyType NewKey);

            const std::string& Name() const{
                return mName;
            }

            std::size_t Size() const{
                return mSize;
            }

            bool IsComponent() const{
                return mIsComponent;
            }

            bool IsNotComponent() const{
                return !mIsComponent;
            }

            // 从 mKey 中提取组件的索引
            KeyType GetComponentIndex() const{
                constexpr KeyType first_7_bits = 127;
                return (mKey & first_7_bits);
            }

            const VariableData& GetSourceVariable() const{
                QUEST_ERROR_IF(mpSourceVariable == nullptr) << "No source variable is defined for this component." << std::endl;
                return *mpSourceVariable;
            }

            virtual const void* pSize() const{
                QUEST_ERROR << "Calling base class method." << std::endl;
            }

            virtual std::string Info() const;

            virtual void PrintInfo(std::ostream& rOstream) const;

            virtual void PrintData(std::ostream& rOstream) const;

            // 生成一个唯一的键，该键结合了变量的名称、大小以及一些标志信息
            static KeyType GenerateKey(const std::string& Name, std::size_t Size, bool IsComponent, char ComponentIndex);

            friend bool operator == (const VariableData& rFirstVariable, const VariableData& rSecondVariable){
                return rFirstVariable.mKey == rSecondVariable.mKey;
            }

        protected:
            VariableData& operator = (const VariableData& rOtherVariable){
                mName = rOtherVariable.mName;   
                mKey = rOtherVariable.mKey;
                mSize = rOtherVariable.mSize;
                mpSourceVariable = rOtherVariable.mpSourceVariable;
                mIsComponent = rOtherVariable.mIsComponent;

                return *this;
            }

            VariableData(const std::string& NewName, std::size_t NewSize);

            VariableData(const std::string& NewName, std::size_t NewSize, const VariableData* pSourceVariable, char ComponentIndex);

            VariableData() {}

        private:
            std::string mName;
            KeyType mKey;
            std::size_t mSize;

            const VariableData* mpSourceVariable;

            bool mIsComponent;

            friend class Serializer;

            virtual void save(Serializer& rSerializer) const;

            virtual void load(Serializer& rSerializer);

    };

    inline std::istream& operator >> (std::istream& rIstream, VariableData& rThis){}

    inline std::ostream& operator << (std::ostream& rOstream, const VariableData& rThis){
        rThis.PrintInfo(rOstream);
        rThis.PrintData(rOstream);

        return rOstream;
    }

}

#endif // VARIABLE_DATA_HPP