#ifndef QUEST_APPLICATION_HPP
#define QUEST_APPLICATION_HPP

// 系统头文件
#include <string>
#include <iostream>

// 项目头文件
#include "includes/element.hpp"
#include "includes/condition.hpp"
#include "includes/quest_components.hpp"
#include "includes/geometrical_object.hpp"
#include "includes/periodic_condition.hpp"
#include "includes/master_slave_constraint.hpp"
#include "IO/logger.hpp"
#include "utilities/quaternion.hpp"

// 几何定义
#include "geometries/register_quest_components_for_geometry.hpp"
#include "geometries/point.hpp"

// 建模
#include "modeler/modeler.hpp"

namespace Quest{

    /**
     * @brief 定义了所有 Quest 应用程序与 Kernel 的接口
     * @details Application 类定义了为 Kernel 提供所需信息以正确配置整个系统的必要接口
     */
    class QUEST_API(QUEST_CORE) QuestApplication{
        public:
            using NodeType = Node;
            using GeometryType = Geometry<NodeType>;

            QUEST_CLASS_POINTER_DEFINITION(QuestApplication);

        public:
            /**
             * @brief 构造函数
             */
            explicit QuestApplication(const std::string& ApplicationName);


            QuestApplication() = delete;


            /**
             * @brief 赋值构造函数
             */
            QuestApplication(QuestApplication const& rOther): 
                mpVariableData(rOther.mpVariableData),
                mpIntVariables(rOther.mpIntVariables),
                mpUnsignedIntVariables(rOther.mpUnsignedIntVariables),
                mpDoubleVariables(rOther.mpDoubleVariables),
                mpArray1DVariables(rOther.mpArray1DVariables),
                mpArray1D4Variables(rOther.mpArray1D4Variables),
                mpArray1D6Variables(rOther.mpArray1D6Variables),
                mpArray1D9Variables(rOther.mpArray1D9Variables),
                mpVectorVariables(rOther.mpVectorVariables),
                mpMatrixVariables(rOther.mpMatrixVariables),
                mpGeometries(rOther.mpGeometries),
                mpElements(rOther.mpElements),
                mpConditions(rOther.mpConditions),
                mpMasterSlaveConstraints(rOther.mpMasterSlaveConstraints),
                mpModelers(rOther.mpModelers) {}


            /**
             * @brief 析构函数
             */
            virtual ~QuestApplication() {}


            /**
             * @brief 注册所需组件
             */
            virtual void Register()
            {
                RegisterQuestCore();
            }


            /**
             * @brief 注册 Quest 核心组件
             */
            void RegisterQuestCore();


            /**
             * @brief 注册所有变量
             */
            void RegisterVariables();


            /**
             * @brief 注册全局指针变量
             */
            void RegisterGlobalPointerVariables();


            /**
             * @brief 获取应用名称
             */
            const std::string& Name() const { return mApplicationName; }


            /**
             * @brief 返回组件
             */
            QuestComponents<Variable<int> >::ComponentsContainerType& GetComponents(
                Variable<int> const& rComponentType
            ){
                return *mpIntVariables;
            }

            QuestComponents<Variable<unsigned int> >::ComponentsContainerType&
            GetComponents(Variable<unsigned int> const& rComponentType
            ){
                return *mpUnsignedIntVariables;
            }

            QuestComponents<Variable<double> >::ComponentsContainerType& GetComponents(
                Variable<double> const& rComponentType
            ){
                return *mpDoubleVariables;
            }

            QuestComponents<Variable<Array1d<double, 3> > >::ComponentsContainerType&
            GetComponents(Variable<Array1d<double, 3> > const& rComponentType
            ){
                return *mpArray1DVariables;
            }

            QuestComponents<Variable<Array1d<double, 4> > >::ComponentsContainerType&
            GetComponents(Variable<Array1d<double, 4> > const& rComponentType
            ){
                return *mpArray1D4Variables;
            }

            QuestComponents<Variable<Array1d<double, 6> > >::ComponentsContainerType&
            GetComponents(Variable<Array1d<double, 6> > const& rComponentType
            ){
                return *mpArray1D6Variables;
            }

            QuestComponents<Variable<Array1d<double, 9> > >::ComponentsContainerType&
            GetComponents(Variable<Array1d<double, 9> > const& rComponentType
            ){
                return *mpArray1D9Variables;
            }

            QuestComponents<Variable<Quaternion<double> > >::ComponentsContainerType&
            GetComponents(Variable<Quaternion<double> > const& rComponentType
            ){
                return *mpQuaternionVariables;
            }

            QuestComponents<Variable<Vector> >::ComponentsContainerType& GetComponents(
                Variable<Vector> const& rComponentType
            ){
                return *mpVectorVariables;
            }

            QuestComponents<Variable<Matrix> >::ComponentsContainerType& GetComponents(
                Variable<Matrix> const& rComponentType
            ){
                return *mpMatrixVariables;
            }

            QuestComponents<VariableData>::ComponentsContainerType& GetVariables() {
                return *mpVariableData;
            }

            QuestComponents<Geometry<Node>>::ComponentsContainerType& GetGeometries() {
                return *mpGeometries;
            }

            QuestComponents<Element>::ComponentsContainerType& GetElements() {
                return *mpElements;
            }

            QuestComponents<Condition>::ComponentsContainerType& GetConditions() {
                return *mpConditions;
            }

            QuestComponents<MasterSlaveConstraint>::ComponentsContainerType& GetMasterSlaveConstraints() {
                return *mpMasterSlaveConstraints;
            }

            QuestComponents<Modeler>::ComponentsContainerType& GetModelers() {
                return *mpModelers;
            }


            /**
             * @brief 设置组件
             */
            void SetComponents(
                const QuestComponents<VariableData>::ComponentsContainerType& VariableDataComponents
            ){
                for (auto it = mpVariableData->begin(); it != mpVariableData->end(); it++) {
                    std::string const& r_variable_name = it->second->Name();
                    auto it_variable = VariableDataComponents.find(r_variable_name);
                    QUEST_ERROR_IF(it_variable == VariableDataComponents.end()) << "This variable is not registered in Kernel : " << *(it_variable->second) << std::endl;
                }
            }

            void SetComponents(QuestComponents<Geometry<Node>>::ComponentsContainerType const& GeometryComponents){
                mpGeometries->insert(GeometryComponents.begin(), GeometryComponents.end());
            }

            void SetComponents(QuestComponents<Element>::ComponentsContainerType const&ElementComponents){
                mpElements->insert(ElementComponents.begin(), ElementComponents.end());
            }

            void SetComponents(QuestComponents<MasterSlaveConstraint>::ComponentsContainerType const&MasterSlaveConstraintComponents){
                mpMasterSlaveConstraints->insert(MasterSlaveConstraintComponents.begin(), MasterSlaveConstraintComponents.end());
            }

            void SetComponents(QuestComponents<Modeler>::ComponentsContainerType const& ModelerComponents){
                mpModelers->insert(ModelerComponents.begin(), ModelerComponents.end());
            }

            void SetComponents(QuestComponents<Condition>::ComponentsContainerType const& ConditionComponents){
                mpConditions->insert(
                    ConditionComponents.begin(), ConditionComponents.end());
            }


            /**
             * @brief 获取注册对象
             */
            Serializer::RegisteredObjectsContainerType& GetRegisteredObjects(){
                return *mpRegisteredObjects;
            }


            /**
             * @brief 获取注册对象名称
             */
            Serializer::RegisteredObjectsNameContainerType& GetRegisteredObjectsName() {
                return *mpRegisteredObjectsName;
            }


            virtual std::string Info() const
            {
                return "QuestApplication";
            }


            virtual void PrintInfo(std::ostream& rOStream) const
            {
                rOStream << Info();
            }


            virtual void PrintData(std::ostream& rOStream) const
            {
                rOStream << "Variables:" << std::endl;
                QuestComponents<VariableData>().PrintData(rOStream);
                rOStream << std::endl;

                rOStream << "Geometries:" << std::endl;
                QuestComponents<Geometry<Node>>().PrintData(rOStream);

                rOStream << "Elements:" << std::endl;
                QuestComponents<Element>().PrintData(rOStream);
                rOStream << std::endl;

                rOStream << "Conditions:" << std::endl;
                QuestComponents<Condition>().PrintData(rOStream);
                rOStream << std::endl;

                rOStream << "MasterSlaveConstraints:" << std::endl;
                QuestComponents<MasterSlaveConstraint>().PrintData(rOStream);
                rOStream << std::endl;

                rOStream << "Modelers:" << std::endl;
                QuestComponents<Modeler>().PrintData(rOStream);
            }

        protected:
            // 应用名称
            std::string mApplicationName;

            // 必须定义的几何体
            const Point mPointPrototype;

            // 网格条件

            // 主从约束
            const MasterSlaveConstraint mMasterSlaveConstraint;

            // 周期性条件
            // const PeriodicCondition mPeriodicCondition;
            // const PeriodicCondition mPeriodicConditionEdge;
            // const PeriodicCondition mPeriodicConditionCorner;

            // 网格单元

            // 建模器
            const Modeler mModeler;

            // 基础本构法则定义
            const ConstitutiveLaw mConstitutiveLaw;

            // quest组件定义
            QuestComponents<VariableData>::ComponentsContainerType* mpVariableData;
            QuestComponents<Variable<int> >::ComponentsContainerType* mpIntVariables;
            QuestComponents<Variable<unsigned int> >::ComponentsContainerType* mpUnsignedIntVariables;
            QuestComponents<Variable<double> >::ComponentsContainerType* mpDoubleVariables;
            QuestComponents<Variable<Array1d<double, 3> > >::ComponentsContainerType* mpArray1DVariables;
            QuestComponents<Variable<Array1d<double, 4> > >::ComponentsContainerType* mpArray1D4Variables;
            QuestComponents<Variable<Array1d<double, 6> > >::ComponentsContainerType* mpArray1D6Variables;
            QuestComponents<Variable<Array1d<double, 9> > >::ComponentsContainerType* mpArray1D9Variables;
            QuestComponents<Variable<Quaternion<double> > >::ComponentsContainerType* mpQuaternionVariables;
            QuestComponents<Variable<Vector> >::ComponentsContainerType* mpVectorVariables;
            QuestComponents<Variable<Matrix> >::ComponentsContainerType* mpMatrixVariables;
            QuestComponents<Geometry<Node>>::ComponentsContainerType* mpGeometries;
            QuestComponents<Element>::ComponentsContainerType* mpElements;
            QuestComponents<Condition>::ComponentsContainerType* mpConditions;
            QuestComponents<MasterSlaveConstraint>::ComponentsContainerType* mpMasterSlaveConstraints;
            QuestComponents<Modeler>::ComponentsContainerType* mpModelers;

            // 序列化操作
            Serializer::RegisteredObjectsContainerType* mpRegisteredObjects;
            Serializer::RegisteredObjectsNameContainerType* mpRegisteredObjectsName;

        private:
            QuestApplication& operator=(QuestApplication const& rOther);

    };

    inline std::istream& operator >> (std::istream& rIstream, QuestApplication& rThis);

    inline std::ostream& operator << (std::ostream& rOstream, const QuestApplication& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }

}


#endif //QUEST_APPLICATION_HPP