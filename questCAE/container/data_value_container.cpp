/*----------------------------------------------------
data_value_container.hpp类的实现代码
----------------------------------------------------*/

// 项目头文件
#include "container/data_value_container.hpp"

namespace Quest{

    QUEST_CREATE_LOCAL_FLAG(DataValueContainer, OVERWRITE_OLD_VALUES, 0);

    void DataValueContainer::Merge(const DataValueContainer& rOther, const Flags Options){
        const bool overwrite_values = Options.Is(OVERWRITE_OLD_VALUES);

        if (overwrite_values) {
            for (const_iterator i = rOther.mData.begin(); i != rOther.mData.end(); ++i) {
                bool variable_already_exist = false;
                    for (iterator j = mData.begin(); j != mData.end(); ++j) {
                        if (i->first == j->first) {
                            variable_already_exist = true;
                            j->first->Delete(j->second);
                            j->second = i->first->Clone(i->second);
                        }
                    }
                if (!variable_already_exist) { 
                    mData.push_back(ValueType(i->first, i->first->Clone(i->second)));
                }
            }
        } else {
            for (const_iterator i = rOther.mData.begin(); i != rOther.mData.end(); ++i) {
                bool variable_already_exist = false;
                    for (iterator j = mData.begin(); j != mData.end(); ++j) {
                        if (i->first == j->first) {
                            variable_already_exist = true;
                        }
                    }
                if (!variable_already_exist) {
                    mData.push_back(ValueType(i->first, i->first->Clone(i->second)));
                }
            }
        }
    }


    void DataValueContainer::save(Serializer& rSerializer) const{
        QUEST_TRY

        std::size_t size = mData.size();
        rSerializer.save("Size", size);
        for(std::size_t i = 0; i < size; ++i){
            rSerializer.save("Variable Name", mData[i].first->Name());
            mData[i].first->Save(rSerializer, mData[i].second);
        }

        QUEST_CATCH("")
    }


    void DataValueContainer::load(Serializer& rSerializer){
        QUEST_TRY

        std::size_t size;
        rSerializer.load("Size", size);
        mData.resize(size);
        std::string name;
        for(std::size_t i = 0; i < size; ++i){
            rSerializer.load("Variable Name", name);
            mData[i].first = QuestComponents<VariableData>::pGet(name);
            mData[i].first->Allocate(&(mData[i].second));
            mData[i].first->Load(rSerializer, mData[i].second);
        }

        QUEST_CATCH("")
    }

}