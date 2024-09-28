/*-------------------------------------------------------
Variable 类提供了一种灵活且类型安全的方式来管理和检索数据
-------------------------------------------------------*/

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

    template<typename TDataType>
    class Variable:public VariableData{
        public:
            QUEST_CLASS_POINTER_DEFINITION(Variable);
            using Type = TDataType;
            using KeyType = VariableData::KeyType;
            using VariableType = Variable<TDataType>;

            explicit Variable(
                const std::string& NewName,
                const TDataType Zero = TDataType(),
                const VariableType* pTimeDerivativeData = nullptr
            ): VariableData(NewName,sizeof(TDataType)), mZero(Zero), mpTimeDerivativeData(pTimeDerivativeData){
                RegisterThisVariable():
            }


            explicit Variable(
                const std::string& NewName,
                const VariableType* pTimeDervativeVariable
            ):VariableData(NewName,sizeof(TDataType)), mZero(TDataType()), mpTimeDerivativeData(pTimeDervativeVariable){
                RegisterThisVariable();
            }


            template<typename TSourceVariableType>
            explicit Variable(
                const std::string& NewName,
                TSourceVariableType* pSourceVariable,
                char ComponentIndex,
                const TDataType Zero = TDataType()
            ):VariableData(NewName,sizeof(TDataType),pSourceVariable,ComponentIndex), mZero(Zero){
                RegisterThisVariable();
            }


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


            explicit Variable(const VariableType& rOther):VariableData(rOther), mZero(rOther.mZero), mpTimeDerivativeData(rOther.mpTimeDerivativeData){}

            ~Variable() override {}

            VariableType& operator = (const VariableType& rOther) = delete;

            void* Clone(const void* pSource) const override{
                return new TDataType(*static_cast<const TDataType*>(pSource));
            }

            void* Copy(const void* pSource, void* pDestination) const override{
                return new(pDestination) TDataType(*static_cast<const TDataType*>(pSource));
            }

            void Assign(const void* pSource, void* pDestination) const override{
                *static_cast<TDataType*>(pDestination) = *static_cast<const TDataType*>(pSource);
            }

            void AssignZero(void* pDestination) const override{
                new (pDestination) TDataType(mZero);
            }

            void Delete(void* pSource) const override{
                delete static_cast<TDataType*>(pSource);
            }

            void Destruct(void* pSource) const override{
                static_cast<TDataType*>(pSource)->~TDataType();
            }

            void Print(const void* pSource, std::ostream& rOstream) const override{
                if(IsComponent()){
                    rOstream << "Component of " << GetSourceVariable().Name() << " variable: " << *static_cast<const TDataType*>(pSource);
                } else {
                    rOstream << Name() << " : " << *static_cast<const TDataType*>(pSource);
                }
            }

            void PrintData(const void* pSource, std::ostream& rOstream) const override{
                rOstream << *static_cast<const TDataType*>(pSource);
            }

            void Save(Serializer& rSerializer, void* pData) const override{
                rSerializer.Save("Data",*static_cast<TDataType*>(pData));
            }

            void Allocate(void** pData) const override{
                *pData = new TDataType;
            }

            void Load(Serializer& rSerializer, void* pData) const override{
                rSerializer.Load("Data",*static_cast<TDataType*>(pData));
            }

            static const VariableType& StaticObject(){
                const static Variable<TDataType> static_object("NONE");
                return static_object;
            }

            TDataType& GetValue(void* pData) const{
                return GetValueByIndex(static_cast<TDataType*>(pData),GetComponentIndex());
            }

            const TDataType& GetValue(const void* pData) const{
                return GetValueByIndex(static_cast<const TDataType*>(pData),GetComponentIndex());
            }

            const VariableType& GetTimeDerivative() const{
                QUEST_DEBUG_ERROR_IF(mpTimeDerivativeData == nullptr) << "Time derivative for Variable \"" << Name() << "\" was not assigned" << std::endl;
                return *mpTimeDerivativeData;
            }

            const TDataType& Zero() const{
                return mZero;
            }

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
            Variable() {}

            template<typename TValueType>
            TDataType& GetValueByIndex(TDataType* pValue, std::size_t index) const{
                return *static_cast<TDataType*>(pValue + index);
            }

            template<typename TValueType>
            const TDataType& GetValueByIndex(const TDataType* pValue, std::size_t index) const{
                return *static_cast<const TDataType*>(pValue + index);
            }

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
            static const VariableType msStaticObject;  // 静态变量
            TDataType mZero;
            const VariableType* mpTimeDerivativeData = nullptr;

    };

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