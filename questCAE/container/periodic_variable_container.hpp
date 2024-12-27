#ifndef QUEST_PERIODIC_VARIABLE_CONTAINER_HPP
#define QUEST_PERIODIC_VARIABLE_CONTAINER_HPP

// 系统头文件
#include <string>
#include <iostream>

// 第三方头文件
#include <boost/iterator/indirect_iterator.hpp>

// 项目头文件
#include "includes/define.hpp"
#include "includes/serializer.hpp"
#include "container/variable.hpp"
#include "includes/quest_components.hpp"

namespace Quest{

    /**
     * @class PeriodicVariablesContainer
     * @brief 用于定义周期性边界条件的变量容器
     * @details 由PeriodicCondition使用，用于标识周期性边界条件适用的自由度（Dofs）
     */
    class PeriodicVariablesContainer final{
        public:
            QUEST_CLASS_POINTER_DEFINITION(PeriodicVariablesContainer);

            using DoubleVariableType = Variable<double>;
            using DoubleVariablesContainerType = std::vector<const DoubleVariableType*>;
            using DoubleVariablesConstIterator = boost::indirect_iterator<DoubleVariablesContainerType::const_iterator>;
            using SizeType = std::size_t;

            /**
             * @brief 构造函数
             */
            PeriodicVariablesContainer(): mPeriodicDoubleVars(){}

            /**
             * @Brief 复制构造函数
             */
            PeriodicVariablesContainer(const PeriodicVariablesContainer& rOther): mPeriodicDoubleVars(rOther.mPeriodicDoubleVars){}

            /**
             * @brief 析构函数
             */
            ~PeriodicVariablesContainer(){}

            /**
             * @brief 赋值运算符
             */
            PeriodicVariablesContainer& operator = (const PeriodicVariablesContainer& rOther){
                this->mPeriodicDoubleVars = rOther.mPeriodicDoubleVars;
                return *this;
            }

            /**
             * @brief 添加一个周期性变量
             */
            void Add(const DoubleVariableType& rVar){
                mPeriodicDoubleVars.push_back(&rVar);
            }

            /**
             * @brief 清空容器
             */
            void Clear(){
                mPeriodicDoubleVars.clear();
            }

            /**
             * @brief 返回周期性变量容器首元素的迭代器
             */
            DoubleVariablesConstIterator DoubleVariablesBegin() const{
                return DoubleVariablesConstIterator(mPeriodicDoubleVars.begin());
            }

            /**
             * @brief 返回周期性变量容器尾元素的迭代器
             */
            DoubleVariablesConstIterator DoubleVariablesEnd() const{
                return DoubleVariablesConstIterator(mPeriodicDoubleVars.end());
            }

            /**
             * @brief 返回周期性变量容器大小
             */
            SizeType Size() const{
                return mPeriodicDoubleVars.size();
            }

            std::string Info() const{
                std::stringstream buffer;
                buffer << "PeriodicVariablesContainer";
                return buffer.str();
            }

            void PrintInfo(std::ostream& rOstream) const{
                rOstream << "PeriodicVariablesContainer";
            }

            void PrintData(std::ostream& rOstream) const{
                rOstream << "Double Variables:" << std::endl;
                for(DoubleVariablesContainerType::const_iterator it = mPeriodicDoubleVars.begin(); it!= mPeriodicDoubleVars.end(); it++){
                    (*it)->PrintInfo(rOstream);
                    rOstream << std::endl;
                }
            }

        protected:

        private:
            friend class Serializer;

            virtual void save(Serializer& rSerializer) const{
                std::size_t DoubleVarSize = mPeriodicDoubleVars.size();
                rSerializer.save("DoubleVarSize", DoubleVarSize);
                for(std::size_t i = 0; i < DoubleVarSize; i++){
                    rSerializer.save("Variable Name", mPeriodicDoubleVars[i]->Name());
                }
            }

            virtual void load(Serializer& rSerializer){
                std::string Name;
                std::size_t DoubleVarSize;
                rSerializer.load("DoubleVarSize", DoubleVarSize);
                for(std::size_t i = 0; i < DoubleVarSize; i++){
                    rSerializer.load("Variable Name", Name);
                    Add(*(static_cast<DoubleVariableType*>(QuestComponents<VariableData>::pGet(Name))));
                }
            }

        private:
            /**
             * @brief 存储周期性变量的容器
             * @details std::vector<const Variable<double>*>
             */
            DoubleVariablesContainerType mPeriodicDoubleVars;

    };

    inline std::istream& operator >> (std::istream& rIstream, PeriodicVariablesContainer& rThis){
        return rIstream;
    }

    inline std::ostream& operator << (std::ostream& rOstream, const PeriodicVariablesContainer& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }

} // namespace Quest


#endif //QUEST_PERIODIC_VARIABLE_CONTAINER_HPP