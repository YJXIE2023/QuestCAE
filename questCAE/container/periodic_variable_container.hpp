/*-------------------------------------------
作为存储器用于管理与周期性边界条件相关的变量
主要用于定义和处理周期性条件应用到的自由度
-------------------------------------------*/

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

    class PeriodicVariablesContainer final{
        public:
            QUEST_CLASS_POINTER_DEFINITION(PeriodicVariablesContainer);

            using DoubleVariableType = Variable<double>;
            using DoubleVariablesContainerType = std::vector<const DoubleVariableType*>;
            using DoubleVariablesConstIterator = boost::indirect_iterator<DoubleVariablesContainerType::const_iterator>;
            using SizeType = std::size_t;

            PeriodicVariablesContainer(): mPeriodicDoubleVars(){}

            PeriodicVariablesContainer(const PeriodicVariablesContainer& rOther): mPeriodicDoubleVars(rOther.mPeriodicDoubleVars){}

            ~PeriodicVariablesContainer(){}

            PeriodicVariablesContainer& operator = (const PeriodicVariablesContainer& rOther){
                this->mPeriodicDoubleVars = rOther.mPeriodicDoubleVars;
                return *this;
            }

            void Add(const DoubleVariableType& rVar){
                mPeriodicDoubleVars.push_back(&rVar);
            }

            void Clear(){
                mPeriodicDoubleVars.clear();
            }

            DoubleVariablesConstIterator DoubleVariablesBegin() const{
                return DoubleVariablesConstIterator(mPeriodicDoubleVars.begin());
            }

            DoubleVariablesConstIterator DoubleVariablesEnd() const{
                return DoubleVariablesConstIterator(mPeriodicDoubleVars.end());
            }

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