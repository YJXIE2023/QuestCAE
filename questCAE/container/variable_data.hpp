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

    /**
     * @brief 变量抽象类
     * @details 存储变量名称与其键值编号,为所有变量及其分量提供基本信息存储和操作的最低抽象层
     */
    class QUEST_API(QUEST_CORE) VariableData{
        public:
            QUEST_CLASS_POINTER_DEFINITION(VariableData);
            using KeyType = std::size_t;

            /**
             * @brief 复制构造函数
             */
            VariableData(const VariableData& rOtherVariable);


            /**
             * @brief 析构函数
             */
            virtual ~VariableData() {}


            /**
             * @brief 支持将VariableData对象转换为std::size_t类型
             */
            operator size_t() const { return mKey; }

            
            /**
             * @brief 克隆函数
             */
            virtual void* Clone(const void* pSource) const;

            
            /**
             * @brief 拷贝函数
             * @details 该函数的作用是将原指针所指向的变量数据拷贝到新指针所指向的内存中，并返回原始指针
             */
            virtual void* Copy(const void* pSource, void* pDestination) const;

            
            /**
             * @brief 将一个变量的值赋值给另一个已有的变量
             * @details Assign 不会创建一个新的对象，而是将值赋给已经存在的目标变量。
             */
            virtual void Assign(const void* pSource, void* pDestination) const;

            
            /**
             * @brief 将一个变量的值设置为0
             */
            virtual void AssignZero(void* pDestination) const;

            
            /**
             * @brief 删除一个变量类型的对象，并且释放该对象占用的内存
             */
            virtual void Delete(void* pSource) const;

            
            /**
             * @brief 销毁一个对象，但不释放其占用的内存
             */
            virtual void Destruct(void* pSource) const;

            
            /**
             * @brief 输出给定变量的完整信息，通常包括变量的地址、类型以及其值
             */
            virtual void Print(const void* pSource, std::ostream& rOstream) const;

            
            /**
             * @brief 输出变量的值
             */
            virtual void PrintData(const void* pSource, std::ostream& rOstream) const;

            
            /**
             * @brief 为该变量分配内存
             */
            virtual void Allocate(void** pData) const;

            
            /**
             * @brief 存储变量数据到序列化对象中
             */
            virtual void Save(Serializer& rSerializer, void* pData) const;

            
            /**
             * @brief 从序列化对象中读取变量数据
             */
            virtual void Load(Serializer& rSerializer, void* pData) const;


            /**
             * @brief 生成变量的哈希值
             */
            KeyType HashKey() const {
                KeyType key = mKey;
                key &= 0xFFFFFFFF00;

                return key;
            }


            /**
             * @brief 返回变量的键值
             */
            KeyType Key() const { return mKey; }


            /**
             * @brief 返回源变量的键值
             */
            KeyType SourceKey() const {
                return mpSourceVariable->mKey;
            }


            /**
             * @brief 设置变量的键值
             */
            void SetKey(KeyType NewKey);


            /**
             * @brief 获取变量的名称
             */
            const std::string& Name() const{
                return mName;
            }


            /**
             * @brief 获取变量的大小
             */
            std::size_t Size() const{
                return mSize;
            }


            /**
             * @brief 判断变量是否为分量
             */
            bool IsComponent() const{
                return mIsComponent;
            }


            /**
             * @brief 判断变量是否不为分量
             */
            bool IsNotComponent() const{
                return !mIsComponent;
            }

            
            /**
             * @brief 获取分量的索引
             */
            KeyType GetComponentIndex() const{
                constexpr KeyType first_7_bits = 127;
                return (mKey & first_7_bits);
            }


            /**
             * @brief 获取分量的源变量
             */
            const VariableData& GetSourceVariable() const{
                QUEST_ERROR_IF(mpSourceVariable == nullptr) << "No source variable is defined for this component." << std::endl;
                return *mpSourceVariable;
            }


            /**
             * @brief 返回零值数据
             */
            virtual const void* pSize() const{
                QUEST_ERROR << "Calling base class method." << std::endl;
            }

            virtual std::string Info() const;

            virtual void PrintInfo(std::ostream& rOstream) const;

            virtual void PrintData(std::ostream& rOstream) const;

            
            /**
             * @brief 生成变量的键值编号
             * @details 包含关于变量的多个关键信息（如名称、大小、是否为分量及其索引等）
             *  64           size         32-bit hash                comp. index 0
             *  |-----------|----|---------------------------------------------|-|
             */
            static KeyType GenerateKey(const std::string& Name, std::size_t Size, bool IsComponent, char ComponentIndex);


            /**
             * @brief 友元函数，用于比较两个变量是否相等
             */
            friend bool operator == (const VariableData& rFirstVariable, const VariableData& rSecondVariable){
                return rFirstVariable.mKey == rSecondVariable.mKey;
            }

        protected:
            /**
             * @brief 赋值运算符重载
             */
            VariableData& operator = (const VariableData& rOtherVariable){
                mName = rOtherVariable.mName;   
                mKey = rOtherVariable.mKey;
                mSize = rOtherVariable.mSize;
                mpSourceVariable = rOtherVariable.mpSourceVariable;
                mIsComponent = rOtherVariable.mIsComponent;

                return *this;
            }


            /**
             * @brief 构造函数
             */
            VariableData(const std::string& NewName, std::size_t NewSize);


            /**
             * @brief 构造函数
             */
            VariableData(const std::string& NewName, std::size_t NewSize, const VariableData* pSourceVariable, char ComponentIndex);

            VariableData() {}

        private:
            /**
             * @brief 变量名称
             */
            std::string mName;

            /**
             * @brief 变量键值编号
             */
            KeyType mKey;

            /**
             * @bbrief 变量空间大小
             */
            std::size_t mSize;

            /**
             * @brief 指向分量的源变量的指针
             */
            const VariableData* mpSourceVariable;

            /**
             * @brief 该变量是否为分量
             */
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