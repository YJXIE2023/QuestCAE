/*----------------------------------------------------------------------
提供了一种将组件（如单元、条件、变量等）按照唯一名称进行存储和管理的机制
-------------------------------------------------------------------------*/

#ifndef QUEST_QUSET_COMPONENTS_HPP
#define QUEST_QUSET_COMPONENTS_HPP

// 系统头文件
#include <string>
#include <iostream>
#include <map>
#include <typeinfo>

// 第三方头文件
#include "includes/define.hpp"
#include "includes/data_communicator.hpp"
#include "container/flags.hpp"
#include "container/variable.hpp"
#include "utilities/quaternion.hpp"

namespace Quest{

    template<typename TComponentType>
    class QUEST_API QuestComponents{
        public:
            QUEST_CLASS_POINTER_DEFINITION(QuestComponents);
            using ComponentsContainerType = std::map<std::string, const TComponentType*>;
            using ValueType = typename ComponentsContainerType::value_type;

            QuestComponents() = default;

            virtual ~QuestComponents() = default;

            static void Add(const std::string& rName, const TComponentType& rComponent){
                auto it_comp = msComponents.find(rName);
                QUEST_ERROR_IF(it_comp != msComponents.end() && typeid(*(it_comp->second)) != typeid(rComponent)) << "The component \"" << rName << "\" has already been registered with a different type." << std::endl;
                msComponenets.insert(ValueType(rName, &rComponent));
            }

            static void Remove(const std::string& rName){
                std::size_t num_erased = msComponents.erase(rName);
                QUEST_ERROR_IF(num_erased == 0) << "Trying to remove a non-existent component \"" << rName << "\"." << std::endl;
            }

            static const TComponentType& Get(const std::string& rName){
                auto it_comp = msComponents.find(rName);
                QUEST_ERROR_IF(it_comp == msComponents.end()) << GetMessageUnregisteredComponent(rName) << std::endl;
                return *(it_comp->second);
            }

            static void Register() {}

            static ComponentsContainerType& GetComponents(){
                return msComponents;
            }

            static ComponentsContainerType* pGetComponents(){
                return &msComponents;
            }

            static bool Has(const std::string& rName){
                return (msComponents.find(rName)!= msComponents.end());
            }

            virtual std::string Info() const{
                return "Quest Components";
            }

            virtual void PrintData(std::ostream& rOStream) const{
                for(const auto& rComp : msComponents){
                    rOStream << "    " << rComp.first << std::endl;
                }
            }

        protected:

        private:
            QuestComponents(const QuestComponents& rOther);

            QuestComponents& operator=(const QuestComponents& rOther);

            static std::string GetMessageUnregisteredComponent(const std::string& rComponentName){
                std::stringstream msg;
                msg << "The component \"" << rComponentName << "\" is not registered in the QuestComponents." << std::endl;
                QuestComponents instance;
                instance.PrintData(msg);

                return msg.str();
            }

        private:
            static ComponentsContainerType msComponents;

    };


    template<>
    class QUEST_API QuestComponents<VariableData>{
        public:
            QUEST_CLASS_POINTER_DEFINITION(QuestComponents);
            using ComponentsContainerType = std::map<std::string, VariableData*>;
            using ValueType = ComponentsContainerType::value_type;

            QuestComponents() = default;

            virtual ~QuestComponents() = default;

            static void Add(const std::string& rName, VariableData& rComponent){
                msComponents.insert(ValueType(rName, &rComponent));
            }

            static void Remove(const std::string& rName){
                std::size_t num_erased = msComponents.erase(rName);
                QUEST_ERROR_IF(num_erased == 0) << "Trying to remove a non-existent component \"" << rName << "\"." << std::endl;
            }

            static std::size_t Size(){
                return msComponents.size();
            }

            static VariableData& Get(const std::string& rName){
                auto it_comp = msComponents.find(rName);
                QUEST_ERROR_IF(it_comp == msComponents.end()) << GetMessageUnregisteredComponent(rName) << std::endl;
                return *(it_comp->second);
            }

            static VariableData* pGet(const std::string& rName){
                auto it_comp = msComponents.find(rName);
                QUEST_ERROR_IF(it_comp == msComponents.end()) << GetMessageUnregisteredComponent(rName) << std::endl;
                return it_comp->second;
            }

            static void Register() {}

            static ComponentsContainerType& GetComponents(){
                return msComponents;
            }

            static ComponentsContainerType* pGetComponents(){
                return &msComponents;
            }

            static bool Has(const std::string& rName){
                return (msComponents.find(rName)!= msComponents.end());
            }

            virtual std::string Info() const{
                return "Quest Components <VariableData>";
            }

            virtual void PrintData(std::ostream& rOStream) const{
                for(const auto& rComp : msComponents){
                    rOStream << "    " << rComp.first << std::endl;
                }
            }

        protected:

        private:
            QuestComponents& operator=(const QuestComponents& rOther);

            QuestComponents(const QuestComponents& rOther);

            static std::string GetMessageUnregisteredComponent(const std::string& rComponentName){
                std::stringstream msg;
                msg << "The component \"" << rComponentName << "\" is not registered in the QuestComponents." << std::endl;
                QuestComponents instance;
                instance.PrintData(msg);

                return msg.str();
            }

        private:
            static ComponentsContainerType msComponents;

    };


    template<typename TComponentType>
    typename QuestComponents<TComponentType>::ComponentsContainerType QuestComponents<TComponentType>::msComponents;

    QUEST_API_EXTERN template class QUEST_API QuestComponents<Variable<bool>>;  
    QUEST_API_EXTERN template class QUEST_API QuestComponents<Variable<int>>;
    QUEST_API_EXTERN template class QUEST_API QuestComponents<Variable<double>>;
    QUEST_API_EXTERN template class QUEST_API QuestComponents<Variable<unsigned int>>;
    QUEST_API_EXTERN template class QUEST_API QuestComponents<Variable<Array1d<double, 3>>>;
    QUEST_API_EXTERN template class QUEST_API QuestComponents<Variable<Array1d<double, 4>>>;
    QUEST_API_EXTERN template class QUEST_API QuestComponents<Variable<Array1d<double, 6>>>;
    QUEST_API_EXTERN template class QUEST_API QuestComponents<Variable<Array1d<double, 9>>>;
    QUEST_API_EXTERN template class QUEST_API QuestComponents<Variable<Quaternion<double>>>;    
    QUEST_API_EXTERN template class QUEST_API QuestComponents<Variable<Vector>>;
    QUEST_API_EXTERN template class QUEST_API QuestComponents<Variable<Matrix>>;
    QUEST_API_EXTERN template class QUEST_API QuestComponents<Variable<std::string>>;
    QUEST_API_EXTERN template class QUEST_API QuestComponents<Variable<Flags>>;
    QUEST_API_EXTERN template class QUEST_API QuestComponents<Flags>;
    QUEST_API_EXTERN template class QUEST_API QuestComponents<DataCommunicator>;

    template<typename TComponentType>
    inline std::ostream& operator << (std::ostream& rOstream, const QuestComponents<TComponentType>& rComponents){
        rComponents.PrintInfo(rOstream);
        rOstream << std::endl;
        rComponents.PrintData(rOstream);
        return rOstream;
    }

    void QUEST_API AddQuestComponent(const std::string& rName, const Variable<bool>& rComponent);
    void QUEST_API AddQuestComponent(const std::string& rName, const Variable<int>& rComponent);
    void QUEST_API AddQuestComponent(const std::string& rName, const Variable<double>& rComponent);
    void QUEST_API AddQuestComponent(const std::string& rName, const Variable<unsigned int>& rComponent);
    void QUEST_API AddQuestComponent(const std::string& rName, const Variable<Array1d<double, 3>>& rComponent);
    void QUEST_API AddQuestComponent(const std::string& rName, const Variable<Array1d<double, 4>>& rComponent);
    void QUEST_API AddQuestComponent(const std::string& rName, const Variable<Array1d<double, 6>>& rComponent);
    void QUEST_API AddQuestComponent(const std::string& rName, const Variable<Array1d<double, 9>>& rComponent);
    void QUEST_API AddQuestComponent(const std::string& rName, const Variable<Quaternion<double>>& rComponent);
    void QUEST_API AddQuestComponent(const std::string& rName, const Variable<Vector>& rComponent);
    void QUEST_API AddQuestComponent(const std::string& rName, const Variable<Matrix>& rComponent);
    void QUEST_API AddQuestComponent(const std::string& rName, const Variable<std::string>& rComponent);
    void QUEST_API AddQuestComponent(const std::string& rName, const Flags& rComponent);
    void QUEST_API AddQuestComponent(const std::string& rName, const Variable<Flags>& rComponent);

    template<typename TComponentType>
    void AddQuestComponent(const std::string& rName, const TComponentType& rComponent){}

}

#endif //QUEST_QUSET_COMPONENTS_HPP