#ifndef QUEST_COMPARE_ELEMENTS_AND_CONDITIONS_UTILITY_HPP
#define QUEST_COMPARE_ELEMENTS_AND_CONDITIONS_UTILITY_HPP

// 项目头文件
#include "includes/define.hpp"
#include "includes/element.hpp"
#include "includes/condition.hpp"
#include "includes/geometrical_object.hpp"


namespace Quest{

    /**
     * @brief 用于比较单元和条件的动态类型工具类
     */
    class CompareElementsAndConditionsUtility{
        public:
            /**
             * @brief 获取注册几何体的名称
             */
            static void QUEST_API(QUEST_CORE) GetRegisteredName(
                const Geometry<Node>& rGeometry,
                std::string& rName
            );


            /**
             * @brief 获取注册几何体的名称
             */
            static void GetRegisteredName(
                const Geometry<Node>* pGeometry,
                std::string& rName
            ){
                CompareElementsAndConditionsUtility::GetRegisteredName(*pGeometry, rName);
            }


            /**
             * @brief 获取注册的单元名称
             */
            static void QUEST_API(QUEST_CORE) GetRegisteredName(
                const Element& rElement,
                std::string& rName
            );


            /**
             * @brief 获取注册的单元名称
             */
            static void GetRegisteredName(
                const Element* pElement,
                std::string& rName
            ){
                CompareElementsAndConditionsUtility::GetRegisteredName(*pElement, rName);
            }


            /**
             * @brief 获取注册的条件名称
             */
            static void QUEST_API(QUEST_CORE) GetRegisteredName(
                const Condition& rCondition,
                std::string& rName
            );


            /**
             * @brief 获取注册的条件名称
             */
            static void GetRegisteredName(
                const Condition* pCondition,
                std::string& rName
            ){
                CompareElementsAndConditionsUtility::GetRegisteredName(*pCondition, rName);
            }

        public:

        protected:

        private:

        private:

    };

}


#endif //QUEST_COMPARE_ELEMENTS_AND_CONDITIONS_UTILITY_HPP