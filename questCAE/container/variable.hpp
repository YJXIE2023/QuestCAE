#ifndef QUEST_VARIABLE_HPP
#define QUAST_VARIABLE_HPP

// 系统头文件
#include <string>
#include <iostream>

// 项目头文件
#include "includes/define.hpp"
#include "includes/registry.hpp"
#include "variable_data.hpp"
#include "utilities/stl_vector_io.hpp"

namespace Quest{

    /**
     * @brief 变量类
     * @details 继承至VariableData类，包含一个Variable<TDataType>类型的静态实例对象，同时包含0类型的容器和指向变量时间导数的指针
     * 类似单例模式，对于Variable<type1>类型的所有实例，其访问同一个Variable<type1>静态实例对象
     * @tparam TDataType 变量的数据类型
     */
    template<typename TDataType>
    class Variable:public VariableData{
        public:
            QUEST_CLASS_POINTER_DEFINITION(Variable);
            using Type = TDataType;
            using KeyType = VariableData::KeyType;
            using VariableType = Variable<TDataType>;


            /**
             * @brief 构造函数
             * @details 构造函数中默认将变量添加到注册表中
             */
            explicit Variable(
                const std::string& NewName,
                const TDataType Zero = TDataType(),
                const VariableType* pTimeDerivativeData = nullptr
            ): VariableData(NewName,sizeof(TDataType)), mZero(Zero), mpTimeDerivativeData(pTimeDerivativeData){
                RegisterThisVariable():
            }


            /**
             * @brief 构造函数
             */
            explicit Variable(
                const std::string& NewName,
                const VariableType* pTimeDervativeVariable
            ):VariableData(NewName,sizeof(TDataType)), mZero(TDataType()), mpTimeDerivativeData(pTimeDervativeVariable){
                RegisterThisVariable();
            }


            /**
             * @brief 构造函数
             */
            template<typename TSourceVariableType>
            explicit Variable(
                const std::string& NewName,
                TSourceVariableType* pSourceVariable,
                char ComponentIndex,
                const TDataType Zero = TDataType()
            ):VariableData(NewName,sizeof(TDataType),pSourceVariable,ComponentIndex), mZero(Zero){
                RegisterThisVariable();
            }


            /**
             * @brief 构造函数
             */
            template<typename TSourceVariableType>
            explicit Variable(
                const std::string& NewName,
                TSourceVariableType* pSourceVariable,
                char ComponentIndex,
                const VariableType* pTimeDervativeVariable
                const TDataType Zero = TDataType()
            ):VariableData(NewName,sizeof(TDataType),pSourceVariable,ComponentIndex), mZero(Zero), mpTimeDerivativeData(pTimeDervativeVariable){
                RegisterThisVariable();
            }


            /**
             * @brief 构造函数
             */
            explicit Variable(const VariableType& rOther):VariableData(rOther), mZero(rOther.mZero), mpTimeDerivativeData(rOther.mpTimeDerivativeData){}


            /**
             * @brief 析构函数
             */
            ~Variable() override {}


            /**
             * @brief 赋值运算符重载
             */
            VariableType& operator = (const VariableType& rOther) = delete;


            /**
             * @biref 重载的克隆函数
             */
            void* Clone(const void* pSource) const override{
                return new TDataType(*static_cast<const TDataType*>(pSource));
            }


            /**
             * @brief 重载基类的复制函数
             */
            void* Copy(const void* pSource, void* pDestination) const override{
                return new(pDestination) TDataType(*static_cast<const TDataType*>(pSource));
            }


            /**
             * @brief 重载基类的Assign函数
             */
            void Assign(const void* pSource, void* pDestination) const override{
                *static_cast<TDataType*>(pDestination) = *static_cast<const TDataType*>(pSource);
            }


            /**
             * @brief 重载基类的AssignZero函数
             */
            void AssignZero(void* pDestination) const override{
                new (pDestination) TDataType(mZero);
            }


            /**
             * @brief 重载基类的Delete函数
             */
            void Delete(void* pSource) const override{
                delete static_cast<TDataType*>(pSource);
            }


            /**
             * @brief 重载基类的Destruct函数
             */
            void Destruct(void* pSource) const override{
                static_cast<TDataType*>(pSource)->~TDataType();
            }


            /**
             * @brief 重载基类的Print函数
             */
            void Print(const void* pSource, std::ostream& rOstream) const override{
                if(IsComponent()){
                    rOstream << "Component of " << GetSourceVariable().Name() << " variable: " << *static_cast<const TDataType*>(pSource);
                } else {
                    rOstream << Name() << " : " << *static_cast<const TDataType*>(pSource);
                }
            }


            /**
             * @brief 重载基类的PrintData函数
             */
            void PrintData(const void* pSource, std::ostream& rOstream) const override{
                rOstream << *static_cast<const TDataType*>(pSource);
            }


            /**
             * @brief 重载基类的Save函数
             */
            void Save(Serializer& rSerializer, void* pData) const override{
                rSerializer.Save("Data",*static_cast<TDataType*>(pData));
            }


            /**
             * @brief 重载基类的Allocate函数
             */
            void Allocate(void** pData) const override{
                *pData = new TDataType;
            }


            /**
             * @brief 重载基类的Load函数
             */
            void Load(Serializer& rSerializer, void* pData) const override{
                rSerializer.Load("Data",*static_cast<TDataType*>(pData));
            }


            /**
             * @brief 返回该变量的静态实例对象
             */
            static const VariableType& StaticObject(){
                const static Variable<TDataType> static_object("NONE");
                return static_object;
            }


            /**
             * @brief 获取变量的值
             * @param pData 指向节点数据的指针
             */
            TDataType& GetValue(void* pData) const{
                return GetValueByIndex(static_cast<TDataType*>(pData),GetComponentIndex());
            }


            /**
             * @brief 获取变量的值
             * @param pData 指向节点数据的指针
             */
            const TDataType& GetValue(const void* pData) const{
                return GetValueByIndex(static_cast<const TDataType*>(pData),GetComponentIndex());
            }


            /**
             * @brief 获取变量时间导数
             */
            const VariableType& GetTimeDerivative() const{
                QUEST_DEBUG_ERROR_IF(mpTimeDerivativeData == nullptr) << "Time derivative for Variable \"" << Name() << "\" was not assigned" << std::endl;
                return *mpTimeDerivativeData;
            }


            /**
             * @brief 获取0变量（VariableData类型）
             */
            const TDataType& Zero() const{
                return mZero;
            }


            /**
             * @biref 获取0变量的指针
             */
            const void* pZero() const override{
                return &mZero;
            }

            std::string Info() const override{
                std::stringstream buffer;
                buffer << Name() << " Variable #" << static_cast<unsigned int>(Key());
                if(IsComponent()){
                    buffer << Name() << " variable #" << static_cast<unsigned int>(Key()) << " component " << GetComponentIndex() << " of " << GetSourceVariable().Name();
                } else {
                    buffer << Name() << " varibale #" << static_cast<unsigned int>(Key());
                }
                return buffer.str();
            }

            void PrintInfo(std::ostream& rOstream) const override{
                rOstream << Info();
            }

            void PrintData(std::ostream& rOstream) const override{
                VariableData::PrintData(rOstream);
            }

        protected:

        private:
            /**
             * @brief 默认构造函数
             */
            Variable() {}


            /**
             * @brief 通过索引获取实际值
             * @param pValue 指向VariableData类型的指针
             * @param index 分量索引号
             */
            template<typename TValueType>
            TDataType& GetValueByIndex(TDataType* pValue, std::size_t index) const{
                return *static_cast<TDataType*>(pValue + index);
            }


            /**
             * @brief 通过索引获取实际值
             * @param pValue 指向VariableData类型的指针
             * @param index 分量索引号
             */
            template<typename TValueType>
            const TDataType& GetValueByIndex(const TDataType* pValue, std::size_t index) const{
                return *static_cast<const TDataType*>(pValue + index);
            }


            /**
             * @brief 注册该变量到注册表中
             */
            void RegisterThisVariable(){
                std::string variable_path = "variables.all." + Name();
                if(!Registry::HasItem(variable_path)){
                    Registry::AddItem<VariableType>(variable_path, *this);
                }
            }

            friend class Serializer;

            void save(Serializer& rSerializer) const override{
                QUEST_SERIALIZE_SAVE_BASE_CLASS(rSerializer, VariableData);
                rSerializer.Save("Zero",mZero);
                rSerializer.Save("TimeDerivativeData",mpTimeDerivativeData);
            }

            void load(Serializer& rSerializer) override{
                QUEST_SERIALIZE_LOAD_BASE_CLASS(rSerializer, VariableData);
                rSerializer.Load("Zero",mZero);
                rSerializer.Load("TimeDerivativeData",mpTimeDerivativeData);
            }

        private:
            /**
             * @brief Variable<TDataType>类的零值，为静态变量
             */
            static const VariableType msStaticObject;

            /**
             * @brief TDataType类型的零值
             */
            TDataType mZero;

            /**
             * @brief 指向变量时间导数的指针
             */
            const VariableType* mpTimeDerivativeData = nullptr;
    };


    /**
     * @brief 静态变量初始化
     */
    template<typename TDataType>
    const Variable<TDataType> Variable<TDataType>::msStaticObject("");

    template<typename TDataType>
    inline std::istream& operator >> (std::istream& rIstream,Variable<TDataType>& rVariable){}

    template<typename TDataType>
    inline std::ostream& operator << (std::ostream& rOstream,const Variable<TDataType>& rVariable){
        rThis.PrintInfo(rOstream);
        rThis.PrintData(rOstream);
        return rOstream;
    }

}

#endif //QUEST_VARIABLE_HPP