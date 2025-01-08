// 系统头文件
#include <typeinfo>

// 项目头文件
#include "includes/quest_components.hpp"
#include "utilities/compare_elements_and_conditions_utility.hpp"

namespace Quest{

    void CompareElementsAndConditionsUtility::GetRegisteredName(
        const Geometry<Node>& rGeometry,
        std::string& rName
    ){
        QUEST_TRY
        
        for(const auto& component: QuestComponents<Geometry<Node>>::GetComponents()){
            if(Geometry<Node>::IsSame(*(component.second), rGeometry)){
                rName = component.first;
                return;
            }
        }

        QUEST_ERROR << "Geometry \"" << typeid(rGeometry).name() << "\" not found!" << std::endl;

        QUEST_CATCH("")
    }


    void CompareElementsAndConditionsUtility::GetRegisteredName(
        const Element& rElement,
        std::string& rName
    ){
        QUEST_TRY
        
        for(const auto& component: QuestComponents<Element>::GetComponents()){
            if(GeometricalObject::IsSame(*(component.second), rElement)){
                rName = component.first;
                return;
            }
        }

        QUEST_ERROR << "Element \"" << typeid(rElement).name() << "\" not found!" << std::endl;

        QUEST_CATCH("")
    }


    void CompareElementsAndConditionsUtility::GetRegisteredName(
        const Condition& rCondition,
        std::string& rName
    ){
        QUEST_TRY
        
        for(const auto& component: QuestComponents<Condition>::GetComponents()){
            if(GeometricalObject::IsSame(*(component.second), rCondition)){
                rName = component.first;
                return;
            }
        }

        QUEST_ERROR << "Condition \"" << typeid(rCondition).name() << "\" not found!" << std::endl;

        QUEST_CATCH("")
    }

}